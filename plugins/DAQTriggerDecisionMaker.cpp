#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/ThreadHelper.hpp"
#include "appfwk/DAQModuleHelper.hpp"

#include "triggermodules/daqtriggerdecisionmaker/Nljs.hpp"
#include "dune-trigger-algs/Supernova/TriggerDecisionMaker_Supernova.hh"

#include "CommonIssues.hpp"

#include <ers/Issue.h>
#include <ers/ers.h>
#include <TRACE/trace.h>

#include <chrono>
#include <memory>
#include <string>
#include <vector>


using pd_clock = std::chrono::duration<double, std::ratio<1, 50000000>>;
using namespace triggeralgs;

namespace dunedaq {
  namespace trigger {

    class DAQTriggerDecisionMaker: public dunedaq::appfwk::DAQModule, triggeralgs::TriggerDecisionMakerSupernova {
    public:
      explicit DAQTriggerDecisionMaker(const std::string& name);

      DAQTriggerDecisionMaker(const DAQTriggerDecisionMaker&) = delete;
      DAQTriggerDecisionMaker& operator=(const DAQTriggerDecisionMaker&) = delete;
      DAQTriggerDecisionMaker(DAQTriggerDecisionMaker&&) = delete;
      DAQTriggerDecisionMaker& operator=(DAQTriggerDecisionMaker&&) = delete;

      void init(const nlohmann::json& obj) override;
      
    private:
      void do_start    (const nlohmann::json& obj);
      void do_stop     (const nlohmann::json& obj);
      void do_configure(const nlohmann::json& obj);

      dunedaq::appfwk::ThreadHelper thread_;
      void do_work(std::atomic<bool>&);

      int64_t window;
      uint16_t thresh;
      uint16_t hit_thresh;

      using source_t = dunedaq::appfwk::DAQSource<TriggerCandidate>;
      std::unique_ptr<source_t> inputQueue_;

      using sink_t = dunedaq::appfwk::DAQSink<TriggerDecision>;
      std::unique_ptr<sink_t> outputQueue_;

      std::chrono::milliseconds queueTimeout_;
    };
    

    DAQTriggerDecisionMaker::DAQTriggerDecisionMaker(const std::string& name):
        DAQModule(name),
        thread_(std::bind(&DAQTriggerDecisionMaker::do_work, this, std::placeholders::_1)),
        inputQueue_(nullptr),
        outputQueue_(nullptr),
        queueTimeout_(100) {
      
      register_command("start"    , &DAQTriggerDecisionMaker::do_start    );
      register_command("stop"     , &DAQTriggerDecisionMaker::do_stop     );
      register_command("conf", &DAQTriggerDecisionMaker::do_configure);
    }

    void DAQTriggerDecisionMaker::init(const nlohmann::json& iniobj) {
      auto qi = appfwk::qindex(iniobj, {"input","output"});
      try {
	inputQueue_.reset(new source_t(qi["input"].inst));
      } catch (const ers::Issue& excpt) {
        throw dunedaq::dunetrigger::InvalidQueueFatalError(ERS_HERE, get_name(), "input", excpt);
      }

      try {
	outputQueue_.reset(new sink_t(qi["output"].inst));
      } catch (const ers::Issue& excpt) {
        throw dunedaq::dunetrigger::InvalidQueueFatalError(ERS_HERE, get_name(), "output", excpt);
      }

    }

    void DAQTriggerDecisionMaker::do_start(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_start() method";
      thread_.start_working_thread();
      ERS_LOG(get_name() << " successfully started");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_start() method";
    }

    void DAQTriggerDecisionMaker::do_stop(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_stop() method";
      thread_.stop_working_thread();
      //flush()
      ERS_LOG(get_name() << " successfully stopped");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_stop() method";
    }

    void DAQTriggerDecisionMaker::do_configure(const nlohmann::json& config/*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_configure() method";
      auto params = config.get<dunedaq::triggermodules::daqtriggerdecisionmaker::Conf>();

      window = params.time_window;
      thresh = params.threshold;
      hit_thresh = params.hit_threshold;
      
      try {
        m_time_window = {window};
        m_threshold = {thresh};
        m_hit_threshold = {hit_thresh};
      } catch(...)  {
        ERS_LOG(get_name() << " unsuccessfully configured");
      }
      ERS_LOG(get_name() << " successfully configured");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_configure() method";
    }



    void DAQTriggerDecisionMaker::do_work(std::atomic<bool>& running_flag) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_work() method";
      int receivedCount = 0;
      int sentCount = 0;
      TriggerCandidate cand;

      while (running_flag.load()) {
        TLOG(TLVL_CANDIDATE) << get_name() << ": Going to receive data from input queue";
        try {
          inputQueue_->pop(cand, queueTimeout_);
        } catch (const dunedaq::appfwk::QueueTimeoutExpired& excpt) {
          // it is perfectly reasonable that there might be no data in the queue 
          // some fraction of the times that we check, so we just continue on and try again
          continue;
        }
        ++receivedCount;

        std::vector<TriggerDecision> tds;
        this->operator()(cand,tds);
        
        std::string oss_prog = "Cand received #"+std::to_string(receivedCount);
        ers::debug(dunedaq::dunetrigger::ProgressUpdate(ERS_HERE, get_name(), oss_prog));
        for (auto const& td: tds) {
          std::cout << "\033[36mTimestamp : "     << td.algorithm  << "\033[0m  ";
          std::cout << "\033[36mtd.time_start : " << td.time_start << "\033[0m  ";
          std::cout << "\033[36mtd.time_end : "   << td.time_end   << "\033[0m\n";
		for (auto const& tc: td.tc_list) {
		  std::cout << "\033[36mtc.: "   << tc.algorithm<< "\033[0m\n";
			for (auto const& ta: tc.ta_list) {
			  std::cout << "\033[36mta.: "   << ta.algorithm<< "\033[0m\n";
				for (auto const& tp: ta.tp_list) {
				  std::cout << "\033[36mtp.: "   << tp.algorithm<< "\033[0m\n";
				  std::cout << "\033[36mtp.time_start: "   << tp.time_start<< "\033[0m\n";
				  std::cout << "\033[36mtp.channel: "   << tp.channel << "\033[0m\n";
				}
			}
		}
	}
        while(tds.size()) {
          bool successfullyWasSent = false;
          while (!successfullyWasSent && running_flag.load()) {
            try {
              outputQueue_->push(tds.back(), queueTimeout_);
              tds.pop_back();
              successfullyWasSent = true;
              ++sentCount;
            } catch (const dunedaq::appfwk::QueueTimeoutExpired& excpt) {
              std::ostringstream oss_warn;
              oss_warn << "push to output queue \"" << outputQueue_->get_name() << "\"";
              ers::warning(dunedaq::appfwk::QueueTimeoutExpired(ERS_HERE, get_name(), oss_warn.str(),
                                                                std::chrono::duration_cast<std::chrono::milliseconds>(queueTimeout_).count()));
            }
          }
        }
      }

      std::ostringstream oss_summ;
      oss_summ << ": Exiting do_work() method, received " << receivedCount
               << " TCs and successfully sent " << sentCount << " TDs. ";
      ers::info(dunedaq::dunetrigger::ProgressUpdate(ERS_HERE, get_name(), oss_summ.str()));
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_work() method";
    }


  } // namespace trigger
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::trigger::DAQTriggerDecisionMaker)

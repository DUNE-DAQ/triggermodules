#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/ThreadHelper.hpp"

#include "dune-trigger-algs/Supernova/TriggerDecisionMaker_Supernova.hh"

#include "CommonIssues.hpp"

#include <ers/Issue.h>
#include <ers/ers.h>
#include <TRACE/trace.h>

#include <memory>
#include <string>
#include <vector>


using namespace DuneTriggerAlgs;

namespace dunedaq {
  namespace trigger {

    class DAQTriggerDecisionMaker: public dunedaq::appfwk::DAQModule, DuneTriggerAlgs::TriggerDecisionMakerSupernova {
    public:
      explicit DAQTriggerDecisionMaker(const std::string& name);

      DAQTriggerDecisionMaker(const DAQTriggerDecisionMaker&) = delete;
      DAQTriggerDecisionMaker& operator=(const DAQTriggerDecisionMaker&) = delete;
      DAQTriggerDecisionMaker(DAQTriggerDecisionMaker&&) = delete;
      DAQTriggerDecisionMaker& operator=(DAQTriggerDecisionMaker&&) = delete;

      void init() override;
      
    private:
      void do_start(const std::vector<std::string>& args);
      void do_stop(const std::vector<std::string>& args);
      void do_configure_threshold(const std::vector<std::string>& args);

      dunedaq::appfwk::ThreadHelper thread_;
      void do_work(std::atomic<bool>&);

      std::unique_ptr<dunedaq::appfwk::DAQSource<TriggerCandidate>> inputQueue_;
      std::unique_ptr<dunedaq::appfwk::DAQSink<TriggerDecision>> outputQueue_;
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
      register_command("configure_threshold", &DAQTriggerDecisionMaker::do_configure_threshold);
    }

    void DAQTriggerDecisionMaker::init() {
      try {
        inputQueue_.reset(new dunedaq::appfwk::DAQSource<TriggerCandidate>(get_config()["input"].get<std::string>()));
      } catch (const ers::Issue& excpt) {
        throw dunedaq::dunetrigger::InvalidQueueFatalError(ERS_HERE, get_name(), "input", excpt);
      }

      try {
        outputQueue_.reset(new dunedaq::appfwk::DAQSink<TriggerDecision>(get_config()["output"].get<std::string>()));
      } catch (const ers::Issue& excpt) {
        throw dunedaq::dunetrigger::InvalidQueueFatalError(ERS_HERE, get_name(), "output", excpt);
      }

    }

    void DAQTriggerDecisionMaker::do_start(const std::vector<std::string>& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_start() method";
      thread_.start_working_thread();
      ERS_LOG(get_name() << " successfully started");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_start() method";
    }

    void DAQTriggerDecisionMaker::do_stop(const std::vector<std::string>& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_stop() method";
      thread_.stop_working_thread();
      //flush()
      ERS_LOG(get_name() << " successfully stopped");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_stop() method";
    }

    void DAQTriggerDecisionMaker::do_configure_threshold(const std::vector<std::string>& args) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_configure() method";
      
      int thresh = m_threshold;
      try {
        thresh = std::stoi(args.at(0));
        m_threshold = thresh;
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
          std::cout << "\033[35mtd.time_start : " << td.time_start << "\033[0m  ";
          std::cout << "\033[35mtd.time_end : "   << td.time_end << "\033[0m\n";
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

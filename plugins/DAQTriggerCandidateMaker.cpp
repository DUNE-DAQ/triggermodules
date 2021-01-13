#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/ThreadHelper.hpp"
#include "appfwk/DAQModuleHelper.hpp"

#include "dune-trigger-algs/Supernova/TriggerCandidateMaker_Supernova.hh"

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

    class DAQTriggerCandidateMaker: public dunedaq::appfwk::DAQModule, DuneTriggerAlgs::TriggerCandidateMakerSupernova {
    public:
      explicit DAQTriggerCandidateMaker(const std::string& name);

      DAQTriggerCandidateMaker(const DAQTriggerCandidateMaker&) = delete;
      DAQTriggerCandidateMaker& operator=(const DAQTriggerCandidateMaker&) = delete;
      DAQTriggerCandidateMaker(DAQTriggerCandidateMaker&&) = delete;
      DAQTriggerCandidateMaker& operator=(DAQTriggerCandidateMaker&&) = delete;

      //void init() override;
      void init(const nlohmann::json& obj) override;
      
    private:
      void do_start(const nlohmann::json& obj);
      void do_stop(const nlohmann::json& obj);
      void do_configure(const nlohmann::json& obj);

      dunedaq::appfwk::ThreadHelper thread_;
      void do_work(std::atomic<bool>&);

      //std::unique_ptr<dunedaq::appfwk::DAQSource<TriggerActivity>> inputQueue_;
      using source_t = dunedaq::appfwk::DAQSource<TriggerActivity>;
      std::unique_ptr<source_t> inputQueue_;

      //std::unique_ptr<dunedaq::appfwk::DAQSink<TriggerCandidate>> outputQueue_;
      using sink_t = dunedaq::appfwk::DAQSink<TriggerCandidate>;
      std::unique_ptr<sink_t> outputQueue_;

      std::chrono::milliseconds queueTimeout_;
    };
    

    DAQTriggerCandidateMaker::DAQTriggerCandidateMaker(const std::string& name):
        DAQModule(name),
        thread_(std::bind(&DAQTriggerCandidateMaker::do_work, this, std::placeholders::_1)),
        inputQueue_(nullptr),
        outputQueue_(nullptr),
        queueTimeout_(100) {
      
      register_command("start"    , &DAQTriggerCandidateMaker::do_start    );
      register_command("stop"     , &DAQTriggerCandidateMaker::do_stop     );
      register_command("configure", &DAQTriggerCandidateMaker::do_configure);
    }

    void DAQTriggerCandidateMaker::init(const nlohmann::json& iniobj) {
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

    void DAQTriggerCandidateMaker::do_start(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_start() method";
      thread_.start_working_thread();
      ERS_LOG(get_name() << " successfully started");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_start() method";
    }

    void DAQTriggerCandidateMaker::do_stop(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_stop() method";
      thread_.stop_working_thread();
      //flush()
      ERS_LOG(get_name() << " successfully stopped");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_stop() method";
    }

    void DAQTriggerCandidateMaker::do_configure(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_configure() method";
      
      int thresh = m_threshold;
      try {
        //thresh = std::stoi(args.at(0));
        m_threshold = thresh;
      } catch(...)  {
        ERS_LOG(get_name() << " unsuccessfully configured");
      }
      ERS_LOG(get_name() << " successfully configured");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_configure() method";
    }



    void DAQTriggerCandidateMaker::do_work(std::atomic<bool>& running_flag) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_work() method";
      int receivedCount = 0;
      int sentCount = 0;
      TriggerActivity activ;

      while (running_flag.load()) {
        TLOG(TLVL_CANDIDATE) << get_name() << ": Going to receive data from input queue";
        try {
          inputQueue_->pop(activ, queueTimeout_);
        } catch (const dunedaq::appfwk::QueueTimeoutExpired& excpt) {
          // it is perfectly reasonable that there might be no data in the queue 
          // some fraction of the times that we check, so we just continue on and try again
          continue;
        }
        ++receivedCount;

        std::vector<TriggerCandidate> tcs;
        this->operator()(activ,tcs);
        
        std::string oss_prog = "Activity received #"+std::to_string(receivedCount);
        ers::debug(dunedaq::dunetrigger::ProgressUpdate(ERS_HERE, get_name(), oss_prog));
        for (auto const& tc: tcs) {
          std::cout << "\033[35mtc.time_start : " << tc.time_start << "\033[0m  ";
          std::cout << "\033[35mtc.time_end : "   << tc.time_end << "\033[0m\n";
        }
        while(tcs.size()) {
          bool successfullyWasSent = false;
          while (!successfullyWasSent && running_flag.load()) {
            try {
              outputQueue_->push(tcs.back(), queueTimeout_);
              tcs.pop_back();
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
               << " TCs and successfully sent " << sentCount << " TCs. ";
      ers::info(dunedaq::dunetrigger::ProgressUpdate(ERS_HERE, get_name(), oss_summ.str()));
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_work() method";
    }


  } // namespace trigger
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::trigger::DAQTriggerCandidateMaker)

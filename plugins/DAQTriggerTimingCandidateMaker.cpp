#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/ThreadHelper.hpp"
#include "appfwk/DAQModuleHelper.hpp"

#include "triggermodules/daqtriggercandidatemaker/Nljs.hpp"
#include "dune-trigger-algs/Supernova/TriggerCandidateMaker_Timing.hh"

#include "CommonIssues.hpp"

#include <ers/Issue.h>
#include <ers/ers.h>
#include <TRACE/trace.h>

#include <memory>
#include <string>
#include <vector>
#include <fstream>


using namespace triggeralgs;

namespace dunedaq {
  namespace trigger {

    class DAQTriggerCandidateMaker_Timing: public dunedaq::appfwk::DAQModule, triggeralgs::TriggerCandidateMakerTiming {
    public:
      explicit DAQTriggerCandidateMaker_Timing(const std::string& name);

      DAQTriggerCandidateMaker_Timing(const DAQTriggerCandidateMaker_Timing&) = delete;
      DAQTriggerCandidateMaker_Timing& operator=(const DAQTriggerCandidateMaker_Timing&) = delete;
      DAQTriggerCandidateMaker_Timing(DAQTriggerCandidateMaker_Timing&&) = delete;
      DAQTriggerCandidateMaker_Timing& operator=(DAQTriggerCandidateMaker_Timing&&) = delete;

      //void init() override;
      void init(const nlohmann::json& obj) override;
      
    private:
      void do_configure(const nlohmann::json& obj);
      void do_start(const nlohmann::json& obj);
      void do_stop(const nlohmann::json& obj);
      void do_unconfigure(const nlohmann::json& obj);

      dunedaq::appfwk::ThreadHelper thread_;

      void do_work(std::atomic<bool>&);

      int64_t window;
      uint16_t thresh;
      uint16_t hit_thresh;

      //std::unique_ptr<dunedaq::appfwk::DAQSource<TriggerPrimitive>> inputQueue_;
      using source_t = dunedaq::appfwk::DAQSource<TimeStampedData>;
      std::unique_ptr<source_t> inputQueue_;

      //std::unique_ptr<dunedaq::appfwk::DAQSink<TriggerCandidate>> outputQueue_;
      using sink_t = dunedaq::appfwk::DAQSink<TriggerCandidate>;
      std::unique_ptr<sink_t> outputQueue_;

      std::chrono::milliseconds queueTimeout_;
    };
    

    DAQTriggerCandidateMaker_Timing::DAQTriggerCandidateMaker_Timing(const std::string& name):
        DAQModule(name),
        thread_(std::bind(&DAQTriggerCandidateMaker_Timing::do_work, this, std::placeholders::_1)),
        inputQueue_(nullptr),
        outputQueue_(nullptr),
        queueTimeout_(100) {
      
      register_command("conf",    &DAQTriggerCandidateMaker_Timing::do_configure);
      register_command("start"    , &DAQTriggerCandidateMaker_Timing::do_start    );
      register_command("stop"     , &DAQTriggerCandidateMaker_Timing::do_stop     );
      register_command("scrap"      , &DAQTriggerCandidateMaker_Timing::do_unconfigure);
    }

    void DAQTriggerCandidateMaker_Timing::init(const nlohmann::json& iniobj) {
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

    void DAQTriggerCandidateMaker_Timing::do_configure(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_configure() method";
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_configure() method";
    }



    void DAQTriggerCandidateMaker_Timing::do_start(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_start() method";
      thread_.start_working_thread();
      ERS_LOG(get_name() << " successfully started");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_start() method";
    }

    void DAQTriggerCandidateMaker_Timing::do_stop(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_stop() method";
      thread_.stop_working_thread();
      //flush()
      ERS_LOG(get_name() << " successfully stopped");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_stop() method";
    }


    void DAQTriggerCandidateMaker_Timing::do_work(std::atomic<bool>& running_flag) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_work() method";
          std::cout << "\033[35mthreshold : " << thresh << "\033[0m  ";
          std::cout << "\033[35mhit_threshold: "   << hit_thresh << "\033[0m\n";
      int receivedCount = 0;
      int sentCount = 0;
      TimeStampedData data;
      /*std::string outputfilename = "/scratch/tc.out";*/

      while (running_flag.load()) {
        TLOG(TLVL_CANDIDATE) << get_name() << ": Going to receive data from input queue";
        try {
          inputQueue_->pop(data, queueTimeout_);
        } catch (const dunedaq::appfwk::QueueTimeoutExpired& excpt) {
          // it is perfectly reasonable that there might be no data in the queue 
          // some fraction of the times that we check, so we just continue on and try again
          continue;
        }
        ++receivedCount;

        std::vector<TriggerCandidate> tcs;
        this->operator()(data,tcs);
	/*std::ofstream outputfile;
        outputfile.open(outputfilename);*/
        
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
	/*outputfile.close();*/
      }

      std::ostringstream oss_summ;
      oss_summ << ": Exiting do_work() method, received " << receivedCount
               << " TCs and successfully sent " << sentCount << " TCs. ";
      ers::info(dunedaq::dunetrigger::ProgressUpdate(ERS_HERE, get_name(), oss_summ.str()));
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_work() method";
    }

    void DAQTriggerCandidateMaker_Timing::do_unconfigure(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_unconfigure() method";
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_unconfigure() method";
    }


  } // namespace trigger
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::trigger::DAQTriggerCandidateMaker_Timing)

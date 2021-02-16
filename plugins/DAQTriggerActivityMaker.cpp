#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/ThreadHelper.hpp"
#include "appfwk/DAQModuleHelper.hpp"

#include "triggermodules/daqtriggeractivitymaker/Nljs.hpp"
#include "dune-trigger-algs/Supernova/TriggerActivityMaker_Supernova.hh"

#include "CommonIssues.hpp"

#include <ers/Issue.h>
#include <ers/ers.h>
#include <TRACE/trace.h>

#include <memory>
#include <string>
#include <vector>


using namespace triggeralgs;

namespace dunedaq {
  namespace trigger {

    class DAQTriggerActivityMaker: public dunedaq::appfwk::DAQModule, triggeralgs::TriggerActivityMakerSupernova {
    public:
      explicit DAQTriggerActivityMaker(const std::string& name);

      DAQTriggerActivityMaker(const DAQTriggerActivityMaker&) = delete;
      DAQTriggerActivityMaker& operator=(const DAQTriggerActivityMaker&) = delete;
      DAQTriggerActivityMaker(DAQTriggerActivityMaker&&) = delete;
      DAQTriggerActivityMaker& operator=(DAQTriggerActivityMaker&&) = delete;

      void init(const nlohmann::json& obj) override;
      
    private:
      void do_start    (const nlohmann::json& obj);
      void do_stop     (const nlohmann::json& obj);
      void do_configure(const nlohmann::json& obj);

      dunedaq::appfwk::ThreadHelper thread_;
      void do_work(std::atomic<bool>&);

      int64_t time_tolerance;
      int32_t channel_tolerance;

      //std::unique_ptr<dunedaq::appfwk::DAQSource<TriggerPrimitive>> inputQueue_;
      using source_t = dunedaq::appfwk::DAQSource<TriggerPrimitive>;
      std::unique_ptr<source_t> inputQueue_;

      //std::unique_ptr<dunedaq::appfwk::DAQSink<TriggerActivity>> outputQueue_;
      using sink_t = dunedaq::appfwk::DAQSink<TriggerActivity>;
      std::unique_ptr<sink_t> outputQueue_;

      std::chrono::milliseconds queueTimeout_;
    };
    

    DAQTriggerActivityMaker::DAQTriggerActivityMaker(const std::string& name):
        DAQModule(name),
        thread_(std::bind(&DAQTriggerActivityMaker::do_work, this, std::placeholders::_1)),
        inputQueue_(nullptr),
        outputQueue_(nullptr),
        queueTimeout_(100) {
      
      register_command("start"    , &DAQTriggerActivityMaker::do_start    );
      register_command("stop"     , &DAQTriggerActivityMaker::do_stop     );
      register_command("conf", &DAQTriggerActivityMaker::do_configure);
    }

    void DAQTriggerActivityMaker::init(const nlohmann::json& iniobj) {
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

    void DAQTriggerActivityMaker::do_start(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_start() method";
      thread_.start_working_thread();
      ERS_LOG(get_name() << " successfully started");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_start() method";
    }

    void DAQTriggerActivityMaker::do_stop(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_stop() method";
      thread_.stop_working_thread();
      //flush()
      ERS_LOG(get_name() << " successfully stopped");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_stop() method";
    }

    void DAQTriggerActivityMaker::do_configure(const nlohmann::json& config/*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_configure() method";
      auto params = config.get<dunedaq::triggermodules::daqtriggeractivitymaker::Conf>();
      
      time_tolerance = params.time_tolerance;
      channel_tolerance = params.channel_tolerance;
      try {
        m_time_tolerance = time_tolerance;
        m_channel_tolerance = channel_tolerance;
      } catch(...)  {
        ERS_LOG(get_name() << " unsuccessfully configured");
      }
      ERS_LOG(get_name() << " successfully configured");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_configure() method";
    }



    void DAQTriggerActivityMaker::do_work(std::atomic<bool>& running_flag) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_work() method";
      int receivedCount = 0;
      int sentCount = 0;
      TriggerPrimitive prim;

      while (running_flag.load()) {
        TLOG(TLVL_CANDIDATE) << get_name() << ": Going to receive data from input queue";
        try {
          inputQueue_->pop(prim, queueTimeout_);
        } catch (const dunedaq::appfwk::QueueTimeoutExpired& excpt) {
          // it is perfectly reasonable that there might be no data in the queue 
          // some fraction of the times that we check, so we just continue on and try again
          continue;
        }
        ++receivedCount;

        std::vector<TriggerActivity> tas;
        this->operator()(prim,tas);
        
        std::string oss_prog = "Clustered prim #"+std::to_string(receivedCount);
        ers::debug(dunedaq::dunetrigger::ProgressUpdate(ERS_HERE, get_name(), oss_prog));
        for (auto const& ta: tas) {
          std::cout << "\033[34mta.time_start : "     << ta.time_start     << "\033[0m  ";
          std::cout << "\033[34mta.channel_start : "  << ta.channel_start  << "\033[0m  -> ";
          std::cout << "\033[34mta.channel_end : "    << ta.channel_end    << "\033[0m ";
          std::cout << "\033[34mta.tp_list.size() : " << ta.tp_list.size() << "\033[0m ";
        }
        while(tas.size()) {
          bool successfullyWasSent = false;
          while (!successfullyWasSent && running_flag.load()) {
            try {
              outputQueue_->push(tas.back(), queueTimeout_);
              tas.pop_back();
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
               << " TPs and successfully sent " << sentCount << " TAs. ";
      ers::info(dunedaq::dunetrigger::ProgressUpdate(ERS_HERE, get_name(), oss_summ.str()));
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_work() method";
    }


  } // namespace trigger
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::trigger::DAQTriggerActivityMaker)

#include "appfwk/ThreadHelper.hpp"
#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/DAQModuleHelper.hpp"
#include "dune-trigger-algs/TriggerDecision.hh"
#include "triggermodules/fakedfo/Nljs.hpp"
#include "ers/ers.h"

#include "CommonIssues.hpp"

#include "appfwk/cmd/Nljs.hpp"

#include <TRACE/trace.h>
#include <chrono>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>




using pd_clock = std::chrono::duration<double, std::ratio<1, 50000000>>;
using namespace triggeralgs;

namespace dunedaq {
  namespace triggermodules {

    class FakeDFO: public dunedaq::appfwk::DAQModule {
    public:
      /**
       * @brief RandomDataListGenerator Constructor
       * @param name Instance name for this RandomDataListGenerator instance
       */
      explicit FakeDFO(const std::string& name);

      FakeDFO(const FakeDFO&) =
        delete; ///< FakeDFO is not copy-constructible
      FakeDFO& operator=(const FakeDFO&) =
        delete; ///< FakeDFO is not copy-assignable
      FakeDFO(FakeDFO&&) =
        delete; ///< FakeDFO is not move-constructible
      FakeDFO& operator=(FakeDFO&&) =
        delete; ///< FakeDFO is not move-assignable

      void init(const nlohmann::json& obj) override;

    private:
      // Commands
      void do_configure  (const nlohmann::json& obj);
      void do_start      (const nlohmann::json& obj);
      void do_stop       (const nlohmann::json& obj);
      void do_unconfigure(const nlohmann::json& obj);

      // Threading
      dunedaq::appfwk::ThreadHelper thread_;
      void do_work(std::atomic<bool>&);

      // Read csv file
      void  WriteFile(const std::string filename, TriggerDecision td, uint32_t timestamp_now);
      std::string filename;

      // Generation

      // Configuration
      using source_t = dunedaq::appfwk::DAQSource<TriggerDecision>;
      std::unique_ptr<source_t> inputQueue_;

      std::chrono::milliseconds queueTimeout_;

    };

    FakeDFO::FakeDFO(const std::string& name) :
      dunedaq::appfwk::DAQModule(name),
      thread_(std::bind(&FakeDFO::do_work, this, std::placeholders::_1)),
      inputQueue_(nullptr),
      queueTimeout_(100)
      {
      register_command("conf"       , &FakeDFO::do_configure  );
      register_command("start"      , &FakeDFO::do_start      );
      register_command("stop"       , &FakeDFO::do_stop       );
      register_command("scrap"      , &FakeDFO::do_unconfigure);
    }

   void FakeDFO::WriteFile(const std::string filename, TriggerDecision td, uint32_t timestamp_now) {
	std::vector<std::vector<int64_t>> tps_vector;
        std::ofstream sink(filename);
        if(!sink.is_open()) throw std::runtime_error("Could not open file");

        sink << "\033[36mTimestamp now : "     << timestamp_now << "\033[0m  ";
        sink << "\033[36mtd.algorithm: "     << td.algorithm  << "\033[0m  ";
        sink << "\033[36mtd.time_start : " << td.time_start << "\033[0m  ";
        sink << "\033[36mtd.time_end : "   << td.time_end   << "\033[0m\n";
	for (auto const& tc: td.tc_list) {
	  sink << "\033[36mtc.: "   << tc.algorithm<< "\033[0m\n";
	    for (auto const& ta: tc.ta_list) {
	      sink << "\033[36mta.: "   << ta.algorithm<< "\033[0m\n";
		for (auto const& tp: ta.tp_list) {
		  sink << "\033[36mtp.: "   << tp.flag << "\033[0m\n";
		  sink << "\033[36mtp.time_start: "   << tp.time_start<< "\033[0m\n";
		  sink << "\033[36mtp.channel: "   << tp.channel << "\033[0m\n";
		}
	    }
	}
        sink.close();
     }

    void FakeDFO::init(const nlohmann::json& iniobj) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering init() method";
      auto qi = appfwk::qindex(iniobj, {"input"});
      try {
        inputQueue_.reset(new source_t(qi["input"].inst));
      } catch (const ers::Issue& excpt) {
        throw dunedaq::dunetrigger::InvalidQueueFatalError(ERS_HERE, get_name(), "input", excpt);
      }
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting init() method";
    }

    void FakeDFO::do_configure(const nlohmann::json& config /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_configure() method";
      auto params = config.get<dunedaq::triggermodules::fakedfo::Conf>();
      filename = params.filename;

      // Check if file is loaded
      // std::ifstream src(filename);
      // if(!src.is_open()) throw InvalidConfiguration(ERS_HERE);
	      // src.close();
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_configure() method";
    }

    void FakeDFO::do_start(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_start() method";
      thread_.start_working_thread();
      ERS_LOG(get_name() << " successfully started");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_start() method";
    }

    void FakeDFO::do_stop(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_stop() method";
      thread_.stop_working_thread();
      ERS_LOG(get_name() << " successfully stopped");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_stop() method";
    }

    void FakeDFO::do_unconfigure(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_unconfigure() method";
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_unconfigure() method";
    }

    void FakeDFO::do_work(std::atomic<bool>& running_flag) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_work() method";
      int receivedCount = 0;
      int sentCount = 0;
      TriggerDecision dec;

      while (running_flag.load()) {
        try {
          inputQueue_->pop(dec, queueTimeout_);
        } catch (const dunedaq::appfwk::QueueTimeoutExpired& excpt) {
          // it is perfectly reasonable that there might be no data in the queue 
          // some fraction of the times that we check, so we just continue on and try again
          continue;
        }
        ++receivedCount;

        std::string oss_prog = "Dec received #"+std::to_string(receivedCount);
        ers::debug(dunedaq::dunetrigger::ProgressUpdate(ERS_HERE, get_name(), oss_prog));
        auto now = std::chrono::steady_clock::now();
        uint32_t timestamp_now = (uint32_t)pd_clock(now.time_since_epoch()).count();
	FakeDFO::WriteFile(filename,dec,timestamp_now);
        std::cout << "\033[36mTimestamp now : "     << timestamp_now << "\033[0m  ";
	
      }

      std::ostringstream oss_summ;
      oss_summ << ": Exiting do_work() method, received " << receivedCount;
      ers::info(dunedaq::dunetrigger::ProgressUpdate(ERS_HERE, get_name(), oss_summ.str()));
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_work() method";
    }


  } // namespace triggermodules
  
} // namespace dunedaq
DEFINE_DUNE_DAQ_MODULE(dunedaq::triggermodules::FakeDFO)


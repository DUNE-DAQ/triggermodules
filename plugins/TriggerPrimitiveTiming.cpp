#include "appfwk/ThreadHelper.hpp"
#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "dune-trigger-algs/TimingMessage.hh"
#include "dune-trigger-algs/TriggerCandidate.hh"

#include "CommonIssues.hpp"

#include "appfwk/cmd/Nljs.hpp"

#include <chrono>
#include <random>

using pd_clock = std::chrono::duration<double, std::ratio<1, 50000000>>;
using namespace triggeralgs;

namespace dunedaq {
  namespace toy_generator {

    class TriggerPrimitiveTiming: public dunedaq::appfwk::DAQModule {
    public:
      /**
       * @brief RandomDataListGenerator Constructor
       * @param name Instance name for this RandomDataListGenerator instance
       */
      explicit TriggerPrimitiveTiming(const std::string& name);

      TriggerPrimitiveTiming(const TriggerPrimitiveTiming&) =
        delete; ///< TriggerPrimitiveTiming is not copy-constructible
      TriggerPrimitiveTiming& operator=(const TriggerPrimitiveTiming&) =
        delete; ///< TriggerPrimitiveTiming is not copy-assignable
      TriggerPrimitiveTiming(TriggerPrimitiveTiming&&) =
        delete; ///< TriggerPrimitiveTiming is not move-constructible
      TriggerPrimitiveTiming& operator=(TriggerPrimitiveTiming&&) =
        delete; ///< TriggerPrimitiveTiming is not move-assignable

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

      // Generation
      std::vector<TimingMessage> GetTimestamp();

      // Configuration
      //std::unique_ptr<dunedaq::appfwk::DAQSink<TriggerPrimitive>> outputQueue_;
      using sink_t = dunedaq::appfwk::DAQSink<TimingMessage>;
      std::unique_ptr<sink_t> outputQueue_;

      std::chrono::milliseconds queueTimeout_;

    
      // Generation
      std::default_random_engine generator;
      std::uniform_int_distribution<int>     rdm_signaltype             = std::uniform_int_distribution<int>    (0, 2);
      std::uniform_int_distribution<int>     rdm_nhit                = std::uniform_int_distribution<int>    (0, 3);
    };

    
    TriggerPrimitiveTiming::TriggerPrimitiveTiming(const std::string& name) :
      dunedaq::appfwk::DAQModule(name),
      thread_(std::bind(&TriggerPrimitiveTiming::do_work, this, std::placeholders::_1)),
      outputQueue_(),
      queueTimeout_(100),
      generator(){
      register_command("config"  , &TriggerPrimitiveTiming::do_configure  );
      register_command("start"      , &TriggerPrimitiveTiming::do_start      );
      register_command("stop"       , &TriggerPrimitiveTiming::do_stop       );
      register_command("scratch" , &TriggerPrimitiveTiming::do_unconfigure);
    }

    void TriggerPrimitiveTiming::init(const nlohmann::json& init_data) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering init() method";
      auto ini = init_data.get<appfwk::cmd::ModInit>();
      for (const auto& qi : ini.qinfos) {
        if (qi.dir != "output") {
          continue;                 // skip all but "output" direction
        }
        try
	{
	  //outputQueue_.emplace_back(new sink_t(qi.inst));
	  outputQueue_.reset(new sink_t(qi.inst));
	}
        catch (const ers::Issue& excpt)
	{
	  throw dunedaq::dunetrigger::InvalidQueueFatalError(ERS_HERE, get_name(), qi.name, excpt);
	}
      }
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting init() method";
    }

    void TriggerPrimitiveTiming::do_configure(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_configure() method";
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_configure() method";
    }

    void TriggerPrimitiveTiming::do_start(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_start() method";
      thread_.start_working_thread();
      ERS_LOG(get_name() << " successfully started");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_start() method";
    }

    void TriggerPrimitiveTiming::do_stop(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_stop() method";
      thread_.stop_working_thread();
      ERS_LOG(get_name() << " successfully stopped");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_stop() method";
    }

    void TriggerPrimitiveTiming::do_unconfigure(const nlohmann::json& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_unconfigure() method";
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_unconfigure() method";
    }

    std::vector<TimingMessage> TriggerPrimitiveTiming::GetTimestamp() {
      std::vector<TimingMessage> tms;
      auto nhit = rdm_nhit(generator);
      int signaltype = rdm_signaltype(generator);
      for (int i=0; i<nhit; ++i) {
        TimingMessage tm{};

        auto tm_start_time = std::chrono::steady_clock::now();
        tm.time_stamp = (uint64_t)pd_clock(tm_start_time.time_since_epoch()).count();
        std::cout << "\033[32mtm.timestamp: " << tm.time_stamp << "\033[0m  ";
        tm.signal_type = signaltype;
        //std::cout << "\033[32m" << tm.time_stamp << ","<< tm.signal_type << ","<< tm.counter << "\033[0m\n";
        auto now = std::chrono::steady_clock::now();
        tm.counter = (uint32_t)pd_clock(now.time_since_epoch()).count();
        tms.push_back(tm);
      }
      return tms;
    }
    
    void TriggerPrimitiveTiming::do_work(std::atomic<bool>& running_flag) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_work() method";
      size_t generatedCount = 0;
      size_t sentCount = 0;

      while (running_flag.load()) {
        TLOG(TLVL_GENERATION) << get_name() << ": Start of sleep between sends";
        std::this_thread::sleep_for(std::chrono::nanoseconds(1000000000));

        std::vector<TimingMessage> tms = GetTimestamp();

        if (tms.size() == 0) {
          std::ostringstream oss_prog;
          oss_prog << "Last TMs packet has size 0, continuing!";
          ers::debug(dunedaq::dunetrigger::ProgressUpdate(ERS_HERE, get_name(), oss_prog.str()));
          continue; 
        } else {
          std::ostringstream oss_prog;
          ers::debug(dunedaq::dunetrigger::ProgressUpdate(ERS_HERE, get_name(), oss_prog.str()));
        }

        generatedCount+=tms.size();
        
        std::string thisQueueName = outputQueue_->get_name();
        TLOG(TLVL_GENERATION) << get_name() << ": Pushing list onto the outputQueue: " << thisQueueName;

        bool successfullyWasSent = false;
        while (!successfullyWasSent && running_flag.load()) {
          TLOG(TLVL_GENERATION) << get_name() << ": Pushing the generated list onto queue " << thisQueueName;

          for (auto const& tm: tms) {
            try {
              outputQueue_->push(tm, queueTimeout_);
              successfullyWasSent = true;
              ++sentCount;
            } catch (const dunedaq::appfwk::QueueTimeoutExpired& excpt) {
              std::ostringstream oss_warn;
              oss_warn << "push to output queue \"" << thisQueueName << "\"";
              ers::warning(dunedaq::appfwk::QueueTimeoutExpired(ERS_HERE, get_name(), oss_warn.str(),
                                                                std::chrono::duration_cast<std::chrono::milliseconds>(queueTimeout_).count()));
            }
          }
        }
        
        std::ostringstream oss_prog2;
        oss_prog2 << "Sent generated Argon 39 hits # " << generatedCount;
        ers::debug(dunedaq::dunetrigger::ProgressUpdate(ERS_HERE, get_name(), oss_prog2.str()));
        
        ERS_LOG(get_name() << " end of while loop");
      }

      std::ostringstream oss_summ;
      oss_summ << ": Exiting the do_work() method, generated " << generatedCount
               << " TM set and successfully sent " << sentCount << " copies. ";
      ers::info(dunedaq::dunetrigger::ProgressUpdate(ERS_HERE, get_name(), oss_summ.str()));
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_work() method";
    }

  } // namespace toy_generator
  
} // namespace dunedaq
DEFINE_DUNE_DAQ_MODULE(dunedaq::toy_generator::TriggerPrimitiveTiming)


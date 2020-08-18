#include "appfwk/ThreadHelper.hpp"
#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "dune-trigger-algs/TriggerPrimitive.hh"

#include "CommonIssues.hpp"

#include <chrono>
#include <random>

using pd_clock = std::chrono::duration<double, std::ratio<1, 50000000>>;
using namespace DuneTriggerAlgs;

namespace dunedaq {
  namespace toy_generator {

    class TriggerPrimitiveSupernova: public dunedaq::appfwk::DAQModule {
    public:
      /**
       * @brief RandomDataListGenerator Constructor
       * @param name Instance name for this RandomDataListGenerator instance
       */
      explicit TriggerPrimitiveSupernova(const std::string& name);

      TriggerPrimitiveSupernova(const TriggerPrimitiveSupernova&) =
        delete; ///< TriggerPrimitiveSupernova is not copy-constructible
      TriggerPrimitiveSupernova& operator=(const TriggerPrimitiveSupernova&) =
        delete; ///< TriggerPrimitiveSupernova is not copy-assignable
      TriggerPrimitiveSupernova(TriggerPrimitiveSupernova&&) =
        delete; ///< TriggerPrimitiveSupernova is not move-constructible
      TriggerPrimitiveSupernova& operator=(TriggerPrimitiveSupernova&&) =
        delete; ///< TriggerPrimitiveSupernova is not move-assignable

      void init() override;

    private:
      // Commands
      void do_configure  (const std::vector<std::string>& args);
      void do_start      (const std::vector<std::string>& args);
      void do_stop       (const std::vector<std::string>& args);
      void do_unconfigure(const std::vector<std::string>& args);

      // Threading
      dunedaq::appfwk::ThreadHelper thread_;
      void do_work(std::atomic<bool>&);

      // Generation
      std::vector<TriggerPrimitive> GetSupernovaEvt();

      // Configuration
      std::unique_ptr<dunedaq::appfwk::DAQSink<TriggerPrimitive>> outputQueue_;
      std::chrono::milliseconds queueTimeout_;

    
      // Generation
      std::default_random_engine generator;
      std::uniform_real_distribution<double> rdm_probability         = std::uniform_real_distribution<double>(0, 1);
      std::uniform_int_distribution<int>     rdm_nevt                = std::uniform_int_distribution<int>    (4, 50);
      std::uniform_int_distribution<int>     rdm_channel             = std::uniform_int_distribution<int>    (0, 2560);
      std::uniform_int_distribution<int>     rdm_nhit                = std::uniform_int_distribution<int>    (0, 10);
      std::normal_distribution<double>       rdm_adc                 = std::normal_distribution<double>      (20, 5);
      std::normal_distribution<double>       rdm_time_over_threshold = std::normal_distribution<double>      (100, 20); // nanosec
      std::normal_distribution<double>       rdm_start_time          = std::normal_distribution<double>      (0, 20); // nanosec
      std::uniform_real_distribution<double> rdm_peak_time           = std::uniform_real_distribution<double>(0,1);

      int m_n_supernova_evt;
    };

    
    TriggerPrimitiveSupernova::TriggerPrimitiveSupernova(const std::string& name) :
      dunedaq::appfwk::DAQModule(name),
      thread_(std::bind(&TriggerPrimitiveSupernova::do_work, this, std::placeholders::_1)),
      outputQueue_(),
      queueTimeout_(100),
      generator(){
      register_command("configure"  , &TriggerPrimitiveSupernova::do_configure  );
      register_command("start"      , &TriggerPrimitiveSupernova::do_start      );
      register_command("stop"       , &TriggerPrimitiveSupernova::do_stop       );
      register_command("unconfigure", &TriggerPrimitiveSupernova::do_unconfigure);
    }

    void TriggerPrimitiveSupernova::init() {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering init() method";
      for (auto& output : get_config()["output"]) {
        try {
          outputQueue_.reset(new dunedaq::appfwk::DAQSink<TriggerPrimitive>(output.get<std::string>()));
        } catch (const ers::Issue& excpt) {
          throw dunedaq::dunetrigger::InvalidQueueFatalError(ERS_HERE, get_name(), output.get<std::string>(), excpt);
        }
      }
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting init() method";
    }

    void TriggerPrimitiveSupernova::do_configure(const std::vector<std::string>& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_configure() method";
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_configure() method";
    }

    void TriggerPrimitiveSupernova::do_start(const std::vector<std::string>& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_start() method";
      thread_.start_working_thread();
      ERS_LOG(get_name() << " successfully started");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_start() method";
    }

    void TriggerPrimitiveSupernova::do_stop(const std::vector<std::string>& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_stop() method";
      thread_.stop_working_thread();
      ERS_LOG(get_name() << " successfully stopped");
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_stop() method";
    }

    void TriggerPrimitiveSupernova::do_unconfigure(const std::vector<std::string>& /*args*/) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_unconfigure() method";
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_unconfigure() method";
    }

    std::vector<TriggerPrimitive> TriggerPrimitiveSupernova::GetSupernovaEvt() {
      std::vector<TriggerPrimitive> tps;
      if (m_n_supernova_evt == 0) {
        double prob = rdm_probability(generator);
        if (prob > 0.05) {
          return tps;
        } else {
          std::cout << "\033[31mSUPERNOVAAAAAA!\033[0m\n";
          m_n_supernova_evt = rdm_nevt(generator);
          std::cout << "\033[31mn_evt : " << m_n_supernova_evt << "\033[0m\n";
        }
      }
      auto nhit = rdm_nhit(generator);
      auto central_channel = rdm_channel(generator);
      for (int i=0; i<nhit; ++i) {
        TriggerPrimitive tp{};

        double tot  = rdm_time_over_threshold(generator);
        double peak = rdm_peak_time(generator)*tot;

        std::chrono::nanoseconds tot_time((int)tot);
        std::chrono::nanoseconds peak_time((int)peak);

        auto tp_start_time = std::chrono::steady_clock::now();
        tp.time_start          = pd_clock(tp_start_time.time_since_epoch()).count();
        std::cout << "\033[31mtp.time_start : " << tp.time_start << "\033[0m  ";
        tp.time_over_threshold = pd_clock(tot_time).count();
        tp.time_peak           = pd_clock((tp_start_time+peak_time).time_since_epoch()).count();
        tp.channel             = central_channel+i;
        std::cout << "\033[31mtp.channel : " << tp.channel << "\033[0m\n";
        tp.time_over_threshold = pd_clock(tot_time).count();
        tp.adc_integral        = rdm_adc(generator);
        tp.adc_peak            = rdm_adc(generator);
        tp.detid               = tp.channel;
        tps.push_back(tp);
      }
      m_n_supernova_evt--;
      return tps;
    }
    
    void TriggerPrimitiveSupernova::do_work(std::atomic<bool>& running_flag) {
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_work() method";
      size_t generatedCount = 0;
      size_t sentCount = 0;

      while (running_flag.load()) {
        TLOG(TLVL_GENERATION) << get_name() << ": Start of sleep between sends";
        std::this_thread::sleep_for(std::chrono::nanoseconds(1000000000));

        std::vector<TriggerPrimitive> tps = GetSupernovaEvt();

        if (tps.size() == 0) {
          std::ostringstream oss_prog;
          oss_prog << "Last TPs packet has size 0, continuing!";
          ers::debug(dunedaq::dunetrigger::ProgressUpdate(ERS_HERE, get_name(), oss_prog.str()));
          continue; 
        } else {
          std::ostringstream oss_prog;
          oss_prog << "Generated Supernova TPs #" << generatedCount << " last TPs packet has size " << tps.size();
          ers::debug(dunedaq::dunetrigger::ProgressUpdate(ERS_HERE, get_name(), oss_prog.str()));
        }

        generatedCount+=tps.size();
        
        std::string thisQueueName = outputQueue_->get_name();
        TLOG(TLVL_GENERATION) << get_name() << ": Pushing list onto the outputQueue: " << thisQueueName;

        bool successfullyWasSent = false;
        while (!successfullyWasSent && running_flag.load()) {
          TLOG(TLVL_GENERATION) << get_name() << ": Pushing the generated list onto queue " << thisQueueName;

          for (auto const& tp: tps) {
            try {
              outputQueue_->push(tp, queueTimeout_);
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
        oss_prog2 << "Sent generated Supervova hits # " << generatedCount;
        ers::debug(dunedaq::dunetrigger::ProgressUpdate(ERS_HERE, get_name(), oss_prog2.str()));
        
        ERS_LOG(get_name() << " end of while loop");
      }

      std::ostringstream oss_summ;
      oss_summ << ": Exiting the do_work() method, generated " << generatedCount
               << " TP set and successfully sent " << sentCount << " copies. ";
      ers::info(dunedaq::dunetrigger::ProgressUpdate(ERS_HERE, get_name(), oss_summ.str()));
      TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_work() method";
    }

  } // namespace toy_generator
  
} // namespace dunedaq
DEFINE_DUNE_DAQ_MODULE(dunedaq::toy_generator::TriggerPrimitiveSupernova)

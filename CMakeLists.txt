cmake_minimum_required(VERSION 3.12)
project(DAQDuneTriggers VERSION 2.0.0)

#set(DuneTriggerAlgs_DIR ${DUNETRIGGERALGS_DIR})
set(DuneTriggerAlgs_DIR /dune/app/users/tbezerra/dunetpc/DuneTriggerAlgs_thiago/DuneTriggerAlgs/install/)

find_package(daq-cmake REQUIRED )

daq_setup_environment()

find_package(appfwk REQUIRED)
find_package(DuneTriggerAlgs REQUIRED)

##############################################################################

#add_library(DAQDuneTriggers_TriggerPrimitiveRadiological_duneDAQModule src/TriggerPrimitiveRadiological.cpp)
#target_link_libraries(DAQDuneTriggers_TriggerPrimitiveRadiological_duneDAQModule appfwk::appfwk)
#target_include_directories(DAQDuneTriggers_TriggerPrimitiveRadiological_duneDAQModule PUBLIC ${DUNETRIGGERALGS_INCLUDES})

daq_add_plugin(TriggerPrimitiveRadiological duneDAQModule LINK_LIBRARIES appfwk::appfwk)
target_include_directories(DAQDuneTriggers_TriggerPrimitiveRadiological_duneDAQModule PUBLIC ${DUNETRIGGERALGS_INCLUDES})

#daq_add_library(TriggerPrimitiveRadiological.cpp LINK_LIBRARIES DuneTriggerAlgs::DuneTriggerAlgs appfwk::appfwk)


#add_library(DAQDuneTriggers_TriggerPrimitiveSupernova_duneDAQModule src/TriggerPrimitiveSupernova.cpp)
#target_link_libraries(DAQDuneTriggers_TriggerPrimitiveSupernova_duneDAQModule appfwk::appfwk)
#target_include_directories(DAQDuneTriggers_TriggerPrimitiveSupernova_duneDAQModule PUBLIC ${DUNETRIGGERALGS_INCLUDES})

daq_add_plugin(TriggerPrimitiveSupernova duneDAQModule LINK_LIBRARIES appfwk::appfwk)
target_include_directories(DAQDuneTriggers_TriggerPrimitiveSupernova_duneDAQModule PUBLIC ${DUNETRIGGERALGS_INCLUDES})

#add_library(DAQDuneTriggers_DAQTriggerActivityMaker_duneDAQModule src/DAQTriggerActivityMaker.cpp)
#target_link_libraries(DAQDuneTriggers_DAQTriggerActivityMaker_duneDAQModule appfwk::appfwk)
#target_link_libraries(DAQDuneTriggers_DAQTriggerActivityMaker_duneDAQModule ${DUNETRIGGERALGS_LIBRARIES}/libSupernovaTrigger.so)
#target_include_directories(DAQDuneTriggers_DAQTriggerActivityMaker_duneDAQModule PUBLIC ${DUNETRIGGERALGS_INCLUDES})

daq_add_plugin(DAQTriggerActivityMaker duneDAQModule LINK_LIBRARIES appfwk::appfwk)
target_link_libraries(DAQDuneTriggers_DAQTriggerActivityMaker_duneDAQModule ${DUNETRIGGERALGS_LIBRARIES}/libSupernovaTrigger.so)
target_include_directories(DAQDuneTriggers_DAQTriggerActivityMaker_duneDAQModule PUBLIC ${DUNETRIGGERALGS_INCLUDES})

#add_library(DAQDuneTriggers_DAQTriggerCandidateMaker_duneDAQModule src/DAQTriggerCandidateMaker.cpp)
#target_link_libraries(DAQDuneTriggers_DAQTriggerCandidateMaker_duneDAQModule appfwk::appfwk)
#target_link_libraries(DAQDuneTriggers_DAQTriggerCandidateMaker_duneDAQModule ${DUNETRIGGERALGS_LIBRARIES}/libSupernovaTrigger.so)
#target_include_directories(DAQDuneTriggers_DAQTriggerCandidateMaker_duneDAQModule PUBLIC ${DUNETRIGGERALGS_INCLUDES})

daq_add_plugin(DAQTriggerCandidateMaker duneDAQModule LINK_LIBRARIES appfwk::appfwk)
target_link_libraries(DAQDuneTriggers_DAQTriggerCandidateMaker_duneDAQModule ${DUNETRIGGERALGS_LIBRARIES}/libSupernovaTrigger.so)
target_include_directories(DAQDuneTriggers_DAQTriggerCandidateMaker_duneDAQModule PUBLIC ${DUNETRIGGERALGS_INCLUDES})

##############################################################################


daq_install()

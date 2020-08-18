cmake_minimum_required(VERSION 3.12)
project(DAQDuneTriggers VERSION 1.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../daq-buildtools/cmake ${CMAKE_MODULE_PATH})

set(DuneTriggerAlgs_DIR /dune/app/users/plasorak/framework/dt/)
include(DAQ)

daq_setup_environment()

find_package(appfwk REQUIRED)
find_package(DuneTriggerAlgs REQUIRED)

##############################################################################
daq_point_build_to( src )

add_library(DAQDuneTriggers_TriggerPrimitiveRadiological_duneDAQModule src/TriggerPrimitiveRadiological.cpp)
target_link_libraries(DAQDuneTriggers_TriggerPrimitiveRadiological_duneDAQModule appfwk::appfwk)
target_include_directories(DAQDuneTriggers_TriggerPrimitiveRadiological_duneDAQModule PUBLIC ${DUNETRIGGERALGS_INCLUDES})

add_library(DAQDuneTriggers_TriggerPrimitiveSupernova_duneDAQModule src/TriggerPrimitiveSupernova.cpp)
target_link_libraries(DAQDuneTriggers_TriggerPrimitiveSupernova_duneDAQModule appfwk::appfwk)
target_include_directories(DAQDuneTriggers_TriggerPrimitiveSupernova_duneDAQModule PUBLIC ${DUNETRIGGERALGS_INCLUDES})

add_library(DAQDuneTriggers_DAQTriggerCandidateMaker_duneDAQModule src/DAQTriggerCandidateMaker.cpp)
target_link_libraries(DAQDuneTriggers_DAQTriggerCandidateMaker_duneDAQModule appfwk::appfwk)
target_link_libraries(DAQDuneTriggers_DAQTriggerCandidateMaker_duneDAQModule ${DUNETRIGGERALGS_LIBRARIES}/libSupernovaTrigger.so)
target_include_directories(DAQDuneTriggers_DAQTriggerCandidateMaker_duneDAQModule PUBLIC ${DUNETRIGGERALGS_INCLUDES})

add_library(DAQDuneTriggers_DAQTriggerDecisionMaker_duneDAQModule src/DAQTriggerDecisionMaker.cpp)
target_link_libraries(DAQDuneTriggers_DAQTriggerDecisionMaker_duneDAQModule appfwk::appfwk)
target_link_libraries(DAQDuneTriggers_DAQTriggerDecisionMaker_duneDAQModule ${DUNETRIGGERALGS_LIBRARIES}/libSupernovaTrigger.so)
target_include_directories(DAQDuneTriggers_DAQTriggerDecisionMaker_duneDAQModule PUBLIC ${DUNETRIGGERALGS_INCLUDES})

##############################################################################
daq_point_build_to( conf )

file(COPY conf/simplest_trigger.json DESTINATION conf)

daq_install(TARGETS
  DAQDuneTriggers_TriggerPrimitiveSupernova_duneDAQModule
  DAQDuneTriggers_TriggerPrimitiveRadiological_duneDAQModule
  DAQDuneTriggers_DAQTriggerCandidateMaker_duneDAQModule
  DAQDuneTriggers_DAQTriggerDecisionMaker_duneDAQModule
  )


cmake_minimum_required(VERSION 3.12)
project(triggermodules VERSION 2.2.0)

# JCF, Dec-15-2020
# At the command line you need to add the installation directory of the triggeralgs package before attempting to build, i.e.
#
# export CMAKE_PREFIX_PATH=/area/where/triggeralgs/is/installed:${CMAKE_PREFIX_PATH}
#

find_package(daq-cmake REQUIRED )

daq_setup_environment()

find_package(appfwk REQUIRED)
find_package(triggeralgs REQUIRED)

##############################################################################

	daq_add_plugin(TriggerPrimitiveRadiological duneDAQModule LINK_LIBRARIES appfwk::appfwk SupernovaTrigger)
	daq_add_plugin(TriggerPrimitiveSupernova    duneDAQModule LINK_LIBRARIES appfwk::appfwk SupernovaTrigger)
	daq_add_plugin(TriggerPrimitiveFromFile	    duneDAQModule SCHEMA LINK_LIBRARIES appfwk::appfwk SupernovaTrigger)
	daq_add_plugin(TriggerPrimitiveHE           duneDAQModule LINK_LIBRARIES appfwk::appfwk SupernovaTrigger)
	daq_add_plugin(DAQTriggerActivityMaker      duneDAQModule SCHEMA LINK_LIBRARIES appfwk::appfwk SupernovaTrigger)
	daq_add_plugin(DAQTriggerCandidateMaker     duneDAQModule SCHEMA LINK_LIBRARIES appfwk::appfwk SupernovaTrigger)
	daq_add_plugin(DAQTriggerDecisionMaker      duneDAQModule SCHEMA LINK_LIBRARIES appfwk::appfwk SupernovaTrigger)

##############################################################################


daq_install()

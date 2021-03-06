/**
 * @file CommonIssues.hpp
 *
 * This file contains the definitions of ERS Issues that are common
 * to two or more of the DAQModules in this package.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#pragma once

#include "appfwk/DAQModule.hpp"
#include <ers/Issue.h>

#include <string>

#define TRACE_NAME "CandidateMaker" // NOLINT
#define TLVL_ENTER_EXIT_METHODS 10
#define TLVL_GENERATION 11
#define TLVL_CANDIDATE 15

namespace dunedaq {

ERS_DECLARE_ISSUE_BASE(dunetrigger,
                       ProgressUpdate,
                       appfwk::GeneralDAQModuleIssue,
                       message,
                       ((std::string)name),
                       ((std::string)message))

ERS_DECLARE_ISSUE_BASE(dunetrigger,
                       InvalidQueueFatalError,
                       appfwk::GeneralDAQModuleIssue,
                       "The " << queueType << " queue was not successfully created.",
                       ((std::string)name),
                       ((std::string)queueType))

} // namespace dunedaq

/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains struct and other type definitions for shema in 
 * namespace dunedaq::DAQDuneTrigger::triggerprimitivefromfile.
 */
#ifndef DUNEDAQ_DAQDUNETRIGGER_TRIGGERPRIMITIVEFROMFILE_STRUCTS_HPP
#define DUNEDAQ_DAQDUNETRIGGER_TRIGGERPRIMITIVEFROMFILE_STRUCTS_HPP

#include <cstdint>

#include <string>

namespace dunedaq::DAQDuneTrigger::triggerprimitivefromfile {

    // @brief File path, file name
    using Path = std::string;

    // @brief TriggerPrimitiveFromFile configuration
    struct Conf {

        // @brief File name of input csv file for trigger primitives
        Path filename = "/tmp/example.csv";
    };

} // namespace dunedaq::DAQDuneTrigger::triggerprimitivefromfile

#endif // DUNEDAQ_DAQDUNETRIGGER_TRIGGERPRIMITIVEFROMFILE_STRUCTS_HPP
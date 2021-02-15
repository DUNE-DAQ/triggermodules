/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains functions struct and other type definitions for shema in 
 * namespace dunedaq::DAQDuneTrigger::triggerprimitivefromfile to be serialized via nlohmann::json.
 */
#ifndef DUNEDAQ_DAQDUNETRIGGER_TRIGGERPRIMITIVEFROMFILE_NLJS_HPP
#define DUNEDAQ_DAQDUNETRIGGER_TRIGGERPRIMITIVEFROMFILE_NLJS_HPP

// My structs
#include "Structs.hpp"


#include <nlohmann/json.hpp>

namespace dunedaq::DAQDuneTrigger::triggerprimitivefromfile {

    using data_t = nlohmann::json;
    
    inline void to_json(data_t& j, const Conf& obj) {
        j["filename"] = obj.filename;
    }
    
    inline void from_json(const data_t& j, Conf& obj) {
        if (j.contains("filename"))
            j.at("filename").get_to(obj.filename);    
    }
    
} // namespace dunedaq::DAQDuneTrigger::triggerprimitivefromfile

#endif // DUNEDAQ_DAQDUNETRIGGER_TRIGGERPRIMITIVEFROMFILE_NLJS_HPP

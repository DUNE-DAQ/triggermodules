local moo = import "moo.jsonnet";
local ns = "dunedaq.triggermodules.daqtriggeractivitymaker";
local s = moo.oschema.schema(ns);

local types = {
    time_t : s.number("time_t", "i8", doc="Time"),
    channel_t : s.number("channel_t", "i4", doc="Channel number"),

    conf: s.record("Conf", [
        s.field("time_tolerance", self.time_t, 250,
                doc="Maximum tolerated time difference between two primitives to form an activity (in 50 MHz clock ticks)"),
        s.field("channel_tolerance", self.channel_t, 2,
                doc="Maximum tolerated channel number difference between two primitives to form an activity"),
    ], doc="Thresholds"),

};

moo.oschema.sort_select(types, ns)

local moo = import "moo.jsonnet";
local ns = "dunedaq.triggermodules.daqtriggercandidatemaker";
local s = moo.oschema.schema(ns);

local types = {
    time_t : s.number("time_t", "i8", doc="Time"),
    threshold_t : s.number("threshold_t", "u2", doc="Threshold number"),

    conf: s.record("Conf", [
	s.field("time_window", self.time_t,500000000,
		doc="Sliding time window to count activities"),
        s.field("threshold", self.threshold_t, 6,
                doc="Minimum number of activities in the time window to issue a trigger"),
        s.field("hit_threshold", self.threshold_t, 3,
                doc="Minimum number of primitives in an activity"),
    ], doc="Thresholds"),

};

moo.oschema.sort_select(types, ns)

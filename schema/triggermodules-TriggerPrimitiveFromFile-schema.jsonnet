local moo = import "moo.jsonnet";
local ns = "dunedaq.DAQDuneTrigger.triggerprimitivefromfile";
local s = moo.oschema.schema(ns);

local types = {
    pathname : s.string("Path", "path", doc="File path, file name"),

    conf: s.record("Conf", [
        s.field("filename", self.pathname, "/tmp/example.csv",
                doc="File name of input csv file for trigger primitives"),
    ], doc="TriggerPrimitiveFromFile configuration"),

};

moo.oschema.sort_select(types, ns)

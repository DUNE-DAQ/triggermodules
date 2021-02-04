local moo = import "moo.jsonnet";
local ns = "dunedaq.DAQDuneTrigger.triggerprimitivefromfile";
local s = moo.oschema.schema(ns);

local types = {
    path : s.string("Path", format="path", doc="File path, file name"),
    conf: s.record("Conf", [
        s.field("filename", self.path, "/scratch/example.csv",
                doc="File name of input csv file for trigger primitives"),
    ], doc="TriggerPrimitiveFromFile configuration"),

};

moo.oschema.sort_select(types, ns)

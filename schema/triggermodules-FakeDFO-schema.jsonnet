local moo = import "moo.jsonnet";
local ns = "dunedaq.triggermodules.fakedfo";
local s = moo.oschema.schema(ns);

local types = {
    pathname : s.string("Path", "path", doc="File path, file name"),

    conf: s.record("Conf", [
        s.field("filename", self.pathname, "/tmp/output.txt",
                doc="File name of output file"),
    ], doc="FakeDFO configuration"),

};

moo.oschema.sort_select(types, ns)

local moo = import "moo.jsonnet";
local cmd = import "sourcecode/appfwk/schema/appfwk-cmd-make.jsonnet";
local TPsGenerator = import "sourcecode/triggermodules/schema/triggermodules-TriggerPrimitiveFromFile-make.jsonnet";

///////////	Queues
local queues = {
	TPsQueue: cmd.qspec("TPsQueue",
		"FollyMPMCQueue",
		1000),
			
	TAsQueue: cmd.qspec("TAsQueue",
		"FollyMPMCQueue",
		100),
		
	TCsQueue: cmd.qspec("TCsQueue",
		"FollyMPMCQueue",
		10),
};



///////////	Conf
[
	cmd.init([queues.TPsQueue],
[cmd.mspec("TPsGenerator",
		"TriggerPrimitiveFromFile",
		[cmd.qinfo("output",
			"TPsQueue",
			cmd.qdir.output)])])
		{ waitms: 1000},

	cmd.conf(	[cmd.mcmd("TPsGenerator",TPsGenerator.conf("/tmp/test1.csv")
			   )])
		{ waitms: 1000},
	
cmd.start(42)
{ waitms: 1000},
	cmd.stop(){ waitms: 1000},
]

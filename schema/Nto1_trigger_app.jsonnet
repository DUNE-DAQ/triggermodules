local moo = import "moo.jsonnet";
local cmd = import "sourcecode/appfwk/schema/appfwk-cmd-make.jsonnet";
local TPsGeneratorFromFileMake = import "sourcecode/triggermodules/schema/triggermodules-TriggerPrimitiveFromFile-make.jsonnet";
local TAsGeneratorMake = import "sourcecode/triggermodules/schema/triggermodules-DAQTriggerActivityMaker-make.jsonnet";
local TCsGeneratorMake = import "sourcecode/triggermodules/schema/triggermodules-DAQTriggerCandidateMaker-make.jsonnet";
local TDsGeneratorMake = import "sourcecode/triggermodules/schema/triggermodules-DAQTriggerDecisionMaker-make.jsonnet";

///////////	Queues
local queues = {
	TPsQueue1: cmd.qspec("TPsQueue1",
		"FollyMPMCQueue",
		1000),

	TPsQueue2: cmd.qspec("TPsQueue2",
		"FollyMPMCQueue",
		1000),
			
	TAsQueue1: cmd.qspec("TAsQueue1",
		"FollyMPMCQueue",
		100),
		
	TAsQueue2: cmd.qspec("TAsQueue2",
		"FollyMPMCQueue",
		100),
		
	TCsQueue: cmd.qspec("TCsQueue",
		"FollyMPMCQueue",
		10),

	TDsQueue: cmd.qspec("TDsQueue",
		"FollyMPMCQueue",
		10),
};

///////////	Modules
local modules = {
	TPsGenerator1a: cmd.mspec("TPsGenerator1a",
		"TriggerPrimitiveRadiological",
		[cmd.qinfo("output",
			"TPsQueue1",
			cmd.qdir.output)]),

	TPsGenerator1b: cmd.mspec("TPsGenerator1b",
		"TriggerPrimitiveFromFile",
		[cmd.qinfo("output",
			"TPsQueue2",
			cmd.qdir.output)]),

	TPsGenerator2a: cmd.mspec("TPsGenerator2a",
		"TriggerPrimitiveRadiological",
		[cmd.qinfo("output",
			"TPsQueue2",
			cmd.qdir.output)]),

	TPsGenerator2b: cmd.mspec("TPsGenerator2b",
		"TriggerPrimitiveSupernova",
		[cmd.qinfo("output",
			"TPsQueue1",
			cmd.qdir.output)]),

	TAsGenerator1: cmd.mspec("TAsGenerator1",
		"DAQTriggerActivityMaker",
		[cmd.qinfo("input",
			"TPsQueue1",
			cmd.qdir.input),
		cmd.qinfo("output",
			"TAsQueue1",
			cmd.qdir.output)]),
	
	TAsGenerator2: cmd.mspec("TAsGenerator2",
		"DAQTriggerActivityMaker",
		[cmd.qinfo("input",
			"TPsQueue2",
			cmd.qdir.input),
		cmd.qinfo("output",
			"TAsQueue2",
			cmd.qdir.output)]),

	TCsGenerator1: cmd.mspec("TCsGenerator1",
		"DAQTriggerCandidateMaker",
		[cmd.qinfo("input",
			"TAsQueue1",
			cmd.qdir.input),
		cmd.qinfo("output",
			"TCsQueue",
			cmd.qdir.output)]),

	TCsGenerator2: cmd.mspec("TCsGenerator2",
		"DAQTriggerCandidateMaker",
		[cmd.qinfo("input",
			"TAsQueue2",
			cmd.qdir.input),
		cmd.qinfo("output",
			"TCsQueue",
			cmd.qdir.output)]),

	TDsGenerator: cmd.mspec("TDsGenerator",
		"DAQTriggerDecisionMaker",
		[cmd.qinfo("input",
			"TCsQueue",
			cmd.qdir.input),
		cmd.qinfo("output",
			"TDsQueue",
			cmd.qdir.output)]),
};


///////////	Conf
[
	cmd.init([	queues.TPsQueue1,queues.TPsQueue2,
			queues.TAsQueue1,queues.TAsQueue2,
			queues.TCsQueue,
			queues.TDsQueue],
		[	modules.TPsGenerator1a,modules.TPsGenerator1b,
			modules.TPsGenerator2a,modules.TPsGenerator2b,
			modules.TAsGenerator1,modules.TAsGenerator2,
			modules.TCsGenerator1,modules.TCsGenerator2,
			modules.TDsGenerator])
		{ waitms: 1000},

	cmd.conf([
		cmd.mcmd("TPsGenerator1a"),
		cmd.mcmd("TPsGenerator1b",TPsGeneratorFromFileMake.conf("/tmp/test2.csv")),
		cmd.mcmd("TPsGenerator2a"),
		cmd.mcmd("TPsGenerator2b"),
		cmd.mcmd("TAsGenerator1",TAsGeneratorMake.conf(250,2)),
		cmd.mcmd("TAsGenerator2",TAsGeneratorMake.conf(250,2)),
		cmd.mcmd("TCsGenerator1",TCsGeneratorMake.conf(500000000,1,1)),
		cmd.mcmd("TCsGenerator2",TCsGeneratorMake.conf(500000000,1,1)),
		cmd.mcmd("TDsGenerator",TDsGeneratorMake.conf(500000000,1,1))
		])
		{ waitms: 1000},

	cmd.start(40){ waitms: 1000},
	cmd.stop(){ waitms: 1000},
]

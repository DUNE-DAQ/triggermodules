local moo = import "moo.jsonnet";
local cmd = import "sourcecode/appfwk/schema/appfwk-cmd-make.jsonnet";

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

///////////	Modules
local modules = {
	TPsGenerator: cmd.mspec("TPsGenerator",
		"TriggerPrimitiveRadiological",
		[cmd.qinfo("output",
			"TPsQueue",
			cmd.qdir.output)]),

	TPsGenerator2: cmd.mspec("TPsGenerator2",
		"TriggerPrimitiveSupernova",
		[cmd.qinfo("output",
			"TPsQueue",
			cmd.qdir.output)]),

	TAsGenerator: cmd.mspec("TAsGenerator",
		"DAQTriggerActivityMaker",
		[cmd.qinfo("input",
			"TPsQueue",
			cmd.qdir.input),
		cmd.qinfo("output",
			"TAsQueue",
			cmd.qdir.output)]),
	
	TCsGenerator: cmd.mspec("TCsGenerator",
		"DAQTriggerCandidateMaker",
		[cmd.qinfo("input",
			"TAsQueue",
			cmd.qdir.input),
		cmd.qinfo("output",
			"TCsQueue",
			cmd.qdir.output)]),
};


///////////	Conf
[
	cmd.init([queues.TPsQueue,queues.TAsQueue,queues.TCsQueue],
		[modules.TPsGenerator, modules.TPsGenerator2, modules.TAsGenerator, modules.TCsGenerator])
		{ waitms: 1000},
//	cmd.start(	["TCsGenerator", "TAsGenerator", "TPsGenerator", "TPsGenerator2"]),
//	cmd.stop(	["TPsGenerator", "TPsGenerator2", "TAsGenerator", "TCsGenerator"]),
	cmd.conf(	[cmd.mcmd("TriggerPrimitiveRadiological"),cmd.mcmd("DAQTriggerActivityMaker"),cmd.mcmd("DAQTriggerCandidateMaker")])
		{ waitms: 1000},//,"TAsGenerator", "TPsGenerator", "TPsGenerator2"]),
	cmd.start(40){ waitms: 1000},
	cmd.stop(){ waitms: 1000},
//	cmd.conf(	["TCsGenerator"]),
]

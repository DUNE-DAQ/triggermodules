local moo = import "moo.jsonnet";
local cmd = import "sourcecode/appfwk/schema/appfwk-cmd-make.jsonnet";

///////////	Queues
local queues = {
	TPsQueue: cmd.qspec("TPsQueue",
		"FollyMPMCQueue",
		1000),

	TPsQueue2: cmd.qspec("TPsQueue2",
		"FollyMPMCQueue",
		1000),
			
	TAsQueue: cmd.qspec("TAsQueue",
		"FollyMPMCQueue",
		100),

	TAsQueue2: cmd.qspec("TAsQueue2",
		"FollyMPMCQueue",
		100),
		
	TCsQueue: cmd.qspec("TCsQueue",
		"FollyMPMCQueue",
		10),

	TCsQueue2: cmd.qspec("TCsQueue2",
		"FollyMPMCQueue",
		10),
};

///////////	Modules
local modules = {
	TPsGenerator: cmd.mspec("TPsGenerator",
		"TriggerPrimitiveRadiological",
		[cmd.qinfo("output",
			"TPsQueue",
			cmd.qdir.output),
		cmd.qinfo("output",
			"TPsQueue2",
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
	
	TAsGenerator2: cmd.mspec("TAsGenerator2",
		"DAQTriggerActivityMaker",
		[cmd.qinfo("input",
			"TPsQueue2",
			cmd.qdir.input),
		cmd.qinfo("output",
			"TAsQueue2",
			cmd.qdir.output)]),

	TCsGenerator: cmd.mspec("TCsGenerator",
		"DAQTriggerCandidateMaker",
		[cmd.qinfo("input",
			"TAsQueue",
			cmd.qdir.input),
		cmd.qinfo("output",
			"TCsQueue",
			cmd.qdir.output)]),

	TCsGenerator2: cmd.mspec("TCsGenerator",
		"DAQTriggerCandidateMaker",
		[cmd.qinfo("input",
			"TAsQueue2",
			cmd.qdir.input),
		cmd.qinfo("output",
			"TCsQueue2",
			cmd.qdir.output)]),
};


///////////	Conf
[
	cmd.init([queues.TPsQueue, queues.TPsQueue2, queues.TAsQueue, queues.TAsQueue2, queues.TCsQueue, queues.TCsQueue2],
		[modules.TPsGenerator, modules.TPsGenerator2, modules.TAsGenerator, modules.TAsGenerator2, modules.TCsGenerator, modules.TCsGenerator2])
		{ waitms: 1000},
//	cmd.start(	["TCsGenerator", "TAsGenerator", "TPsGenerator", "TPsGenerator2"]),
//	cmd.stop(	["TPsGenerator", "TPsGenerator2", "TAsGenerator", "TCsGenerator"]),
	cmd.conf(	[cmd.mcmd("TriggerPrimitiveRadiological"),cmd.mcmd("DAQTriggerActivityMaker"),cmd.mcmd("DAQTriggerCandidateMaker")])
		{ waitms: 1000},//,"TAsGenerator", "TPsGenerator", "TPsGenerator2"]),
	cmd.start(40){ waitms: 1000},
	cmd.stop(){ waitms: 1000},
//	cmd.conf(	["TCsGenerator"]),
]

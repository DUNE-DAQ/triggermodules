local moo = import "moo.jsonnet";
local cmd = import "appfwk-cmd-make.jsonnet";

///////////	Queues
local queues = {
	TPsQueue: cmd.qspec("TPsQueue",
		"FollyMPMQueue",
		1000),
			
	TAsQueue: cmd.qspec("TAsQueue",
		"FollyMPMQueue",
		100),
		
	TCsQueue: cmd.qspec("TCsQueue",
		"FollyMPMQueue",
		10),
};

///////////	Modules
local modules = {
	TPsGenerator: cmd.mspec("TPsGenerator",
		"TriggerPrimitiveRadiological",
		cmd.qinfo("output",
			"TPsQueue",
			cmd.qdir.output)),

	TPsGenerator2: cmd.mspec("TPsGenerator",
		"TriggerPrimitiveSupernova",
		cmd.qinfo("output",
			"TPsQueue",
			cmd.qdir.output)),

	TAsGenerator: cmd.mspec("TAsGenerator",
		"DAQTriggerActivityMaker",
		[cmd.qinfo("input",
			"TPsQueue",
			cmd.qdir.input),
		cmd.qinfo("output",
			"TAsQueue",
			cmd.qdir.output)]),
	
	TCsGenerator: cmd.mspec("TCsGenerator",
		"DAQTriggerCandidateMake",
		[cmd.qinfo("input",
			"TAsQueue",
			cmd.qdir.input),
		cmd.qinfo("output",
			"TCsQueue",
			cmd.qdir.output)]),
};


///////////	Conf
[
	cmd.init(queues,modules),
//	cmd.start(	["TCsGenerator", "TAsGenerator", "TPsGenerator", "TPsGenerator2"]),
//	cmd.stop(	["TPsGenerator", "TPsGenerator2", "TAsGenerator", "TCsGenerator"]),
//	cmd.conf(	["TCsGenerator"]),
]

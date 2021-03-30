local moo = import "moo.jsonnet";
local cmd = import "sourcecode/appfwk/schema/appfwk-cmd-make.jsonnet";
local TDsGeneratorMake = import "sourcecode/triggermodules/schema/triggermodules-DAQTriggerDecisionMaker-make.jsonnet";

///////////	Queues
local queues = {
	TSDsQueue: cmd.qspec("TSDsQueue",
		"FollyMPMCQueue",
		1000),
			
	TCsQueue: cmd.qspec("TCsQueue",
		"FollyMPMCQueue",
		100),

	TDsQueue: cmd.qspec("TDsQueue",
		"FollyMPMCQueue",
		100),
};

///////////	Modules
local modules = {
	TSDsGenerator: cmd.mspec("TSDsGenerator",
		"TriggerPrimitiveTiming",
		[cmd.qinfo("output",
			"TSDsQueue",
			cmd.qdir.output)]),

	
	TimingTCsGenerator: cmd.mspec("TimingTCsGenerator",
		"DAQTriggerTimingCandidateMaker",
		[cmd.qinfo("input",
			"TSDsQueue",
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
	cmd.init([queues.TSDsQueue,queues.TCsQueue,queues.TDsQueue],
		[modules.TSDsGenerator, modules.TimingTCsGenerator,modules.TDsGenerator])
		{ waitms: 1000},
	cmd.conf(
		[cmd.mcmd("TSDsGenerator"),
		cmd.mcmd("TimingTCsGenerator"),
		cmd.mcmd("TDsGenerator",TDsGeneratorMake.conf(500000000,1,1))])
		{ waitms: 1000},
	cmd.start(40){ waitms: 1000},
	cmd.stop(){ waitms: 1000},
]

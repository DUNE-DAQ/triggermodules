local moo = import "moo.jsonnet";
local cmd = import "sourcecode/appfwk/schema/appfwk-cmd-make.jsonnet";
local TDsGeneratorMake = import "sourcecode/triggermodules/schema/triggermodules-DAQTriggerDecisionMaker-make.jsonnet";

///////////	Queues
local queues = {
	TMsQueue: cmd.qspec("TMsQueue",
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
	TMsGenerator: cmd.mspec("TMsGenerator",
		"TriggerPrimitiveTiming",
		[cmd.qinfo("output",
			"TMsQueue",
			cmd.qdir.output)]),

	
	TimingTCsGenerator: cmd.mspec("TimingTCsGenerator",
		"DAQTriggerTimingCandidateMaker",
		[cmd.qinfo("input",
			"TMsQueue",
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
	cmd.init([queues.TMsQueue,queues.TCsQueue,queues.TDsQueue],
		[modules.TMsGenerator, modules.TimingTCsGenerator,modules.TDsGenerator])
		{ waitms: 1000},
	cmd.conf(
		[cmd.mcmd("TMsGenerator"),
		cmd.mcmd("TimingTCsGenerator"),
		cmd.mcmd("TDsGenerator",TDsGeneratorMake.conf(500000000,1,1))])
		{ waitms: 1000},
	cmd.start(40){ waitms: 1000},
	cmd.stop(){ waitms: 1000},
]

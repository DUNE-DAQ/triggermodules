// Example commands for a triggermodules job.
//
// It is TP-TA-TC sequence.
//       TP_generator -> (TP) -> TA_maker -> (TA) -> TC_maker -> (TC)
//

local moo = import "moo.jsonnet";
local cmd = import "appfwk-cmd-make.jsonnet";

local queues = {
      TPsQueue:	cmd.qspec("TPsQueue","FollyMPMCQueue",1000),
      TAsQueue:	cmd.qspec("TAsQueue","FollyMPMCQueue",100),
      TCsQueue:	cmd.qspec("TCsQueue","FollyMPMCQueue",10),
};

[
      cmd.init([queues.TPsQueue, queues.TAsQueue, queues.TCsQueue],
	       [cmd.mspec("TPsGenerator","TriggerPrimitiveRadiological", [
                   cmd.qinfo("output", "TPsQueue", cmd.qdir.output)]),

	        cmd.mspec("TPsGenerator2","TriggerPrimitiveSupernova", [
                   cmd.qinfo("output", "TPsQueue", cmd.qdir.output)]),

                cmd.mspec("TAsGenerator","DAQTriggerActivityMaker", [
                   cmd.qinfo("input", "TPsQueue", cmd.qdir.input), cmd.qinfo("output","TAsQueue",cmd.qdir.output)]),

                cmd.mspec("TCsGenerator","DAQTriggerCandidateMaker", [
                   cmd.qinfo("input", "TAsQueue", cmd.qdir.input), cmd.qinfo("output","TCsQueue",cmd.qdir.output)])]) { waitms: 1000},

      cmd.conf([cmd.mcmd("TriggerPrimitiveRadiological")]) { waitms: 1000 },
      cmd.start(42) { waitms: 1000 },
      cmd.stop() { waitms: 1000 },
]

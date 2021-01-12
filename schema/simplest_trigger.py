import moo
import ddcmd
import dunedaq.appfwk.cmd as cmd

#########	Queues
TPsQueue = cmd.QueueSpec(
	kind = cmd.QueueKind("FollyMPMCQueue"),
	inst = "TPsQueue",
	capacity = 1000))
TAsQueue = cmd.QueueSpec(
	kind = cmd.QueueKind("FollyMPMCQueue"),
	inst = "TPsQueue",
	capacity = 100))
TCsQueue = cmd.QueueSpec(
	kind = cmd.QueueKind("FollyMPMCQueue"),
	inst = "TPsQueue",
	capacity = 10))
queues = cmd.QueueSpecs([TPsQueue,TAsQueue,TCsQueue)

#########	Modules	
TPsGenerator = cmd.ModSpec(
	inst = "TPsGenerator",
	plugin = "TriggerPrimitiveRadiological",
	data = QueueInfo(
		inst = "TPsQueue",
		name = "output",
		dir = "output"))
TPsGenerator2 = cmd.ModSpec(
	inst = "TPsGenerator",
	plugin = "TriggerPrimitiveSupernova",
	data = QueueInfo(
		inst = "TPsQueue",
		name = "output",
		dir = "output"))
TAsGenerator = cmd.ModSpec(
	inst = "TAsGenerator",
	plugin = "DAQTriggerActivityMaker",
	data = QueuesInfo([
		QueueInfo(
			inst = "TPsQueue",
			name = "input",
			dir = "input"),
		QueueInfo(
			inst = "TAsQueue",
			name = "output",
			dir = "output")))
TCsGenerator = cmd.ModSpec(
	inst = "TCsGenerator",
	plugin = "DAQTriggerCandidateMaker",
	data = QueuesInfo([
		QueueInfo(
			inst = "TAsQueue",
			name = "input",
			dir = "input"),
		QueueInfo(
			inst = "TCsQueue",
			name = "output",
			dir = "output")))
modules = cmd.ModSpecs([TPsGenerator,TPsGenerator2,TAsGenerator,TCsGenerator])

#########	Conf	
commands_init = ddcmd.init(queues=queues, modules=modules)

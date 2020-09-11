# DAQDuneTriggers

## Broad overview / Philosophy

This package is the prototype interface between DuneTriggerAlgs (https://github.com/plasorak/DuneTriggerAlgs) and appfwk (https://github.com/DUNE-DAQ/appfwk). This is where the queues, DAQModule and their are implemented.

This package also is the interface in the case you want to simulate the trigger decision process and feed in simulated TPs. The implementation of that happens in DuneTriggerGenerators (https://github.com/plasorak/DuneTriggerGenerators, I know, it doesn't exist yet).

## Installation
To install you need to first have the application framework, you can find instructions here: https://github.com/DUNE-DAQ/appfwk/wiki/Compiling-and-running
You can omit the `listrev` package:

```
git clone https://github.com/DUNE-DAQ/listrev.git
./build_daq_software.sh --install --pkgname listrev
```

Then, you need to build DuneTriggerAlgs, which is a dependency of this package, the way to do that is here (well, not really, but soon): https://github.com/plasorak/DuneTriggerAlgs

Optionally, you can install DuneTriggerGenerators: https://github.com/plasorak/DuneTriggerGenerators.

Finally, you should go back to the place called "MyTopDir" in the first link from the appfwk instruction, and do

```
git clone git@github.com:plasorak/DAQDuneTriggers.git
./build_daq_software.sh --install --pkgname DAQDuneTriggers
```

Hopefully, you didn't get any errors and you now have everything compiled.

## Running
Logging from fresh shell, execute `. ./setup_runtime_environment` from MyTopDir, and then:
```
daq_application -c QueryResponseCommandFacility -j DAQDuneTriggers/conf/sim_trigger_sn.json
```

Or any other json file in `DAQDuneTriggers/conf`

## Implementation

What actually happens behind the scene is the following:
 - Queues are setup, these queues are Multiple Producer - Multiple Consumer (MPMC). They hold collections of objects that are needed for making trigger decisions
 - DAQModule as setup, these are the stuff that "do the work". They produce TPs, TAs, TCs and TDs. Each have at least 2 threads, one which is responsible to receive the commands and another one that does the job.
 - Each of the DAQModule are assigned input or output queue, which they should read from and/or write to.
 
I think this is simple enough to understand from any json file, so have a look in the conf file (but sadly we can't comment in json).

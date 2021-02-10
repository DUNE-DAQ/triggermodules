# triggermodules

## Broad overview / Philosophy

This package is the prototype interface between triggeralgs (https://github.com/DUNE-DAQ/triggeralgs) and appfwk (https://github.com/DUNE-DAQ/appfwk). This is where the queues, DAQModule and their are implemented.

## Installation (for daq-v2.2.0)
To install you need to first have the application framework, you can find instructions here: https://github.com/DUNE-DAQ/appfwk/wiki/Compiling-and-running-under-v2.2.0

Then, you need to build triggeralgs, which is a dependency of this package.

With both appfwk and triggeralgs installed, go to "MyTopDir/sourcecode", under appfwk installation and clone this directory:
```
git clone https://github.com/DUNE-DAQ/triggermodules.git
```

You must also add "triggermodules" on the list at the bottom of "./sourcecode/CMakeLists.txt", and you should tell the compiler where triggeralgs was installed:
```
export CMAKE_PREFIX_PATH=/area/where/triggerals/is/installed:${CMAKE_PREFIX_PATH}
```

Now you are ready to install it:
```
dbt-build.sh --install
```

Hopefully, you didn't get any errors and you now have everything compiled.

## Running
Log in on a fresh shell and execute:
```
cd /area/where/daq/is/installed/
source daq-buildtools/setup_dbt.sh
cd MyTopDir
setup_runtime_environment
daq_application -c stdin://sourcecode/triggermodules/test/DAQDuneTriggers-trigger-app.json
```

## Implementation

What actually happens behind the scene is the following:
 - Queues are setup, these queues are Multiple Producer - Multiple Consumer (MPMC). They hold collections of objects that are needed for making trigger candidates.
 - DAQModule as setup, these are the stuff that "do the work". They produce TPs, TAs and TCs. Each have at least 2 threads, one which is responsible to receive the commands and another one that does the job.
 - Each of the DAQModule are assigned input or output queue, which they should read from and/or write to.

APPNAME=simplest_trigger_app
APPNAME=1toNTP_trigger_app
cd ../../..
moo compile sourcecode/DAQDuneTriggers/schema/"$APPNAME".jsonnet > sourcecode/DAQDuneTriggers/schema/"$APPNAME"_compiled.json
cd sourcecode/DAQDuneTriggers/schema/
daq_application -c "$APPNAME"_compiled.json 

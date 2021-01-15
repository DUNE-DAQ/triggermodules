cd ../../..
moo compile sourcecode/DAQDuneTriggers/schema/simplest_trigger_app.jsonnet > sourcecode/DAQDuneTriggers/schema/simplest_trigger_app_compiled.json
cd sourcecode/DAQDuneTriggers/schema/
daq_application -c simplest_trigger_app_compiled.json 

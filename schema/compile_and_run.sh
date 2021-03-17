APPNAME=simplest_trigger_app
APPNAME=1toNTP_trigger_app
APPNAME=full_csv_trigger_app
APPNAME=csv_trigger_app
APPNAME=Nto1_trigger_app
APPNAME=2csv-to-1-trigger_app
APPNAME=timing_message_app
APPNAME=full_trigger_app
cd ../../..
moo compile sourcecode/triggermodules/schema/"$APPNAME".jsonnet > sourcecode/triggermodules/schema/"$APPNAME"_compiled.json
cd sourcecode/triggermodules/schema/
daq_application -c "$APPNAME"_compiled.json 

APPNAME=simplest_trigger_app
APPNAME=1toNTP_trigger_app
APPNAME=full_trigger_app
APPNAME=csv_trigger_app
cd ../../..
moo compile sourcecode/DAQDuneTriggers/schema/"$APPNAME".jsonnet > sourcecode/DAQDuneTriggers/schema/"$APPNAME"_compiled.json
moo -g /lang:ocpp.jsonnet -M /home/lukas.arnold/appfwk-env/sourcecode/DAQDuneTriggers/schema -A path=dunedaq.triggerprimitivefromfile.Nljs -A ctxpath=dunedaq.triggerprimitivefromfile -A os=daqdunetrigger-triggerprimitivefromfile-schema.jsonnet render omodel.jsonnet o.hpp.j2
cd sourcecode/DAQDuneTriggers/schema/
daq_application -c "$APPNAME"_compiled.json 

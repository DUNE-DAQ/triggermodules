APPNAME=$1
cd ../../../..
moo compile sourcecode/triggermodules/schema/latency/"$APPNAME".jsonnet > sourcecode/triggermodules/schema/latency/"$APPNAME"_compiled.json
cd sourcecode/triggermodules/schema/latency
daq_application -c "$APPNAME"_compiled.json 

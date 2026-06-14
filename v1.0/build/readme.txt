

README.TXT for Comic Chat build process

4/4/96, rossb



To use this build process, you'll need to have VC4 installed and have
run "vcvars32.bat" (from msdev\bin, most likely).  Type "nmake" to
determine whether you're ready -- you should see an NMAKE error
message.

To build something, you'll type "build" plus one of the following:

all (this will ssync, rebuild, etc. on your machine)

exe (this will just build the rel and dbg exe's on your machine)

drop (this will copy whatever you just built to \\curlew\vwgpub)





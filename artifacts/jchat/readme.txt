Each of the 4 program directories (chat1j, proxy, script, html) contains a build.bat file
which is used to compile and obfuscate each program.
The 3 support directories (hashjava, jdk114, sdk20) contain the files necessary to run
the build.bat files.

Contents of Sub Directories.

Directory	Contents			Description

chat1j		*.java				Java source for MSChat 1.0j
		toolbar.gif			Image resource file
		usa.properties			English string resource file
		build.bat			Build batch file
		/ob				Obfuscated class file output directory

proxy		ProtocolProxy.java		Protocol Proxy server (extends Server)
		ProtocolProxy.properties	Sample server configuration file
		build.bat			Build batch file
		/ob				Obfuscated class file output directory

script		IRCClient.java			Provides simple IRC support
		IRCListener.java		Interface for receiving IRC messages
		IRCApplet.java			Applet used in Script pages. (extends IRCCLient)
		default.htm			HTML page for accessing samples
		/simple				Simple frames sample page
		/form				Form sample page
		build.bat			Build batch file
		/ob				Obfuscated class file output directory

html		HIServer.java			HTML IRC Server source code. compile, and run "jview HIServer"
		Server.java			Generic multithreaded server
		*.java				Supporting java source files
		index.html			Sample default page
		HIServer.properties		Sample HIServer configuration file
		*.irct				Sample IRC Template files
		readme.txt			Simple setup instructions
		build.bat			Build batch file
		/ob				Obfuscated class file output directory
		/images				Contains images to be used with enhanced.irct file

hashjava	*.* /s				Hashjava Obfuscator program

jdk114		*.* /s				Sun Java Development Kid 1.1.4

sdk10		*.* /s				Microsoft SDK for Java 2.0


Instructions on running each project

Chat1j:
	To test stand-alone From the "ob" directory, type "jview MSChat".
To test as an applet, copy the contents of the ob directory to a web-directory on a machine
that is running both a web-server and a chat server.  Point your browser to the default.htm file
in the directory you copied it into. !Make sure you are loading page via http, as opposed to locally!

Proxy:
	Change to the "ob" directory, type "jview ProtocolProxy 80".
Your local machine should now be listening for connections on port 80, and redirecting them
to the machines listed in the ProtocolProxy.properties files.


Script:
	Copy the contents of the "ob" directory to a directory on a web/chat server, and follow same
directions as Chat1j above.


HTML:
	Change to the "ob" directory, and type "jview HIServer".
You can now point your browser toward your local computer "http://localcomputername", and it should
load the logon page.  After you submit the form, the irc server you are connected to is defined
in the HIServer.properties file.




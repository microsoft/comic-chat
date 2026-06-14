------------------------------------------------------------------
             Microsoft Chat SDK 1.1 Readme File
                         August 1997            
------------------------------------------------------------------

             (c) Copyright Microsoft Corporation, 1997


HOW TO USE THIS DOCUMENT
========================

To view Readme.txt on screen in Windows Notepad, maximize the Notepad 
window.

To print Readme.txt, open the file in Notepad or another word processor,
and then on the File menu, click Print.

CONTENTS
========

MICROSOFT CHAT 1.0J OPTIONAL PARAMETERS AND PUBLIC METHODS
SETTING UP MICROSOFT CHAT 1.0J ON A CHAT AND WEB SERVER

MICROSOFT CHAT 1.0J OPTIONAL PARAMETERS AND PUBLIC METHODS
================================================================
You can design your web pages to include Microsoft Chat 1.0 j. If users' 
browser supports Java, then they will be able to use Microsoft Chat 1.0j. Here 
are the optional parameters and public methods for Microsoft Chat 1.0j.


Optional Parameters for Microsoft Chat 1.0j:

Name		Values			Default		Information
======		==============		==============	======================================================
port		[number]		6667		the port the chat server is running on, usually 6667
nickname	[user nickname]		"Anonymous"	the default user name used for autoconnect
room		[roomname]		-		if left blank, the room list will be shown
toolbar		true | false		true		whether or not the toolbar should be visible
memberlist	true | false		true		whether or not the memberlist should be visible
observer	true | false		false		if true, the user can not participate in conversation
banner		[imagename]		-		specifies a welcome image that should be displayed when 
							the applet loads
autoconnect	true | false		false		if true and room and nickname are specified, applet will 
							automatically connect when loaded
autodisconnect	true | false		false		if true, the applet will automatically disconnect when the 
							page is left
id		[unique string]		"null"		unique ids must be supplied if multiple chat applets will 
							be on the same page.
motd		true | false		true		if true, the applet will display the Message of the Day 
							after connecting
stats 		true | false 		true 		if true, the applet will display Server stats after connecting 
id 		[unique string] 	"null" 		unique ids must be supplied if multiple chat applets will be 
							on the same page. 
font 		fontname,style,size 	Helvetica,0,11 	Helvetica | Dialog | TimesRoman | Courier, 
							0(plain),1(bold),2(italic),3(bold+italic),8-20 
bgcolor 	ffffff 			c0c0c0 		specify the color using hex notation just like in html body tag 


Public Methods (these can be called from a scripting language): 

Returns		Method					Information
========	====================================	============================================================
void		connect(String nickname)		connects to the server with the specified nickname
void		connect(String nickname, String room)	same as above, but user joins specified room, 
							overrides room parameter (if present)
void		disconnect()				user exits room if in one, and disconnects from server
String		getNick()				returns the users nickname, or null if user hasn't logged on yet.
String		getRoom()				returns the room the user is in, or null if not in a room.
void 		join(String room, String password) 	leaves current room if any, and joins specified room. Password 
							ignored if null or "" 
void 		showRoomList() 				Displays the room list window. 
void 		showEnterRoomDialog() 			displays the enter room dialog box. 
void 		showFontDialog() 			displays the change font dialog. 
void 		showConnectDialog() 			shows the dialog which prompts user to connect to server. 
void 		showAboutDialog() 			displays the about Microsoft Chat dialog. 
 	 	 

SETTING UP MICROSOFT CHAT 1.0J ON A CHAT AND WEB SERVER
=======================================================
These are the steps to get the samples and Microsoft Chat 1.0 j to work with your chat server.

1) You will need to setup a web server and IRC chat server on the same computer to use Microsoft Chat 1.0j. 
2) After you setup the web server and chat server, then you should copy mschat1j.exe in the chat1j directory
   to your web server. 
3) You will need to unzip the selfextracting exe mschat1j.exe by running the program mschat1j.exe. 
4) To use Microsoft Chat 1.0 j you will need to use these files after running the program mschat1j.exe: 
   all *.class, *.gif, *.cab, *.properties and *.jar files. You will also need a html file that points to the applet, you 
   can either use the sample html files or create your own.
5) Make sure that the toolbar.gif, and the usa.properties files are in the same directory
   as the class files.


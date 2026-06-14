How to set up the Microsoft HTML Chat Program.

Contents:

11 java class files, including:

HIServer.class		-The main program you run (Html Irc Server)

an "images" directory, containing various possibly useful icons.
These images are used in the sample templates, but their locations
are hard-coded to "http://keezer" if you wish to use these images,
you will need to place them on a web-server, and change the references
to their new location.  The HIServer program that serves up chat
pages is not a web-server, so the image references cannot be relative to it.


1. Place all *.class, *.irct, and the HIServer.properties files into a directory
on a server.

2. Edit the HIServer.properties file:
	a. change the "irc.host" parameter to the irc server that you wish html clients to use.
	b. change the "listen.port" parameter to the port you want to run the server on.
	   Preferably port 80, which means that you cannot also have a web-server running.
	c. change the "templates" parameter by adding any new template files you wish to use.
	d. change the "default.page" if desired. It should be a login page.

3. Edit any template files that you create.
   If you are going to use the "enhanced.irct" or "autorefresh.irct" templates,
   you will need to change, or remove the image tags from them, since they are hard-coded.

4. Start the server by typing "jview HIServer".
   It should read in any template files, and then start listening for connections on the
   specified port.

5. If you connect to the server, it should give you the html page specified by "default.page" parameter.
   This page should contain a form for the user to submit to login, such as this:

<form name=login method=get action=enhanced.irct>
	Nickname<br>
	<input type=text name=nick size=20><br>
	Room<br>
	<input type=text name=room value=newbies size=20><br>
	<input type=submit value="Connect"><br>
</form>



Notes on editing the template files:

The template files are just html files that contain some key tags which the server
uses to format conversation and events.  (all special tags are case-sensitive)

<chat length=10> indicates the beginning of the section where chat will be displayed.
and the length parameter indicates how many lines to display before trimming old ones.
Specifying length is necessary to prevent refreshes from taking longer, and longer etc.

</chat> indicates the end of the chat display area.

enclosed within the chat tags, are method handling sections like this:

messageArrived(source, target, message)
<img src=http://keezer/images/say.gif> <font color=2020ff>%source% says: </font><font color=000000>%message%</font><br>

The first line indicates the method of the event that is happening,
and the second line indicates how to display that event in the chat history.
any fields present in the method signature, can be placed in the message by
surrounding them with % signs, as in the example.
The second line must all be on the same line, as in the example.
If nothing should be displayed for a message, just leave the second line blank.

<members> indicates the beginning of the member display area.
</members> indicates the end.
enclosed by the members tags should be exactly three lines,
the first line should contain the variable %host%
the second line should contain the variable %speaker%
and the third should contain the variable %observer%
These three lines tell the server how to format the html for each type of chatter,
most people will be regular speakers, (unless the room is moderated, then they will be observers)
Example:

<form name=input method=get action=enhanced.irct>
	<input type=hidden name=nick value=%localnick%>
	<input type=hidden name=room value=%localroom%>
	<input type=hidden name=tag value=%tag%>
	<input type=text name=message size=60>
	<input type=submit value="Send">
	<select name=target>
		<option selected>%localroom%
		<members>
		<option>@%host%
		<option>%speaker%
		<option>-%observer%
		</members>
	</select>
</form>

Notice, that this form, contains three hidden fields:
nick, room and tag.  These fields are filled in with the global variables:
%localnick%, %localroom%, and %tag% respectively. These variables can be used anywhere
on the page, and they will be replaced appropriately.

All form submissions to the server, must contain at least the following
query parameter nick, room and tag.  The tag parameter, ensures that the
user will retrieve the appropriate chat history.

This meta refresh tag, is an example of a minimum submission:

<meta name="Refresh" http-equiv="Refresh" content="30;url=/autorefresh.irct?nick=%localnick%&room=%roomnopound%&tag=%tag%">

Various global variables:
%localnick%	the current nickname of user accessing this page
%localroom%	the current chat room of the user accessing page
%tag%		unique identifier, necessary for all form submissions
%roomnopound%	the same as %localroom%, but strips out the preceding "#" symbol to prevent problems in some browsers
%date%		this is replaced by the current date and time on the server.


This documentation is not complete, (or pretty).
Please contact me with any questions or requests.
There are many cool things you can do with this (especially since room hosts can embed html in messages),
so feel free to ask if you need some ideas.

Kris Nye



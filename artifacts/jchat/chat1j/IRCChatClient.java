////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	IRCChatClient 
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.io.*;
import java.net.*;
import java.util.*;

public class IRCChatClient implements ChatClient, Runnable
{
	///////////////////
	//
	//	static members
	//
	///////////////////
	static char ctcp = 1;
	static ThreadGroup group;
	
	///////////////////
	//
	//	public members
	//
	///////////////////
	
	//	ChatListener to notify of messages etc.
	ChatListener chatListener;
	//	user fields
	String nickname, username, realname, roomname;
	String servername;
	int port;
	String password;
	//	the message of the day
	StringBuffer motd = new StringBuffer();
	StringBuffer stats = new StringBuffer();
	//	Server connection fields
	Socket connection;
	DataInputStream input;
	PrintStream output;
	InputStreamReader reader;
	//	Saves the topics for given rooms
	Properties topics = new Properties();
	//	Stores identity information on users
	Properties identities = new Properties();
	//	Vector contains banlist
	Vector banList = new Vector();
	Object banWait = new Object();
	
	IRCChatClient()
	{
		group = Thread.currentThread().getThreadGroup();
	}

	//	Constructor
	public IRCChatClient(ChatListener listener)
	{
		this();
		if (listener == null)
			throw new NullPointerException();
		setChatListener(listener);
	}

	//	Methods	
	public void setChatListener(ChatListener chatListener)
	{
		this.chatListener = chatListener;				
	}

	public void setNickname(String nickname)
	{
		if (nickname != null)
		{
			this.nickname = nickname.replace(' ', '_');
			// if connected, we should do a NICK command
			if (connection != null)
				nick();
		}
	}

	public void setUsername(String username)
	{
		this.username = username;
	}

	public void setRealname(String realname)
	{
		this.realname = realname;
	}

	//	this is a request to connect to a server, password may be null
	//	user will not be informed that they have connected, until
	//	this client receives the message of the day
	public void connect(String server, int port, String password)
	{
		disconnect();
		stats.setLength(0);
		motd.setLength(0);
		this.servername = server;
		this.port = port;
		this.password = password;
		Thread thread = new Thread(group, this);
		thread.setPriority(3);
		thread.start();
	}

	public void run()
	{
		//	ensure we are disconnected first
		if (servername == null || username == null || realname == null || nickname == null)
		{
			//	we will be more specific later
			connectFailed("Invalid Parameters");
			return;
		}
		if (port < 2)
			port = 6667;
		try
		{
			connection = new Socket(servername, port);
			input = new DataInputStream(connection.getInputStream());
			output = new PrintStream(connection.getOutputStream());
			reader = new InputStreamReader(input, this);
			synchronized(reader)
			{
				pass(password);
				nick();
				user();								
			}
			return;
		}
		catch(UnknownHostException e)
		{
			connectFailed(localize("host.not.found") + " " + servername);
			return;
		}
		catch(Exception e)
		{
			// UnknownHostException etc.
			System.out.println(e);
			connectFailed("Unknown Error : " + e);
		}
	}

	public String getClassName()
	{
		return getClass().getName();
	}

	public void disconnect()
	{
		try
		{
			reader.stop();
		}
		catch(Exception e)
		{
			// do nothing
		}
		initialize();
	}

	//	done after disconnect
	public void initialize()
	{
		try
		{
			if (connection != null)
				connection.close();
			if (input != null)
				input.close();
			if (output != null)
				output.close();
		}
		catch(Exception e)
		{
			// IOException while closing
		}
		connection = null;
		input = null;
		output = null;
		reader = null;
	}

	public boolean connected()
	{
		if (connection != null)
			return true;
		else
			return false;
	}

	//	retrieves a list of all channel names
	public void getRooms()
	{
		list();
	}
	//	get detailed information on specified channel
	public Properties getRoomProperties(String name)
	{
		return null;
	}
	//	get the nicknames of all users on the server
	public String[] getUsers()
	{
		return null;
	}
	//	get the nicknames of all users on channel
	public String[] getUsers(String channel)
	{
		return null;
	}
	//	get detailed information about specified user
	public Properties getUserProperties(String nickname)
	{
		return null;
	}
	//	join a channel
	public void join(String channel, String password)
	{
		if (channel != null)
		{
			channel = channel.trim();
			if (!(channel.startsWith("#") || channel.startsWith("&")))
				channel = "#" + channel;
			String commandLine = "JOIN " + channel;
			if (password != null)
				commandLine += " " + password;
			toServer(commandLine);
		}
	}
	//	depart from channel
	public void part(String room)
	{
		if (room != null && roomname != null)
			toServer("PART " + room);			
	}
	//	send a string to a specified user or channel
	public void send(String target, String message, Properties props)
	{
		// temp code
		if(message == null)
			return;
	//	if (message.startsWith("/"))
	//		toServer(message.substring(1));
	//	else
			sendPrivateMessage(target, message);
	}

	public void send(String[] target, String message, Properties props)
	{
		if (message == null)
			return;
		sendPrivateMessage(target, message);
	}

	public void version(String target)
	{
		sendPrivateMessage(target, ctcp + "VERSION" + ctcp);
	}
	
	public void lagTime(String target)
	{
		sendPrivateMessage(target, ctcp + "PING " + System.currentTimeMillis() + ctcp);
	}

	public void localTime(String target)
	{
		sendPrivateMessage(target, ctcp + "TIME" + ctcp);
	}

	public void identity(String target)
	{
		if (target != null)
			toServer("WHOIS " + target);
	}

	public void profile(String target)
	{
		if (target != null)
			sendPrivateMessage(target, "# GetInfo");
	}

	public void addOperator(String user)
	{
		if (user != null && roomname != null)
			toServer("MODE " + roomname + " +o " + user);
	}

	public void removeOperator(String user)
	{
		if (user != null && roomname != null)
			toServer("MODE " + roomname + " -o " + user);
	}

	public void addSpeaker(String user)
	{
		if (user != null && roomname != null)
			toServer("MODE " + roomname + " +v " + user);
	}

	public void removeSpeaker(String user)
	{
		if (user != null && roomname != null)
			toServer("MODE " + roomname + " -v " + user);
	}

	public String getIdentity(String user)
	{
		if (user == null)
			return null;
		return identities.getProperty(user);
	}

	public void kick(String room, String user, String reason)
	{
		if (room != null && user != null)
			toServer("KICK " + room + " " + user + ((reason != null)?" :" + reason:""));
	}

	public void setBanmask(String room, String mask, boolean ban)
	{
		if (room != null && mask != null)
		{
			toServer("MODE " + room + " " + (ban?"+":"-") + "b " + mask);
			//	now request updated list
			requestBanlist();
		}
	}

	public String[] getBanlist(String room)
	{
		requestBanlist();
		synchronized(banWait)
		{
			try
			{
				banWait.wait(2000);
			}
			catch(Exception e)
			{
				return null;
			}
		}
		return Static.toStringArray(banList);
	}

	//	Protected methods
	
	//	This method processes incoming messages from the server
	protected void process(String line)
	{
		if (line == null)
			return;
		StringTokenizer st = new StringTokenizer(line, " :\r\n", true);
		String from = null;
		String command = null;
		Vector vector = new Vector();
		String firstToken = st.nextToken();
	//	System.out.println("firstToken \"" + firstToken + "\"");
		if (firstToken.equals(":"))
		{
			from = st.nextToken();
			// command is the next token, not counting white space
			while (true)
			{
				command = st.nextToken();
				if (!command.equals(" "))
					break;
			}
		}
		else
		{
			command = firstToken;
		}
	//	System.out.println("from \"" + from + "\"");
	//	System.out.println("command \"" + command + "\"");
		//	now read in parameters
		while (st.hasMoreTokens())
		{
			String token = st.nextToken();
			// ignore space characters until we see a ":"
			if (!token.equals(" "))
			{
				if (token.equals(":"))
				{
					if (!st.hasMoreTokens())	// no more tokens, this shouldn't happen
						break;
					// whatever remains is final parameter
					String lastToken = st.nextToken("\r\n");
					vector.addElement(lastToken);
					break;
				}
				else
					vector.addElement(token);
			}
		}
		if (from == null)
			from = servername;
		command = command.toUpperCase();
		String[] parameters = null;
		//	if there are any parameters, put them in a String[]
		if (!vector.isEmpty())
		{
			parameters = new String[vector.size()];
			for (int i = 0; i < parameters.length; i++)
				parameters[i] = (String)vector.elementAt(i);
		}
		try
		{
			//	if the command is an integer then processReply
			int reply = Integer.parseInt(command);
			processReply(from, reply, parameters);
		}
		catch(NumberFormatException e)
		{//	the command is a string, not a number so processCommand
			processCommand(from, command, parameters);
		}
	}

	//	process a command from server, from may be null
	protected void processCommand(String from, String command, String[] parameters)
	{
		String lastParam = (parameters != null)?parameters[parameters.length-1]:"";
		if (command.equals("PRIVMSG"))
		{
			String fromNick = getNickname(from);
			//	store identity info
			identities.put(fromNick, from);
			String target = parameters[0];
			String message = lastParam;
			if (message != null && message.startsWith(ctcp + ""))
				processCTCPCommand(target, from, message);
			else
				messageArrived(target, fromNick, message);
		}
		else if (command.equals("NOTICE"))
		{	
			String fromNick = getNickname(from);
			String target = parameters[0];
			String message = lastParam;
			if (message != null && message.startsWith(ctcp + "")) //	incoming ctcp replies are handled by the historyProcessor
				messageArrived(target, fromNick, message);
			else if (fromNick.equalsIgnoreCase(servername))	//	other incoming notices are considered system messages
				systemMessage(lastParam);
			else
				messageArrived(target, fromNick, message);
		}
		else if (command.equals("PING"))
		{
			pong();
		}
		else if (command.equals("JOIN"))
		{
			if (parameters.length > 0)
			{
				String nick = getNickname(from);
				//	store identity info
				identities.put(nick, from);
				this.roomname = parameters[0];
				if (nick.equalsIgnoreCase(nickname))
				{
					//	notify client that room has been successfully joined
					String topic = topics.getProperty(roomname, "");
					Properties props = new Properties();
					props.put("topic", topic);
					roomEntered(roomname, props);
				}
				else
					userJoining(getNickname(from), roomname, null, true);
			}
		}
		else if (command.equals("PART") || command.equals("QUIT"))
		{
			if (parameters.length > 0)
				chatListener.userLeaving(getNickname(from), parameters[0]);
		}
		else if (command.equals("KICK"))
		{
			if (parameters.length > 1)
			{
				String room = parameters[0];
				String user = parameters[1];
				String comment = (parameters.length > 2)?parameters[2]:null;
				if (user != null && username != null && user.equals(username))
				{
					//	remember that user has been kicked
					roomname = null;
					chatListener.removedFromRoom(getNickname(from), room, comment);
				}
				else
				{
					systemMessage(getNickname(from) + " kicked " + user + " from room " + room + (comment != null ? (", saying \"" + comment + "\"") : "") + ".");
					chatListener.userLeaving(user, room);
				}
			}
		}
		else if (command.equals("NICK"))
		{
			if (parameters.length > 0)
			{
				String oldname = getNickname(from);
				String newname = parameters[0];
				identities.put(newname, from);
				userUpdate(oldname, newname, null);
			}
		}
		else if (command.equals("INVITE"))
		{
			if (parameters.length > 1)
			{
				String nick = getNickname(from);
				String room = parameters[1];
				systemMessage(nick + " has invited you to join room " + room);
			}
		}
		else if (command.equals("TOPIC"))
		{
			if (parameters.length > 1)
			{
				String room = parameters[0];
				String topic = parameters[1];
				Properties p = new Properties();
				p.put("topic", topic);
				roomUpdate(room, p);
			}
		}
		else if (command.equals("MODE"))
		{
			if (parameters.length > 1)
			{
				String name = parameters[0];
				String username = (parameters.length > 2)?parameters[2]:name;
				String mode = parameters[1];
				String bool = mode.startsWith("+")?"true":"false";
				Properties props = new Properties();
				for (int i = 1; i < mode.length(); i++)
				{
					String c = String.valueOf(mode.charAt(i));
					props.put(c, bool);
				}
				//	this is a room mode command if there are only 2 parameters, and name startswith # or &
				boolean room = (name.startsWith("#") || name.startsWith("&")) && (parameters.length < 3);
				if (room)
					roomUpdate(name, props);
				else
					userUpdate(username, username, props);
			}
		}
		else
		{
			System.out.println("Command");
			System.out.println(from);
			System.out.println(command);
			if (parameters != null)
			{
				for (int i = 0; i < parameters.length; i++)
					System.out.println("param " + i + " \"" + parameters[i] + "\"");
			}
			System.out.println();
		}
	}

	protected void processCTCPCommand(String target, String from, String message)
	{
		if (from == null || message == null)
			return;
		String nick = getNickname(from);
		System.out.println(nick + " " + message);

		//	correct for chatsock problems
		//	it sends back ctcp replies as privmsgs starting with ctcp + *
		if (message.startsWith(ctcp + "*"))
		{
			//	this message is a chatsock reply message
			//	so it should be corrected, and sent on as a regular message
			//	the historyProcessor will handle displaying it
			String corrected = ctcp + message.substring(2);
			messageArrived(target, nick, corrected);
			return;
		}
		
		if (message.equalsIgnoreCase(ctcp + "VERSION" + ctcp))
		{
			System.out.println(localize("ctcp.version"));
			sendNotice(nick, ctcp + "VERSION " +localize("ctcp.version") + ctcp);
		}
		else if (message.equalsIgnoreCase(ctcp + "TIME" + ctcp))
			sendNotice(nick, ctcp + "TIME " + new Date() + ctcp);
		else if (message.startsWith(ctcp + "PING"))
			sendNotice(nick, message);
		else if (message.startsWith(ctcp + "ACTION") || message.startsWith(ctcp + "SOUND"))//	action commands are handled by the historyProcessor
			messageArrived(target, nick, message);
		else
		{
			System.out.println("Unrecognized CTCP command:");
			System.out.println(nick + " " + message);
		}
	}

	protected String localize(String key)
	{
		return chatListener.localize(key);
	}

	protected void processReply(String from, int reply, String[] parameters)
	{
		if(reply > 399)
		{
			processError(from, reply, parameters);
			return;
		}
		if (reply == 367)
		{
			System.out.println("Reply");
			System.out.println(from);
			System.out.println(reply);
			if (parameters != null)
			{
				for (int i = 0; i < parameters.length; i++)
					System.out.println("param " + i + " \"" + parameters[i] + "\"");
			}
			System.out.println();
		}
		String lastParam = null;
		if (parameters != null)
			lastParam = parameters[parameters.length-1];
		switch(reply)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 251:
		case 252:
		case 253:
		case 254:
		case 255:
			if (parameters != null)
				appendTo(stats, parameters, 1);
			break;
		case 312:	// RPL_WHOISSERVER
		case 313:	// RPL_WHOISOPERATOR
		case 317:	// RPL_WHOISIDLE
		case 318:	// RPL_ENDOFWHOIS
		case 319:	// RPL_WHOISCHANNELS
			break;
		case 311:	// RPL_WHOISUSER
			if (parameters.length > 2)
			{
				//	we will treat this command as if it was
				//	a ctcp IDENTITY reply from user
				String target = parameters[0];
				String nick = parameters[1];
				String user = parameters[2];
				String host = parameters[3];
				String message = ctcp + "IDENTITY " + user + '@' + host + ctcp;
				messageArrived(target, nick, message);
			}
			break;
		case 321:	// RPL_LISTSTART
			chatListener.roomListStart();
			break;
		case 322:	//	RPL_LIST
			if (parameters.length > 2)
			{
				String room = parameters[1];
				int users = Integer.parseInt(parameters[2]);
				String topic = (parameters.length > 3)?parameters[3]:"";
				chatListener.roomListAdd(room, users, topic);
			}
			break;
		case 323:	//	RPL_LISTEND
			chatListener.roomListEnd();
			break;
		case 324:	//	RPL_CHANNELMODEIS
			if (parameters.length > 2)
			{
				String room = parameters[1];
				String mode = parameters[2];
				//	build properties
				Properties p = new Properties();
				String bool = (mode.startsWith("+"))?"true":"false";
				for (int i = 1; i < mode.length(); i++)
				{
					String c = String.valueOf(mode.charAt(i));
					//	later, we will take into account parameters
					//	limit, banlist etc.
					p.put(c, bool);
				}
				roomUpdate(room, p);
			}
			break;
		case 332:	//	RPL_TOPIC
			if (parameters.length > 2)
			{
				String room = parameters[1];
				String topic = parameters[2];
				//	save the room topic
				topics.put(room, topic);
			}
			break;
		case 353:	//	RPL_NAMREPLY	// lists users in room client is joining
			if (chatListener != null)
			{
				//	notify the client of each user in the room
				String room = parameters[2];
				StringTokenizer st = new StringTokenizer(lastParam, " \r\n");
				while (st.hasMoreTokens())
					userJoining(st.nextToken(), room, null, false);
			}
			break;
		case 366:	//	RPL_ENDOFNAMES
			//	query the server for the room mode now
			toServer("MODE " + roomname);
			break;
		case 375:	//	RPL_MOTDSTART
			motd.setLength(0);
			break;
		case 372:	//	RPL_MOTD
			if (parameters != null)
				appendTo(motd, parameters, 1);
			break;
		//	this following reply indicates that we are successfully logged on,
		//	now we notify the chatListener
		case 376:	//	RPL_ENDOFMOTD
			chatListener.connected(servername, stats.toString(), motd.toString());
			//	clear the message of the day
			motd.setLength(0);
			break;
		case 367:	//	RPL_BANLIST
			if (parameters.length > 2)
			{
				String banID = parameters[2];
				banList.addElement(banID);
			}
			break;
		case 368:	//	RPL_ENDOFBANLIST
			{
				synchronized(banWait)
				{
					banWait.notify();
				}
			}
			break;
		default:
			systemMessage(lastParam);
		}
	}

	protected void processError(String from, int error, String[] parameters)
	{
		String lastParam = null;
		if (parameters != null)
			lastParam = parameters[parameters.length-1];
		switch (error)
		{
		case 422:	//	ERR_NOMOTD
			chatListener.connected(servername, stats.toString(), null);
			//	clear the message of the day
			motd.setLength(0);
			break;
		case 404:	//	ERR_CANNOTSENDTOCHAN
			systemMessage("Error: Cannot send to moderated channel.");
			break;
		case 432:	//	ERR_ERRONEUSNICKNAME
		case 433:	//	ERR_NICKNAMEINUSE
		case 436:	//	ERR_NICKCOLLISION
			if (parameters.length > 0)
				chatListener.invalidNickname(parameters[0]);
			break;
		case 473:
			if (parameters.length > 1)
				systemMessage(localize("invite.only") + " " + parameters[1]);
			break;
		case 475:	//	ERR_BADCHANNELKEY
			if (parameters.length > 0)
				chatListener.invalidPassword(parameters[1]);
			break;
		default:
			systemMessage("Error: " + lastParam);
			System.out.println("Error");
			System.out.println(from);
			System.out.println(error);
			if (parameters != null)
			{
				for (int i = 0; i < parameters.length; i++)
					System.out.println("param " + i + " \"" + parameters[i] + "\"");
			}
			System.out.println();
			break;
		}
	}

	public void appendTo(StringBuffer sb, String[] params, int start)
	{
		if (params == null)
			return;
		StringBuffer buffer = new StringBuffer();
		for (int i = start; i < params.length; i++)
		{
			buffer.append(params[i]);
			if (i + 1 < params.length)
				buffer.append(" ");
		}
		appendTo(sb, buffer.toString());
	}

	public void appendTo(StringBuffer sb, String line)
	{
		if (line != null)
			sb.append(line + "\r\n");
	}

	public void roomUpdate(String name, Properties p)
	{
		if (name != null && p != null)
		{
			//	send bogus appears as message if 
		//	if (p.get("m") == null || p.get("m").equals("false"))
		//		sendPrivateMessage(name, "# Appears as TEXT.");					
			chatListener.roomUpdate(name, p);
		}
	}

	public void roomEntered(String name, Properties p)
	{
		if (name != null && p != null)
			chatListener.roomEntered(name, p);
	}

	public void connectFailed(String reason)
	{
		chatListener.connectFailed(reason);
	}

	public void userUpdate(String oldname, String newname, Properties props)
	{
		if (oldname != null && newname != null)
			chatListener.userUpdate(oldname, newname, props);
	}

	public void userJoining(String name, String channel, Properties props, boolean newMember)
	{
		if (name == null)
			return;
		if (props == null)
			props = new Properties();
		if (name.startsWith("@"))
		{
			props.put("o", "true");
			name = name.substring(1);
		}
		if (name.startsWith("+"))
		{
			props.put("v", "true");
			name = name.substring(1);
		}
		if (newMember)
			chatListener.userJoining(name, channel, props);
		else
			chatListener.userListAdd(name, channel, props);
	}

	protected String getNickname(String fullname)
	{
		if (fullname == null)
			return "Anonymous";
		int index = fullname.indexOf('!');
		if (index < 1)
			return fullname;
		return fullname.substring(0, index);
	}

	protected void sendNotice(String target, String message)
	{
		if (target != null && message != null)
			toServer("NOTICE " + target + " :" + message);
	}

	protected void sendPrivateMessage(String target, String message)
	{
		if (target != null && message != null)
			toServer("PRIVMSG " + target + " :" + message);
	}

	protected void sendPrivateMessage(String[] target, String message)
	{
		if (target != null && message != null)
			toServer("PRIVMSG " + toCommaDelimitedString(target) + " :" + message);
	}

	protected static String encodeUTF8(String text)
	{
		try
		{
			ByteArrayOutputStream b = new ByteArrayOutputStream();
			DataOutputStream o = new DataOutputStream(b);
			o.writeUTF(text);
			byte[] d = b.toByteArray();
			byte[] c = new byte[d.length - 2];
			System.arraycopy(d, 2, c, 0, c.length);
			return new String(c, 0);
		}
		catch(Exception e)
		{
			return text;
		}
	}

	protected static String decodeUTF8(String text)
	{
		try
		{
			ByteArrayOutputStream b = new ByteArrayOutputStream();
			DataOutputStream o = new DataOutputStream(b);
			o.writeShort(text.length());
			o.writeBytes(text);
			ByteArrayInputStream f = new ByteArrayInputStream(b.toByteArray());
			DataInputStream i = new DataInputStream(f);
			return i.readUTF();
		}
		catch(Exception e)
		{
			return text;
		}
	}

	//	this will remove the users name from a whisper
	protected String toCommaDelimitedString(String[] strings)
	{
		if (strings == null)
			return null;
		StringBuffer buffer = new StringBuffer(strings[0]);
		for (int i = 1; i < strings.length; i++)
		{
			if (!strings[i].equals(nickname))
				buffer.append("," + strings[i]);
		}
		return buffer.toString();
	}

	protected void systemMessage(String message)
	{
		if (message != null)
			chatListener.systemMessage(message);
	}

	protected void messageArrived(String target, String from, String message)
	{
		if (message != null)
			chatListener.messageArrived(target, from, decodeUTF8(message), null);
	}

	protected void toServer(String line)
	{
		if (output != null)
			output.println(encodeUTF8(line));
		else
			System.out.println(line);
	}

	////////////////////////////////////////////
	//
	//	Command Methods to be send to the server
	//
	////////////////////////////////////////////
	
	protected void pass(String password)
	{
		if (password != null)
			toServer("PASS " + password);
	}

	protected void nick()
	{
		if (nickname != null)
			toServer("NICK " + nickname);
	}

	protected void user()
	{
		if (username == null || servername == null || realname == null)
		{
			System.out.println("Invalid User Data");
			return;
		}
		toServer("USER " + username + " " + servername + " " + servername + " " + realname);
	}

	protected void requestBanlist()
	{
		if (roomname != null)
		{
			banList.removeAllElements();
			toServer("MODE " + roomname + " +b");
		}
	}

	protected void list()
	{
		toServer("LIST");
	}

	protected void ping()
	{
		toServer("PING " + servername);
	}

	protected void pong()
	{
		toServer("PONG " + servername);
	}

}

class InputStreamReader extends Thread
{
	DataInputStream input;
	IRCChatClient client;

	public InputStreamReader(DataInputStream input, IRCChatClient client)
	{
		this.input = input;
		this.client = client;
		start();
	}

	public void run()
	{
		try
		{
			while(true)
			{
				String line = input.readLine();
				if (line == null)
					break;
				try
				{
					//	don't process empty lines
					if (!line.trim().equals(""))
						client.process(IRCChatClient.decodeUTF8(line));
				}
				catch(Exception e)
				{
					System.out.println("Caught Exception " + e);
					System.out.println("Error processing line");
					System.out.println(line);
					System.out.println(e);
					e.printStackTrace(System.out);
				}
			}
		}
		catch(Exception e)
		{
			//	Uknown Exception
			System.out.println("Caught Outer Exception " + e);
		}
		client.initialize();
		client.chatListener.disconnected("Socket Closed");
		System.out.println("InputStreamReader Stopped");
	}
}




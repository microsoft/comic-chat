////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	IRCCLient provides abstracted access to IRC 
//	functionality.
//	It supports multiple rooms, and a large subset
//	of full IRC functionality.
//	A class that wishes to use IRCClient must implement
//	the IRCListener interface.
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.io.*;
import java.net.*;
import java.util.*;
import java.applet.*;

public class IRCClient extends Applet implements Runnable
{

	private static final char   CTCPCHAR	= 1;
	private static final String CTCPVERSION	= CTCPCHAR + "VERSION";
	private static final String CTCPPING	= CTCPCHAR + "PING";
	private static final String CTCPTIME	= CTCPCHAR + "TIME";
	private static final char   SEPARATOR	= 2;
	protected static final int  BUILD	= 70;
	protected static String version = "Microsoft IRCClient 1.0 build " + BUILD;

	IRCListener listener;

	String host;
	int port;
	String password;
	String localnick;
	Hashtable temporary	= new Hashtable();	//	Hashtable holds room members, while getting room list
	Hashtable rooms		= new Hashtable();	//	Vectors of members keyed by roomname
	Hashtable moderated	= new Hashtable();	//	Rooms which are moderated
	Hashtable topics	= new Hashtable();	//	Contains topics keyed by roomname

	Socket connection;
	DataInputStream input;
	PrintStream output;
	Thread reader;
	boolean registered	= false;

	Vector motd	= new Vector();		//	stores the message of the day
	Vector roomlist	= new Vector();		//	stores the room list elements
	Vector userlist	= new Vector();		//	stores the user list elements

	public IRCClient()
	{
	}
	
	public IRCClient(IRCListener listener)
	{
		setListener(listener);
	}

	public boolean isConnected()
	{
		return connection != null && registered;
	}

	public void setListener(IRCListener listener)
	{
		this.listener = listener;
	}

	public IRCListener getListener()
	{
		return listener;
	}

	public void connect(String host, int port, String localnick, String password)
	{
		if (isConnected())
			return;
		try
		{
			if (host == null || host.trim().equals(""))
				host = getCodeBase().getHost();
			if (port < 0)
				port = 6667;
			localnick = correctName(localnick);
			this.host = host;
			this.port = port;
			this.localnick = localnick;
			this.password = password;
			connection = new Socket(host, port);
			input = new DataInputStream(connection.getInputStream());
			output = new PrintStream(connection.getOutputStream());
			pass(password);
			nick(localnick);
			user(localnick);
			reader = new Thread(this);
			reader.start();
		}
		catch(Exception e)
		{
			listener.disconnected(host);
			e.printStackTrace(System.out);
		}
	}

	public void disconnect()
	{
		registered = false;
		if (connection != null)
		{
			try
			{
				connection.close();
			}
			catch(Exception e)
			{
			}
			connection = null;
		}
		if (reader != null)
		{
			Thread thread = reader;
			reader = null;
			if (thread.isAlive())
				thread.stop();
		}
	}

	public void sendServer(String raw)
	{
		if (raw != null)
			output.println(raw);
	}

	public void sendMessage(String target, String message)
	{
		if (target != null && message != null)
			privmsg(target, message);
	}

	public void sendNotice(String target, String message)
	{
		if (target != null && message != null)
			notice(target, message);
	}

	public void sendInvitation(String nick, String room)
	{
		if (nick != null && room != null)
			invite(nick, room);
	}

	public void kickMember(String room, String member, String reason)
	{
		if (room == null || member == null)
			return;
		output.println("KICK " + correctRoomname(room) + " " + member + (reason != null ? " :" + reason : ""));
	}

	//	bans specified user
	public void banUser(String nick, String user, String host, String room, boolean cond)
	{
		if (room == null)
			return;
		if (nick == null)
			nick = "*";
		if (user == null)
			user = "*";
		if (host == null)
			host = "*";
		output.println("MODE " + room + " " + (cond?'+':'-') + "b " + nick + "!" + user + "@" + host);
	}

	public String extractNickname(String id)
	{
		if (id == null)
			return null;
		int index = id.indexOf('!');
		if (index < 1)
			return id;
		return id.substring(0, index);
	}

	public String extractUsername(String id)
	{
		if (id == null)
			return null;
		int start = id.indexOf('!');
		int end = id.indexOf('@');
		if (start > 0 && end > 2)
			return id.substring(start + 1, end);
		else
			return extractNickname(id);
	}
	
	public String extractAddress(String id)
	{
		if (id == null)
			return null;
		int index = id.indexOf('@');
		if (index < 1)
			return null;
		return id.substring(index + 1);
	}

	public void setLocalNickname(String newnick)
	{
		if (newnick == null)
			return;
		//	we must anticipate success if not yet registered
		if (!registered)
			localnick = newnick;
		nick(correctName(newnick));
	}

	public String getLocalNickname()
	{
		return localnick;
	}

	public String getLocalRoom()
	{
		Enumeration e = rooms.keys();
		if (e.hasMoreElements())
			return (String)e.nextElement();
		else
			return null;
	}

	//	getVersion is overridden by IRCApplet class used for "Script Chat"
	public String getVersion()
	{
		return IRCClient.version;
	}

	public void setVersion(String version)
	{
		IRCClient.version = version;
	}

	public String getLocalRooms(String delim)
	{
		if (delim == null)
			delim = ",";
		return toString(rooms.keys(), delim);
	}

	public void requestMemberList(String room)
	{
		if (room != null)
			names(room);
	}

	public void requestRoomList()
	{
		listRooms();
	}

	public void requestUserList()
	{
		who();
	}

	public String getTopic(String roomname)
	{
		return roomname != null ? (String)topics.get(roomname) : null;
	}

	public void setTopic(String room, String topic)
	{
		if (room != null && topic != null)
			topic(room, topic);
	}

	public boolean isModerated(String roomname)
	{
		return roomname != null ? (moderated.get(roomname) != null) : false;
	}

	public void setModerated(String room, boolean mod)
	{
		if (room == null || output == null)
			return;
		output.println("MODE " + correctRoomname(room) + (mod ? " +m" : " -m"));		
	}

	public void joinRoom(String room, String password)
	{
		if (room != null)
			join(correctRoomname(room), password);
	}

	public void partRoom(String room)
	{
		if (room != null)
			part(correctRoomname(room));
	}

	public void partAllRooms()
	{
		Enumeration e = rooms.keys();
		while (e.hasMoreElements())
		{
			String room = (String)e.nextElement();
			partRoom(room);
		}
	}

	public void setHost(String user, String room, boolean host)
	{
		if (user != null && room != null && output != null)
			output.println("MODE " + correctRoomname(room) + (host ? " +o " : " -o ") + correctName(user));
	}

	public void setSpeaker(String user, String room, boolean speaker)
	{
		if (user != null && room != null && output != null)
			output.println("MODE " + correctRoomname(room) + (speaker ? " +v " : " -v ") + correctName(user));
	}

	public boolean isHost(String user, String room)
	{
		if (user == null || room == null)
			return false;
		Vector members = getMembers(room);
		if (members == null)
			return false;
		if (user.charAt(0) == '@' || user.charAt(0) == '+')
			user = user.substring(1);
		Enumeration e = members.elements();
		while (e.hasMoreElements())
		{
			String member = e.nextElement().toString();
			if (member.endsWith(user))
			{
				if (member.startsWith("@"))
					return true;
				else
					return false;
			}
		}
		return false;
	}

	public boolean isSpeaker(String user, String room)
	{
		if (user == null || room == null)
			return false;
		Vector members = getMembers(room);
		if (!isModerated(room))
			return true;
		if (members == null)
			return false;
		if (user.charAt(0) == '@' || user.charAt(0) == '+')
			user = user.substring(1);
		Enumeration e = members.elements();
		while (e.hasMoreElements())
		{
			String member = e.nextElement().toString();
			if (member.endsWith(user))
			{
				if (member.startsWith("+"))
					return true;
				else
					return false;
			}
		}
		return false;
	}

	public boolean isMember(String user, String room)
	{
		if (user == null || room == null)
			return false;
		Vector members = getMembers(correctRoomname(room));
		if (members == null)
			return false;
		if (user.charAt(0) == '@' || user.charAt(0) == '+')
			user = user.substring(1);
		Enumeration e = members.elements();
		while (e.hasMoreElements())
		{
			String member = e.nextElement().toString();
			if (member.endsWith(user))
				return true;
		}
		return false;
	}

	///////////////////////////
	//	internal methods
	///////////////////////////

	protected static String SPACECOLONCRLF = " :\r\n";
	protected static String CRLF = "\r\n";
	protected static String SPACE = " ";
	protected static String COLON = ":";

	protected void process(String line)
	{
		if (line == null)
			return;
		StringTokenizer st = new StringTokenizer(line, SPACECOLONCRLF, true);
		String from = null;
		String command = null;
		Vector vector = new Vector();
		if (!st.hasMoreTokens())
			return;
		String firstToken = st.nextToken();
		if (firstToken.equals(COLON))
		{
			if (!st.hasMoreTokens())
				return;
			from = st.nextToken();
			while (st.hasMoreTokens())
			{
				command = st.nextToken();
				if (!command.equals(SPACE))
					break;
			}
		}
		else
		{
			command = firstToken;
		}
		while (st.hasMoreTokens())
		{
			String token = st.nextToken();
			if (!token.equals(SPACE))
			{
				if (token.equals(COLON))
				{
					try
					{
						String lastToken = st.nextToken(CRLF);
						vector.addElement(lastToken);
						//	strip out remaining tokens "CRLF"
						while (st.hasMoreTokens())
							st.nextToken();
					}
					catch(Exception ne)
					{
						vector.addElement("");
					}
				}
				else
				{
					vector.addElement(token);
				}
			}
		}
		if (from == null)
			from = host;
		command = command.toUpperCase();
		String[] parameters = toArray(vector);
		try
		{
			int reply = Integer.parseInt(command);
			processReply(from, reply, parameters);
		}
		catch(NumberFormatException e)
		{
			processCommand(from, command, parameters);
		}
		catch(Exception e)
		{
		//	System.out.println("Error processing command:");
		//	System.out.println(line);
		//	e.printStackTrace(System.out);
		}
	}

	protected void processCommand(String from, String command, String[] parameters)
	{
		String lastParam = (parameters != null)?parameters[parameters.length-1]:"";
		String fromnick = extractNickname(from);
		if (command.equals("PRIVMSG"))
		{
			String target = parameters[0];
			String message = lastParam;
			if (message.charAt(0) == CTCPCHAR)
			{
				if (message.length() >= CTCPVERSION.length() && message.substring(0, CTCPVERSION.length()).equalsIgnoreCase(CTCPVERSION))
					notice(fromnick, CTCPVERSION + " " + getVersion() + CTCPCHAR);
				else if (message.length() >= CTCPPING.length() && message.substring(0, CTCPPING.length()).equalsIgnoreCase(CTCPPING))
					notice(fromnick, message);
				else if (message.length() >= CTCPTIME.length() && message.substring(0, CTCPTIME.length()).equalsIgnoreCase(CTCPTIME))
					notice(fromnick, CTCPTIME + " " + new Date() + CTCPCHAR);
			}
			else
			{
				listener.messageArrived(fromnick, target, message, from);
			}
		}
		else if (command.equals("NOTICE"))
		{
			String target = parameters[0];
			String message = lastParam;
			if (motd.isEmpty())	//	keezer, NOTICE message.
				motd.addElement(message);
			else
				listener.messageArrived(fromnick, target, message, from);
		}
		else if (command.equals("PING"))
		{
			System.out.println("Pong " + getLocalRoom() + ":" + localnick);
			output.println("PONG " + host);
		}
		else if (command.equals("JOIN"))
		{
			String roomname = parameters[0];
			Vector members = getMembers(roomname);
			if (members == null)
				members = createMembers(roomname);
			members.addElement(fromnick);
			listener.memberJoined(roomname, fromnick, from);
		}
		else if (command.equals("PART"))
		{
			String roomname = parameters[0];
			if (fromnick.equals(localnick))
				removeMembers(roomname);
			else
				removeMember(getMembers(roomname), fromnick);
			listener.memberParted(roomname, fromnick);
		}
		else if (command.equals("QUIT"))
		{
			removeMember(fromnick);
			if (fromnick.equals(localnick))
				rooms.clear();
			listener.memberParted(host, fromnick);
		}
		else if (command.equals("KICK"))
		{
			String roomname = parameters[0];
			String kicked = parameters[1];
			String comment = (parameters.length > 2)?parameters[2]:null;
			if (kicked.equals(localnick))
				removeMembers(roomname);
			else
				removeMember(getMembers(roomname), kicked);
			listener.memberKicked(roomname, kicked, fromnick, comment);
		}
		else if (command.equals("NICK"))
		{
			if (parameters.length > 0)
			{
				String oldnick = fromnick;
				String newnick = parameters[0];
				Enumeration e = rooms.keys();
				while (e.hasMoreElements())
				{
					Vector members = getMembers(e.nextElement().toString());
					//	change his name, but keep @ or + if present
					String status = removeMember(members, oldnick);
					//	if user was present, put him back in with status
					if (status != null)
						members.addElement(status + newnick);
				}
				if (oldnick.equals(localnick))
					localnick = newnick;
				listener.nickChanged(oldnick, newnick, from);
			}
		}
		else if (command.equals("MODE"))
		{
			String room = parameters[0];
			String mode = parameters[1];
			String user = parameters.length > 2 ? parameters[2] : null;
			modeChange(room, user, mode, null);
		}
		else if (command.equals("TOPIC"))
		{
			String roomname = parameters[0];
			String topic = lastParam;
			modeChange(roomname, null, null, topic);
		}
		else if (command.equals("INVITE"))
		{
			if (parameters.length > 1)
				listener.invitationArrived(fromnick, parameters[1]);
		}
	}

	protected void processReply(String from, int reply, String[] parameters)
	{
		if(reply > 399)
		{
			processError(from, reply, parameters);
			return;
		}
		String lastParam = (parameters != null) ? lastParam = parameters[parameters.length-1] : null;
		switch(reply)
		{
		case 321:	//	RPL_LISTSTART
			//	should clear room lists here.
			roomlist.removeAllElements();
			break;
		case 322:	//	RPL_LIST
			if (parameters.length > 2)
			{
				String room = parameters[1];
				String count = parameters[2];
				String topic = parameters.length > 3 ? parameters[3] : "";
				String rct = room + SEPARATOR + count + SEPARATOR + topic;
				insertElementSorted(rct, roomlist);
			}
			break;
		case 323:	//	RPL_LISTEND
			sendRoomList();
			break;
		case 324:	//	RPL_CHANNELMODEIS
			if (parameters.length > 2)
			{
				String room = parameters[1];
				String mode = parameters[2];
				modeChange(parameters[1], null, parameters[2], null);
			}
			break;
		case 352:	//	RPL_WHOREPLY
			if (parameters.length > 4)
			{
				String nick = parameters[5];
				String user = parameters[2];
				String room = parameters[1];
				String host = parameters[3];
				String nurh = nick + SEPARATOR + user + SEPARATOR + room + SEPARATOR + host;
				insertElementSorted(nurh, userlist);												
			}
			break;
		case 315:	//	RPL_ENDOFWHO
			sendUserList();
			break;
		case 353:	//	RPL_NAMREPLY
			if (parameters.length >= 2)
			{
				StringTokenizer st = new StringTokenizer(lastParam, " \r\n");
				while (st.hasMoreTokens())
					temporary.put(st.nextToken(), this);
			}
			break;
		case 366:	//	RPL_ENDOFNAMES
			if (parameters.length >= 2)
			{
				String room = parameters[parameters.length - 2];
				Vector members = new Vector();
				synchronized (temporary)
				{
					Enumeration e = temporary.keys();
					while (e.hasMoreElements())
						members.addElement(e.nextElement());
					temporary.clear();
				}
				rooms.put(room, members);
				sendMemberList(room);
			}
			break;
		case 375:	//	RPL_MOTDSTART
			motd.removeAllElements();
			break;
		case 372:	//	RPL_MOTD
			motd.addElement(lastParam);
			break;
		case 376:	//	RPL_ENDOFMOTD
			registered = true;
			listener.connected(host, toArray(motd));
			break;
		default:
			// do nothing
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
			//	clear the message of the day
			motd.removeAllElements();
			registered = true;
			listener.connected(host, toArray(motd));
			break;
		case 432:	//	ERR_ERRONEUSNICKNAME
		case 433:	//	ERR_NICKNAMEINUSE
		case 436:	//	ERR_NICKCOLLISION
			String badnick = parameters[parameters.length - 2];
			listener.invalidNick(badnick);
			break;
		default:
			break;
		}
	}

	protected void addMember(Vector v, String nick)
	{
		removeMember(v, nick);
		v.addElement(nick);
	}

	protected void removeMember(String nick)
	{
		Enumeration e = rooms.elements();
		while (e.hasMoreElements())
		{
			Object object = e.nextElement();
			if (object instanceof Vector)
				removeMember((Vector)object, nick);
		}
	}

	//	removes a user from a room
	//	returns
	//	"@"  if user was a host in room
	//	"+"  if user was a speaker in room
	//	""   if user was a member of room
	//	null if user was not in room
	protected String removeMember(Vector members, String nick)
	{
		if (members == null || nick == null)
			return null;
		Enumeration e = members.elements();
		while (e.hasMoreElements())
		{
			String member = (String)e.nextElement();
			if (sameNick(nick, member))
			{
				members.removeElement(member);
				if (member.startsWith("@") || member.startsWith("+"))
					return member.substring(0, 1);
				else
					return "";
			}
		}
		return null;
	}

	protected boolean sameNick(String nick1, String nick2)
	{
		if (nick1.startsWith("@"))
			nick1 = nick1.substring(1);
		if (nick2.startsWith("@"))
			nick2 = nick2.substring(1);
		return nick1.equalsIgnoreCase(nick2);
	}

	protected Vector getMembers(String roomname)
	{
		return (Vector)rooms.get(roomname);
	}

	protected Vector createMembers(String roomname)
	{
		Vector members = new Vector();
		rooms.put(roomname, members);
		return members;
	}

	protected void removeMembers(String roomname)
	{
		rooms.remove(roomname);
		moderated.remove(roomname);
	}

	protected void pass(String password)
	{
		if (password != null && output != null)
			output.println("PASS " + password);
	}

	protected void nick(String nick)
	{
		if (output != null)
			output.println("NICK " + nick);
	}

	protected void user(String nick)
	{
		if (output != null)
			output.println("USER " + nick + " " + nick + " " + nick + " " + nick);
	}

	protected void join(String room, String password)
	{
		//	mode first, so we will know whether or not it is moderated
		if (output != null)
		{
			output.println("MODE " + room);
			output.println("TOPIC " + room);
			output.println("JOIN " + room + (password != null ? " " + password : ""));
		}
	}

	protected void privmsg(String target, String message)
	{
		if (output != null)
			output.println("PRIVMSG " + target + " :" + message);
	}

	protected void notice(String target, String message)
	{
		if (output != null)
			output.println("NOTICE " + target + " :" + message);
	}

	protected void who()
	{
		if (output != null)
			output.println("WHO");
	}

	protected void topic(String room, String topic)
	{
		if (output != null)
			output.println("TOPIC " + room + " :" + topic);
	}

	protected void part(String room)
	{
		if (output != null)
			output.println("PART " + room);
	}

	protected void names(String room)
	{
		if (output != null)
			output.println("NAMES" + (room != null ? " " + room : ""));
	}

	protected void invite(String nick, String room)
	{
		if (output != null)
			output.println("INVITE " + nick + " " + room);
	}

	protected void listRooms()
	{
		if (output != null)
			output.println("list");
	}

	protected void sendMemberList(String room)
	{
		Vector all = getMembers(room);
		if (all == null)
			return;
		//	prevent Vector access while processing
		Vector hosts = new Vector();
		Vector speakers = new Vector();
		boolean moderated = isModerated(room);
		Vector observers = moderated ? new Vector() : speakers;
		Enumeration e = all.elements();
		while (e.hasMoreElements())
		{
			String member = e.nextElement().toString();
			if (member.startsWith("@"))
				insertElementSorted(member.substring(1), hosts);
			else if (member.startsWith("+"))
				insertElementSorted(member.substring(1), speakers);
			else
				insertElementSorted(member, observers);
		}
		listener.memberList(room, toArray(hosts), toArray(speakers), toArray(moderated ? observers : new Vector()));
	}

	protected void sendRoomList()
	{
		Vector roomlist = this.roomlist;
		int length = roomlist.size();
		String[] titles = new String[length];
		String[] counts = new String[length];
		String[] topics = new String[length];
		for (int i = 0; i < length; i++)
		{
			String nct = (String)roomlist.elementAt(i);
			int first = nct.indexOf(SEPARATOR);
			int second = nct.indexOf(SEPARATOR, first + 1);
			titles[i] = nct.substring(0, first);
			counts[i] = nct.substring(first + 1, second);
			topics[i] = nct.substring(second + 1);
		}
		roomlist.removeAllElements();
		listener.roomList(titles, counts, topics);
	}

	protected void sendUserList()
	{
		Vector userlist = this.userlist;
		int length = userlist.size();
		String[] nicks = new String[length];
		String[] users = new String[length];
		String[] rooms = new String[length];
		String[] hosts = new String[length];
		for (int i = 0; i < length; i++)
		{
			String nurh	= (String)userlist.elementAt(i);
			int first	= nurh.indexOf(SEPARATOR);
			int second	= nurh.indexOf(SEPARATOR, first + 1);
			int third	= nurh.indexOf(SEPARATOR, second + 1);
			nicks[i]	= nurh.substring(0, first);
			users[i]	= nurh.substring(first + 1, second);
			rooms[i]	= nurh.substring(second + 1, third);
			hosts[i]	= nurh.substring(third + 1);
		}
		userlist.removeAllElements();
		listener.userList(nicks, users, rooms, hosts);
	}

	protected void modeChange(String roomname, String user, String mode, String topic)
	{
		StringTokenizer st = new StringTokenizer(mode);
		mode = st.nextToken();
		if (topic != null)
			topics.put(roomname, topic);
		if (mode.indexOf('m') >= 0)
		{
			if (mode.indexOf('+') >= 0)
				moderated.put(roomname, roomname);
			else
				moderated.remove(roomname);
		}
		listener.roomStatusChanged(roomname, isModerated(roomname), getTopic(roomname));
	}

	protected static String correctRoomname(String rn)
	{
		rn = correctName(rn);
		if (!(rn.startsWith("#") || rn.startsWith("&") || rn.startsWith("%")))
				rn = "#" + rn;
		return rn;
	}

	protected static String correctName(String nick)
	{
		return nick.trim().replace(' ', '_');
	}

	protected static String[] toArray(Vector v)
	{
		if (v == null || v.isEmpty())
			return new String[0];
		String[] array = new String[v.size()];
		for (int i = 0; i < array.length; i++)
			array[i] = (String)v.elementAt(i);
		return array;
	}

	protected static int insertElementSorted(Object o, Vector v)
	{
		String value = o.toString().toUpperCase();
		int index;
		for (index = 0; index < v.size(); index++)
		{
			String compare = v.elementAt(index).toString().toUpperCase();
			int dif = compare.compareTo(value);
			if (dif > 0)
			{
				v.insertElementAt(o, index);
				return index;
			}
		}
		//	if not inserted, then place at end
		v.addElement(o);
		return index;
	}

	private static String toString(Vector v, String delim)
	{
		return toString(v.elements(), delim);
	}

	private static String toString(Enumeration e, String delim)
	{
		if (!e.hasMoreElements())
			return "";
		StringBuffer sb = new StringBuffer(e.nextElement().toString());
		while (e.hasMoreElements())
			sb.append(e.nextElement() + delim);
		return sb.toString();
	}

	public final void run()
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
					process(line);
				}
				catch(Exception e)
				{
				//	e.printStackTrace(System.out);
				}
			}
		}
		catch(Exception e)
		{
		//	e.printStackTrace(System.out);
			listener.disconnected(host);
			disconnect();
		}
	}
}

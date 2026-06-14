////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	HIClient maintains an IRCClient object which 
//	is connected to an IRC server on an html based clients
//	behalf.  Their is one unique HIClient running
//	for each html based client.
//	Web-based update requests are rerouted by the HIServer 
//	to the correct HIClient based upon the "tag" parameter.
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.io.*;
import java.net.*;
import java.util.*;

public class HIClient implements IRCListener
{
	protected static String ROOM	= "room";
	protected static String NICK	= "nick";
	protected static String HOST	= "host";
	protected static String MESSAGE	= "message";
	protected static String TARGET	= "target";
	protected static String PASSWORD= "password";
	protected static String EMPTY	= "";
	protected static String COMIC_FORMAT = "(#G";
	protected static String APPEARS_AS = "# Appears";

	//	when "reload" whispered to self, will update template file by reloading from server if necessary
	protected static String RELOAD  = "reload";

	//	amount of milliseconds to wait for a message from IRCCLient
	protected static long	WAITTIMEOUT = 30000;

	protected HIServer		server;
	protected IRCClient		client;
	protected Vector		newChat = new Vector();	//	messages the user has not yet seen
	protected Vector		oldChat = new Vector();	//	messages the user has already seen
	protected IRCTemplate		template;		//	template to use in building an html file
	protected String 		tag;
	protected long			lastRequest = System.currentTimeMillis();
	protected Socket		socket;		//	the latest socket to access this HIClient

	//	cache of room members;
	protected String[]		hosts;
	protected String[]		speakers;
	protected String[]		observers;

	public HIClient(HIServer server, String tag)
	{
		this.server = server;
		this.tag = tag;
	}

	public void handle(Socket socket, HTTPRequest request)
	{
		this.socket = socket;
		//	HTTP request and header has already been read the first time
		try
		{
			DataInputStream input = new DataInputStream(socket.getInputStream());
			DataOutputStream output = new DataOutputStream(socket.getOutputStream());
			
			lastRequest = System.currentTimeMillis();

			String file = request.getFile();
			if (file == null)
				return;

			//	first character '/' is trimmed, since server does not store it that way
			IRCTemplate newTemplate = server.getTemplate(file.substring(1));

			if (newTemplate != null)
				template = newTemplate;
			if (template == null)
				return;

			String room = request.get(ROOM, null);
			String nick = request.get(NICK, null);
			if (isEmpty(room) || isEmpty(nick))
				return;
			if (room.charAt(0) != '#')
				room = '#' + room;

			if (client == null)
				client = new IRCClient(this);

			if (!client.isConnected())
			{
				String host = request.get("host", null);
				if (host == null)
					host = server.getIRCHost();
				int port = request.get("port", -1);
				if (port == -1)
					port = server.getIRCPort();
				client.connect(host, port, nick, null);
			}

			if (!nick.equals(getNick()))
			{
				client.setLocalNickname(nick);
				waitForClient(WAITTIMEOUT);
			}
			String currentRoom = getRoom();
			if (currentRoom == null || !currentRoom.equalsIgnoreCase(room))
			{
				client.partAllRooms();
				clearRoomMembers();
				client.joinRoom(room, request.get(PASSWORD, null));
			}

			if (!roomMembersLoaded())
			{
				waitForClient(WAITTIMEOUT);
			}

			//	send message if queued
			String message = request.get(MESSAGE, null);
			String target = request.get(TARGET, null);
			if (target != null && message != null && !message.equals(EMPTY))
			{
				//	trim off preceding '@', '+', or '-'
				int first = target.charAt(0);
				if (first  == '@' || first == '+' || first == '-')
					target = target.substring(1);
				if (target.equals(nick) && message.equals(RELOAD))
				{
					template.update();
				}
				else
				{
					client.sendMessage(target, message);
					boolean whisper = !target.endsWith(room);
					addChat(template.messageArrived(getNick(), target, message, whisper));
				}
			}
	
			currentRoom = getRoom();
			if (currentRoom == null)
				currentRoom = IRCClient.correctName(room);	//	caused by invalid nick
			String page = template.buildPage(tag, getNick(), currentRoom, newChat, oldChat, hosts, speakers, observers);
			reply(page, request.keepAlive(), output, request.getVersion());
		
		}
		catch(Exception e)
		{
			e.printStackTrace(System.out);
		}
		//	Socket will automatically be closed when this method completes
		//	and HIServer will call "disconnect()" on "this"
	}

	public void disconnect()
	{
		if (client != null)
		{
			client.disconnect();
			client = null;
		}
		if (socket != null)
		{
			try
			{
				socket.close();
			}
			catch(Exception e)
			{
			}
			socket = null;
		}
	}

	public long getIdleTime()
	{
		long time = System.currentTimeMillis();
		long idle = time - lastRequest;
		return idle;
	}

	public synchronized void waitForClient(long timeout)
	{
		try
		{
			wait(timeout);
		}
		catch(Exception e)
		{
			e.printStackTrace(System.out);
		}
	}

	public synchronized void notifyHandler()
	{
		notifyAll();
	}

	public static void reply(String page, boolean keepalive, DataOutputStream output, String version) throws IOException
	{
		reply(toByteArray(page), keepalive, output, version);
	}

	public static void reply(byte[] page, boolean keepalive, DataOutputStream output, String version) throws IOException
	{
		try
		{
			synchronized(output)
			{
				output.writeBytes(version + " 200 OK\r\n");
				output.writeBytes("Content-Type: text/html\r\n");
				output.writeBytes("Content-Length: " + page.length + "\r\n");
				if (keepalive)
					output.writeBytes("Connection: Keep-Alive\r\n");
				output.writeBytes("\r\n");

				output.write(page);
			}
		}
		catch(Exception e)
		{
			println("Exception while replying");
			e.printStackTrace(System.out);
			throw new IOException(e.toString());
		}
	}

	public static byte[] toByteArray(String s)
	{
		int length = s.length();
		byte[] array = new byte[length];
		for (int i = 0; i < length; i++)
			array[i] = (byte)s.charAt(i);
		return array;
	}

	public void addChat(String line)
	{
		if (line != null && !line.equals(EMPTY))
			newChat.addElement(line);
	}

	public String getNick()
	{
		if (client != null)
			return client.getLocalNickname();
		else
			return null;
	}

	public String getRoom()
	{
		if (client == null)
			return null;
		return client.getLocalRoom();
	}

	public boolean roomMembersLoaded()
	{
		return hosts != null || speakers != null || observers != null;
	}

	public void clearRoomMembers()
	{
		hosts = null;
		speakers = null;
		observers = null;
	}

	public void storeRoomMembers(String[] hosts, String[] speakers, String[] observers)
	{
		this.hosts = hosts;
		this.speakers = speakers;
		this.observers = observers;
	}

	public void connected(String host, String[] motd)
	{
		String line = template.connected(host, motd);
		addChat(line);
	}

	public void disconnected(String host)
	{
	}

	public void invalidNick(String badnick)
	{
		String line = template.invalidNick(badnick);
		addChat(line);
		notifyHandler();
	}

	public void nickChanged(String oldnick, String newnick, String identity)
	{
		String line = template.nickChanged(oldnick, newnick);
		addChat(line);
		client.requestMemberList(getRoom());
		//	new member list will notify handler
	}

	public void roomStatusChanged(String room, boolean moderated, String topic)
	{
		client.requestMemberList(getRoom());
	}

	public void invitationArrived(String source, String room)
	{
		String line = template.invitationArrived(source, room);
		addChat(line);
	}

	public void messageArrived(String source, String target, String message, String identity)
	{
		if (!client.isHost(source, getRoom()))
		{
			message = message.replace('<', ' ');
			message = message.replace('>', ' ');
		}
		//	remove comic chat formating strings
		if (message.startsWith(APPEARS_AS))
			return;
		if (message.startsWith(COMIC_FORMAT))
			message = message.substring(message.indexOf(')') + 1);
		String line = template.messageArrived(source, target, message, target.equals(getNick()));
		addChat(line);
	}

	public void memberJoined(String room, String nick, String identity)
	{
		String line = template.memberJoined(room, nick);
		addChat(line);
		client.requestMemberList(getRoom());
	}

	public void memberParted(String room, String nick)
	{
		String line = template.memberParted(room, nick);
		addChat(line);
		client.requestMemberList(getRoom());
	}
	
	public void memberKicked(String room, String nick, String kicker, String reason)
	{
		String line = template.memberKicked(room, nick, kicker, reason);
		addChat(line);
		client.requestMemberList(getRoom());
	}

	public void memberList(String room, String[] hosts, String[] speakers, String[] observers)
	{
		storeRoomMembers(hosts, speakers, observers);
		notifyHandler();
	}

	public void userList(String[] nicks, String[] users, String[] rooms, String[] hosts)
	{}

	public void roomList(String[] rooms, String[] count, String[] topic)
	{}

	public final boolean isEmpty(String s)
	{
		return server.isEmpty(s);
	}

	public static void println(Object o)
	{
		System.out.println(o);
	}
}

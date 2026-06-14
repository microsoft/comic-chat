////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	ChatClient interface
//	will be implemented by IRCClient and others later
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.util.*;

public interface ChatClient
{
	/**
	*	the chat client notify the specified ChatListener
	*	when messages arrive, etc.
	*/
	public void setChatListener(ChatListener listener);
	/**
	*	Sets the users nickname to the specified nickname
	*	Method must be called before calling connect
	*/
	public void setNickname(String nickname);
	/**
	*	Sets the users username to the specified username
	*	Method must be called before calling connect
	*/
	public void setUsername(String username);
	/**
	*	Sets the users realname to the specified username
	*	Method must be called before calling connect
	*/
	public void setRealname(String realname);
	/**
	*	this is a request to connect to a server,
	*	port=-1 indicates use default port, password may be null
	*	returns true if we connected successfully
	*/
	public void connect(String server, int port, String password);
	/**
	*	diconnects from the specified webserver
	*/
	public void disconnect();
	/**
	*	checks to see if we are connected to a chat server
	*/
	public boolean connected();
	/**
	*	retrieves a list of all channel names
	*/
	public void getRooms();
	/**
	*	get detailed information on specified channel
	*	this information should be cached within the class
	*	so that individual requests to the server are not needed
	*/
	public Properties getRoomProperties(String name);
	/**
	*	get the nicknames of all users on the server
	*/
	public String[] getUsers();
	/**
	*	get the nicknames of all users on channel
	*/
	public String[] getUsers(String channel);
	/**
	*	get detailed information about specified user
	*/
	public Properties getUserProperties(String nickname);
	/**
	*	join a channel
	*/
	public void join(String channel, String password);
	/**
	*	depart from channel
	*/
	public void part(String channel);
	/**
	*	send a string to a specified user or channel
	*/
	public void send(String target, String message, Properties props);
	/**
	*	send a string to all specified users
	*/
	public void send(String[] target, String message, Properties props);
	/**
	*	request version information from specified user
	*/
	public void version(String target);
	/**
	*	request the lag time for a round trip message to a chat user
	*/
	public void lagTime(String target);
	/**
	*	request the local time of a chat user
	*/
	public void localTime(String target);
	/**
	*	request the identity of a chat user
	*/
	public void identity(String target);
	/**
	*	request a user profile
	*/
	public void profile(String target);
	/**
	*	promote a user to operator
	*/
	public void addOperator(String user);
	/**
	*	demote a user from operator to speaker
	*/
	public void removeOperator(String user);
	/**
	*	promote a user to speaker
	*/
	public void addSpeaker(String user);
	/**
	*	demote a user to spectator
	*/
	public void removeSpeaker(String user);
	/**
	*	gets the identity of a user immediately if known
	*/
	public String getIdentity(String user);
	/**
	*	kicks the user off of the room
	*/
	public void kick(String room, String user, String reason);
	/**
	*	bans or unbans a user from the room
	*/
	public void setBanmask(String room, String mask, boolean ban);
	/**
	*	get the banlist for a room
	*/
	public String[] getBanlist(String room);
}

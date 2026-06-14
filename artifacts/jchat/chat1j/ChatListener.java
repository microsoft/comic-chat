////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	ChatListener is the interface that must be implemented
//	by a Chat Program User Interface class.
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.util.*;

public interface ChatListener
{
	/**
	*	notifies the listener that the list of rooms is starting.
	*/
	public void roomListStart();
	/**
	*	notifies the listener to add another room to its list.
	*/
	public void roomListAdd(String name, int users, String topic);
	/**
	*	notifies the listener that the end of the list has been reached.
	*/
	public void roomListEnd();
	/**
	*	notifies the user that a room has been successfully entered.
	*/
	public void roomEntered(String name, Properties p);
	/**
	*	notification that user cannot enter specified room for reason.
	*/
	public void roomEntryFailed(String room, String reason);
	/**
	*	notification of room topic or mode change.
	*/
	public void roomUpdate(String name, Properties p);
	/**
	*	client has connected to server, stats = server info, message is message of the day.
	*/
	public void connected(String host, String stats, String message);
	/**
	*	client has been disconnected from server
	*/
	public void disconnected(String message);
	/**
	*	client has failed to connect for following reason.
	*/
	public void connectFailed(String reason);
	/**
	*	this is called after a room is joined for each member already in the room.
	*/
	public void userListAdd(String nickname, String room, Properties p);
	/**
	*	The specified user has joined specified room.
	*	@param p User properties such as o - operator, v - speaker
	*/
	 public void userJoining(String nickname, String room, Properties p);
	/**
	*	The specified user is leaving specified channel.
	*/
	public void userLeaving(String nickname, String room);
	/**
	*	a users name or properties are changing.
	*/
	public void userUpdate(String oldname, String newname, Properties p);
	/**
	*	A new message has arrived from specified source.
	*	standard properties variables:
	*	font.name=Courier
	*	font.size=10
	*	font.style=1		Note: 0=REGULAR, 1=BOLD, 2=ITALIC, 3=BOLD+ITALIC
	*	font.color=ff0000
	*/
	public void messageArrived(String target, String source, String message, Properties p);
	/**
	*	Special message from this client.
	*	IRC NOTICE commands are delivered as a systemMessage
	*/
	public void systemMessage(String message);
	/**
	*	Generic error message from the client.
	*/
	public void errorMessage(String message);
	/**
	*	the user has been KICKed from a room
	*/
	public void removedFromRoom(String by, String room, String reason);
	/**
	*	Error message indicating nickname must be changed immediately
	*/
	public void invalidNickname(String nickname);
	/**
	*	Message indicating a password is required to enter specified room
	*/
	public void invalidPassword(String room);
	/**
	*	Localizes a specified string.
	*	This is provided so that clients can get strings from the user interface portion.
	*/
	public String localize(String phrase);
}
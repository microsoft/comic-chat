////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	This interface must be implemented by any class
//	which wishes to use an IRCClient.
//	these methods allow the IRCClient provide
//	asynchronous notification of IRC events and messages.
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////

public interface IRCListener
{
	public void connected(String host, String[] motd);
	public void disconnected(String host);
	public void invalidNick(String badnick);
	public void nickChanged(String oldnick, String newnick, String identity);
	public void roomStatusChanged(String room, boolean moderated, String topic);
	public void invitationArrived(String source, String room);
	public void messageArrived(String source, String target, String message, String identity);
	public void memberJoined(String room, String nick, String identity);
	public void memberParted(String room, String nick);
	public void memberKicked(String room, String nick, String kicker, String reason);
	public void memberList(String room, String[] hosts, String[] speakers, String[] observers);
	public void userList(String[] nicks, String[] users, String[] rooms, String[] hosts);
	public void roomList(String[] rooms, String[] count, String[] topic);
}
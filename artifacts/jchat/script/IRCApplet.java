////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	IRCApplet is an extension of IRCClient that allows
//	a browser based scripting engine, such as javascript,
//	to access an IRCClient, and it also implements
//	IRCListener, and stores the messages, allowing
//	the javascript to poll it for new messages.
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.applet.Applet;
import java.util.Vector;

public class IRCApplet extends IRCClient implements IRCListener
{

	protected Vector queue = new Vector();

	public String getVersion(){return "Microsoft Script Chat 1.0 build " + BUILD;}

	public IRCApplet()
	{
		setListener(this);
	}

	public String nextMethod()
	{
		if (queue.isEmpty())
			return null;
		String method = queue.elementAt(0).toString();
		queue.removeElementAt(0);
		return method;
	}

	protected void addMethod(String name, Object a, Object b, Object c, Object d)
	{
		char ms = '{';	//	main separator
		char mr = '[';	//	main replacement
		char as = '}';	//	array separator
		char ar = ']';	//	array replacement
		Object[] method = {name, a, b, c, d};
		StringBuffer buffer = new StringBuffer();
		for (int i = 0; i < method.length; i++)
		{
			Object object = method[i];
			if (object == null)
				break;
			if (object instanceof Object[])
			{
				Object[] array = (Object[])object;
				StringBuffer sb = new StringBuffer();
				for (int j = 0; j < array.length; j++)
					sb.append(as + array[j].toString().replace(as, ar));
				object = sb.toString();
			}
			else
			{
				object = object.toString().replace(as, ar);
			}
			buffer.append(ms + object.toString().replace(ms, mr));
		}
		queue.addElement(buffer.toString());
	}

	public String disableTags(String message)
	{
		message = message.replace('<', ' ');
		message = message.replace('>', ' ');
		return message;
	}

	//	IRCListener implementation
	public void connected(String host, String[] motd)
	{
		addMethod("connected", host, motd, null, null);
	}

	public void disconnected(String host)
	{
		addMethod("disconnected", host, null, null, null);
	}

	public void invalidNick(String badnick)
	{
		addMethod("invalidNick", badnick, null, null, null);
	}

	public void nickChanged(String oldnick, String newnick, String identity)
	{
		addMethod("nickChanged", oldnick, newnick, null, null);
	}

	public void roomStatusChanged(String room, boolean moderated, String topic)
	{
		addMethod("roomStatusChanged", room, moderated ? "TRUE" : "FALSE", topic, null);
	}

	public void invitationArrived(String source, String room)
	{
		addMethod("invitationArrived", source, room, null, null);
	}

	public void messageArrived(String source, String target, String message, String identity)
	{
		addMethod("messageArrived", source, target, message, null);
	}

	public void memberJoined(String room, String nick, String identity)
	{
		addMethod("memberJoined", room, nick, null, null);
	}

	public void memberParted(String room, String nick)
	{
		addMethod("memberParted", room, nick, null, null);
	}

	public void memberKicked(String room, String nick, String kicker, String reason)
	{
		addMethod("memberKicked", room, nick, kicker, reason);
	}

	public void memberList(String room, String[] hosts, String[] speakers, String[] observers)
	{
		System.out.println("add memberList");
		System.out.println(hosts.length);
		System.out.println(speakers.length);
		System.out.println(observers.length);
		for (int i = 0; i < speakers.length; i++)
			System.out.println(speakers[i]);
		addMethod("memberList", room, hosts, speakers, observers);
	}

	public void userList(String[] nicks, String[] users, String[] rooms, String[] hosts)
	{
		addMethod("userList", nicks, users, rooms, hosts);
	}

	public void roomList(String[] rooms, String[] count, String[] topic)
	{
		System.out.println("add roomList");
		addMethod("roomList", rooms, count, topic, null);
	}

}
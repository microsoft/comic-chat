////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	MSChat creates a new Chatpanel
//	to display in itself.
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////

import java.awt.*;
import java.net.*;
import java.applet.*;

public class MSChat extends Applet implements Runnable
{
	static final int ABOUT = 1, CONNECTDIALOG = 2, ENTER = 3, FONT = 4, ROOM = 5, CONNECT = 6, DISCONNECT = 7, JOIN = 8;

	Thread thread;
	ChatPanel chatPanel;
	int command = 0;	// no command in queue
	String arg1, arg2, arg3;
	
	public MSChat()
	{
		thread = new Thread(Thread.currentThread().getThreadGroup(), this);
		thread.start();
	}
	
	public void init()
	{
		Static.setApplet(this);
		String bgcolor = getParameter("bgcolor");
		setBackground(Static.getParameterColor(this, "bgcolor", Color.lightGray));
		setLayout(new GridLayout(1, 1));
		//	this adds persistence when netscape is resized, and reloads applet
		chatPanel = ChatPanel.getChatPanel(this);
		if (chatPanel != null)
		{
			Container parent = chatPanel.getParent();
			if (parent instanceof Frame)
			{
				Frame frame = (Frame)parent;
				frame.hide();
				frame.dispose();
			}
		}
		else
		{
			URL base = getCodeBase();
			chatPanel = new ChatPanel(this, getBackground());
		}
		chatPanel.setApplet(this);
		add(chatPanel);
	}

	public static void main(String[] args)
	{
		try
		{
			ChatPanel cp = new ChatPanel(null);
			Frame f = new ChatFrame(cp);
			cp.setBackground(Color.lightGray);
			f.show();
		}
		catch(Exception e)
		{
			System.out.println(e);
			e.printStackTrace(System.out);
		}
	}

	public void start()
	{
		Static.setApplet(this);
		if (Static.getParameter(this, "autoconnect", false) && !chatPanel.isConnected())
		{
			String name = chatPanel.nickname;
			if (name == null || name.trim().equals(""))
			{
				name = Static.getParameter(this, "nickname", null);
			}
			if (name == null)
			{
				chatPanel.showNickDialog(true);
				name = chatPanel.nickname;
			}
			if (name != null)
			{
				String room = chatPanel.lastRoom;
				if (room == null || room.trim().equals(""))
					room = Static.getParameter(this, "room", null);
				chatPanel.connect(name, room);
			}
		}
		layout();
	}

	public void stop()
	{
		if (Static.getParameter(this, "autodisconnect", false))
			disconnect();
	}

	public synchronized void connect(String nickname, String room)
	{
		command = CONNECT;
		arg1 = nickname;
		arg2 = room;
		notify();
	}

	public synchronized void disconnect()
	{
		command = DISCONNECT;
		notify();
	}

	public String getNick()
	{
		return chatPanel.nickname;
	}

	public String getRoom()
	{
		Room room = chatPanel.room;
		if (room == null)
			return null;
		else
			return room.getName();
	}

	public synchronized void join(String room, String password)
	{
		command = JOIN;
		arg1 = room;
		arg2 = password;
		notify();
	}

	public void run()
	{
		while (true)
		{
			synchronized(this)
			{
				try
				{
					wait();
				}
				catch(Exception e)
				{
					e.printStackTrace(System.out);
				}
			}
			switch(command)
			{
			case ABOUT:
				chatPanel.showAboutDialog();
				break;
			case CONNECTDIALOG:
				chatPanel.showConnectDialog();
				break;
			case ENTER:
				chatPanel.showEnterRoomDialog();
				break;
			case FONT:
				chatPanel.showFontDialog();
				break;
			case ROOM:
				chatPanel.showRoomList();
				break;
			case CONNECT:
				String nickname = arg1, room = arg2;
				chatPanel.connect(nickname, room == null || room.equals("") ? getParameter("room") : room);
				break;
			case DISCONNECT:
				chatPanel.disconnect();
				break;
			case JOIN:
				String rm = arg1, password = arg2;
				if (password == null || password.trim().equals(""))
					chatPanel.join(rm);
				else
					chatPanel.join(rm, password);		
				break;
			default:
			}
			command = 0;
		}
	}

	public synchronized void showAboutDialog()
	{
		command = ABOUT;
		notify();
	}

	public synchronized void showConnectDialog()
	{
		command = CONNECTDIALOG;
		notify();
	}

	public synchronized void showEnterRoomDialog()
	{
		command = ENTER;
		notify();
	}

	public synchronized void showFontDialog()
	{
		command = FONT;
		notify();
	}

	public synchronized void showRoomList()
	{
		command = ROOM;
		notify();
	}

}
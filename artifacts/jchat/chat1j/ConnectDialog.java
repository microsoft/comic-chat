////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	ConnectDialog is showed before Chat Starts
//	it prompts the user for a nickname, realname
//	and optionally server
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;
import java.net.*;
import java.applet.*;

public class ConnectDialog extends ModalDialog
{
	ChatPanel parent;
	TextField nickname, server, roomname;
	Button cancelButton, okButton;
	String host = "chat.microsoft.com";
	Applet applet;
	
	String OK, CANCEL, NICKNAME, CHAT_SERVER, ROOM, CONNECT;
	
	public void loadStrings()
	{
		OK = get("ok");
		CANCEL = get("cancel");
		NICKNAME = get("nickname");
		CHAT_SERVER = get("chat.server");
		ROOM = get("room");
		CONNECT = get("connect");
	}

	public ConnectDialog(Frame frame, ChatPanel parent, Applet applet)
	{
		super(frame, parent);
		setTitle(CONNECT);
		this.parent = parent;
		this.applet = applet;
		setBackground(parent.getBackground());
		this.setLayout(new GridLayout(1, 1));
		nickname = new TextField(20);
		server = new TextField("chat.microsoft.com");
		roomname = new TextField(20);
		okButton = new Button(OK);
		cancelButton = new Button(CANCEL);
		Panel center = new Panel();
		center.setBackground(getBackground());
		center.setLayout(new GridLayout(4, 2, 8, 4));
		center.add(new Label(NICKNAME));
		center.add(nickname);
		center.add(new Label(CHAT_SERVER));
		center.add(server);
		center.add(new Label(ROOM));
		center.add(roomname);
		center.add(okButton);
		center.add(cancelButton);
		Panel insetPanel = new InsetPanel(8, 6, 4, 6);
		insetPanel.add(center);
		add(insetPanel);		
		if (applet != null)
		{
			// set and lock server value
			URL base = applet.getCodeBase();
			if (base != null)
			{
				StringBuffer buffer = new StringBuffer();
				buffer.append(base.getHost());
				if (buffer.length() > 0)
					host = buffer.toString();
			}
			//	check for default room
		}
		setResizable(false);
	}

	public void show()
	{
		if (host != null)
			server.setText(host);
		if (applet != null)
		{
			String room = Static.getParameter(applet, "room", "");
			if (room == null)
				room = "";
			roomname.setText(room);
		}
		super.show();
	}

	public void ok()
	{
		if (parent != null)
		{
			String nick = nickname.getText();
			String serv = server.getText();
			String room = roomname.getText();
			if (nick == null || nick.trim().equals(""))
			{
				nickname.requestFocus();
				return;
			}
			if (serv == null || serv.trim().equals(""))
			{
				server.requestFocus();
				return;
			}
			hide();
			parent.connect(nick.trim(), serv.trim(), room.trim());
		}
		close();
	}

	public boolean action(Event e, Object arg)
	{
		if (e.target == okButton)
		{
			ok();
			return true;
		}
		else if (e.target == cancelButton)
		{
			close();
			return true;
		}
		return super.action(e, arg);
	}

}
////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	MemberListPanel lists all members in a chat room
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;
import java.util.*;

public class MemberListPanel extends Panel implements MenuListener
{

	//	internationalized variables
	String CHANGE_NICKNAME;
	String GET_VERSION;
	String GET_IDENTITY;
	String GET_PROFILE;
	String LAG_TIME;
	String LOCAL_TIME;
	String HOST;
	String SPEAKER;
	String SPECTATOR;
	String IGNORE;
	String KICK;
	String BAN_UNBAN;

	ElementList list;
	//	the room, that this memberlist is displaying
	Room room = null;
	Label roomLabel = new Label();
	ChatPanel chatPanel;
	//	popupMenu, if one is visible
	PopMenu menu = null;

	public void loadStrings()
	{
		CHANGE_NICKNAME = get("change.nickname");
		GET_VERSION = get("get.version");
		GET_IDENTITY = get("get.identity");
		GET_PROFILE = get("get.profile");
		LAG_TIME = get("lag.time");
		LOCAL_TIME = get("local.time");
		HOST = get("host");
		SPEAKER = get("speaker");
		SPECTATOR = get("spectator");
		IGNORE = get("ignore");
		KICK = get("kick");
		BAN_UNBAN = get("ban.unban");
	}

	public String get(String key)
	{
		return chatPanel.get(key);
	}

	public MemberListPanel(ImageArray icons, ChatPanel chatPanel)
	{
		this(icons, chatPanel, 150);
	}
	
	public MemberListPanel(ImageArray icons, ChatPanel chatPanel, int width)
	{
		this.chatPanel = chatPanel;
		loadStrings();
		setLayout(new BorderLayout());
	//	add("North", roomLabel);
		BarBorderPanel center = new BarBorderPanel(new Insets(6,6,6,6));
		list = new ElementList(icons, new Dimension(width, 300));
		list.setBackground(Color.white);
		list.setMultipleSelections(true);
		center.add(list);
		add("Center", center);
	}

	public Room getRoom()
	{
		return room;
	}

	public synchronized void setRoom(Room room)
	{
		this.room = room;
	//	roomLabel.setText((room == null)?"":room.getName());
		update();
	}

	public String getSelectedItem()
	{
		return list.getSelectedItem();
	}

	public String[] getSelectedItems()
	{
		return list.getSelectedItems();
	}

	//	updates the list view with the latest changes
	public synchronized void update()
	{
		//	remember which users were selected, so we can rehighlight them
		String[] highlights = list.getSelectedItems();
		list.clearElements(false);
		if (room == null)
		{
			list.setScrollbarValues();
			list.repaint();
			return;
		}
		String[] images = {"gavel", "chat", "glasses", "ignored"};
		for (int i = 0; i < 4; i++)
		{
			Enumeration e = room.getGroupVector(i).elements();
			while (e.hasMoreElements())
			{
				String name = e.nextElement().toString();
				list.addItem(images[i], name, false);
			}
		}
		//	now reselect users
		if (highlights != null)
		{
			for (int i = 0; i < highlights.length; i++)
			{
				list.setHighlight(highlights[i], true, false);
			}
		}
		//	hack to avoid flicker jumping etc.
		list.setScrollbarValues();
		list.origin.y = list.vertical.getValue();
		list.repaint();
	}

	public PopMenu buildPopupMenu(String target)
	{
		String localNick = chatPanel.nickname;
		PopMenu menu = new PopMenu(getFrame(), this, target);
		menu.setBackground(chatPanel.getBackground());
		if (localNick.equals(target))
		{
			menu.addMenuItem(CHANGE_NICKNAME);
			menu.addMenuItem(null);
		}
		menu.addMenuItem(GET_IDENTITY);
		menu.addMenuItem(GET_PROFILE);
		menu.addMenuItem(IGNORE);
		menu.addMenuItem(null);
		menu.addMenuItem(GET_VERSION);
		menu.addMenuItem(LAG_TIME);
		menu.addMenuItem(LOCAL_TIME);
		User localUser = room.getUser(localNick);
		User targetUser = room.getUser(target);
		if (targetUser.isOperator())
		{
			menu.setEnabled(IGNORE, false);
		}
		if (localUser.isOperator())
		{
			menu.addMenuItem(null);
			menu.addMenuItem(KICK);
			menu.addMenuItem(BAN_UNBAN);
			menu.addMenuItem(null);
			menu.addMenuItem(HOST);
			menu.addMenuItem(SPEAKER);
			menu.addMenuItem(SPECTATOR);
			if (targetUser.isOperator())
			{
				menu.setDown(HOST, true);
			}
			else if (!room.isModerated() || targetUser.isSpeaker())
			{
				menu.setDown(SPEAKER, true);
			}
			else
			{
				menu.setDown(SPECTATOR, true);
			}
			if (!room.isModerated())
			{
				menu.setEnabled(SPECTATOR, false);
			}
		}
		if (!targetUser.isMSChatUser())
		{
			menu.setEnabled(GET_PROFILE, false);
		}
		if (targetUser.isIgnored())
		{
			menu.setDown(IGNORE, true);
		}
		if (targetUser == localUser)
		{
			menu.setEnabled(IGNORE, false);
		}
		return menu;
	}

	public boolean mouseDown(Event e, int x, int y)
	{
		hidePopupMenu();
		return true;
	}

	public boolean mouseUp(Event e, int x, int y)
	{
	//	hidePopupMenu();
		return true;
	}

	public boolean handleEvent(Event e)
	{
		if (e.id == Event.LIST_SELECT || e.id == Event.LIST_DESELECT)
		{
			if (e.metaDown() || e.shiftDown())
				showPopupMenu((String)e.arg, e.x, e.y);
			else
				chatPanel.entry.requestFocus();
			return true;
		}
		return super.handleEvent(e);
	}

	public void showPopupMenu(String userName, int x, int y)
	{
		// hide the first one
		hidePopupMenu();
		Point absolute = Static.getAbsoluteLocation(list, x, y);
		menu = buildPopupMenu(userName);
		//	this ensures that the menu is visible due to netscape bug
		Static.onScreen(menu);
		menu.show(absolute);
	}

	//	this is called by the PopupMenu
	public void menuSelection(String command, String name)
	{
		if (menu != null && menu.isVisible())
			menu.hide();
		User user = room.getUser(name);
		ChatClient client = chatPanel.getClient();
		if (client == null)
		{
			System.out.println("Null ChatClient in MemberListPanel menuSelection");
			return;
		}
		if (command.equals(CHANGE_NICKNAME))
		{
			chatPanel.showNickDialog(false);
		}
		if (command.equals(GET_VERSION))
		{
			client.version(name);
		}
		else if (command.equals(GET_IDENTITY))
		{
			client.identity(name);
		}
		else if (command.equals(LAG_TIME))
		{
			client.lagTime(name);
		}
		else if (command.equals(LOCAL_TIME))
		{
			client.localTime(name);
		}
		else if (command.equals(IGNORE))
		{
			if (user != null)
			{
				//	toggle ignored
				user.setIgnored(!user.isIgnored());
				//	resort the room to put user in ignored group (or remove)
				room.resort();
				//	update this panel to display status change
				update();
			}
		}
		else if (command.equals(GET_PROFILE))
		{
			client.profile(name);
		}
		else if (command.equals(HOST))
		{
			if (user != null && !user.isOperator())
			{
				client.addOperator(name);
			}
		}
		else if (command.equals(SPEAKER))
		{
			if (user != null && user.isOperator())
			{
				client.removeOperator(name);
			}
			if (room.isModerated())
			{
				client.addSpeaker(name);
			}
		}
		else if (command.equals(SPECTATOR))
		{
			if (user != null && user.isOperator())
			{
				client.removeOperator(name);
			}
			if (room.isModerated())
			{
				client.removeSpeaker(name);
			}
		}
		else if (command.equals(KICK))
		{
			new KickDialog(getFrame(), name, chatPanel);
		}
		else if (command.equals(BAN_UNBAN))
		{
			new BanDialog(getFrame(), name, chatPanel);
		}
		else
		{
			//System.out.println("MenuSelection " + command + " , " + name);
		}
		if (menu != null)
		{
			menu.dispose();
			menu = null;
		}
	}

	public void hidePopupMenu()
	{
		if (menu != null)
		{
			if (menu.isVisible())
				menu.hide();
			menu = null;
		}
	}

	public Frame getFrame()
	{
		if (chatPanel != null)
		{
			return chatPanel.getFrame();
		}
		return null;
	}	

}





////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	ChatPanel is a Panel which contains the User Interface
//	for an entire chat program.  It can be placed in an
//	applet, or in a frame.
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.io.*;
import java.awt.*;
import java.net.*;
import java.util.*;
import java.applet.*;

public class ChatPanel extends Panel implements ChatListener, FontListener
{
	//	static hashtable contains all loaded chatpanels keyed by documentbase#id
	static Hashtable chatPanels = new Hashtable();
	
	//	ChatClient handles all of the nitty gritty protocol stuff
	ChatClient client;
	//	chat history vector
	Vector history;
	//	historyprocessor, may be changed while running
	HistoryProcessor processor;
	// Icon images used by Toolbar and ElementPanel
	ImageArray icons;
	//	Display components
	ElementPanel display;
	Toolbar toolbar;
	Toolbar saybar;
	MemberListPanel memberListPanel;
	TextField entry;
	//	dialog boxes etc
	AboutDialog aboutDialog;
	ConnectDialog connectDialog;
	FontDialog fontDialog;
	RoomDialog enterRoomDialog;
	//	Room List components
	ModalDialog roomListFrame;
	RoomListPanel roomListPanel;
	NickDialog nickDialog;
	PasswordDialog passwordDialog;

	boolean roomNeedRefresh = true;
	// user font and color
	Font userFont;	// the default font
	Color userFontColor = Color.black;
	Font motdFont;
	Color motdColor = Color.blue.darker().darker();
	// user connection information
	String nickname;
	String server;
	//	the room the user is currently in
	Room room = null;
	String lastRoom = null;
	//	the name of the room the user selected in the connectDialog
	String connectRoom = null;
	//	keeps track of whether we are set to say or whisper
	boolean say = true;

	// following moved here from class Static
	Applet applet = null;	// null => standalone
	// Properties object containing localized strings etc
	Properties properties;

	//	the following added for MSN audience capabilities
	boolean push = false;		//	allow hosts permission to push (showDocument) on others.
	String pushFrame = null;	//	default frame to push urls into.
	boolean relay = false;		//	whether or not to process messages like
					//	"joe says: hello all" , so they appear to come from joe
	String send = null;		//	target user to send all messages to.
	boolean suppress = false;	//	suppresses all system messages.

	public ChatPanel()
	{
		this(null, Color.lightGray);
	}

	public ChatPanel(Applet applet)
	{
		this(applet, Color.lightGray);
	}

	public ChatPanel(Applet applet, Color bg)
	{
		this.applet = applet;
		if (applet != null)
			chatPanels.put(getKey(applet), this);
		properties = Static.loadProperties(applet, Static.getParameter(applet, "locale", "usa"));
		userFont = new Font("Helvetica", Font.PLAIN, 11);
		if (bg == null)
			bg = Color.lightGray;
		this.setBackground(bg);
		String fontString = Static.getParameter(applet, "font", "Helvetica, 0, 11");
		try
		{
			StringTokenizer st = new StringTokenizer(fontString, " ,:");
			if (st.countTokens() >= 3)
			{
				String fontname = st.nextToken();
				int style = Integer.parseInt(st.nextToken());
				int size = Integer.parseInt(st.nextToken());
				userFont = new Font(fontname, style, size);
			}
		}
		catch(Exception e)
		{
		}
		motdFont = new Font("Dialog", Font.PLAIN, userFont.getSize());
		history = new Vector();
		display = new ChatHistoryPanel();
		processor = new ChatHistoryProcessor(history, display, this);
		roomListPanel = new RoomListPanel(this);
		nickDialog = new NickDialog(getFrame(), this);
		passwordDialog = new PasswordDialog(getFrame(), this);

		icons = createIcons(bg);
		toolbar = createToolbar(true);
		saybar = createSayToolbar();
		int memberListWidth = Static.getParameter(applet, "memberlistwidth", 150);
		memberListPanel = new MemberListPanel(icons, this, memberListWidth);
		memberListPanel.setRoom(null);
		entry = new TextField();
		setLayout(new BorderLayout());
			if (Static.getParameter(applet, "toolbar", true))
			{
				InsetPanel toolbarPanel = new InsetPanel(3, 3, 3, 3);
				toolbarPanel.setBorder(true);
				toolbarPanel.add(toolbar);
				add("North", toolbarPanel);
			}
			Panel center = new Panel();
			center.setLayout(new BorderLayout(1, 1));
			center.add("Center", embed(display, new Insets(6,6,6,6)));
			if (Static.getParameter(applet, "memberlist", true))
				center.add("East", memberListPanel);
			InsetPanel centerInsetPanel = new InsetPanel(3, 3, 4, 4);
			centerInsetPanel.setBorder(true);
			centerInsetPanel.add(center);
		add("Center", centerInsetPanel);
		if (!Static.getParameter(applet, "observer", false))
		{
			Panel south = new Panel();
			south.setLayout(new BorderLayout());
				entry.setFont(userFont);
			south.add("Center", entry);
			south.add("East", saybar);
			InsetPanel southPanel = new InsetPanel(4, 4, 4, 4);
			southPanel.setBorder(true);
			southPanel.add(south);
			add("South", southPanel);
		}
		// the default chat client is IRC
		client = new IRCChatClient(this);
		//	check the banner parameter, and load if necessary
		String banner = Static.getParameter(applet, "banner", null);
		if (banner != null)
			display.add(new ImageElement(Static.getImage(applet, banner)));
		setButtonStates();

		//	initalize dialog boxes
		//	roomList
	/*	roomListFrame = new ModalDialog(getFrame(), this, false);
		roomListFrame.setResizable(true);
		roomListFrame.setTitle(get("room.list"));
		roomListFrame.setDispose(false);
		roomListFrame.setLayout(new GridLayout(1, 1));
		roomListFrame.add(roomListPanel);*/
		//	other dialogs
	//	aboutDialog = new AboutDialog(getFrame(), this);
	//	fontDialog = new FontDialog(getFrame(), this, userFont);
	//	enterRoomDialog = new RoomDialog(getFrame(), this);
	//	connectDialog = new ConnectDialog(getFrame(), this, applet);

		//	MSN additions
		relay = Static.getParameter(applet, "relay", false);
		send = Static.getParameter(applet, "send", null);
		push = Static.getParameter(applet, "push", false);
		pushFrame = Static.getParameter(applet, "pushframe", null);
		suppress = Static.getParameter(applet, "suppress", false);
	}

	public void setApplet(Applet applet)
	{
		if (applet != null)
			this.applet = applet;
	}

	public static String getKey(Applet applet)
	{
		if (applet == null)
			return "null";
		return applet.getDocumentBase() + "#" + applet.getParameter("id");
	}

	public static ChatPanel getChatPanel(Applet applet)
	{
		if (applet == null)
			return null;
		return (ChatPanel)chatPanels.get(getKey(applet));
	}

	public void changeNickname(String newnick)
	{
		//	try to change, we will be notified if it succeeds
		if (newnick == null || newnick.trim().equals(""))
			return;
		newnick = newnick.trim().replace(' ', '_');
		client.setNickname(newnick);
	}

	public void connect(String nickname, String room)
	{
		if (applet == null)
			return;
		String host = applet.getCodeBase().getHost();
		if (room == null || room.trim().equals(""))
			room = Static.getParameter(applet, "room", null);
		connect(nickname, host, room);
	}

	public void connect(String nickname)
	{
		connect(nickname, null);
	}

	// this method is called from ConnectDialog
	public void connect(String nickname, String server, String room)
	{
		if (client.connected())
			return;
		if (!suppress)
			display(localize("connecting.to") + " " + server + "...");
		if (nickname == null || nickname.trim().equals(""))
			nickname = "anonymous";
		if (client == null || isConnected())
			return;
		nickname = nickname.trim().replace(' ', '_');
		this.nickname = nickname;
		client.setNickname(nickname);
		client.setUsername(nickname);
		client.setRealname("Anonymous");
		int port = -1;
		try
		{
			port = Integer.parseInt(Static.getParameter(applet, "port", "-1"));
		}
		catch(Exception e)
		{
			// numberformatexception => use default port, client chooses
		}
		//	this information will be used by chatpanel to
		//	either automatically go to a room, or show the room list
		if (room == null || room.trim().equals(""))
			connectRoom = null;
		else
			connectRoom = room;
		client.connect(server, port, null);
	}

	public void disconnected(String message)
	{

		display(localize("error.disconnected.from.server") + " " + message);
		initialize();
	}

	// disconnects from the chatserver
	public void disconnect()
	{
		String s = server;
		client.disconnect();
		initialize();
		if (!suppress)
			display(localize("disconnecting.from.server") + " " + s);
	}

	//	done when connection is disconnected
	public void initialize()
	{
		departRoom();
		server = null;
		roomNeedRefresh = true;
		setButtonStates();
	}

	public void setButtonStates()
	{
		boolean connected = isConnected();
		boolean inRoom = room != null;
		toolbar.setEnabled("connect", !connected);
		toolbar.setEnabled("disconnect", connected);
		toolbar.setEnabled("enter.room", connected);
		toolbar.setEnabled("exit.room", inRoom);
		toolbar.setEnabled("room.list", connected);
		saybar.setEnabled("say", inRoom);
		saybar.setEnabled("think", inRoom);
		saybar.setEnabled("whisper", inRoom);
		saybar.setEnabled("action", inRoom);
		entry.enable(inRoom);
		if (inRoom)
			entry.requestFocus();
	}

	public ChatClient getClient()
	{
		return client;
	}

	//
	//	ChatListener Methods
	//

	public void userListAdd(String nickname, String channel, Properties p)
	{
		// later check here to see if 
		if (nickname != null && room != null)
		{
			room.add(nickname, p);
			memberListPanel.update();
		}
	}
	
	public void userJoining(String nickname, String channel, Properties p)
	{
		if (!suppress && !nickname.equals(this.nickname))
			display(nickname + " " + get("has.joined.the.conversation"));
		userListAdd(nickname, channel, p);
		showRoomStatus();
	}

	public void userLeaving(String nickname, String channel)
	{
		if (nickname != null && room != null)
		{
			if (!suppress)
				display(nickname + " " + get("has.left.the.conversation"), userFont, Color.red.brighter());
			room.remove(nickname);
			memberListPanel.update();
		}
		showRoomStatus();
	}

	public void userUpdate(String oldname, String newname, Properties p)
	{
		if (oldname != null && newname != null && room != null)
		{
			if (oldname.equals(nickname))
				this.nickname = newname;
			room.userUpdate(oldname, newname, p);
			memberListPanel.update();
			if (!suppress && !oldname.equals(newname))
				display(oldname + " " + get("is.now.known.as") + " " + newname, userFont, Color.cyan.darker().darker());
		}
	}

	public void connected(String server, String stats, String message)
	{
		this.server = server;
		if (!suppress)
		{
			if (Static.getParameter(applet, "stats", true) && stats != null)
				display(stats, motdFont, motdColor.darker());
			if (Static.getParameter(applet, "motd", true) && message != null)
				display(message, motdFont, motdColor);
		}
		// show room list
		Static.showStatus(localize("connected.to") + " " + server);
		if (connectRoom != null)
		{
			join(connectRoom);
			connectRoom = null;
		}
	//	else
	//		showRoomList();
		setButtonStates();
	}

	public void connectFailed(String reason)
	{
		//	give the user some info here
		display(localize("failed.to.connect.to.server") + " " + reason);
		showConnectDialog();
	}

	public void roomListStart()
	{
		roomListPanel.roomListStart();
	}
	
	public void roomListAdd(String name, int users, String topic)
	{
		roomListPanel.roomListAdd(name, users, topic);
	}
	
	public void roomListEnd()
	{
		roomListPanel.roomListEnd();
	}

	public void roomEntered(String name, Properties p)
	{
		if (name == null)
			return;
		room = new Room(name, p);
		memberListPanel.setRoom(room);
		if (!suppress)
			display(localize("entered.room") + " " + name);
		//	add yourself to the room list
		room.add(nickname, null);
		showRoomStatus();
		setButtonStates();
	}

	public void roomUpdate(String name, Properties p)
	{
		if (name == null || p == null || room == null || !name.equals(room.getName()))
			return;
		room.update(p);
		memberListPanel.update();
		showRoomStatus();
	}
	
	public void roomEntryFailed(String room, String reason)
	{
		display(localize("cannot.enter.room") + " " + room + " , " + reason);
	}

	public boolean isPushCommand(String source, String message)
	{
		if (push && applet != null && room != null && message.startsWith("push "))
		{
			User pusher = room.getUser(source);
			if (pusher != null && pusher.isOperator())
			{
				try
				{
					StringTokenizer st = new StringTokenizer(message, " ,\r\n");
					if (st.countTokens() < 2)
						return true;
					st.nextToken();	//	"push"
					String relative = st.nextToken();
					URL url = new URL(applet.getDocumentBase(), relative);
					String frame = st.hasMoreTokens() ? st.nextToken() : pushFrame;
					if (frame == null)
						return true;
					if (frame.equals("_self") && (relative.endsWith(".gif") || relative.endsWith(".jpg")))
						display.add(new ImageElement(applet.getImage(url)));
					else
						applet.getAppletContext().showDocument(url, frame);
				}
				catch(Exception e){e.printStackTrace(System.out);}
			}
			return true;
		}
		return false;
	}
	
	public void messageArrived(String target, String source, String message, Properties p)
	{
		if (source != null && message != null)
		{
			if (isPushCommand(source, message))
				return;
			int colon = message.indexOf(':');
			if (relay && colon > 0)
			{
				source = new StringTokenizer(message).nextToken();
				message = message.substring(colon + 1);
			}
			//	check to see if the user is ignored
			if (room == null)
			{
				display(target, source, message);
				return;
			}
			User fromUser = room.getUser(source);
			//	dont display only if the user is in our room, and isIgnored
			if (fromUser == null || !fromUser.isIgnored())
				display(target, source, message);
		}
	}

	public void systemMessage(String message)
	{
		if (!suppress)
		{
			if (server == null)//	user the motd font since sometimes notices are sent with MOTD
				display(message, motdFont, motdColor);
			else
				display(message);
		}
	}

	public void invalidNickname(String nickname)
	{
		if (!suppress)
			display(localize("error.invalid.nickname"));
		if (room == null)
		{
			showNickDialog(true);
			connect(nickname);
		}
	}

	public void showNickDialog(boolean mustchange)
	{
		nickDialog.reset(nickname, mustchange);
		show(nickDialog);
	}

	public void invalidPassword(String room)
	{
		passwordDialog.reset(room);
		show(passwordDialog);
	}

	public void errorMessage(String message)
	{
		if (!suppress)
			display(message);
		else
			System.out.println(message);
	}

	public void removedFromRoom(String by, String room, String reason)
	{
		if (room != null)
		{
			if (reason != null)
				display(by + " " + localize("kicked.you.from.room") + " " + room + " " + localize("saying") + " \"" + reason + "\".");
			else
				display(by + " " + localize("kicked.you.from.room") + " " + room + ".");
			departRoom();
		}
	}

	public void display(String message)
	{
		display(null, null, message, userFont, null);
	}

	public void display(String source, String message)
	{
		display(null, source, message, userFont, null);
	}

	public void display(String target, String source, String message)
	{
		display(target, source, message, userFont, null);
	}

	public void display(String message, Font font, Color color)
	{
		display(null, null, message, font, color);
	}

	public void display(String target, String source, String message, Font font, Color color)
	{
		if (message != null)
			processor.add(new Message(target, source, message, font, color));
	}

	public void join(String roomName)
	{
		join(roomName, null);
	}

	public void join(String roomName, String password)
	{
		if (client == null || roomName == null)
			return;
		if (room != null)
		{
			if (room.equals(roomName))
				return;
			departRoom();
		}
		client.join(roomName.trim().replace(' ', '_'), password);
	}

	public void setFont(Font font, Color color)
	{
		userFont = font;
		userFontColor = color;
		entry.setFont(font);
		entry.setForeground(color);
		try
		{
			layout();
		}
		catch(Exception e)
		{
			e.printStackTrace(System.out);
		}
	}

	public Panel embed(Panel panel, Insets insets)
	{
		Panel host = new BarBorderPanel(insets);
		host.add(panel);
		panel.setBackground(Color.white);
		return host;
	}

	public void addUser(String name, Properties props)
	{
		if (name != null)
		{
			room.add(name, props);
			memberListPanel.update();
		}
	}

	public boolean isConnected()
	{
		return client.connected();
	}

	public ImageArray createIcons(Color bg)
	{
		String imagename = "toolbar.gif";
		Image iconImages;
		iconImages = Static.getImage(applet, getClass(), imagename);
		String[] iconNames = {"open", "save", "about",
								"connect", "disconnect", "enter.room",
								"exit.room", "create.room", "comic.view",
								"text.view", "room.list", "user.list",
								"get.identity", "invite", "whisper.box",
								"glasses", "visit.homepage", "ignored",
								"set.font", "dock.undock", "gavel", "chat",
								"say", "think", "whisper", "action"};
		return new ImageArray(iconImages, iconNames, bg);
	}

	public Toolbar createSayToolbar()
	{
		Toolbar toolbar = new Toolbar(this, icons, true);
		String divider = "-";
		toolbar.add("say");
		toolbar.setDown("say", true);
		if (Static.getParameter(applet, "memberlist", true))
			toolbar.add("whisper");
		toolbar.add("think");
		toolbar.add("action");
		return toolbar;		
	}

	public Toolbar createToolbar(boolean horizontal)
	{
		Toolbar toolbar = new Toolbar(this, icons, horizontal);
		String divider = "-";
	//	toolbar.add(divider);
		toolbar.add("connect");
		toolbar.add("disconnect");
		toolbar.setEnabled("disconnect", false);
		toolbar.add(divider);
		toolbar.add("enter.room");
		toolbar.add("exit.room");
	//	toolbar.add("create.room");	// Not necessay, same as enter room
	//	toolbar.add(divider);
		toolbar.add("room.list");
	//	toolbar.add("user.list");
		toolbar.add(divider);
	//	toolbar.add("get.identity");
	//	toolbar.setEnabled("get.identity", false);
	//	toolbar.add("invite");
	//	toolbar.add("whisper.box");
	//	toolbar.add(divider);
		toolbar.add("visit.homepage");
		toolbar.add("set.font");
		toolbar.add("about");
	//	toolbar.add(divider);
	//	toolbar.add("text.view");
	//	toolbar.setDown("text.view", true);
	//	toolbar.add("comic.view");
	//	toolbar.add(divider);
		if (applet != null)
		{
			toolbar.add(divider);
			toolbar.add("dock.undock");
		}
		return toolbar;
	}

	public boolean keyDown(Event e, int key)
	{
		if (e.target == entry && key == 10)
		{
			sendMessage(entry.getText());
			entry.setText("");
			return true;
		}
		return false;
	}

	public void sendMessage(String text)
	{
		if (text == null || text.trim().equals(""))
			return;
		if (room == null)
			return;

		//	allow user to push himself if host
		isPushCommand(nickname, text);

		String[] names = memberListPanel.getSelectedItems();
		if (say || names == null || names.length == 0)
		{
			sendToRoom(text);
			if (!relay)
				processor.add(new Message(room.getName(), nickname, text, userFont, null));
		}
		else
		{
			Vector holder = new Vector();
			for (int i = 0; i < names.length; i++)
			{
				if (!names[i].equals(nickname))
					holder.addElement(names[i]);
			}
			processor.add(new Message(Static.toString(names, ", "), nickname, text, userFont, userFontColor));
			if (holder.isEmpty())
				return;
			names = new String[holder.size()];
			for (int i = 0; i < names.length; i++)
				names[i] = holder.elementAt(i).toString();
			sendToUsers(names, text);
			//	target info
		}
	}

	public void sendToUser(String name, String text)
	{
		if (client != null && text != null)
			client.send(name, text, null);
	}

	public void sendToUsers(String[] names, String text)
	{
		if (client != null || names != null || text != null)
			client.send(names, text, null);
	}

	public void sendToRoom(String text)
	{
		if (client != null && room != null && text != null)
			client.send(send == null ? room.getName() : send, text, null);
	}

	public void toolbarButtonPressed(Event e, String button)
	{
	/*	if (button.equals("text.view"))
		{
			toolbar.setDown("comic.view", false);
			toolbar.setDown("text.view", true);
			processor = new ChatHistoryProcessor(history, display, this);
		}
		else if (button.equals("comic.view"))
		{
			toolbar.setDown("text.view", false);
			toolbar.setDown("comic.view", true);
			processor = new HistoryProcessor(history, display);
		}
		else*/if (button.equals("set.font"))
		{
			showFontDialog();
		}
		else if (button.equals("visit.homepage"))
			Static.showDocument("http://www.microsoft.com/ie/comichat/");			
		else if (button.equals("connect"))
			showConnectDialog();
		else if (button.equals("disconnect"))
			disconnect();
		else if (button.equals("room.list"))
			showRoomList();
		else if (button.equals("enter.room"))
			showEnterRoomDialog();
		else if (button.equals("exit.room"))
		{	// hack 
			toolbar.getButton("exit.room").mouseExited(null);
			// end hack
			departRoom();
		}
		else if (button.equals("dock.undock"))
			dockUndock();
		else if (button.equals("say"))
		{
			say = true;
			saybar.setDown("whisper", false);
			saybar.setDown("say", true);
			sendMessage(entry.getText());
			resetEntry();
		}
		else if (button.equals("whisper"))
		{
			say = false;
			saybar.setDown("say", false);
			saybar.setDown("whisper", true);
			sendMessage(entry.getText());
			resetEntry();
		}
		else if (button.equals("think") || button.equals("action"))
		{
			boolean think = button.equals("think");
			String message = entry.getText();
			if (message == null || message.trim().equals(""))
				return;
			char ctcp = 1;
			message = ctcp + "ACTION " + (think?localize("thinks") + ": ":"") + message + ctcp;
			sendMessage(message);
			resetEntry();
		}
		else if (button.equals("about"))
			showAboutDialog();
		else if (processor != null)
			display(button + " is not yet functional.");
		//	set the focus to the entry field
	}

	public void showAboutDialog()
	{
		if (aboutDialog == null)
			aboutDialog = new AboutDialog(getFrame(), this);
		show(aboutDialog);
	}

	public void showEnterRoomDialog()
	{
		if (enterRoomDialog == null)
			enterRoomDialog = new RoomDialog(getFrame(), this);
		show(enterRoomDialog);
	}

	public void showFontDialog()
	{
		if (fontDialog == null)
			fontDialog = new FontDialog(getFrame(), this, userFont);
		fontDialog.setFontSelections(userFont);
		show(fontDialog);
	}

	public void show(Window window)
	{
		Static.center(window);
		if (window.isVisible())
		{
			window.toFront();
		}
		else
		{
			window.show();
		}
	}

	public void resetEntry()
	{
		entry.setText("");
		entry.requestFocus();
	}

	public void dockUndock()
	{
		if (applet == null)
			return;
		Container parent = getParent();
		if (parent instanceof Applet)				// we are in an applet so put in frame
		{
			applet.remove(this);
			Frame f = new ChatFrame(this);
			f.resize(applet.size().width, applet.size().height);
			Static.center(f);
			f.show();
		}
		else if (parent instanceof Frame)	// we are in a frame, so put in applet
		{
			Frame frame = (Frame)parent;
			frame.hide();
			frame.remove(this);
			applet.removeAll();
			applet.setLayout(new BorderLayout());
			applet.add("Center", this);
			applet.invalidate();
			applet.validate();
			frame.dispose();
		}
		// since toolbar may not receive mouseExit event
		toolbar.mouseExit();
	}

	public Room getRoom()
	{
		return room;
	}

	public String getRoomName()
	{
		return room != null ? room.getName() : null;
	}

	public void departRoom()
	{
		if (room != null)
		{
			lastRoom = room.getName();
			if (!suppress)
				display("Leaving Room: " + room.getName());
			client.part(room.getName());
			room = null;
		}
		memberListPanel.setRoom(null);	// will automatically clear itself
		memberListPanel.repaint();
		setButtonStates();
	}

	public void showRoomStatus()
	{
		if (room != null)
			Static.showStatus(get("room") + " : " + room.getName() + "    " + get("users") + " : " + room.getUserCount());
	}

	public void showRoomList()
	{
		if (!isConnected())
			return;
		if (roomListFrame == null)
		{
			roomListFrame = new ModalDialog(getFrame(), this, false);
			roomListFrame.setResizable(true);
			roomListFrame.setTitle(get("room.list"));
			roomListFrame.setDispose(false);
			roomListFrame.setLayout(new GridLayout(1, 1));
			roomListFrame.add(roomListPanel);
		}
		roomListFrame.pack();
		show(roomListFrame);
	}

	public void refreshRoomList()
	{
		System.out.println("Refreshing Room List");
		client.getRooms();
	}

	public void showConnectDialog()
	{
		if (connectDialog == null)
			connectDialog = new ConnectDialog(getFrame(), this, applet);
		show(connectDialog);
	}
	//	this method is provided because dialog boxes need a frame
	//	argument in their constructors, and applets aren't in frames
	public Frame getFrame()
	{
	//	return new Frame();
		return Static.getFrame(this);
	}

	public boolean action(Event e, Object arg)
	{
		if (e.target instanceof Toolbar && arg instanceof String)
		{
			toolbarButtonPressed(e, (String)arg);
			return true;
		}
		return super.action(e, arg);
	}

	public boolean handleEvent(Event e)
	{
		if (e.id == Event.MOUSE_UP && e.target instanceof FlatScrollbar && entry.isEnabled())
			entry.requestFocus();
		return super.handleEvent(e);
	}

	public boolean mouseEnter(Event e, int x, int y)
	{
		showRoomStatus();
		return super.mouseMove(e, x, y);
	}

	public String get(String name)
	{
		return get(name, null);
	}

	public String localize(String name)
	{
		return get(name, null);
	}

	public String get(String name, String def)
	{
		String value = properties.getProperty(name);
		if (value != null)
			return value;
		return def;
	}
	
	public boolean gotFocus(Event evt, Object what)
	{
		if (evt.target == this && entry.isEnabled())
			entry.requestFocus();
		return super.gotFocus(evt, what);
	}

}


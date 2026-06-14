///////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	RoomListPanel
//
//	Author:
//		Kris Nye
///////////////////////////////////////////////////
import java.awt.*;
import java.util.*;

public class RoomListPanel extends InsetPanel
{
	String ROOM;
	String USERS;
	String TOPIC;
	String ENTER;
	String REFRESH;
	String DISPLAY;
	String CANCEL;
	String SHOW_ROOMS;

	MultiColumnList roomList;
	Hashtable rooms = new Hashtable();
	Button displayButton, refreshButton, cancelButton, enterButton;
	TextField filterField;
	ChatPanel chatPanel;

	public void loadStrings()
	{
		ROOM = get("room");
		USERS = get("users");
		TOPIC = get("topic");
		ENTER = get("enter");
		REFRESH = get("refresh");
		DISPLAY = get("display");
		CANCEL = get("cancel");
		SHOW_ROOMS = get("show.only.rooms.containing");
	}

	public String get(String value)
	{
		return chatPanel.get(value);
	}
		
	public RoomListPanel(ChatPanel chatPanel)
	{
		super(6, 6, 6, 6);
		this.chatPanel = chatPanel;
		loadStrings();
		setLayout(new GridLayout(1, 1));
		String[] fields = {ROOM + "                                                                       ",
						   USERS + "       ",
						   TOPIC + "                                                                                "};
		enterButton = new Button(ENTER);
		refreshButton = new Button(REFRESH);
		displayButton = new Button(DISPLAY);
		cancelButton = new Button(CANCEL);
		filterField	= new TextField(20);
		Panel main = new Panel();
		main.setLayout(new BorderLayout(2,2));
			Panel north = new Panel();
			north.setLayout(new BorderLayout(4, 4));
				Panel nw = new Panel();
				nw.setLayout(new BorderLayout(4, 4));
				nw.add("West", new Label(SHOW_ROOMS + ":", Label.LEFT));
				nw.add("Center", filterField);
				nw.add("East", displayButton);
			north.add("West", nw);
			north.add("Center", new Canvas());
		main.add("North", north);
		roomList = new MultiColumnList(fields);
		roomList.setMultipleSelections(false);
		roomList.setBackground(Color.white);
		BarBorderPanel bbp = new BarBorderPanel(new Insets(8,4,9,6));
		bbp.add(roomList);
		main.add("Center", bbp);
			Panel south = new Panel();
			south.setLayout(new BorderLayout());
				Panel sw = new Panel();
				sw.setLayout(new GridLayout(1, 3, 8, 4));
				sw.add(refreshButton);
				sw.add(enterButton);
				sw.add(cancelButton);
			south.add("West", sw);
			south.add("Center", new Canvas());
		main.add("South", south);
		add(main);
	}

	public void paint(Graphics g)
	{
		roomList.top.setBackground(chatPanel.getBackground());
		super.paint(g);
		if (chatPanel.roomNeedRefresh)
		{
			refresh();
			chatPanel.roomNeedRefresh = false;
		}
	}

	public void roomListStart()
	{
		refreshButton.enable(false);
		roomList.clearElements();
		rooms.clear();
	}

	//	notifies the listener to add another room to its list
	public void roomListAdd(String name, int users, String topic)
	{
		if (name == null)
		{
			return;
		}
		if (topic == null)
		{
			topic = "";
		}
		String[] roomInfo = {name, Integer.toString(users), topic};
		addToRoomList(roomInfo, true);
		rooms.put(roomInfo[0], roomInfo);
	}

	//	notifies the listener that the end of the list has been reached
	public void roomListEnd()
	{
		sort();
		refreshButton.enable(true);
	}

	public void enter(String room)
	{
		if (room != null && chatPanel != null)
		{
			chatPanel.join(room);
			hide();
		}
	}

	public void show()
	{
		refreshButton.enable(true);
		super.show();
	}

	public void hide()
	{
		Container parent = getParent();
		if (parent != null && parent instanceof Window && parent.isVisible())
			((Window)parent).hide();
		chatPanel.requestFocus();
	}

	public Enumeration sort(Enumeration e)
	{
		SortedVector holder = new SortedVector();
		while (e.hasMoreElements())
		{
			holder.insertElementSorted(e.nextElement());
		}
		return holder.elements();
	}

	public void sort()
	{
		Enumeration e = sort(rooms.keys());
		roomList.clearElements(false);
		while (e.hasMoreElements())
		{
			String[] roomInfo = (String[])rooms.get(e.nextElement());
			addToRoomList(roomInfo, false);
		}
		roomList.setScrollbarValues();
		roomList.repaint();
	}

	public void setCursor(int cursor)
	{
		System.out.println("Set Cursor " + cursor);
		Container parent = getParent();
		while (parent != null)
		{
			if (parent instanceof Frame)
			{
				System.out.println("Parent is Frame");
				Frame frame = (Frame)parent;
				frame.setCursor(cursor);
				return;
			}
			parent = parent.getParent();
		}
	}

	public void addToRoomList(String[] roomInfo, boolean paint)
	{
		if (!isVisible())
			paint = false;
		String filter = filterField.getText();
		if (filter != null && filter.trim().equals(""))
		{
			filter = null;
		}
		else
		{
			filter = filter.toUpperCase();
		}
		if (roomInfo != null && (filter == null || roomInfo[0].toUpperCase().indexOf(filter) > 0))
		{
			roomList.addItem(roomInfo, paint);
		}
	}

	public void display()
	{
		Enumeration e = sort(rooms.keys());
		roomList.clearElements(true);
		while (e.hasMoreElements())
		{
			String[] roomInfo = (String[])rooms.get(e.nextElement());
			addToRoomList(roomInfo, true);
		}
	}

	public void refresh()
	{
		chatPanel.refreshRoomList();
	}

	public Dimension preferredSize()
	{
		return getPreferredSize();
	}

	public Dimension getPreferredSize()
	{
		Dimension size = super.preferredSize();
		size.width = Math.max(size.width, 600);
		size.height = Math.max(size.height, 400);
		return size;
	}

	public boolean action(Event e, Object arg)
	{
		if (e.target == roomList && arg instanceof String)
		{
			System.out.println(e);
			enter((String)arg);
			return true;
		}
		else if (e.target == enterButton)
		{
			enter(roomList.getSelectedItem());
			return true;
		}
		else if (e.target == refreshButton)
		{
			refresh();
			return true;
		}
		else if (e.target == cancelButton)
		{
			hide();
			return true;
		}
		else if (e.target == displayButton)
		{
			display();
			return true;
		}
		else
		{
			return super.action(e, arg);
		}
	}

}

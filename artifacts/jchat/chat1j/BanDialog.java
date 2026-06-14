////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	BanDialog
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;
import java.util.*;

public class BanDialog extends ModalDialog implements Runnable
{
	String BAN;
	String UNBAN;
	String BAN_UNBAN;
	String USERS_BANNED_FROM;
	String FORMAT_NICK_USER_HOST;
	String OK;
	
	ChatPanel chatPanel;
	String user;
	String room;

	TextField banmask = new TextField(20);
	List banList = new List(8, true);
	//	used to avoid duplicating entries in banList
	Hashtable banTable = new Hashtable();
	
	Button banButton = new Button(BAN);
	Button unbanButton = new Button(UNBAN);
	Button okButton = new Button(OK);
	
	public BanDialog(Frame frame, String user, ChatPanel chatPanel)
	{
		super(frame, chatPanel);
		setTitle(BAN_UNBAN);
		this.chatPanel = chatPanel;
		this.user = user;
		this.room = chatPanel.getRoomName();
		setLayout(new GridLayout(1, 1));
		setBackground(chatPanel.getBackground());
		//	the tabs provide insight into the nesting of the panels used for layout
		Panel insetPanel = new InsetPanel(6, 6, 6, 6);
			Panel main = new Panel();
				main.setLayout(new BorderLayout(4, 4));
					Panel north = new Panel();
					north.setLayout(new GridLayout(3, 1, 6, 6));
					north.add(new Label(FORMAT_NICK_USER_HOST));
						banmask.setText(getBanmask(user));
					north.add(banmask);
					north.add(banButton);
				main.add("North", north);
					Panel center = new Panel();
					center.setLayout(new BorderLayout(6, 2));
					center.add("North", new Label(USERS_BANNED_FROM + " " + room));
						fillBanList();
					center.add("Center", banList);
				main.add("Center", center);
					Panel south = new Panel();
					south.setLayout(new GridLayout(1, 2, 6, 6));
					south.add(unbanButton);
					south.add(okButton);
				main.add("South", south);
		insetPanel.add(main);
		add(insetPanel);
		show();
	}

	public void loadStrings()
	{
		BAN = get("ban");
		UNBAN = get("unban");
		BAN_UNBAN = get("ban.unban");
		USERS_BANNED_FROM = get("users.banned.from");
		FORMAT_NICK_USER_HOST = get("format.nick.user.host");
		OK = get("ok");
	}

	public void run()
	{
		while (true)
		{
			try
			{
				Thread.sleep(500);
			}
			catch(Exception e)
			{
			}
			String[] items = banList.getSelectedItems();
			//	only enabled the unban button when something is selected
			unbanButton.enable(items != null && items.length > 0);
		}
	}

	public void fillBanList()
	{
		String[] list = chatPanel.getClient().getBanlist(room);
		if (list == null || list.length == 0)
		{
			return;
		}
		for (int i = 0; i < list.length; i++)
		{
			addToBanList(list[i]);
		}
	}

	public void addToBanList(String item)
	{
		if (item == null)
		{
			return;
		}
		if (!banTable.containsKey(item))
		{
			banList.addItem(item);
			banTable.put(item, "true");
		}
	}

	public String getBanmask(String user)
	{
		String identity = chatPanel.getClient().getIdentity(user);
		if (identity == null)
			return user + "!*@*";
		identity = "*!*" + identity.substring(identity.indexOf("@"));
		return identity;
	}

	public String fixMask(String mask)
	{
		if (mask == null)
			return user + "!*@*";
		int bangIndex = mask.indexOf("!");
		int atIndex = mask.indexOf("@");
		if (bangIndex > 0 && atIndex > 0)
			return mask;
		return mask.substring(Math.min(bangIndex, atIndex)) + "!*@*";
	}

	public void ban(String mask)
	{
		if (mask == null || mask.trim().equals(""))
			return;
		String fixedMask = fixMask(mask);
		if (!mask.equals(fixedMask))
			banmask.setText(fixedMask);
		ChatClient client = chatPanel.getClient();
		String room = chatPanel.getRoomName();
		client.setBanmask(room, fixedMask, true);
		addToBanList(fixedMask);
	}

	public void unban()
	{
		String[] banIDs = banList.getSelectedItems();
		if (banIDs == null || banIDs.length == 0)
		{
			return;
		}
		ChatClient client = chatPanel.getClient();
		String room = chatPanel.getRoomName();
		for (int i = 0; i < banIDs.length; i++)
		{
			client.setBanmask(room, banIDs[i], false);
		}
		//	now remove them from the list
		int[] indexes = banList.getSelectedIndexes();
		if (indexes == null || indexes.length == 0)
		{
			return;
		}
		//	count backwards, to avoid messing up positions
		//	since all above one removed will be decremented down
		for (int i = indexes.length - 1; i >= 0; i--)
		{
			banList.delItem(indexes[i]);
		}
	}

	public void ok()
	{
		close();
	}

	public boolean keyDown(Event e, int key)
	{
		if (e.target == banmask && key == 10)
		{
			ban(banmask.getText());
		}
		return super.keyDown(e, key);
	}

	public boolean action(Event e, Object arg)
	{
		if (e.target == okButton)
		{
			ok();
			return true;
		}
		else if (e.target == banButton)
		{
			ban(banmask.getText());
			return true;
		}
		else if (e.target == unbanButton)
		{
			unban();
			return true;
		}
		return super.action(e, arg);
	}

}
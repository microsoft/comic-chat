////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	KickDialog
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;

public class KickDialog extends ModalDialog
{
	String KICK;
	String WHY_ARE_YOU_KICKING;
	String ALSO_BAN;
	String OK;
	String CANCEL;
	
	ChatPanel chatPanel;
	String user;
	TextField reason = new TextField();
	Checkbox alsoBan = new Checkbox(ALSO_BAN);
	TextField banMask = new TextField();
	Button okButton = new Button(OK);
	Button cancelButton = new Button(CANCEL);

	public void loadStrings()
	{
		KICK = get("kick");
		WHY_ARE_YOU_KICKING = get("why.are.you.kicking");
		ALSO_BAN = get("also.ban");
		OK = get("ok");
		CANCEL = get("cancel");
	}

	public KickDialog(Frame frame, String user, ChatPanel chatPanel)
	{
		super(frame, chatPanel);
		setTitle(KICK);
		this.chatPanel = chatPanel;
		this.user = user;
		setLayout(new GridLayout(1, 1));
		setBackground(chatPanel.getBackground());
		Panel main = new Panel();
		main.setLayout(new BorderLayout(4, 6));
		main.add("North", new Label(WHY_ARE_YOU_KICKING + " " + user + "?"));
		main.add("Center", reason);
		Panel south = new Panel();
		south.setLayout(new GridLayout(2, 1, 6, 6));
		Panel southNorth = new Panel();
		southNorth.setLayout(new BorderLayout());
		southNorth.add("West", alsoBan);
		banMask.setText(getBanmask(user));
		banMask.enable(false);
		southNorth.add("Center", banMask);
		south.add(southNorth);
		Panel southSouth = new Panel();
		southSouth.setLayout(new GridLayout(1, 2, 6, 6));
		southSouth.add(okButton);
		southSouth.add(cancelButton);
		south.add(southSouth);
		main.add("South", south);
		InsetPanel insetPanel = new InsetPanel(6, 6, 6, 6);
		insetPanel.add(main);
		add(insetPanel);
		setResizable(false);
		show();
	}

	public void ok()
	{
		ChatClient client = chatPanel.getClient();
		if (client != null)
		{
			String room = chatPanel.getRoomName();
			if (alsoBan.getState())
			{
				client.setBanmask(room, banMask.getText(), true);
			}
			client.kick(room, user, reason.getText());
		}
		close();
	}

	public boolean action(Event e, Object arg)
	{
		if (e.target == alsoBan)
		{
			banMask.enable(alsoBan.getState());
			return true;
		}
		else if (e.target == okButton)
		{
			ok();
		}
		else if (e.target == cancelButton)
		{
			close();
		}
		return super.action(e, arg);
	}

	public String getBanmask(String user)
	{
		String identity = chatPanel.getClient().getIdentity(user);
		if (identity == null)
		{
			return "user!*?*";
		}
		identity = "*!*" + identity.substring(identity.indexOf("@"));
		return identity;
	}

}
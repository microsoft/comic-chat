////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1997
//
//	Description:
//
//	PasswordDialog prompts the user to enter a
//		password before entering a room
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;

public class PasswordDialog extends ModalDialog
{
	ChatPanel parent;
	TextField password = new TextField();
	String room;
	String CANCEL, OK, ROOM_PASSWORD, ENTER_PASSWORD_FOR_ROOM;
	Button cancel = new Button(CANCEL);
	Button ok = new Button(OK);
	Label label;

	public void loadStrings()
	{
		CANCEL = get("cancel");
		OK = get("ok");
		ROOM_PASSWORD = get("room.password");
		ENTER_PASSWORD_FOR_ROOM = get("enter.password.for.room");
	}
	
	public PasswordDialog(Frame frame, ChatPanel parent)
	{
		super(frame, parent);
		setTitle(ROOM_PASSWORD);
		this.parent = parent;
		Panel main = new Panel();
		main.setLayout(new GridLayout(3, 1, 4, 6));
		label = new Label("");
		main.add(label);
			password.setEchoCharacter('*');
		main.add(password);
			Panel south = new Panel();
			south.setLayout(new BorderLayout());
				Panel sw = new Panel();
				sw.setLayout(new GridLayout(1, 2, 4, 4));
				sw.add(ok);
				sw.add(cancel);
			south.add("West", sw);
			south.add("Center", new Canvas());
		main.add(south);
		Panel insetPanel = new InsetPanel(8, 8, 8, 8);
		insetPanel.add(main);
		this.setLayout(new GridLayout(1, 1));
		this.add(insetPanel);
		setResizable(false);
	}

	public void reset(String room)
	{
		label.setText(ENTER_PASSWORD_FOR_ROOM + " " + room);
	}

	public void ok()
	{
		if (parent != null)
		{
			String key = password.getText();
			hide();
			parent.join(room, key);
			close();
		}
	}

	public boolean action(Event e, Object arg)
	{
		if (e.target == ok)
		{
			ok();
			return true;
		}
		else if (e.target == cancel)
		{
			close();
			return true;
		}
		return super.action(e, arg);
	}

}

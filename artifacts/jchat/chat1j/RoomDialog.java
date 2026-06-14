////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	RoomDialog	prompts the user to enter a room name to enter
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;

public class RoomDialog extends ModalDialog
{
	ChatPanel parent;
	TextField room = new TextField();
	TextField password = new TextField();
	Button cancel, ok;
	String OK, CANCEL, ENTER_ROOM, ENTER_ROOM_NAME, OPTIONAL_PASSWORD;

	public void loadStrings()
	{
		OK = get("ok");
		CANCEL = get("cancel");
		ENTER_ROOM = get("enter.room");
		ENTER_ROOM_NAME = get("enter.room.name");
		OPTIONAL_PASSWORD = get("optional.password");
	}
	
	public RoomDialog(Frame frame, ChatPanel parent)
	{
		super(frame, parent);
		this.parent = parent;
		setTitle(ENTER_ROOM);
		ok = new Button(OK);
		cancel = new Button(CANCEL);
		Panel main = new Panel();
		main.setLayout(new GridLayout(5, 1, 4, 6));
		main.add(new Label(ENTER_ROOM_NAME));
		main.add(room);
		main.add(new Label(OPTIONAL_PASSWORD));
			password.setEchoCharacter('*');
		main.add(password);
			Panel south = new Panel();
			south.setLayout(new GridLayout(1, 2, 4, 4));
			south.add(ok);
			south.add(cancel);
		main.add(south);
		Panel insetPanel = new InsetPanel(8, 8, 8, 8);
		insetPanel.add(main);
		this.setLayout(new GridLayout(1, 1));
		this.add(insetPanel);
	}

	public void ok()
	{
		if (parent != null)
		{
			String name = room.getText();
			String key = password.getText();
			if (name == null || name.trim().equals(""))
			{
				room.requestFocus();
				return;
			}
			hide();
			room.setText("");
			password.setText("");
			parent.join(name.trim(), key);
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
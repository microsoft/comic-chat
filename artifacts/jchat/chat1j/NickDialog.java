////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	NickDialog
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;
import java.util.*;

public class NickDialog extends ModalDialog
{
	String CHANGE_NICKNAME;
	String NEW_NICKNAME;
	String CANCEL;
	String OK;
	
	ChatPanel chatPanel;

	TextField nameField = new TextField(30);
	Button cancelButton = new Button(CANCEL);
	Button okButton = new Button(OK);
	boolean storeChange;

	public void loadStrings()
	{
		CHANGE_NICKNAME = get("change.nickname");
		NEW_NICKNAME = get("new.nickname");
		CANCEL = get("cancel");
		OK = get("ok");
	}
	
	public NickDialog(Frame frame, ChatPanel chatPanel)
	{
		super(frame, chatPanel);
		setTitle(CHANGE_NICKNAME);
		this.chatPanel = chatPanel;
		setLayout(new GridLayout(1, 1));
		setBackground(chatPanel.getBackground());
		//	the tabs provide insight into the nesting of the panels used for layout
		Panel insetPanel = new InsetPanel(6, 6, 6, 6);
			Panel main = new Panel();
			main.setLayout(new BorderLayout(4, 4));
			main.add("North", new Label(NEW_NICKNAME));
			main.add("Center", nameField);
				Panel south = new Panel();
				south.setLayout(new GridLayout(1, 2, 4, 4));
				south.add(okButton);
				south.add(cancelButton);
			main.add("South", south);
		insetPanel.add(main);
		add(insetPanel);
		setResizable(false);
	}

	public void reset(String oldnick, boolean storeChange)
	{
		nameField.setText(oldnick);
		this.storeChange = storeChange;
	}

	public void ok()
	{
		String newNick = nameField.getText();
		if (newNick != null && !newNick.trim().equals(""))
		{
			if (storeChange)
				chatPanel.nickname = newNick;
			chatPanel.changeNickname(newNick);
			hide();
			super.close();
		}
		else
		{
			nameField.requestFocus();
		}
	}

	public boolean action(Event e, Object arg)
	{
		System.out.println(e);
		if (e.target == okButton)
		{
			ok();
			return true;
		}
		if (e.target == cancelButton)
		{
			close();
		}
		return super.action(e, arg);
	}

	public void close()
	{
		if (storeChange)
			chatPanel.disconnect();
		super.close();
	}

}
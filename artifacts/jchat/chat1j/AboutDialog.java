////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	AboutDialog
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////

import java.awt.*;

public class AboutDialog extends ModalDialog
{

	String ABOUT;
	String TITLE;
	String VERSION_NUMBER;
	String COPYRIGHT;
	String WARNING;
	String OK;

	Button okButton = new Button(OK);
	
	public AboutDialog(Frame frame, ChatPanel parent)
	{
		super(frame, parent);
		setTitle(ABOUT);
		setLayout(new GridLayout(1, 1));
		Panel main = new Panel();
		main.setLayout(new BorderLayout());
		ElementPanel ep = new ElementPanel();
		ep.resize(300, 200);
		Font font = new Font("TimesRoman", 0, 14);
		Color color = Color.black;
		ep.add(new TextElement(font, color, TITLE + " (" + VERSION_NUMBER + ")"));
		ep.add(new TextElement(font, color, COPYRIGHT));
		ep.add(new TextElement(font, color, "\n \n"));
		ep.add(new TextElement(font, color, WARNING));
		ep.add(new TextElement(font, color, "\n \n"));
		main.add("Center", ep);
			Panel south = new Panel();
			south.setLayout(new GridLayout(1, 3, 20, 5));
			south.add(okButton);
			south.add(new Canvas());
			south.add(new Canvas());
		main.add("South", south);
		InsetPanel insetPanel = new InsetPanel(15, 15, 15, 15);
		insetPanel.setBackground(parent.getBackground());
		insetPanel.add(main);
		add(insetPanel);
		resize(400, 300);
		Static.center(this);
		setResizable(false);
	}

	public void loadStrings()
	{
		ABOUT = get("about.microsoft.java.chat");
		TITLE = get("title");
		VERSION_NUMBER = get("version.number");
		COPYRIGHT = get("copyright");
		WARNING = get("warning");
		OK = get("ok");
	}

	public void ok()
	{
		close();
	}

	public boolean action(Event e, Object arg)
	{
		if (e.target == okButton)
		{
			close();
			return true;
		}
		return super.action(e, arg);
	}

}
////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	URLTextElement is an Extension of TextElement
//	which automatically underlines itself, and will
//	push a browser to the specified url if it is clicked.
//
//	if the optional chatPanel argument is passed, then
//	this url represents a chat room and will link to it
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;
import java.net.*;
import java.applet.*;

public class URLTextElement extends TextElement implements MouseListener
{
	ChatPanel chatPanel = null;
	
	public URLTextElement(Font font, Color color, String text)
	{
		this(font, color, text, null);
	}

	public URLTextElement(Font font, Color color, String text, ChatPanel chatPanel)
	{
		super(font, color, text);
		this.chatPanel = chatPanel;
	}

	public void paint(Graphics g)
	{
		super.paint(g);
		Color oldColor = g.getColor();
		g.setColor(highlight?background:color);
		g.drawLine(0, bounds.height-1, bounds.width, bounds.height-1);
		g.setColor(oldColor);
	}

	public void mouseClicked(Event event)
	{
		if (chatPanel == null)
		{
			Static.showDocument(text);
		}
		else
		{
			if (chatPanel.room == null || !text.equalsIgnoreCase(chatPanel.room.getName()))
			{
				chatPanel.join(text.trim());
			}
		}
	}

	public void mouseEntered(Event e)
	{
		setHighlight(true);
		paint();
	}

	public void mouseExited(Event e)
	{
		setHighlight(false);
		paint();
	}

}
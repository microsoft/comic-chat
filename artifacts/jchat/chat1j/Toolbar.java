////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	class Toolbar emulates a Win95 flat Toolbar
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;
import java.util.*;

public class Toolbar extends ElementPanel
{
	ImageArray imageArray;
	Dimension buttonSize = new Dimension(24, 22);
	boolean horizontal = true;
	int thickness = 8;
	Hashtable buttons = new Hashtable();
	ChatPanel parent;

	public Toolbar(ChatPanel parent, ImageArray imageArray, boolean horizontal)
	{
		setLayout(null);
		this.parent = parent;
		this.horizontal = horizontal;
		this.imageArray = imageArray;
	}

	public void setButtonSize(Dimension buttonSize)
	{
		if (buttonSize != null)
			this.buttonSize = buttonSize;
	}

	public Dimension getButtonSize()
	{
		return buttonSize;		
	}

	public void add(String name)
	{
		if (name == null)
			return;
		if (name.equals("-"))
		{
			if (horizontal)
				add(new ToolbarDivider(thickness, buttonSize.width));
			else
				add(new ToolbarDivider(buttonSize.height, thickness));
			return;
		}
		if (imageArray == null)
			return;
		addButton(new ToolbarButton(this, imageArray, buttonSize, name));		
	}

	public void addButton(ToolbarButton button)
	{
		buttons.put(button.getName(), button);
		add(button);
	}

	public void removeButton(String name)
	{
		if (name != null)
			removeButton(getButton(name));
	}

	public ToolbarButton getButton(String name)
	{
		return (ToolbarButton)buttons.get(name);
	}

	public void removeButton(ToolbarButton button)
	{
		if (button != null)
		{
			buttons.remove(button.getName());
			remove(button);
		}
	}

	public Dimension getPreferredSize()
	{
		int length = 0;
		Enumeration e = getElements();
		while (e.hasMoreElements())
		{
			Element element = (Element)e.nextElement();
			if (horizontal)
			{
				length += element.getPreferredSize().width;
			}
			else
			{
				length += element.getPreferredSize().height;
			}			
		}
		if (horizontal)
		{
			return new Dimension(length, buttonSize.height);
		}
		else
		{
			return new Dimension(buttonSize.width, length);
		}
	}

	public void setDown(String name, boolean down)
	{
		if (name != null)
		{
			ToolbarButton button = getButton(name);
			if (button != null)
			{
				button.setDown(down);
			}
		}
	}

	public void setEnabled(String name, boolean enabled)
	{
		if (name != null)
		{
			ToolbarButton button = getButton(name);
			if (button != null)
			{
				button.setEnabled(enabled);
			}
		}
	}

}

class ToolbarButton extends Element implements MouseListener
{
	Toolbar parent;
	boolean mouseOver = false;
	boolean mouseDown = false;
	boolean down = false;
	String name;
	ImageArray imageArray;

	public ToolbarButton(Toolbar parent, ImageArray imageArray, Dimension size, String name)
	{
		this.parent = parent;
		this.imageArray = imageArray;
		setSize(size);
		this.name = name;
	}

	public String getName()
	{
		return name;
	}

	public void setName(String name)
	{
		this.name = name;
	}

	public boolean isDown()
	{
		return down;
	}

	public void setDown(boolean down)
	{
		boolean paint = !this.down == down;
		this.down = down;
		if (paint)
		{
			paint();
		}
	}

	public void mouseClicked(Event e)
	{
		Event event = new Event(host, Event.ACTION_EVENT, name);
		event.x = e.x;
		event.y = e.y;
		dispatchEvent(event);
	}

	public void mouseEntered(Event e)
	{
		mouseOver = true;
		Static.showStatus(parent.parent.get(name));
		paint();
	}
	
	public void mouseExited(Event e)
	{
		mouseOver = false;
		mouseDown = false;
		Static.showStatus("");
		paint();
	}

	public void mousePressed(Event e)
	{
		mouseDown = true;
		paint();
	}

	public void mouseReleased(Event e)
	{
		mouseDown = false;
		paint();
	}

	public Color modify(Color color, int add)
	{
		if (color == null)
			return null;
		return new Color(
				Math.min(color.getRed() + add, 255),
				Math.min(color.getGreen() + add, 255),
				Math.min(color.getBlue() + add, 255));
	}

	public Toolbar getToolbar()
	{
		if (host instanceof Toolbar)
			return (Toolbar)host;
		return null;
	}

	public void paint(Graphics g)
	{
		Color color = getBackground();
		g.setColor(color);
		if (down && !mouseOver && enabled)
		{
			// set color a little lighter
			g.setColor(modify(color, +20));
		}
		Dimension size = getSize();
		g.fillRect(0, 0, size.width, size.height);
		Dimension imageSize = imageArray.getImageSize();
		int x = (size.width - imageSize.width) / 2;
		int y = (size.height - imageSize.height) / 2;
		if (!enabled)
		{
			imageArray.drawGrayImage(g, name, x, y);
			return;
		}
		int depress = (mouseDown || down)?1:0;
		x += depress;
		y += depress;
		if (mouseOver || mouseDown || down)
			g.draw3DRect(0, 0, size.width-1, size.height-1, !(mouseDown || down));
		imageArray.drawImage(g, name, x, y);
	
	}

}

////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	InsetPanel	is a panel that only accepts one child
//		and has the specified insets
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.Graphics;
import java.awt.Dimension;
import java.awt.Panel;
import java.awt.Component;
import java.awt.Insets;


public class InsetPanel extends Panel
{
	Insets insets = new Insets(0, 0, 0, 0);
	Component child = null;
	boolean border = false;

	public InsetPanel(int top, int left, int bottom, int right)
	{
		setLayout(null);
		insets.top = top;
		insets.left = left;
		insets.bottom = bottom;
		insets.right = right;
	}

	public void setBorder(boolean border)
	{
		this.border = border;
	}

	public void paint(Graphics g)
	{
		if (border)
		{
			g.setColor(getBackground());
			g.draw3DRect(0, 0, size().width-1, size().height-1, true);
		}
	}

	public Component add(Component c)
	{
		if (c != null)
		{
			removeAll();
			child = c;
			return super.add(c);
		}
		return null;
	}

	public Dimension preferredSize()
	{
		Dimension size = new Dimension(100, 100);
		if (child != null)
		{
			size = child.preferredSize();
		}
		size.width += insets.left + insets.right;
		size.height += insets.top + insets.bottom;
		return size;
	}

	public void update(Graphics g)
	{
		paint(g);
	}

	public void layout()
	{
		if (child != null)
		{
			int x = insets.left;
			int y = insets.top;
			int width = size().width - (insets.left + insets.right);
			int height = size().height - (insets.top + insets.bottom);
			child.reshape(x, y, width, height);
		}
	}

}

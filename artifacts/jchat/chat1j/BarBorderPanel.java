////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	BarBorderPanel accepts one child component,
//	and displays it with a 4 pixel wide raised border
//	around it.
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;

public class BarBorderPanel extends Panel
{
	Insets insets;
	Component child;

	// insets specify how much room to leave on each side of child component
	public BarBorderPanel(Insets insets)
	{
		this.insets = insets;
	}

	public Component add(Component c)
	{
		removeAll();
		child = c;
		return super.add(c);
	}

	public void layout()
	{
		doLayout();
	}

	public Dimension preferredSize()
	{
		return getPreferredSize();
	}

	public Dimension getPreferredSize()
	{
		if (child == null)
		{
			return new Dimension(100, 100);
		}
		Dimension want = child.preferredSize();
		want.width += insets.left + insets.right;
		want.height += insets.top + insets.bottom;
		return want;
	}

	public void doLayout()
	{
		if (child == null)
		{
			return;
		}
		Dimension size = size();
		child.reshape(insets.left, insets.top,
			size.width - (insets.left + insets.right),
			size.height - (insets.top + insets.bottom));
	}

	public void update(Graphics g)
	{
		paint(g);
	}

	public void paint(Graphics g)
	{
		g.setColor(getBackground());
		Dimension size = size();
		g.fillRect(0, 0, size.width, size.height);
		int x = insets.left;
		int y = insets.top;
		int width = size.width - (insets.left + insets.right);
		int height = size.height - (insets.top + insets.bottom);
		drawBar(g, x, y, width, height);
	}

	public void drawBar(Graphics g, int x, int y, int width, int height)
	{
		width += 1;		//	fix
		height += 1;	//	fix
		Color color = g.getColor();
		Color lighter = color.brighter();
		Color darker = color.darker();
		Color black = Color.black;
		g.setColor(color);
		g.drawLine(x, y+height, x+width, y+height);
		g.drawLine(x+width, y, x+width, y+height);
		g.setColor(black);
		g.drawLine(x-1, y-1, x+width, y-1);
		g.drawLine(x-1, y-1, x-1, y+height);
		g.drawLine(x-4, y+height+3, x+width+3, y+height+3);
		g.drawLine(x+width+3, y-4, x+width+3,y+height+3);
		g.setColor(darker);
		g.drawLine(x-2, y-2, x+width, y-2);
		g.drawLine(x-2, y-2, x-2, y+height);
		g.drawLine(x-3, y+height+2, x+width+2, y+height+2);
		g.drawLine(x+width+2, y-3, x+width+2, y+height+2);
		g.setColor(lighter);
		g.drawLine(x-4, y-4, x+width+2, y-4);
		g.drawLine(x-4, y-4, x-4, y+height+2);
		g.drawLine(x+width, y-1, x+width, y+height);
		g.drawLine(x-1, y+height, x+width, y+height);
	}

}

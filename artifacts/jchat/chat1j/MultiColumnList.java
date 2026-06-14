////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	MultiColumnList
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;

public class MultiColumnList extends ElementList
{
	String[] fields;
	int[] positions;
	MultiColumnElement top;
		
	//	note, Strings in the items array can be padded with spaces
	//	to achieve the desired spacing
	public MultiColumnList(String[] fields)
	{
		if (fields == null)
		{
			throw new NullPointerException("Null Arg in MultiColumnList constructor");
		}
		this.fields = fields;
		FontMetrics fm = Static.getFontMetrics();
		positions = new int[fields.length];
		positions[0] = 5;	// left hand buffer
		for (int i = 1; i < fields.length; i++)
		{
			positions[i] = positions[i-1] + fm.stringWidth(fields[i-1]);									
		}
		top = new MultiColumnElement(fields, positions, itemHeight);
		top.setEnabled(false);
		top.top = true;
		add(top);
	}

	public void clearElements(boolean repaint)
	{
		super.clearElements(repaint);
		add(top);
		doLayout();
		if (repaint)
		{
			//	force synchronous repaint
			paint(getGraphics());
		}
	}

	public void addItem(String[] items)
	{
		addItem(items, true);
	}

	public void addItem(String[] items, boolean paint)
	{
		if (items == null || items.length != fields.length)
		{
			return;
		}
		add(new MultiColumnElement(items, positions, itemHeight), paint);				
	}

	public Dimension getPreferredSize()
	{
		// temp code
		return new Dimension(300, 300);
	}

}

class MultiColumnElement extends ImageTextElement
{
	String[] items;
	int[] positions;
	int height;
	int baseline;
	FontMetrics fm;
	boolean top = false;

	public MultiColumnElement(String[] items, int[] positions, int height)
	{
		super(null, null, null, null, items[0], height);
		if (items == null || positions == null || items.length != positions.length)
		{
			throw new IllegalArgumentException();
		}
		this.items = items;
		this.positions = positions;
		this.height = height;
		fm = Static.getFontMetrics();
		baseline = fm.getAscent() + (fm.getLeading() + height - fm.getHeight()) / 2;
	}

	public Dimension getPreferredSize()
	{
		return new Dimension(fm.stringWidth(items[items.length-1]) + positions[positions.length-1], height);
	}

	public void setPositions(int[] positions)
	{
		if (positions == null || positions.length != this.positions.length)
		{
			return;
		}
		this.positions = positions;
	}

	public void drawString(Graphics g)
	{
		Color old = g.getColor();
		for (int i = 0; i < items.length; i++)
		{
			int x = positions[i];
			int y = baseline;
			g.setColor(highlight?highlightColor:background);
			g.fillRect(x-4, 0, bounds.width - (x-4), height);
			g.setColor(old);
			if (top)
				g.drawString(items[i], x + 2, y + 3);
			else
				g.drawString(items[i], x , y);
		}
		if (top)
		{
			g.setColor(background.darker());
			g.drawLine(0, bounds.height-1, bounds.width-1, bounds.height-1);
			g.setColor(old);
		}
	}

	public String toString()
	{
		if (items == null || items.length == 0)
		{
			return super.toString();
		}
		StringBuffer sb = new StringBuffer(items[0]);
		for (int i = 1; i < items.length; i++)
		{
			sb.append(" " + items[i]);
		}
		return sb.toString();
	}
}


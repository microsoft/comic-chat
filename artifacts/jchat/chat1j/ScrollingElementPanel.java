////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	a Vertically Scrolling ElementPanel
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;
import java.util.*;

public class ScrollingElementPanel extends ElementPanel
{
	int scrollbarWidth = 16;
	FlatScrollbar vertical = new FlatScrollbar(FlatScrollbar.VERTICAL);
	FlatScrollbar horizontal = new FlatScrollbar(FlatScrollbar.HORIZONTAL);
	Canvas lowerRight = new Canvas();
	int buffer = 0;
	Dimension lastSize = new Dimension(0, 0);

	//	maximum number of elements displayed before top are pruned
	int maximum = -1;

	public ScrollingElementPanel()
	{
		setLayout(null);
		add(vertical);
		add(horizontal);
		add(lowerRight);
	}

	public void setBottomBuffer(int buffer)
	{
		this.buffer = buffer;
		setScrollbarValues();
	}

	public void doLayout()
	{
		boolean bottom = atBottom();
		super.doLayout();
		Dimension size = size();
		vertical.reshape(size.width - scrollbarWidth, 0,
			scrollbarWidth, size.height - (horizontal.isVisible() ? scrollbarWidth : 0));
		horizontal.reshape(0, size.height - scrollbarWidth,
			size.width - (vertical.isVisible() ? scrollbarWidth : 0), scrollbarWidth);
		if (vertical.isVisible() && horizontal.isVisible())
			lowerRight.reshape(size.width - scrollbarWidth, size.height - scrollbarWidth, scrollbarWidth, scrollbarWidth);
		else
			lowerRight.hide();
		// setScrollbarValues
		setScrollbarValues();
		if (bottom)
			toBottom();
		repaint();
		lastSize = size;
	}

	public void add(Element element, boolean paint)
	{
		if (element != null)
		{
			elements.addElement(element);
			if (element instanceof MouseListener)
				mouseListeners.addElement(element);
			element.setHost(this);
			layout(element);
			if (maximum > 0 && elements.size() > maximum)
				trim();
			if (paint)
			{
				setScrollbarValues();
				repaint();
			}
		}
	}

	//	this trims 10% from the bottom of elements vector
	public void trim()
	{
		System.out.println("******************************************");
		System.out.println("Trimming " + maximum / 10);
		System.out.println("******************************************");
		boolean bottom = atBottom();
		int number = maximum / 10;
		for (int i = 0; i < number; i++)
		{
			remove(firstElement());
		}
		Element first = firstElement();
		//	now the first element is at y = 0;
		int y = first.getBounds().y;
		Enumeration e = elements.elements();
		while (e.hasMoreElements())
		{
			Element element = (Element)e.nextElement();
			element.bounds.y -= y;
		}
		area.height -= y;
		origin.y -= y;
	}

	public Element elementAt(int x, int y)
	{
		// override this method to be more efficient
		return super.elementAt(x, y);
	}

	public void setScrollbarValues()
	{
		Dimension size = getSize();
		vertical.setPageIncrement(size.height);
		vertical.setLineIncrement(10);
		horizontal.setPageIncrement(size.width);
		horizontal.setLineIncrement(20);
		// scrollbar is needed
		int min = 0;
		int max = area.height + buffer;
		int value = vertical.getValue();
		int visible = size.height - (horizontal.isVisible()?scrollbarWidth:0);
		vertical.setValues(value, visible, min, max);
		
		max = area.width;
		value = horizontal.getValue();
		visible = size.width - (vertical.isVisible()?scrollbarWidth:0);
		horizontal.setValues(value, visible, min, max);
	}

	public boolean atBottom()
	{
		Element element = lastElement();
		if (element == null)
		{
			return true;
		}
		Rectangle verticalBounds = getVerticalBounds();
		verticalBounds.height = lastSize.height;
		return element.intersects(verticalBounds);
	}

	public void toBottom()
	{
		int y = vertical.getMaximum() - vertical.getVisibleAmount();
		vertical.setValue(y);
	}

	public boolean keyDown(Event e, int key)
	{
		if (key > 1001 && key < 1006)
		{
			if (key == 1002)		//	page up
				vertical.blockDecrement();
			else if (key == 1003)	//	page down
				vertical.blockIncrement();
			else if (key == 1004)	//	up arrow
				vertical.unitDecrement();
			else if (key == 1005)	//	down arrow
				vertical.unitIncrement();
			//	forge a mousemove to avoid highlight moving out from under mouse
			mouseMove(new Event(this, Event.MOUSE_MOVE, null), last.x, last.y);
		}
		return super.keyDown(e, key);
	}

	public boolean handleEvent(Event e)
	{
		if (e.target == vertical || e.target == horizontal)
		{
			if (e.id > 600 && e.id < 606)
				scrollEvent(horizontal.getValue(), vertical.getValue());
		//	else if (e.id == Event.MOUSE_ENTER)
		//		mouseExit();
		}
		return super.handleEvent(e);
	}

	public Dimension getSize()
	{
		Dimension size = size();
		if (vertical.isVisible())
			size.width -= scrollbarWidth;
		if (horizontal.isVisible())
			size.height -= scrollbarWidth;
		return size;
	}

	public void scrollEvent(int x, int y)
	{
		try
		{
			boolean repaint = x != origin.x || y != origin.y;
			origin.x = x;
			origin.y = y;
			if (repaint)
				repaint();
		}
		catch(Exception e)
		{
			// not expected
			System.out.println(e);
		}
	}

}


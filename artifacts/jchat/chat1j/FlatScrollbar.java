////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	FlatScrollbar emulates IE 4.0 java Scrollbar
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;

public class FlatScrollbar extends Canvas
{
	public final static int HORIZONTAL = 0;
	public final static int VERTICAL = 1;

	protected final static int DECREMENT = 0;
	protected final static int BLOCK_DECREMENT = 1;
	protected final static int SLIDEBAR = 2;
	protected final static int BLOCK_INCREMENT = 3;
	protected final static int INCREMENT = 4;
	protected final static int UP = 0;
	protected final static int DOWN = 1;
	protected final static int LEFT = 2;
	protected final static int RIGHT = 3;

	protected static int width = 16;

	protected int value, visible, minimum, maximum, unit, block, orientation;

	protected Image offscreen;
	// indicates what part of the component the mouse was pressed on.
	protected int down = -1;
	protected Point pressedOnBar = new Point(-1, -1);
	protected Point last = new Point(-1, -1);
	protected Thread thread = new FlatScrollbarThread(this, 50);
	
	// Constructors
	public FlatScrollbar()
	{
		this(VERTICAL);
	}

	public FlatScrollbar(int orientation)
	{
		this(orientation, 0, 10, 0, 100);
	}

	public FlatScrollbar(int orientation, int value, int visible, int minimum, int maximum)
	{
		setOrientation(orientation);
		setValues(value, visible, minimum, maximum);
	}
	// Component Methods
	public void update(Graphics g)
	{
		paint(g);
	}

	public Color getBackground()
	{
		Container gparent = getParent().getParent();
		if (gparent != null)
			return gparent.getBackground();
		else
			return super.getBackground();
	}

	public void paint(Graphics g)
	{
		Dimension size = getSize();
		boolean vertical = isVertical();
		Rectangle decrement = getDecrementBounds();
		Rectangle increment = getIncrementBounds();
		Rectangle slidebar = getSlidebarBounds();
		boolean enabled = !slidebar.isEmpty() && isEnabled();
		Graphics og = getOffscreenGraphics();
		if (og == null)
			return;
		og.setColor(enabled?getBackground().darker():getBackground());
		og.fillRect(0, 0, size.width, size.height);
		og.setColor(getBackground());
		fill3DRect(og, decrement, down != DECREMENT);
		if (!slidebar.isEmpty())
			fill3DRect(og, slidebar, true);
		fill3DRect(og, increment, down != INCREMENT);
		og.setColor(enabled?Color.black:og.getColor().darker());
		drawCenteredArrow(og, decrement, vertical?UP:LEFT, down == DECREMENT);
		drawCenteredArrow(og, increment, vertical?DOWN:RIGHT, down == INCREMENT);
		g.drawImage(offscreen, 0, 0, this);
	}

	public Graphics getOffscreenGraphics()
	{
		Dimension size = getSize();
		if (offscreen == null || offscreen.getWidth(null) != size.width || offscreen.getHeight(null) != size.height)
		{
			if (offscreen != null)
				offscreen.flush();
			offscreen = createImage(size.width, size.height);
		}
		if (offscreen != null)
			return offscreen.getGraphics();
		else
			return null;
	}

	public boolean mouseDown(Event e, int x, int y)
	{
		if (e.metaDown())
			return false;
		Rectangle slidebar = getSlidebarBounds();
		if (slidebar.isEmpty() || !isEnabled())
			return false;
		pressedOnBar.x = x - slidebar.x;
		pressedOnBar.y = y - slidebar.y;
		Rectangle increment = getIncrementBounds();
		Rectangle decrement = getDecrementBounds();
		if (decrement.inside(x, y))
		{
			setDown(DECREMENT);
			unitDecrement();
			repaint();
			notifyThread();
		}
		else if (increment.inside(x, y))
		{
			setDown(INCREMENT);
			unitIncrement();
			repaint();
			notifyThread();
		}
		else if (slidebar.inside(x, y))
		{
			setDown(SLIDEBAR);
		}
		else if (isVertical() && y < slidebar.y || isHorizontal() && x < slidebar.x)
		{
			setDown(BLOCK_DECREMENT);
			blockDecrement();
			notifyThread();
		}
		else
		{
			setDown(BLOCK_INCREMENT);
			blockIncrement();
			notifyThread();
		}
		last.x = x;
		last.y = y;
		return false;
	}

	protected synchronized void setDown(int down)
	{
		this.down = down;
	}

	protected synchronized int getDown()
	{
		return down;
	}

	// wakes up this scrollbars thread for processing
	protected void notifyThread()
	{
		synchronized(thread)
		{
			thread.notify();
		}
	}

	public boolean mouseDrag(Event e, int x, int y)
	{
		if (down == SLIDEBAR)
		{
			int loc = (isVertical()?y - pressedOnBar.y:x - pressedOnBar.x);
			setValue(getValueForLocation(loc));
			return false;
		}
		last.x = x;
		last.y = y;
		return false;
	}

	public boolean mouseUp(Event e, int x, int y)
	{
		boolean repaint = down == INCREMENT || down == DECREMENT;
		setDown(-1);
		if (repaint)
			repaint();
		return false;
	}

	public Dimension getSize()
	{
		return size();
	}

	public Dimension getPreferredSize()
	{
		if (isVertical())
			return new Dimension(width, 4 * width);
		else
			return new Dimension(4 * width, width);
	}

	public Dimension getMinimumSize()
	{
		return getPreferredSize();
	}

	// protected Methods
	protected void fill3DRect(Graphics g, Rectangle r, boolean raised)
	{
		g.fillRect(r.x, r.y, r.width, r.height);
		Color save = g.getColor();
		Color brighter = save.brighter();
		Color darker = save.darker().darker();
		g.setColor(raised?brighter:darker);
		g.drawLine(r.x, r.y, r.x + r.width-1, r.y);
		g.drawLine(r.x, r.y, r.x, r.y + r.height-1);
		g.setColor(raised?darker:brighter);
		g.drawLine(r.x + r.width-1, r.y, r.x + r.width-1, r.y +r.height-1);
		g.drawLine(r.x, r.y + r.height-1, r.x + r.width-1, r.y + r.height-1);
		g.setColor(save);
	}

	protected void drawCenteredArrow(Graphics g, Rectangle r, int direction, boolean pressed)
	{
		Dimension size = new Dimension(7, 4);
		if (direction == UP || direction == DOWN)
		{
			boolean up = direction == UP;
			int width = size.width;
			int height = size.height;
			int dx = r.x + (r.width - width) / 2 + (pressed?1:0);
			int dy = r.y + (r.height - height) / 2 + (pressed?1:0);
			int length = up?0:width-1;
			for (int i = 0; i < height; i++)
			{
				int x = dx + (width - length) / 2;
				int y = dy + i;
				g.drawLine(x, y, x + length, y);
				length += up?2:-2;
			}
		}
		else
		{
			boolean left = direction == LEFT;
			int width = size.height;
			int height = size.width;
			int dx = r.x + (r.width - width) / 2 + (pressed?1:0) + (left?-1:0);
			int dy = r.y + (r.height - height) / 2 + (pressed?1:0);
			int length = left?0:height-1;
			for (int i = 0; i < width; i++)
			{
				int x = dx + i;
				int y = dy + (height - length) / 2;
				g.drawLine(x, y, x, y + length);
				length += left?2:-2;
			}
		}
	}

	protected void unitIncrement()
	{
		setValue(value + unit);
	}

	protected void unitDecrement()
	{
		setValue(value - unit);
	}

	protected void blockIncrement()
	{
		setValue(value + block);
	}

	protected void blockDecrement()
	{
		setValue(value - block);
	}

	protected boolean isVertical()
	{
		return orientation == VERTICAL;
	}

	protected boolean isHorizontal()
	{
		return orientation == HORIZONTAL;
	}

	protected Rectangle getDecrementBounds()
	{
		Dimension size = getSize();
		if (isVertical())
			return new Rectangle(0, 0, size.width, width);
		else
			return new Rectangle(0, 0, width, size.height);
	}

	protected Rectangle getIncrementBounds()
	{
		Dimension size = getSize();
		if (isVertical())
			return new Rectangle(0, size.height - width, size.width, width);
		else
			return new Rectangle(size.width - width, 0, width, size.height);
	}

	protected Rectangle getSlidebarBounds()
	{
		Dimension size = getSize();
		boolean vertical = isVertical();
		int length = (vertical?size.height:size.width) - 2 * width;
		int range = maximum - minimum;
		if (visible >= range)
			return new Rectangle(0, 0, 0, 0);
		int barLength = ((range > 0)?visible * length / range:length) + 1;
		if (barLength > length)
			barLength = length;
		int minLength = 5;
		if (barLength < minLength)
			barLength = minLength;
		int barLocation = ((range > 0)?value * length / range:0) + width;
		if (length + width < barLocation + barLength)
			barLocation = length + width - barLength;
		if (vertical)
			return new Rectangle(0, barLocation, size.width, barLength);
		else
			return new Rectangle(barLocation, 0, barLength, size.height);
	}

	protected int getValueForLocation(int loc)
	{
		int length = (isVertical()?getSize().height:getSize().width) - 2 * width;
		return (loc - width) * (maximum - minimum) / length;
	}

	// Scrollbar Methods
	public int getUnitIncrement()
	{
		return unit;
	}

	public void setUnitIncrement(int unit)
	{
		this.unit = unit;
	}

	public int getBlockIncrement()
	{
		return block;
	}

	public void setBlockIncrement(int block)
	{
		this.block = block;
	}

	public int getMaximum()
	{
		return maximum;
	}

	public void setMaximum(int maximum)
	{
		setValues(value, visible, minimum, maximum);
	}

	public int getMinimum()
	{
		return minimum;
	}

	public void setMinimum(int minimum)
	{
		setValues(value, visible, minimum, maximum);
	}

	public int getOrientation()
	{
		return orientation;
	}

	public void setOrientation(int orientation)
	{
		if (orientation == VERTICAL || orientation == HORIZONTAL)
			this.orientation = orientation;
		// repaint;
	}

	public int getVisibleAmount()
	{
		return visible;
	}

	public void setVisibleAmount(int visible)
	{
		setValues(value, visible, minimum, maximum);
		//	repaint;
	}

	public int getValue()
	{
		return value;
	}

	public void setValue(int value)
	{
		setValues(value, visible, minimum, maximum);
	}

	public void setValues(int value, int visible, int minimum, int maximum)
	{
		if (maximum <= minimum)
			maximum = minimum + 1;
        if (visible > maximum - minimum)
			visible = maximum - minimum;
        if (visible < 1)
			visible = 1;
		if (value < minimum)
			value = minimum;
		if (value > maximum - visible)
			value = maximum - visible;
		boolean repaint =
			this.value != value ||
			this.visible != visible ||
			this.minimum != minimum ||
			this.maximum != maximum;
		this.value = value;
		this.visible = visible;
		this.minimum = minimum;
		this.maximum = maximum;
		if (repaint)
			repaint();
		// post event
		postEvent();
	}

	public void postEvent()
	{
		postEvent(new Event(this, Event.SCROLL_ABSOLUTE, new Integer(value)));
	}

	protected String paramString()
	{
		return "value=" + value + " visible=" + visible + " minimum=" + minimum + " maximum=" + maximum;
	}

	// Deprecated Methods
	public int getVisible()
	{
		return getVisibleAmount();
	}

	public void setPageIncrement(int l)
	{
		setBlockIncrement(l);
	}

	public void setLineIncrement(int l)
	{
		setUnitIncrement(l);
	}

	public int getLineIncrement()
	{
		return getUnitIncrement();
	}

	public int getPageIncrement()
	{
		return getBlockIncrement();
	}

	public Dimension preferredSize()
	{
		return getPreferredSize();
	}

	public Dimension minimumSize()
	{
		return getMinimumSize();
	}

}

class FlatScrollbarThread extends Thread
{

	FlatScrollbar parent;
	int delay;

	public FlatScrollbarThread(FlatScrollbar parent, int delay)
	{
		this.parent = parent;
		this.delay = delay;
		setPriority(Thread.NORM_PRIORITY - 1);
		start();
	}

	public void run()
	{
		while (true)
		{
			try
			{
				synchronized(this)
				{
					wait();
				}
			}
			catch(Exception e)
			{
			}
			while (parent.getDown() >= 0)
			{
				try
				{
					sleep(delay);
				}
				catch(Exception e)
				{
				}
				int down = parent.getDown();
				if (down == parent.INCREMENT)
					parent.unitIncrement();
				else if (down == parent.DECREMENT)
					parent.unitDecrement();
				else
				{
					Rectangle slider = parent.getSlidebarBounds();
					Point last = parent.last;
					boolean vertical = parent.isVertical();
					if (down == parent.BLOCK_DECREMENT)
					{
						if ((vertical && last.y < slider.y) || (!vertical && last.x < slider.x))
							parent.blockDecrement();
					}
					else if (down == parent.BLOCK_INCREMENT)
					{
						if ((vertical && last.y > slider.height + slider.y) || (!vertical && last.x > slider.width + slider.x))
							parent.blockIncrement();
					}
				}
			}
		}
	}

}




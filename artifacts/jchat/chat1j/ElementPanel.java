import java.awt.*;
import java.util.*;

public class ElementPanel extends Panel implements ElementHost
{
	Vector elements = new Vector();
	
	Vector mouseListeners = new Vector();
	Element mouseFocus = null;	// the element the mouse is currently over
	Element mouseDown = null;	// the element the mouse was pressed on
	
	Dimension area = new Dimension(0, 0);	// area occupied by all elements
	// coordinate representing the location of the top left corner
	// of this panel, its y value will increase as the panel is scrolled down
	Point origin = new Point(0, 0);
	Point last = new Point(0, 0);

	// Graphics context for double buffering
	Image offscreen;

	public ElementPanel()
	{
		setLayout(null);
	}
	
	public void resetArea()
	{
		area.width = 0;
		area.height = 0;
	}

	public void layout()
	{
		doLayout();
	}

	public void doLayout()
	{
		resetArea();
		Element previous = null;
		Enumeration e = getElements();
		while (e.hasMoreElements())
		{
			Element element = (Element)e.nextElement();
			layout(element, previous);
			previous = element;
		}
	}

	public Rectangle getVisibleBounds()
	{
		return new Rectangle(origin.x, origin.y, getSize().width, getSize().height);
	}

	public Rectangle getVerticalBounds()
	{
		return new Rectangle(0, origin.y, Math.max(area.width, getSize().width), getSize().height);
	}

	public void layout(Element element)
	{
		if (element != null)
		{
			int index = elements.indexOf(element);
			layout(element, elementAt(index - 1));
		}
	}

	public void layout(Element element, Element previous)
	{
		if (element != null)
		{
			element.setSize(element.getPreferredSize());
			if (previous == null)
			{
				element.setLocation(0, 0);
			}
			else
			{
				Point previousTopRight = new Point(previous.bounds.x + previous.bounds.width, previous.bounds.y);
				int room = size().width - previousTopRight.x;
				if (element.bounds.width > room || element.addBreak() || previous.addBreak())
				{
					element.setLocation(0, area.height);
				}
				else
				{
					element.setLocation(previousTopRight);
				}
			}
			area.width = Math.max(area.width, element.bounds.x + element.bounds.width);
			area.height = Math.max(area.height, element.bounds.y + element.bounds.height);
		}
	}

	public void toTop(Element element)
	{
		if (elements.removeElement(element))
		{
			elements.insertElementAt(element, 0);
		}
	}

	public void update(java.awt.Graphics g)
	{
		paint(g);
	}

	public java.awt.Graphics getOffscreenGraphics()
	{
		if (offscreen == null || offscreen.getWidth(null) != size().width || offscreen.getHeight(null) != size().height)
			offscreen = createImage(size().width, size().height);
		if (offscreen != null)
			return offscreen.getGraphics();
		else
			return null;
	}

	public synchronized void paint(java.awt.Graphics g)
	{
		Graphics og = getOffscreenGraphics();
		if (g == null || og == null)
			return;
	
		// inefficient code at first;
		og.setColor(getBackground());
		og.fillRect(0, 0, size().width, size().height);
		og.setColor(getForeground());
		Rectangle visible = getVerticalBounds();

		synchronized(elements)
		{
			if (!elements.isEmpty())
			{
				int min = 0;
				int max = elements.size() - 1;
				int check = 0;
				while (true)
				{
					check = (min + max) / 2;
					Element element = (Element)elements.elementAt(check);
					if (element.intersects(visible))
					{
						break;
					}
					else
					{
						if (element.bounds.y < origin.y)
						{
							//	element index is too low, check higher
							min = check + 1;
						}
						else
						{
							//	element index is too high, check lower
							max = check - 1;
						}
					}
				}
				//	int check now refers to a visible element
				//	draw check element and elements above
				int up = check;
				while (true)
				{
					if (up < 0)
					{
						break;
					}
					Element element = (Element)elements.elementAt(up);
					if (element.intersects(visible))
					{
						paint(og, element);
					}
					else
					{
						break;
					}
					up--;
				}
				//	draw elements below check
				int down = check + 1;
				while (true)
				{
					if (down >= elements.size())
					{
						break;
					}
					Element element = (Element)elements.elementAt(down);
					if (element.intersects(visible))
					{
						paint(og, element);
					}
					else
					{
						break;
					}
					down++;
				}
			}
		}

	/*	Enumeration e = getElements();
		while (e.hasMoreElements())
		{
			Element element = (Element)e.nextElement();
			if (element.intersects(visible))
			{
				paint(offscreenGraphics, element);
			}
		}*/
		g.drawImage(offscreen, 0, 0, null);
	}

	public synchronized void paint(Element element)
	{
		repaint();
		/*
		if (!isVisible())
		{
			return;
		}
		if (element != null)
		{
			if (!element.intersects(getVisibleBounds()))
			{
				return;
			}
			Graphics og = getOffscreenGraphics();
			if (og == null)
				return;
			paint(og, element);
			Rectangle bounds = element.getBounds();
			bounds.x -= origin.x;
			bounds.y -= origin.y;
			Graphics g = getGraphics();
			if (g != null)
			{
				g.clipRect(bounds.x, bounds.y, bounds.width, bounds.height);
				g.drawImage(offscreen, 0, 0, null);
			}
		}*/
	}

	public synchronized void paint(java.awt.Graphics g, Element element)
	{
		if (g != null && element != null)
		{
			element.paint(g, - origin.x, - origin.y);
		}
	}

	public void dispatchEvent(Event event)
	{
		if (event != null)
		{
			postEvent(event);
		}
	}

	public Dimension preferredSize()
	{
		return getPreferredSize();
	}

	public Dimension getPreferredSize()
	{
		return new Dimension(area.width, area.height);
		//return new Dimension(300, 500);
	}

	public void add(Element element)
	{
		add(element, true);
	}
	
	public void add(Element element, boolean paint)
	{
		if (element != null)
		{
			elements.addElement(element);
			if (element instanceof MouseListener)
			{
				mouseListeners.addElement(element);
			}
			element.setHost(this);
			layout(element);
			if (paint && element.intersects(getVisibleBounds()))
			{
				repaint();
			}
		}
	}

	public void remove(Element element)
	{
		if (element != null)
		{
			element.setHost(null);
			elements.removeElement(element);
			if (element instanceof MouseListener)
			{
				mouseListeners.removeElement(element);
			}
		}
	}

	public void invalidate(Element element)
	{
		if (element != null)
		{
			// default is inefficient
			invalidate();
		}
	}

	public boolean contains(Element element)
	{
		if (element != null)
		{
			return elements.contains(element);
		}
		else
		{
			return false;
		}
	}

	public int indexOf(Element element)
	{
		if (element != null)
		{
			return elements.indexOf(element);
		}
		else
		{
			return -1;
		}
	}

	public int countElements()
	{
		return elements.size();
	}

	public Element elementAt(int x, int y)
	{
		// default implementation is inefficient
		Enumeration e = getElements();
		while (e.hasMoreElements())
		{
			Element element = (Element)e.nextElement();
			if (element.contains(x + origin.x, y + origin.y))
			{
				return element;
			}
		}
		return null;
	}

	public Element mouseListenerAt(int x, int y)
	{
		// default implementation is inefficient
		Enumeration e = mouseListeners.elements();
		while (e.hasMoreElements())
		{
			Element element = (Element)e.nextElement();
			if (element.contains(x + origin.x, y + origin.y))
			{
				return element;
			}
		}
		return null;
	}

	public Element firstElement()
	{
		if (!elements.isEmpty())
		{
			return (Element)elements.firstElement();
		}
		else
		{
			return null;
		}
	}

	public Element lastElement()
	{
		if (!elements.isEmpty())
		{
			return (Element)elements.lastElement();
		}
		else
		{
			return null;
		}
	}

	public Element elementAt(int index)
	{
		if (index >= 0 && index < elements.size())
		{
			return (Element)elements.elementAt(index);
		}
		else
		{
			return null;
		}
	}

	public Enumeration getElements()
	{
		return elements.elements();
	}

	public void clearElements()
	{
		clearElements(true);
	}

	public void clearElements(boolean paint)
	{
		elements.removeAllElements();
		mouseListeners.removeAllElements();
		mouseFocus = null;
		mouseDown = null;
		origin.x = 0;
		origin.y = 0;
		resetArea();
		if (paint)
			repaint();
	}

	public boolean mouseDown(Event e, int x, int y)
	{
		if (e.target != this)
			return false;
		mouseDown = mouseFocus;
		if (mouseFocus != null)
		{
			translate(e, mouseFocus);
			if (mouseFocus.isEnabled())
				mouseFocus.mousePressed(e);
		}
		return false;
	}

	public boolean mouseUp(Event e, int x, int y)
	{
		if (e.target != this)
			return false;
		if (mouseFocus != null)
		{
			translate(e, mouseFocus);
			mouseFocus.mouseReleased(e);
			if (mouseDown == mouseFocus && mouseFocus == mouseListenerAt(x, y) && mouseFocus.isEnabled())
				mouseFocus.mouseClicked(e);
		}
		mouseDown = null;
		return false;
	}

	public boolean mouseEnter(Event e, int x, int y)
	{
		if (e.target != this)
			return false;
		mouseFocus = mouseListenerAt(x, y);
		if (mouseFocus != null)
		{
			translate(e, mouseFocus);
			if (mouseFocus.isEnabled())
			{
				mouseFocus.mouseEntered(e);
			}
		}
		return true;
	}

	public void mouseExit()
	{
		Event e = new Event(this, Event.MOUSE_EXIT, null);
		mouseUp(e, 0, 0);
		mouseExit(e, 0, 0);
		repaint();
	}

	public boolean mouseExit(Event e, int x, int y)
	{
		if (e.target != this)
			return false;
		if (mouseFocus != null)
		{
			translate(e, mouseFocus);
			if (mouseFocus.isEnabled())
			{
				mouseFocus.mouseExited(e);
			}
			mouseFocus = null;
		}
		return true;
	}

	public boolean mouseDrag(Event e, int x, int y)
	{
		last.x = x;
		last.y = y;
		if (e.target != this)
			return false;
		//	does not currently support drag
		//	return mouseMove(e, x, y);
		return true;
	}

	public boolean mouseMove(Event e, int x, int y)
	{
		last.x = x;
		last.y = y;
		if (e.target != this)
			return false;
		if (mouseFocus == null)
		{
			mouseFocus = mouseListenerAt(x, y);
			if (mouseFocus != null)
			{
				translate(e, mouseFocus);
				if (mouseFocus.isEnabled())
					mouseFocus.mouseEntered(e);
			}
			return true;
		}
		// quick lookup to see if focus has remained the same
		if (((Element)mouseFocus).contains(x + origin.x, y + origin.y))
		{
			// MouseMotionListener not currently supported
			return true;
		}
		// if we are here, then there is a new focus
		translate(e, mouseFocus);
		if (mouseFocus.isEnabled())
		{
			mouseFocus.mouseExited(e);
		}
		mouseFocus = mouseListenerAt(x, y);
		if (mouseFocus != null)
		{
			translate(e, mouseFocus);
			if (mouseFocus.isEnabled())
				mouseFocus.mouseEntered(e);
		}
		return true;
	}

	public Color getForeground()
	{
		return super.getForeground();
	}

	public Color getBackground()
	{
		return super.getBackground();
	}

	private void translate(Event e, Element element)
	{
		e.x -= element.bounds.x;
		e.y -= element.bounds.y;
	}

	public Dimension getSize()
	{
		return size();
	}
}



////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	An Element represents the smallest visual component
//	of a user interface.  It can receive, and respond to
//	MouseEvents, by implementing MouseListener and
//	overriding the appropriate methods.
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;
import java.awt.Graphics;

public abstract class Element
{
	ElementHost host = null;
	Rectangle bounds = new Rectangle(0,0,0,0);
	boolean visible = true;
	boolean enabled = true;

	public abstract void paint(Graphics g);

	public Dimension getPreferredSize()
	{
		return new Dimension(bounds.width, bounds.height);
	}
	
	// x, y represent its hosts position in the Graphics context
	public final void paint(Graphics g, int x, int y)
	{
		if (visible && g != null)
		{
			int dx = x + bounds.x;
			int dy = y + bounds.y;
			g.translate(dx, dy);
			paint(g);
			g.translate(-dx, -dy);
		}
	}

	public final ElementHost getHost()
	{
		return host;
	}

	public void setHost(ElementHost host)
	{
		this.host = host;
	}

	// indicates item should be on its own line
	public boolean addBreak()
	{
		return false;
	}

	public final void invalidate()
	{
		if (host != null)
		{
			host.invalidate(this);
		}
	}

	public final void paint()
	{
		if (host != null)
		{
			host.paint(this);
		}
	}

	public final boolean isEnabled()
	{
		return enabled;		
	}

	public void setEnabled(boolean enabled)
	{
		boolean paint = this.enabled != enabled;
		this.enabled = enabled;
		if (paint)
		{
			paint();
		}
	}

	public final boolean isVisible()
	{
		return visible;
	}

	public void setVisible(boolean visible)
	{
		this.visible = visible;
	}

	public final Dimension getSize()
	{
		return new Dimension(bounds.width, bounds.height);
	}

	public final void setSize(Dimension size)
	{
		if (size != null)
		{
			setSize(size.width, size.height);
		}
	}

	public void setSize(int width, int height)
	{
		bounds.width = width;
		bounds.height = height;
	}

	public final Point getLocation()
	{
		return new Point(bounds.x, bounds.y);
	}

	public final void setLocation(Point location)
	{
		if (location != null)
		{
			setLocation(location.x, location.y);
		}
	}

	public void setLocation(int x, int y)
	{
		bounds.x = x;
		bounds.y = y;
	}

	public final Rectangle getBounds()
	{
		return new Rectangle(bounds.x, bounds.y, bounds.width, bounds.height);
	}

	public final void setBounds(Rectangle bounds)
	{
		if (bounds != null)
		{
			setBounds(bounds.x, bounds.y, bounds.width, bounds.height);
		}
	}

	public void setBounds(int x, int y, int width, int height)
	{
		bounds.x = x;
		bounds.y = y;
		bounds.width = width;
		bounds.height = height;
	}

	public final boolean contains(Point location)
	{
		if (location != null)
		{
			return contains(location.x, location.y);
		}
		else
		{
			return false;
		}
	}

	public boolean contains(int x, int y)
	{
		return bounds.inside(x, y);
	}

	public boolean intersects(Rectangle r)
	{
		if (r != null)
		{
			return bounds.intersects(r);
		}
		else
		{
			return false;
		}
	}

	public Color getForeground()
	{
		if (host != null)
		{
			return host.getForeground();
		}
		else
		{
			return Color.black;
		}
	}

	public Color getBackground()
	{
		if (host != null)
		{
			return host.getBackground();
		}
		else
		{
			return Color.white;
		}
	}

	public void dispatchEvent(Event e)
	{
		if (e != null && host != null)
		{
			host.dispatchEvent(e);			
		}
	}

	public void mouseClicked(Event e)
	{

	}

	public void mouseEntered(Event e)
	{

	}

	public void mouseExited(Event e)
	{

	}

	public void mousePressed(Event e)
	{

	}

	public void mouseReleased(Event e)
	{

	}

}



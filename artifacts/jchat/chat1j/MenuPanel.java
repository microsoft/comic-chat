
import java.awt.*;
import java.util.*;

public class MenuPanel extends ElementPanel
{
	Hashtable items = new Hashtable();
	Dimension itemSize = new Dimension(10, 10);
	MenuListener listener = null;
	//	passed by the creator, will be passed back
	//	to MenuListener
	String arg;
	
	int dividerHeight = 6;
	//	the buffer between left and right edges, and divider
	int dividerBuffer = 2;
				
	public MenuPanel(MenuListener listener, String arg)
	{
		this.listener = listener;
		this.arg = arg;
	}

	public void addMenuItem(String item)
	{
		if (item == null)
		{
			add(new ToolbarDivider(dividerHeight, dividerHeight));
		}
		else
		{
			Element element = new MenuElement(item);
			add(element);
			items.put(item, element);
		}
	}

	public void addMenuItems(String[] item)
	{
		if (item != null)
		{
			for (int i = 0; i < item.length; i++)
			{
				addMenuItem(item[i]);
			}
		}
	}

	public void menuSelection(String item)
	{
		if (listener != null)
		{
			listener.menuSelection(item, arg);
		}
		dispose();
	}

	public void dispose()
	{
		Container parent = getParent();
		if (parent instanceof Window)
		{
			Window w = (Window)parent;
			w.hide();
			w.dispose();
		}
	}

	protected MenuElement getMenuElement(String name)
	{
		if (name != null)
		{
			return (MenuElement)items.get(name);
		}
		return null;
	}

	public void setDown(String item, boolean down)
	{
		MenuElement mi = getMenuElement(item);
		if (mi != null)
		{
			mi.setDown(down);			
		}
	}

	public void setEnabled(String item, boolean enabled)
	{
		MenuElement mi = getMenuElement(item);
		if (mi != null)
		{
			mi.setEnabled(enabled);			
		}
	}

	public void clear()
	{
		super.clearElements();
		items.clear();
	}

	public Dimension getPreferredSize()
	{
		int numberItems = 0;
		int numberDividers = 0;
		Dimension largest = new Dimension(10, 10);
		for (int i = 0; i < elements.size(); i++)
		{
			Element element = (Element)elements.elementAt(i);
			if (element instanceof MenuElement)
			{
				numberItems++;
				Dimension want = element.getPreferredSize();
				largest.width = Math.max(largest.width, want.width);
				largest.height = Math.max(largest.height, want.height);
			}
			else
			{
				numberDividers++;
			}
		}
		itemSize.width = largest.width;
		itemSize.height = largest.height;
		return new Dimension(largest.width, largest.height * numberItems + dividerHeight * numberDividers);
	}

	public void doLayout()
	{
		int y = 0;
		int width = getSize().width;
		for (int i = 0; i < elements.size(); i++)
		{
			Element element = (Element)elements.elementAt(i);
			if (element instanceof MenuElement)
			{
				element.setBounds(0, y, width, itemSize.height);
				y += itemSize.height;
			}
			else
			{
				element.setBounds(dividerBuffer, y, width - 2 * dividerBuffer, dividerHeight);
				y += dividerHeight;
			}
		}
	}

}

class MenuElement extends Element implements MouseListener
{
	//	this helps separate menu items
	static int yBuffer = 4;
	static int leftBuffer = 10;
	static int rightBuffer = 5;

	String text;
	int fontHeight = 10;
	int fontBaseline = 0;
	int stringWidth = 100;
	boolean down = false;
	boolean over = false;
	Font font;
				
	public MenuElement(String text)
	{
		this.text = text;
		font = Static.getDefaultFont();
		FontMetrics fm = Static.getFontMetrics(font);
		if (fm != null)
		{
			fontHeight = fm.getHeight();
			fontBaseline = fm.getAscent() + fm.getLeading() / 2;
			stringWidth = fm.stringWidth(text);
		}
	}

	public void setDown(boolean down)
	{
		this.down = down;
		paint();
	}

	public Dimension getPreferredSize()
	{
		return new Dimension(leftBuffer + stringWidth + rightBuffer, fontHeight + yBuffer);
	}

	public void paint(Graphics g)
	{
		g.setFont(font);
		int x = leftBuffer;
		int y = (getSize().height - fontHeight) / 2 + fontBaseline;
		if (over)
		{
			g.setColor(Color.blue.darker());
		}
		else
		{
			g.setColor(getBackground());
		}
		g.fillRect(0, 0, bounds.width, bounds.height);
		if (down)
		{
			//	draw a circle to the left
			int oBuffer = 2;
			int oDiameter = 6;
			int oX = oBuffer;
			int oY = (bounds.height - oDiameter) / 2;
			if (enabled)
			{
				g.setColor(Color.black);
				g.fillRect(oX, oY, oDiameter, oDiameter);
			}
			else
			{
				if (!over)
				{
					g.setColor(getBackground().brighter());
					g.fillRect(oX+1, oY+1, oDiameter, oDiameter);
				}
				g.setColor(getBackground().darker());
				g.fillRect(oX, oY, oDiameter, oDiameter);
			}
		}

		if (!enabled)
		{
			if (over)
			{
				g.setColor(getBackground());
				g.drawString(text, x, y);
			}
			else
			{
				g.setColor(getBackground().brighter());
				g.drawString(text, x+1, y+1);
				g.setColor(getBackground().darker());
				g.drawString(text, x, y);
			}
		}
		else
		{
			g.setColor(over?Color.white:Color.black);
			g.drawString(text, x, y);
		}
		g.setColor(getForeground());
	}

	public void mouseClicked(Event e)
	{
		if (!enabled)
		{
			return;
		}
		ElementHost host = getHost();
		if (host != null && host instanceof MenuPanel)
		{
			((MenuPanel)host).menuSelection(text);
		}		
	}

	public void mouseEntered(Event e)
	{
		over = true;
		paint();
	}

	public void mouseExited(Event e)
	{
		over = false;
		paint();
	}

	public String toString()
	{
		return text;
	}
}



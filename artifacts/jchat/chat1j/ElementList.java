import java.awt.*;
import java.util.*;

public class ElementList extends ScrollingElementPanel
{
	Hashtable nameTable = new Hashtable();
	Dimension preferred = new Dimension(100, 200);
	ImageArray imageArray;
	int itemHeight = Math.max(Static.getFontMetrics(getFont()).getHeight(), 10);
	// whether or not multiple selections are possible
	boolean multiple = false;
	// if this is false, then at least one thing must be selected at all times
	boolean deselect = true;

	public ElementList()
	{
		this(null, null);
	}
	
	public ElementList(String[] items)
	{
		this(null, null);
		if (items != null)
		{
			for (int i = 0; i < items.length; i++)
			{
				addItem(items[i]);
			}
		}
	}

	public ElementList(ImageArray imageArray, Dimension preferred)
	{
		this.imageArray = imageArray;
		this.preferred = preferred;
	}

	public void setDeselect(boolean deselect)
	{
		this.deselect = deselect;
		if (!deselect)
		{
			String item = getSelectedItem();
			if (item == null)
			{
				select(0);				
			}
		}
	}

	protected void select(TextElement element)
	{
		if (element != null)
		{
			if (!multiple)
			{
				clearSelections();
			}
			element.setHighlight(true);
			repaint();
		}
	}

	public void select(String name)
	{
		if (name == null)
		{
			return;
		}
		select((TextElement)nameTable.get(name));
	}

	public void select(int index)
	{
		select((TextElement)elementAt(index));
	}

	public void clearElements(boolean paint)
	{
		nameTable.clear();
		super.clearElements(paint);
	}

	public void doLayout()
	{
		Dimension size = size();
		area.width = size.width;
		area.height = 0;
		int width = size.width;
		itemHeight = getMaxItemSize().height;		
		Enumeration e = getElements();
		while (e.hasMoreElements())
		{
			Element element = (Element)e.nextElement();
			element.setBounds(0, area.height, width, itemHeight);
			area.height += itemHeight;
		}
		vertical.reshape(size.width - scrollbarWidth, 0,
								scrollbarWidth, size.height);
		// setScrollbarValues
		setScrollbarValues();
	}

	public void setScrollbarValues()
	{
		super.setScrollbarValues();
		if (area.height > size().height)
			vertical.show();
		else
			vertical.hide();
		horizontal.hide();
		int width = size().width - (vertical.isVisible()?scrollbarWidth:0);
		Enumeration e = getElements();
		while (e.hasMoreElements())
		{
			Element element = (Element)e.nextElement();
			element.bounds.width = width;
		}
	}

	//	this method is called after an add(Element) to avoid doing an entire doLayout
	public void layout(Element element)
	{
		if (element != null)
		{
			element.setBounds(0, area.height, getSize().width - (vertical.isVisible()?scrollbarWidth:0), itemHeight);
			area.height += itemHeight;
		}
	}

	public Dimension getMaxItemSize()
	{
		int maxHeight = 20;
		int maxWidth = 20;
		Enumeration e = getElements();
		while (e.hasMoreElements())
		{
			Element element = (Element)e.nextElement();
			Dimension size = element.getPreferredSize();
			maxHeight = Math.max(maxHeight, size.height);
			maxWidth = Math.max(maxWidth, size.width);
		}
		return new Dimension(maxWidth, maxHeight);
	}

	public void setMultipleSelections(boolean multiple)
	{
		this.multiple = multiple;
		clearSelections();
	}

	public Dimension getPreferredSize()
	{
		int widthBuffer = 20;
		if (preferred == null)
		{
			Dimension itemSize = getMaxItemSize();
			return new Dimension(itemSize.width + widthBuffer, itemSize.height * countElements());
		}
		else
		{
			return new Dimension(preferred.width, preferred.height);
		}
	}

	public void addItem(String item)
	{
		addItem(item, true);
	}
	
	public void addItem(String item, boolean repaint)
	{
		addItem(null, item, repaint);				
	}

	public void addItem(String imageName, String item)
	{
		addItem(imageName, item, true);
	}

	public void addItem(String imageName, String item, boolean repaint)
	{
		Element element = new ImageTextElement(imageArray, imageName, getFont(), getForeground(), item, itemHeight);
		add(element, repaint);
		nameTable.put(item, element);
	}

	public void addItemToTop(String imageName, String item, boolean repaint)
	{
		addItem(imageName, item, repaint);
		Element element = (Element)nameTable.get(item);
		if (element != null)
		{
			toTop(element);
		}
	}

	public void clearSelections(boolean repaint)
	{
		Enumeration e = getElements();
		while (e.hasMoreElements())
		{
			Object object = e.nextElement();
			if (object instanceof TextElement)
			{
				TextElement te = (TextElement)object;
				if (te.getHighlight() == true)
				{
					te.highlight = false;
					return;
				}
			}
		}
		if (repaint)
		{
			repaint();
		}
	}

	public void clearSelections()
	{
		clearSelections(true);
	}

	public void removeItem(String name)
	{
		if (name == null)
		{
			return;
		}
		Element element = (Element)nameTable.get(name);
		if (element == null)
		{
			return;
		}
		nameTable.remove(name);
		super.remove(element);
		//	This will cause the panel to do a new layout, and repaint
		doLayout();
		repaint();
	}

	public void setHighlight(String name, boolean highlight)
	{
		setHighlight(name, highlight, true);
	}

	public void setHighlight(String name, boolean highlight, boolean paint)
	{
		if (name == null)
		{
			return;
		}
		TextElement item = (TextElement)nameTable.get(name);
		if (item != null)
		{
			item.setHighlight(highlight);
			if (paint)
			{
				item.paint();
			}
		}
	}

	public boolean contains(String name)
	{
		if (name == null)
		{
			return false;
		}
		return nameTable.containsKey(name);
	}

	public String getSelectedItem()
	{
		Enumeration e = getElements();
		while (e.hasMoreElements())
		{
			Object object = e.nextElement();
			if (object instanceof TextElement)
			{
				TextElement te = (TextElement)object;
				if (te.getHighlight())
				{
					return te.getText();					
				}
			}
		}
		return null;
	}

	public int getSelectedIndex()
	{
		if (countElements() == 0)
		{
			return -1;
		}
		int length = countElements();
		for (int i = 0; i < length; i++)
		{
			Object object = elements.elementAt(i);
			if (object instanceof TextElement)
			{
				TextElement te = (TextElement)object;
				if (te.getHighlight())
				{
					return i;					
				}
			}
		}
		return -1;
	}

	public String[] getSelectedItems()
	{
		Vector holder = new Vector();
		Enumeration e = getElements();
		while (e.hasMoreElements())
		{
			Object object = e.nextElement();
			if (object instanceof TextElement)
			{
				TextElement te = (TextElement)object;
				if (te.getHighlight())
				{
					holder.addElement(te.getText());					
				}
			}
		}
		int length = holder.size();
		if (length == 0)
		{
			return null;
		}
		String[] items = new String[length];
		for (int i = 0; i < length; i++)
		{
			items[i] = (String)holder.elementAt(i);
		}
		return items;
	}	
}

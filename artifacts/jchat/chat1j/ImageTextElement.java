import java.awt.*;

class ImageTextElement extends TextElement implements MouseListener
{
	ImageArray imageArray;
	String imageName;
	Dimension imageSize = new Dimension(0,0);
	int height = 10;
	int leftInset = 3;	// pixels to buffer left edge of image
	int rightInset = 3;	// pixels to buffer image, text
	boolean mouseOver = false;
	//	indicates which button was pressed last time it was pressed down
	boolean metaDown = false;

	
	public ImageTextElement(ImageArray imageArray, String imageName, Font font, Color color, String text, int height)
	{
		super(font, color, text);
		this.imageArray = imageArray;
		this.imageName = imageName;
		this.height = height;
		if (imageArray != null && imageName != null)
		{
			imageSize = imageArray.getImageSize();
			imageSize.width += leftInset + rightInset;
		}
		else
			imageSize.width += rightInset;
	}

	public Dimension getPreferredSize()
	{
		int width = fontMetrics.stringWidth(text) + imageSize.width;
		int height = Math.max(fontMetrics.getHeight(), imageSize.height);
		return new Dimension(width, height);
	}
	
	public void paint(Graphics g)
	{
		Color oldColor = g.getColor();
		if (color == null)
		{
			if (host != null)
				color = host.getForeground();
			else
				color = oldColor;	// probably black
		}
		if (background == null)
		{
			if (host != null)
				background = host.getBackground();
			else
				background = Color.white;
		}
		g.setColor(highlight?highlightColor:background);
		g.fillRect(0, 0, bounds.width, bounds.height);
		g.setColor(highlight?background:color);
		if (text != null)
		{
			Font oldFont = g.getFont();
			if (font != null)
				g.setFont(font);
			int imageTextDelta = imageSize.height - fontMetrics.getHeight();
			if (imageTextDelta < 0)
				imageTextDelta = 0;
			drawString(g);
			g.setFont(oldFont);
		}
		if (imageArray != null && imageName != null)
		{
			int x = leftInset;
			int y = (height - imageSize.height) / 2;
			imageArray.drawImage(g, imageName, x, y); 
		}		
		// this gives a darker mouseOver effect
		if (mouseOver)
			g.drawRect(0, 0, bounds.width-1, bounds.height-1);
		g.setColor(oldColor);
	}

	public void drawString(Graphics g)
	{
		g.drawString(text, imageSize.width, (bounds.height - fontMetrics.getHeight()) / 2 + baseline);
	}

	long lastClick = System.currentTimeMillis();
	public void mouseClicked(Event e)
	{
		if (getHost() instanceof ElementList && enabled)
		{
			if (!metaDown)
			{
				boolean newHighlight = !highlight;
				ElementList list = (ElementList)getHost();
				if (!list.multiple)
				{
					list.clearSelections(false);
				}
				if (!list.deselect)
				{
					// this can not be deselected
					newHighlight = true;
				}
				highlight = newHighlight;
				list.repaint();
				// dispatch event after everything is done
			}
			long time = System.currentTimeMillis();
			long delta = time - lastClick;
			lastClick = time;
			Event event;
			if (delta < 500)	// this was a double click => action event
			{
				event = new Event(host, Event.ACTION_EVENT, text);
			}
			else
			{
				event = new Event(host, (highlight)?Event.LIST_SELECT:Event.LIST_DESELECT, text);
			}
			event.modifiers = e.modifiers | (metaDown?Event.META_MASK:0);
			event.x = e.x + bounds.x;
			event.y = e.y + bounds.y;
			dispatchEvent(event);
		}
	}

	public void mousePressed(Event e)
	{
		metaDown = e.metaDown();
	}

	public void mouseEntered(Event e)
	{
		mouseOver = true;
		if (enabled)
			paint();
	}

	public void mouseExited(Event e)
	{
		mouseOver = false;
		if (enabled)
			paint();
	}

}
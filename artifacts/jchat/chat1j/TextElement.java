////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	A TextElement representing a line of text such as in chat
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;
import java.util.*;
import java.awt.Graphics;

public class TextElement extends Element
{
	//	static member used for word wrap
	static int rightbuffer = 20;
	//////////////////////////////
	//
	//	member fields
	//
	//////////////////////////////
	Font font;				//	if null, use graphics default font
	Color color;			//	if null, use graphics default color
	Color background;		//	if null, use graphics default background
	Color highlightColor = Color.blue.darker();	// default highlight color is blue
	String text;			//	original text
	String[] lines;			//	text split into an array of lines
	int baseline;			//	font dependent y offset to start drawing text
	FontMetrics fontMetrics;//	font dependent FontMetrics object
	boolean highlight = false;
	int indent = 0;			//	the amount to indent the text from the left
	//////////////////////////////
	//
	//	constructors
	//
	//////////////////////////////
	public TextElement()
	{
		this(null, null, null, null);
	}
	public TextElement(Font font, Color color, String text)
	{
		this(font, color, null, text);
	}
	public TextElement(Font font, Color color, String text, int indent)
	{
		this(font, color, null, text);
		this.indent = indent;
	}
	public TextElement(Font font, Color color, Color background, String text)
	{
		setFont(font);
		setColor(color);
		setBackground(background);
		setText(text);
	}
	//////////////////////////////
	//
	//	methods
	//
	//////////////////////////////
	public Dimension getPreferredSize()
	{
		if (fontMetrics != null)
		{
			int hostWidth = host.getSize().width;
			int width = hostWidth;
			Vector holder = new Vector();
			StringTokenizer st = new StringTokenizer(text, " \r\n", true);
			StringBuffer line = new StringBuffer();
			if (st.hasMoreTokens())
				line.append(st.nextToken());
			while (st.hasMoreTokens())
			{
				String token = st.nextToken();
				if (indent + fontMetrics.stringWidth(line + token) + rightbuffer >= hostWidth || token.equals("\n"))
				{
					String add = line.toString().trim();
					holder.addElement(add);
					width = Math.max(fontMetrics.stringWidth(add) + indent, width);
					line.setLength(0);
				}
				line.append(token);
			}
			if (line.length() > 0)
			{
				String add = line.toString().trim();
				width = Math.max(fontMetrics.stringWidth(add) + indent, width);
				holder.addElement(add);
			}
			lines = new String[holder.size()];
			for (int i = 0; i < lines.length; i++)
			{
				lines[i] = holder.elementAt(i).toString().trim();
			}
			return new Dimension(width, fontMetrics.getHeight() * lines.length);
		}
		return new Dimension(100, 20);
	}

	public boolean addBreak()
	{
		return text != null && text.endsWith("\n");
	}
	
	public void paint(Graphics g)
	{
		Color oldColor = g.getColor();
		if (color == null)
		{
			if (host != null)
			{
				color = host.getForeground();
			}
			else
			{
				color = oldColor;	// probably black
			}
		}
		if (background == null)
		{
			if (host != null)
			{
				background = host.getBackground();
			}
			else
			{
				background = Color.white;
			}
		}
		g.setColor(highlight?highlightColor:background);
		g.fillRect(0, 0, bounds.width, bounds.height);
		g.setColor(highlight?background:color);
		if (text != null)
		{
			Font oldFont = g.getFont();
			if (font != null)
				g.setFont(font);
			int fontHeight = fontMetrics.getHeight();
			int x = 0;
			int y = baseline;
			for (int i = 0; i < lines.length; i++)
			{
				g.drawString(lines[i], x + indent, y);
				y += fontHeight;
			}
			g.setFont(oldFont);
		}
		g.setColor(oldColor);
	}

	public void setFont(Font font)
	{
		if (font == null)
		{
			font = Static.getDefaultFont();
		}
		this.font = font;
		fontMetrics = Static.getFontMetrics(font);
		if (fontMetrics != null)
		{
			baseline = fontMetrics.getAscent() + fontMetrics.getLeading() / 2;
		}
	}

	public void setColor(Color color)
	{
		this.color = color;
	}

	public void setBackground(Color background)
	{
		if (background != null)
		{
			this.background = background;
		}
	}

	public void setText(String text)
	{
		if (text == null)
		{
			text = "";
		}
		this.text = text;
	}

	public Font getFont()
	{
		return font;
	}

	public Color getColor()
	{
		return color;
	}

	public Color getBackground()
	{
		return background;
	}

	public String getText()
	{
		return text;
	}

	public void setHighlightColor(Color highlightColor)
	{
		this.highlightColor = highlightColor;
	}

	public void setHighlight(boolean highlight)
	{
		this.highlight = highlight;
	}

	public boolean getHighlight()
	{
		return highlight;
	}

	public void mouseEntered(Event e)
	{
		setHighlight(true);
		paint();
	}

	public void mouseExited(Event e)
	{
		setHighlight(false);
		paint();
	}

	public String toString()
	{
		return text;
	}
}



////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	Message contains a string and a source
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;

public class Message implements Cloneable
{
	public String target;
	public String source;
	public String text;
	public Font font;
	public Color color;

	public Message(String source, String text)
	{
		this(null, source, text, null, null);
	}

	public Message(String target, String source, String text)
	{
		this(target, source, text, null, null);
	}

	public Message(String source, String text, Font font, Color color)
	{
		this(null, source, text, font, color);
	}
	
	public Message(String target, String source, String text, Font font, Color color)
	{
		this.target = target;
		this.source = source;
		this.text = text;
		this.font = font;
		this.color = color;
	}

	public Message copy()
	{
		return new Message(target, source, text, font, color);
	}

	public String toString()
	{
		return "Message, source: " + source + ", text: " + text +
								" font: " + font + ", color: " + color;
	}

}
////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	ChatHistoryPanel is a vertical scrolling container
//	that contains the lines of chat from a chatroom as
//	people say them.  If you are viewing the bottom, it
//	will autoscroll, but if you are viewing anywhere else
//	than it will not.
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;

public class ChatHistoryPanel extends ScrollingElementPanel
{

	public ChatHistoryPanel()
	{
		horizontal.hide();
		setBottomBuffer(10);
		//	set the maximum number of elements
		this.maximum = 400;
	}

	public void add(Element element)
	{
		if (element != null)
		{
			boolean bottom = atBottom();
			super.add(element, false);
			setScrollbarValues();
			if (bottom)
			{
				toBottom();
			}
			paint(element);
		}
	}

}
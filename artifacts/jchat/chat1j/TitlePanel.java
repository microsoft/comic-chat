////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	TitlePanel contains a label at the top, and
//	a Bordered Panel at the bottom which contains a
//	child component.  This is used in class FontDialog
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;

public class TitlePanel extends Panel
{
	public TitlePanel(String title, Component child, Insets insets)
	{
		setLayout(new BorderLayout());
		add("North", new Label(title));
		BarBorderPanel bbp = new BarBorderPanel(insets);
		bbp.add(child);
		add("Center", bbp);
	}
}


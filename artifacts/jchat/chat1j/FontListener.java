////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	a FontListener object is passed to a FontDialog
//	constructor, so it can be notified when the font changes.
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;

interface FontListener
{
	public void setFont(Font font, Color color);
}


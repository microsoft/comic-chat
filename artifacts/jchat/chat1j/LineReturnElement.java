////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	LineReturnElement
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;

public class LineReturnElement extends Element
{

	public Dimension getPreferredSize()
	{
		return new Dimension(1, 1);
	}

	public boolean addBreak()
	{
		return true;
	}

	public void paint(Graphics g)
	{
		// do nothing
	}

}
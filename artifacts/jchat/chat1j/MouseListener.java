////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	imitates the java 1.1 MouseListener class
//	should be replaced by the 1.1 class when fully
//	available
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;

public interface MouseListener
{

	public void mouseClicked(Event e);

	public void mouseEntered(Event e);

	public void mouseExited(Event e);

	public void mousePressed(Event e);

	public void mouseReleased(Event e);

}




////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	ElementHost	specifies the interface which must be
//	implemented by any object which will contain Elements
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;
import java.util.*;

public interface ElementHost
{

	public void			add(Element element);
	public void			remove(Element element);
	public void			invalidate(Element element);
	public void			paint(Element element);
	public void			clearElements();
	public void			dispatchEvent(Event event);
	public boolean		contains(Element element);
	public int			indexOf(Element element);
	public int			countElements();
	public Color		getForeground();
	public Color		getBackground();
	public Element		firstElement();
	public Element		lastElement();
	public Element		elementAt(int x, int y);
	public Element		elementAt(int index);
	public Enumeration	getElements();
	public Dimension	getSize();

}


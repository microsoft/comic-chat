////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	SortedVector automatically sorts Objects put into
//		it alphabetically
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.util.*;

public class SortedVector extends Vector
{
	//	the direction to sort
	boolean forward = true;		// a - z
	boolean ignoreCase = true;	// ignoreCase

	public void insertElementSorted(Object o)
	{
		if (o == null)
		{
			return;
		}
		String value = o.toString();
		if (ignoreCase)
		{
			value = value.toUpperCase();
		}
		for (int i = 0; i < size(); i++)
		{
			String compare = elementAt(i).toString();
			if (ignoreCase)
			{
				compare = compare.toUpperCase();
			}
			int dif = compare.compareTo(value);
			if ((dif > 0 && forward) || (dif < 0 && !forward))
			{
				insertElementAt(o, i);
				return;
			}
		}
		//	if not inserted, then place at end
		addElement(o);
	}

	public void setIgnoreCase(boolean ignoreCase)
	{
		this.ignoreCase = ignoreCase;
		resort();
	}

	public void setSortDirection(boolean forward)
	{
		this.forward = forward;
		resort();
	}

	public void resort()
	{
		Vector clone = (Vector)clone();
		removeAllElements();
		Enumeration e = clone.elements();
		while (e.hasMoreElements())
		{
			insertElementSorted(e.nextElement());
		}
	}
}
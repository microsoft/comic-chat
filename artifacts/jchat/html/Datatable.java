////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	This extension of java.util.Properties provides
//	additional useful methods for accessing a properties
//	object.
//	Regular Properties objects only let you add and remove Strings.
//	the Datatable lets you add and remove Strings, ints, booleans, and String[]s
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.io.*;
import java.util.*;

public class Datatable extends Properties
{

	public Datatable()
	{
	}

	public Datatable(String filename) throws IOException
	{
		File file = new File(filename);
		FileInputStream input = new FileInputStream(file);
		load(input);
	}

	public String get(String key, String def)
	{
		return getProperty(key, def);
	}

	public String[] get(String key, char delim)
	{
		String values = get(key, null);
		if (values == null)
			return null;
		StringTokenizer st = new StringTokenizer(values, "" + delim);
		String[] array = new String[st.countTokens()];
		for (int i = 0; i < array.length; i++)
			array[i] = st.nextToken();
		return array;
	}

	public int get(String key, int def)
	{
		String s = get(key, null);
		if (s == null)
			return def;
		try
		{
			return Integer.parseInt(s);
		}
		catch(NumberFormatException e)
		{
			return def;
		}
	}

	public boolean get(String key, boolean def)
	{
		String s = get(key, null);
		if (s == null)
			return def;
		return !(s.equalsIgnoreCase("FALSE") ||
				 s.equalsIgnoreCase("NO") ||
				 s.equalsIgnoreCase("0"));
	}

}
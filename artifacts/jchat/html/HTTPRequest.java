////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	The HTTPRequest object reads an HTTP file
//	get request and associated header information
//	from an inputstream, and then provides methods
//	methods for you to access them. (inherited from Datatable)
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.io.*;
import java.net.*;
import java.util.*;

public class HTTPRequest extends Datatable
{
	protected static String GET		= "GET";
	protected static String FILE		= "FILE";
	protected static String VERSION		= "VERSION";
	protected static String QUESTIONMARK	= "?";
	protected static String SEARCH		= "SEARCH";
	protected static String POUNDSIGN	= "#";
	protected static String ANCHOR		= "ANCHOR";
	protected static String SPACECR		= " \r\n";
	protected static String ANDSIGN		= "&";
	protected static String EQUALSIGN	= "=";
	protected static String COLONSPACE	= ": ";
	protected static String EMPTY		= "";
	protected static String PERCENT		= "%";
	protected static String CONNECTION	= "CONNECTION";
	protected static String KEEP_ALIVE	= "KEEP-ALIVE";
	protected static String HTTP1_0		= "HTTP/1.0";

	//	is this a valid http request?
	protected boolean isValid = false;

	public HTTPRequest()
	{
	}

	public HTTPRequest(Socket socket)
	{
		read(socket);
	}

	public HTTPRequest(InputStream input)
	{
		read(input);
	}

	public HTTPRequest(DataInputStream input)
	{
		read(input);
	}

	public void read(Socket socket)
	{
		try
		{
			read(socket.getInputStream());
		}
		catch(Exception e)
		{
			e.printStackTrace(System.out);
		}
	}

	public void read(InputStream input)
	{
		read(new DataInputStream(input));
	}

	public void read(DataInputStream input)
	{
		try
		{
			readRequest(input);
			readHeader(input);
			isValid = true;
		}
		catch(Exception e)
		{
			e.printStackTrace(System.out);
			isValid = false;
		}
	}

	protected void readRequest(DataInputStream input) throws IOException
	{
		String request = input.readLine();
		parseRequest(request);
	}

	public void parseRequest(String request) throws IOException
	{
		StringTokenizer st = new StringTokenizer(request, SPACECR);
		if (st.countTokens() < 3)
			throw new IOException("Invalid HTTP Request: " + request);
		String command	= st.nextToken();
		String resource	= st.nextToken();
		String version	= st.nextToken();
		if (!command.equalsIgnoreCase(GET))
			throw new IOException("Unsupported HTTP Command: " + command);
		String file	= getPrefix(resource, QUESTIONMARK);
		String suffix	= getSuffix(resource, QUESTIONMARK);
		String search	= getPrefix(suffix, POUNDSIGN);
		String anchor	= getSuffix(suffix, POUNDSIGN);
		if (file != null)
			put(FILE, file);
		if (search != null)
		{
			put(SEARCH, search);
			processSearch(search);
		}
		if (anchor != null)
			put(ANCHOR, anchor);
		if (version != null)
			put(VERSION, version);
	}

	protected void readHeader(DataInputStream input) throws IOException
	{
		while (true)
		{
			String pair = input.readLine();
			if (pair == null || pair.trim().equals(EMPTY))
			{
				return;
			}
			else
			{
				String key	= getPrefix(pair, COLONSPACE);
				String value	= getSuffix(pair, COLONSPACE);
				if (key != null && value != null)
					put(key.toUpperCase(), value.toUpperCase());
			}
		}
	}

	protected void processSearch(String search)
	{
		if (search == null)
			return;
		search = search.replace('+',' ');
		StringTokenizer st = new StringTokenizer(search, ANDSIGN);
		while (st.hasMoreTokens())
		{
			String pair	= st.nextToken();
			String key	= getPrefix(pair, EQUALSIGN);
			String value	= getSuffix(pair, EQUALSIGN);
			if (key != null && value != null)
				put(decode(key), decode(value));
		}
	}

	protected static String getPrefix(String line, String delim)
	{
		if (line == null)
			return null;
		int index = line.indexOf(delim);
		if (index < 0)
			return line;
		else
			return line.substring(0, index);
	}

	protected static String getSuffix(String line, String delim)
	{
		if (line == null)
			return null;
		int index = line.indexOf(delim);
		if (index < 0)
			return null;
		else
			return line.substring(index + delim.length());
	}

	public static String decode(String line)
	{
		int pindex = line.indexOf(PERCENT);
		while (pindex >= 0)
		{
			String left = line.substring(0, pindex);
			String right = line.substring(pindex + 3);
			char middle;
			try
			{
				String hex = line.substring(pindex + 1, pindex + 3);
				middle = (char)Integer.parseInt(hex, 16);
			}
			catch(Exception e)
			{
				middle = ' ';
			}
			line = left + middle + right;
			pindex = line.indexOf(PERCENT, pindex + 1);
		}
		return line;
	}

	//	accessor methods
	public boolean isValid()
	{
		return isValid;
	}

	public String getFile()
	{
		return get(FILE, null);
	}

	public String getVersion()
	{
		String version = get(VERSION, null);
		return version != null ? version : HTTP1_0;
	}

	public boolean keepAlive()
	{
		String value = get(CONNECTION, null);
		if (value == null)
			return false;
		else
			return value.equals(KEEP_ALIVE);
	}

}





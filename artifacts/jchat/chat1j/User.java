////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	User represents a chat user
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;
import java.util.*;

public class User
{
	String name;
	Properties props;
	boolean ignored = false;
	boolean msChat = false;

	public User(String name, Properties props)
	{
		this.name = name;
		this.props = props;
	}

	public String getName()
	{
		return name;
	}

	public void setName(String name)
	{
		this.name = name;		
	}

	public boolean isMSChatUser()
	{
		return msChat;
	}

	public void setMSChatUser(boolean msChat)
	{
		this.msChat = msChat;
	}

	public boolean isIgnored()
	{
		return ignored && !isOperator();
	}

	public void setIgnored(boolean ignored)
	{
		this.ignored = ignored;
	}

	public Properties getProperties()
	{
		return props;
	}

	//	these methods are provided for convenience
	public boolean isOperator()
	{
		return get("o", false);
	}

	public boolean isSpeaker()
	{
		return get("v", false);
	}

	public boolean isInvisible()
	{
		return get("i", false);
	}

	public void merge(Properties merge)
	{
		if (merge == null)
		{
			return;
		}
		Enumeration e = merge.keys();
		while (e.hasMoreElements())
		{
			Object key = e.nextElement();
			Object value = merge.get(key);
			props.put(key, value);
		}
	}

	public String get(String param, String def)
	{
		return props.getProperty(param, def);
	}

	public void put(String key, String value)
	{
		props.put(key, value);
	}

	public boolean get(String param, boolean def)
	{
		String value = props.getProperty(param);
		if (value == null)
		{
			return def;
		}
		if (value.equalsIgnoreCase("false"))
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	public void put(String key, boolean value)
	{
		if (key != null)
		{
			props.put(key, "true");
		}
	}
}


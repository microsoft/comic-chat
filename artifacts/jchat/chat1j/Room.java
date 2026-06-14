////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	Room represents a chat room, or channel
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.util.*;

public class Room
{
	//	these are the identities of the groups vectors
	static int OPERATOR = 0;
	static int SPEAKER =  1;
	static int SPECTATOR = 2;
	static int IGNORED = 3;
	
	String name;
	Properties props;

	Hashtable members = new Hashtable();
	//	ordered Vectors of all members names organized by mode
	SortedVector[] groups;

	public Room(String name)
	{
		this(name, "", null);
	}

	public Room(String name, Properties props)
	{
		this(name, "", props);
	}
	
	public Room(String name, String topic, Properties props)
	{
		this.name = name;
		if (props == null)
		{
			props = new Properties();
		}
		this.props = props;
		//	load up the groups array with four vectors
		groups = new SortedVector[4];
		for (int i = 0; i < groups.length; i++)
		{
			groups[i] = new SortedVector();
		}
	}

	public int getUserCount()
	{
		return members.size();
	}

	public String getName()
	{
		return name;
	}

	public String getTopic()
	{
		return get("topic", "");
	}

	public void setTopic(String topic)
	{
		if (topic == null)
		{
			topic = "";			
		}
		put("topic", topic);
	}

	public boolean isModerated()
	{
		return get("m", false);
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

	//	updates the room properties, and then resorts all members
	public void update(Properties p)
	{
		if (p == null)
		{
			return;
		}
		merge(p);
		resort();
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

	public synchronized void put(String key, boolean value)
	{
		if (key != null)
		{
			props.put(key, "true");
		}
	}

	//	adds a new user, or updates a users properties
	public void add(String name, Properties p)
	{
		if (name == null)
		{
			return;
		}
		//	don't add the same person twice
		if (members.get(name) != null)
		{
			//	update instead of adding
			userUpdate(name, name, p);
			return;
		}
		if (p == null)
		{
			p = new Properties();
		}
		add(new User(name, p));
	}

	public void add(User user)
	{
		if (user == null)
		{
			return;
		}
		String name = user.getName();
		members.put(name, user);
		//	insert element alphabetically
		getVector(user).insertElementSorted(name);
	}

	public User remove(String name)
	{
		if (name == null)
		{
			return null;
		}
		User user = (User)members.remove(name);
		if (user != null)
		{
			getVector(user).removeElement(name);
		}
		return user;
	}

	public User getUser(String name)
	{
		return (User)members.get(name);
	}

	public void clear()
	{
		members.clear();
		for (int i = 0; i < groups.length; i++)
		{
			groups[i].removeAllElements();
		}
	}

	public void userUpdate(String oldname, String newname, Properties p)
	{
		//	removes a user without updating;
		User user = remove(oldname);
		if (user == null)
			return;
		user.setName(newname);
		user.merge(p);
		add(user);
	}

	//	returns the specified group
	public Vector getGroupVector(int id)
	{
		if (id >= 0 && id < groups.length)
		{
			return groups[id];
		}
		else
		{
			return null;
		}
	}
	
	//	returns the correct vector for a specifed user
	public SortedVector getVector(User user)
	{
		if (user.isIgnored())
		{
			return groups[IGNORED];
		}
		else if (user.isOperator())
		{
			return groups[OPERATOR];
		}
		else if (!isModerated() || user.isSpeaker())
		{
			return groups[SPEAKER];
		}
		else
		{
			return groups[SPECTATOR];
		}
	}

	public void resort()
	{
		// this will resort all users
		Hashtable save = members;
		members = new Hashtable();
		clear();
		Enumeration e = save.elements();
		while (e.hasMoreElements())
		{
			add((User)e.nextElement());
		}		
	}
}
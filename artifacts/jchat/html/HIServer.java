////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	HIServer is an HTML based IRC Client Server
//	it dynamically builds HTML IRC interfaces
//	based upon predefined IRC Template files "*.irct"
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.io.*;
import java.net.*;
import java.util.*;

public class HIServer extends Server
{
	protected static String parameterFileName	= "HIServer.properties";
	protected static String irchostParameter	= "irc.host";
	protected static String ircportParameter	= "irc.port";
	protected static String listenportParameter	= "listen.port";
	protected static String templatesParameter	= "templates";
	protected static String defaultpageParameter	= "default.page";
	protected static String clientTimeoutParameter	= "client.timeout";
	protected static String clientLimitParameter	= "client.limit";

	protected static String SLASH	= "/";
	protected static String TAG	= "tag";
	protected Hashtable clients	= new Hashtable();

	protected Datatable parameters	= null;
	protected Hashtable templates	= new Hashtable();
	protected String irchost;
	protected int ircport;
	protected byte[] defaultPage;
	protected Thread idleClientRemover;

	public static void main(String[] args)
	{
		HIServer server = new HIServer();
		System.out.println("Listening on port " + server.getPort() + ".");
		server.start();
	}

	public HIServer()
	{
		try
		{
			parameters = new Datatable(parameterFileName);
		}
		catch(Exception e)
		{
			throw new RuntimeException(parameterFileName + " parameter file not found.");
		}
		setPort(parameters.get(listenportParameter, 80));
		irchost = parameters.get(irchostParameter, null);
		if (irchost == null)
			throw new RuntimeException(irchostParameter + " must be specified in " + parameterFileName);
		ircport = parameters.get(ircportParameter, 6667);
		String[] templatefilenames = parameters.get(templatesParameter, ',');
		if (templatefilenames == null)
			throw new RuntimeException("Error: No template files specified.");
		loadTemplates(templatefilenames);
		String defaultPageName = parameters.get(defaultpageParameter, null);
		if (defaultPageName != null)
			defaultPage = load(defaultPageName);
		int clientLimit = parameters.get(clientLimitParameter, 1000);
		setClientLimit(clientLimit);
		int timeout = parameters.get(clientTimeoutParameter, 600);
		idleClientRemover = new IdleClientRemover(this, timeout * 1000);
		idleClientRemover.start();

		IRCClient.version = "Microsoft HTML Chat 1.0b";
	}

	public void loadTemplates(String[] names)
	{
		for (int i = 0; i < names.length; i++)
		{
			try
			{
				String name = names[i];
				IRCTemplate template = new IRCTemplate(name);
				templates.put(name, template);
				System.out.println("Loaded Template Page: " + name + ".");
			}
			catch(Exception e)
			{
				e.printStackTrace(System.out);
			}			
		}
	}

	public Datatable getParameters()
	{
		return parameters;
	}

	//	multithreaded socket handler
	public void handle(Socket socket) throws Exception
	{
		//	this will read the request, and header
		HTTPRequest request = new HTTPRequest(socket);

		while (true)
		{
			if (!request.isValid())
				return;
			//	if there is no "nick" search parameter, then they havent
			//	submitted a login form yet, so give them the login form.
			String nick = request.get(HIClient.NICK, null);
			String room = request.get(HIClient.ROOM, null);
			String tag = request.get(HIServer.TAG, null);
			if (isEmpty(nick) || isEmpty(room))
			{
				//	reply with the default page, and then return (and close connection)
				byte[] page = getDefaultPageBytes();
				if (page != null)
				{
					DataOutputStream output = new DataOutputStream(socket.getOutputStream());
					HIClient.reply(page, false, output, request.getVersion());
				}
				return;
			}
			HIClient client = getClient(tag);	//	if tag == null, will return null
			if (client == null)
				client = newClient(null);	//	if tag == null, will create new one
			
			client.handle(socket, request);	

			if (!request.keepAlive())
				break;

			request.clear();
			if (request.size() > 0)
				System.out.println("Error Request was not cleared: " + request.size());
			request.read(socket);
		}

	}

	public boolean isEmpty(String s)
	{
		return s == null || s.equals(IRCTemplate.EMPTY);
	}

	public IRCTemplate getTemplate(String name)
	{
		IRCTemplate template = (IRCTemplate)templates.get(name);
		return template;
	}

	public String getIRCHost()
	{
		return irchost;
	}

	public int getIRCPort()
	{
		return ircport;
	}

	protected HIClient newClient(String tag)
	{
		if (tag == null)
			tag = getNewTag();
		HIClient client = new HIClient(this, tag);
		clients.put(tag, client);
		return client;
	}

	protected HIClient getClient(String tag)
	{
		if (tag == null)
			return null;
		else
			return (HIClient)clients.get(tag);
	}

	protected synchronized String getNewTag()
	{
		while (true)
		{
			String tag = Integer.toString((int)(Math.random() * Integer.MAX_VALUE));
			if (getClient(tag) == null)
				return tag;
		}
	}

	public byte[] getDefaultPageBytes()
	{
		return defaultPage;
	}

}

//	This low-priority thread checks clients, and removes them if they've been idle too long.
//	based upon "client.timeout" parameter.
class IdleClientRemover extends Thread
{
	HIServer server;
	int timeout;

	public IdleClientRemover(HIServer server, int timeout)
	{
		this.server = server;
		this.timeout = timeout;
		this.setPriority(3);
		this.setDaemon(true);
	}

	public void run()
	{
		Hashtable clients = server.clients;
		while (true)
		{
			try
			{
				Thread.sleep(timeout / 2);
			}
			catch(Exception e)
			{}
			Enumeration e = clients.keys();
			while (e.hasMoreElements())
			{
				String tag = (String)e.nextElement();
				HIClient client = (HIClient)clients.get(tag);
				if (client != null)
				{
					if (client.getIdleTime() > timeout)
					{
						clients.remove(tag);
						client.disconnect();
					}
				}
			}
		}
	}
}




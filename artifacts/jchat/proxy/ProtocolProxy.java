////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	ProtocolProxy is used to reroute multiple connections
//	to alternate servers through a single port.
//	it does this by choosing the server to reconnect to,
//	based upon the first command submitted by the opening
//	client.  For example "get /index.html HTTP/1.0"
//	may get rerouted to a web server, while "nick joe"
//	would be rerouted to an irc chat server
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.io.*;
import java.net.*;
import java.util.*;

public class ProtocolProxy extends Server
{
	Hashtable servers = new Hashtable();
	int port;

	public static void main(String[] args)
	{
		try
		{
			int port = 80;
			if (args.length > 0)
				port = Integer.parseInt(args[0]);
			int clientLimit = 100;
			if (args.length > 1)
				clientLimit = Integer.parseInt(args[1]);
			File file = new File("ProtocolProxy.properties");
			Properties props = new Properties();
			FileInputStream input = new FileInputStream(file);
			props.load(input);
			ProtocolProxy proxy = new ProtocolProxy(port, props);
			proxy.setClientLimit(clientLimit);
			proxy.start();
		}
		catch(Exception e)
		{
			usage();
		}
	}

	public ProtocolProxy(int port, Properties p)
	{
		setPort(port);
		loadServerData(p);
	}

	public void loadServerData(Properties p)
	{
		Enumeration keys = p.keys();
		while (keys.hasMoreElements())
		{
			String commands = keys.nextElement().toString().toLowerCase();
			String serverColonPort = p.getProperty(commands);
			ServerData serverData = new ServerData(serverColonPort);
			StringTokenizer st = new StringTokenizer(commands, ",");
			while (st.hasMoreTokens())
			{
				String command = st.nextToken();
				servers.put(command, serverData);
			}
		}			
	}

	//	override this method to in subclasses of server
	public void handle(Socket socket) throws Exception
	{
		String request = new DataInputStream(socket.getInputStream()).readLine();
		StringTokenizer st = new StringTokenizer(request, " \r\n");
		String command = st.nextToken().toLowerCase();
		ServerData server = (ServerData)servers.get(command);
		if (server == null)
		{
			println("Unrecognized request: " + request);
		}
		else
		{
			println(request);
			println("      connecting to: " + server);
			new ServerConnector(socket, server, request);
		}
		//	socket is automatically terminated when this method returns
	}

	public static void usage()
	{
		PrintStream out = System.out;
		out.println();
		out.println("Usage:");
		out.println();
		out.println("        java ProtocolProxy [port] [clientLimit]");
		out.println();
		out.println("Default:");
		out.println();
		out.println("        java ProtocolProxy 80 100");
		out.println();
		out.println("ProtocolProxy.properties file format:");
		out.println();
		out.println("        command[s],..=server:port");
		out.println();
		out.println("Examples:");
		out.println();
		out.println("        get,post,head=www.domain.com:80");
		out.println("        nick,mode,user=irc.domain.com:6667");
		out.println();
	}

	public static void println(Object e)
	{
		System.out.println(e);
	}
}

class ServerData
{
	String host;
	int port;

	public ServerData(String nameColonPort)
	{
		StringTokenizer st = new StringTokenizer(nameColonPort, " :\r\n");
		if (st.countTokens() != 2)
		{
			throw new IllegalArgumentException("Invalid Host Name");
		}
		host = st.nextToken();
		try
		{
			port = Integer.parseInt(st.nextToken());			
		}
		catch(Exception e)
		{
			throw new IllegalArgumentException("Invalid Port Number");
		}
	}
	
	public String toString()
	{
		return host + ":" + port;
	}
}

class ServerConnector
{
	ServerPump one, two;
	Socket client, server;
	ServerData serverData;
	
	public ServerConnector(Socket client, ServerData serverData, String firstLine)
	{
		try
		{
			this.client = client;
			this.serverData = serverData;
			this.server = new Socket(serverData.host, serverData.port);
			InputStream serverIn = server.getInputStream();
			OutputStream serverOut = server.getOutputStream();
			//	write out the first line to the server
			new PrintStream(serverOut).println(firstLine);
			//
			InputStream clientIn = client.getInputStream();
			OutputStream clientOut = client.getOutputStream();
			this.one = new ServerPump(clientIn, serverOut, this);
			this.two = new ServerPump(serverIn, clientOut, this);
			System.out.println("Opening channels: " + serverData);
			//	this pump will run in a new thread.
			one.start();
			//	this one will run using this current thread.
			two.run();
		}
		catch(Exception e)
		{
			System.out.println("Error connecting to: " + serverData);
		}
	}

	public void done(ServerPump pump)
	{
		one.close();
		two.close();
		try
		{
			server.close();
			client.close();
		}
		catch(Exception e)
		{}
		if (pump == one)
		{
			System.out.println("Closing channel 1: " + serverData);
		}
		else
		{
			System.out.println("Closing channel 2: " + serverData);
		}
	}
}

class ServerPump extends Thread
{
	InputStream input;
	OutputStream output;
	ServerConnector parent;

	public ServerPump(InputStream in, OutputStream out, ServerConnector parent)
	{
		this.input = in;
		this.output = out;
		this.parent = parent;
	}

	public void close()
	{
		try
		{
			input.close();
			output.close();
		}
		catch(Exception e)
		{
		}
	}

	public void run()
	{
		try
		{
			byte[] data = new byte[1000];
			while (true)
			{
				output.write(data, 0, input.read(data));
			}
		}
		catch(Exception e)
		{
			//	IOException	or
			//	ArrayIndexOutOfBoundsException
			//	indicates that we have reached the end of stream
		}
		parent.done(this);
	}

}


////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	Server is a multithreaded server, it listens on
//	a chosen port, and accepts connections, which
//	are then handled in a separate thread.
//	Extend this class, and override "public void handle(Socket s);"
//	in order to provide useful functionality.
//	Server has many methods to configure its behaviour.
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.net.*;
import java.io.*;

public class Server implements Runnable
{

	protected ThreadGroup clients = new ThreadGroup("clients");
	protected int clientPriority = Thread.NORM_PRIORITY;
	protected int clientLimit = 100;
	protected Thread thread = null;
	protected int serverPriority = Thread.NORM_PRIORITY;
	protected int sleepTime = 100;
	protected int port;

	public Server()
	{
		this(80);
	}

	public Server(int port)
	{
		setPort(port);
	}

	public int getPort()
	{
		return port;
	}

	public void setPort(int port)
	{
		this.port = port;
	}

	public int getServerPriority()
	{
		return serverPriority;
	}

	public void setServerPriority(int serverPriority)
	{
		this.serverPriority = serverPriority;
	}

	public int getServerSleepTime()
	{
		return sleepTime;
	}

	public void setServerSleepTime(int sleepTime)
	{
		this.sleepTime = sleepTime;
	}

	public int getClientLimit()
	{
		return clientLimit;
	}

	public void setClientLimit(int clientLimit)
	{
		this.clientLimit = clientLimit;
	}

	public int getClientPriority()
	{
		return clientPriority;
	}

	public void setClientPriority(int clientPriority)
	{
		this.clientPriority = clientPriority;
	}

	public ThreadGroup getClientThreadGroup()
	{
		return clients;
	}

	public int getClientCount()
	{
		return clients.activeCount();
	}

	public Thread[] getClientThreads()
	{
		int count = getClientCount();
		Thread[] threads = new Thread[count];
		clients.enumerate(threads);
		return threads;
	}

	public void dumpThreads(PrintStream out)
	{
		Thread[] threads = getClientThreads();
		for (int i = 0; i < threads.length; i++)
			out.println(threads[i]);
	}

	public boolean isRunning()
	{
		return thread != null && thread.isAlive();
	}

	public void start()
	{
		if (!isRunning())
		{
			thread = new Thread(this);
			thread.setPriority(serverPriority);
			thread.start();
		}
	}

	public void stop()
	{
		if (isRunning())
		{
			Thread thread = this.thread;
			this.thread = null;
			clients.stop();
			thread.stop();
		}
	}

	public void waitForClients()
	{
		while (getClientCount() >= getClientLimit())
		{
			try
			{
				Thread.sleep(sleepTime);
			}
			catch(Exception e)
			{
			}
		}
	}

	public void run()
	{
		ServerSocket server;
		try
		{
			server = new ServerSocket(port);
		}
		catch(Exception e)
		{
			System.out.println("Error opening port " + port);
			e.printStackTrace(System.out);
			return;
		}
		while (true)
		{
			try
			{
				waitForClients();
				Socket socket = server.accept();
				new Client(socket, this);
			}
			catch(Exception e)
			{
			//	System.out.println("Error accepting connection on port " + 80);
			//	e.printStackTrace(System.out);
				return;
			}
		}
	}

	//	override this method to in subclasses of server
	public void handle(Socket socket) throws Exception
	{
		dumpThreads(System.out);
		int remotePort = socket.getPort();
		DataInputStream input = new DataInputStream(socket.getInputStream());
		while(true)
			System.out.println(remotePort + " : " + input.readLine());
	}

	public static void main(String[] args)
	{
		Server server = new Server();
		System.out.println("Starting test server on port " + server.getPort());
		server.start();
	}

	public static byte[] load(String filename)
	{
		return load(new File(filename));
	}

	public static byte[] load(File file)
	{
		try
		{
			FileInputStream input = new FileInputStream(file);
			byte[] data = new byte[(int)file.length()];
			int read = 0;
			while (read < data.length)
				read += input.read(data, read, data.length - read);
			input.close();
			return data;
		}
		catch(Exception e)
		{
			return null;
		}
	}

}

class Client implements Runnable
{
	protected Socket socket;
	protected Server server;

	protected Client(Socket socket, Server server)
	{
		this.socket = socket;
		this.server = server;

		ThreadGroup group = server.getClientThreadGroup();
		String name = socket.getInetAddress().toString();
		int priority = server.getClientPriority();

		Thread thread = new Thread(group, this, name);
		thread.setPriority(priority);
		thread.start();
	}

	public void run()
	{
		try
		{
			server.handle(socket);
		}
		catch(Exception e)
		{
		//	System.out.println("Error handling socket.");
		//	e.printStackTrace(System.out);
		}
		//	ensure that the socket is closed, when the thread terminates
		if (socket != null)
		{
			try
			{
				socket.close();
			}
			catch(Exception e)
			{
			}
		}
	}

}

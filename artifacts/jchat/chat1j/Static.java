////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	class Static contains only static methods, and is
//	used to provide methods which several other classes
//	might need, and would otherwise be duplicated.
//	it also handles methods that are implemented differently
//	in applets vs stand-alone applications
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.io.*;
import java.net.*;
import java.awt.*;
import java.util.*;
import java.applet.*;
import java.awt.image.*;

public abstract class Static
{
	static Component component = new Canvas();
	//	the following are used only for showing urls and showing status.
	static Applet applet = null;
	static AppletContext context = null;
	
	////////////////////////////////////////////
	//
	//	Static Methods	(for convenience)
	//
	////////////////////////////////////////////
	public static void setApplet(Applet applet)
	{
		Static.applet = applet;
		if (applet != null)
			Static.context = applet.getAppletContext();
	}

	public static boolean isApplet()
	{
		return applet != null;
	}

	public static boolean isMicrosoft()
	{
		String vendor = System.getProperty("java.vendor");
		if (vendor != null && vendor.indexOf("icrosoft") > 0)
			return true;
		return false;
	}

	public static Properties loadProperties(Applet applet, String locale)
	{
		Properties properties = new Properties();
		String propertiesFileName = locale + ".properties";
		InputStream input = Static.getInputStream(applet, propertiesFileName);
		if (input == null)
			return properties;
		try
		{
			properties.load(input);
			input.close();
		}
		catch(Exception e)
		{
			// IOException , can't do anything about it
			System.out.println(e);
		}
		return properties;
	}

	public static String getParameter(Applet applet, String param, String def)
	{
		if (applet == null)
		{
			return def;
		}
		String value = applet.getParameter(param);
		if (value != null)
		{
			return value;
		}
		return def;
	}

	public static int getParameter(Applet applet, String param, int def)
	{
		if (applet == null)
			return def;
		String value = applet.getParameter(param);
		if (value == null)
			return def;
		try
		{
			return Integer.parseInt(value);
		}
		catch(NumberFormatException e)
		{
			return def;
		}
	}

	public static boolean getParameter(Applet applet, String param, boolean def)
	{
		if (applet == null)
			return def;
		String value = applet.getParameter(param);
		if (value == null)
			return def;
		if (value.equalsIgnoreCase("false") || value.equals("0") || value.equalsIgnoreCase("no") || value.equals(""))
			return false;
		else
			return true;
	}

	public static Color getParameterColor(Applet applet, String param, Color def)
	{
		if (applet == null)
			return def;
		String hex = applet.getParameter(param);
		if (hex == null)
			return def;
		else
		{
			try
			{
				Color c = new Color(Integer.parseInt(hex, 16));
				return c;
			}
			catch(Exception e){}
		}
		return def;
	}

	public static InputStream getInputStream(Applet applet, String fileName)
	{
		try
		{
			InputStream input;
			if (applet == null)
			{
				// get from file
				File file = new File(fileName);
				input = new FileInputStream(file);
			}
			else
			{
				URL fileURL = getResource(applet, applet.getClass(), fileName);
				input = fileURL.openStream();				
			}
			Runtime runtime = Runtime.getRuntime();
			input = runtime.getLocalizedInputStream(input);
			return input;
		}
		catch(Exception e)
		{
			System.out.println(e);
			return null;
		}
	}

	public static void showDocument(String url)
	{
		try
		{
			if (url.indexOf("://") == -1)
			{
				url = "http://" + url;
			}
			if (context == null)
			{
				String os = System.getProperty("os.name");
				// add support for NT 4.0 and Mac etc
				if (os == null || !os.endsWith("95"))	// check for windows 95
				{
					return;
				}
				// if os is windows 95, try to execute this command
				String command = "start " + url;
				Runtime.getRuntime().exec(command);
			}
			else
			{
				context.showDocument(new URL(url), "_blank");
			}
		}
		catch(Exception e)
		{
			// IOException or MalformedURLException, ignore
		}
	}

	public static void showStatus(String status)
	{
		if (context != null)
		{
			context.showStatus(status);
		}
	}

	public static FontMetrics getFontMetrics(Font font)
	{
		if (font == null)
		{
			return getFontMetrics();
		}
		return getToolkit().getFontMetrics(font);
	}

	public static FontMetrics getFontMetrics()
	{
		return getFontMetrics(getDefaultFont());
	}

	public static Font getDefaultFont()
	{
		// may wish to change this
		return new Font("Dialog", Font.PLAIN, 12);
	}

	public static String[] getFontList()
	{
		return getToolkit().getFontList();
	}

	public static Toolkit getToolkit()
	{
		return Toolkit.getDefaultToolkit();
	}

	public static Image getImage(Applet applet, String name)
	{
		if (applet == null)
			return getToolkit().getImage(name);
		else
			return applet.getImage(applet.getDocumentBase(), name);
	}

	public static Image getImage(Applet applet, Class clas, String name)
	{
		if (applet == null)
			return getImage(null, name);
		URL url = getResource(applet, applet.getClass(), name);
		return applet.getImage(url);
	}

	public static Image filterImage(Image original, ImageFilter filter)
	{
		if (original == null || filter == null)
		{
			return original;
		}
		ImageProducer producer = new FilteredImageSource(original.getSource(), filter);
		Image image = component.createImage(producer);
		loadImage(image);
		return image;
	}
	
	//	converts a vector of Objects to a String[]
	public static String[] toStringArray(Vector v)
	{
		if (v == null || v.isEmpty())
		{
			return null;			
		}
		String[] array = new String[v.size()];
		for (int i = 0; i < array.length; i++)
		{
			array[i] = v.elementAt(i).toString();
		}
		return array;
	}
	
	//	moves the specified window to the center of the screen
	//	and brings to front
	public static void center(Window window)
	{
		if (window == null)
			return;
		Dimension screenSize = getToolkit().getScreenSize();
		Dimension windowSize = window.size();
		int x = (screenSize.width - windowSize.width) / 2;
		int y = (screenSize.height - windowSize.height) / 2;
		if (x < 0)
			x = 0;
		if (y < 0)
			y = 0;
		window.move(x, y);
	}

	//	returns a bold version of specified font
	public static Font bold(Font font)
	{
		if (font == null)
		{
			font = getDefaultFont();
		}
		return new Font(font.getName(), font.getStyle() | Font.BOLD, font.getSize());
	}

	//	returns an italics version of specified font
	public static Font italics(Font font)
	{
		if (font == null)
		{
			font = getDefaultFont();
		}
		return new Font(font.getName(), font.getStyle() | Font.ITALIC, font.getSize());
	}

	//	returns a plain version of specified font
	public static Font plain(Font font)
	{
		if (font == null)
		{
			font = getDefaultFont();
		}
		return new Font(font.getName(), Font.PLAIN, font.getSize());
	}

	//	returns a larger version of the same font
	public static Font larger(Font font)
	{
		return changeSize(font, +2);
	}

	//	returns a smaller version of the same font
	public static Font smaller(Font font)
	{
		return changeSize(font, -2);
	}

	//	returns the same font, but at a different size
	public static Font changeSize(Font font, int change)
	{
		if (font == null)
		{
			font = getDefaultFont();
		}
		return new Font(font.getName(), font.getStyle(), font.getSize() + change);
	}

	//	this ensures that a windows bounds are within the screens viewable area
	public static void onScreen(Window w)
	{
		if (w == null)
			return;
		Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
		if (screenSize == null)
			return;
		Rectangle bounds = w.bounds();
		if (bounds.x < 0)
			w.move(0, bounds.y);
		if ((bounds.x + bounds.width) > screenSize.width)
			w.move(screenSize.width - bounds.width, bounds.y);
		if (bounds.y < 0)
			w.move(bounds.x, 0);
		if ((bounds.y + bounds.height) > screenSize.height)
			w.move(bounds.x, screenSize.height - bounds.height);
	}

	//	returns the absolute screen location of a component
	public static Point getAbsoluteLocation(Component comp, int x, int y)
	{
		Point location = new Point(x, y);
		if (comp == null)
		{
			return location;
		}
		for (Container parent = comp.getParent(); parent != null; parent = parent.getParent())
		{
			Point parentLocation = parent.location();
			location.x += parentLocation.x;
			location.y += parentLocation.y;
		}
		return location;
	}

	public static String toString(String[] array, String separator)
	{
		if (array == null || array.length == 0)
		{
			return "";
		}
		StringBuffer buffer = new StringBuffer(array[0]);
		for (int i = 1; i < array.length; i++)
		{
			buffer.append(separator + array[i]);
		}
		return buffer.toString();
	}

	public static Frame getFrame(Component comp)
	{
		for (Container parent = comp.getParent(); parent != null; parent = parent.getParent())
		{
			if (parent != null && parent instanceof Frame)
			{
				return (Frame)parent;
			}
		}
		return new Frame();
	}

	public static URL getResource(Applet applet, Class clas, String name)
	{
		try
		{
			URL base = applet.getCodeBase();
			String className = clas.getName();
			int index = className.indexOf('.');
			if (index < 0)
				return new URL(base, name);
			String packageName = className.substring(0, index);
			String packageDir = packageName.replace('.', '/');
			String relativeLoc = packageDir + '/' + name;
			URL resource = new URL(base, relativeLoc);
			System.out.println(resource);
			return resource;
		}
		catch(Exception e){e.printStackTrace(System.out);}
		return null;
	}

	public static void loadImage(Image image)
	{
		if (image != null)
		{
			try
			{
				MediaTracker mt = new MediaTracker(component);
				mt.addImage(image, 0);
				mt.waitForAll();
			}
			catch(Exception e)
			{
				// interrupted exception, do nothing
			}
		}
	}
}
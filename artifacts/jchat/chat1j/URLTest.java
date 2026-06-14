import java.net.*;
import java.awt.*;
import java.applet.*;

public class URLTest extends Applet
{
	URL codebase;

	public void init()
	{
		this.codebase = getCodeBase();
	}



	public void paint(Graphics g)
	{
		println(g, "CodeBase: " + codebase.toString(), 1);
		println(g, "    Form: " + codebase.toExternalForm(), 2);
		println(g, "Protocol: " + codebase.getProtocol(), 3);
		println(g, "    Host: " + codebase.getHost(), 4);
		println(g, "    Port: " + codebase.getPort(), 5);
		println(g, "    File: " + codebase.getFile(), 6);
	}

	public void println(Graphics g, String text, int line)
	{
		int x = 20;
		int y = line * getFontMetrics(g.getFont()).getHeight();
		g.drawString(text, x, y);
	}


}
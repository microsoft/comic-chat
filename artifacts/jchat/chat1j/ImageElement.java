////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	A Graphic Element which contains only an Image
//	it will size itself to the images size.
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;
import java.awt.image.*;

public class ImageElement extends Element implements ImageObserver
{

	Image image;
	
	public ImageElement(Image image)
	{
		this.image = image;
		Static.loadImage(image);
	}

	public Dimension getPreferredSize()
	{
		int width = image.getWidth(null);
		int height = image.getHeight(null);
		return new Dimension(Math.max(width, 10), Math.max(height, 10));
	}

	public boolean imageUpdate(Image  img, int  flags, int  x, int  y, int  w, int  h)
	{
		paint();
		return isVisible();
	}

	//	place on its own line.
	public boolean addBreak()
	{
		return true;
	}
	
	public void paint(Graphics g)
	{
		int width = image.getWidth(null);
		int hostWidth = getHost().getSize().width;
		int x = (hostWidth - width) / 2;
		g.drawImage(image, x, 0, this);
	}

}





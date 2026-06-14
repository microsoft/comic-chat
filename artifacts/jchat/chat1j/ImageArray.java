////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	an ImageArray represents one composite image that
//	contains several, equal size frames from left to right.
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.Image;
import java.awt.Graphics;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.Point;
import java.awt.image.*;
import java.util.*;

public class ImageArray
{
	
	Image image;	// the main image containing frames left to right
	Image grayed;
	int count;		// number of images
	int width;		// individual image width
	int height;		// individual image height
	Hashtable namesBounds = new Hashtable();	// bounds of each image keyed by name
	
	///////////////
	//
	//	the following code is necessary to be able to call component.createImage(int,int) successfully
	//
	///////////////

	public ImageArray(Image image, String[] names, Color color)
	{
		this.image = image;
		this.count = names.length;
		Static.loadImage(image);
		this.width = image.getWidth(null);
		this.height = image.getHeight(null) / count;
		for (int i = 0; i < count; i++)
		{
			int y = i * width;
			int x = 0;
			Rectangle bounds = new Rectangle(x, y, width, height);
			namesBounds.put(names[i], bounds);
		}
		setBackground(color);
	}

	public void setBackground(Color filter)
	{
		if (filter != null)
			this.grayed = Static.filterImage(image, new GrayImageFilter(filter));
	}

	public Dimension getImageSize()
	{
		return new Dimension(width, height);
	}

	public void drawImage(Graphics g, String name, int x, int y)
	{
		drawImage(g, image, name, x, y);
	}

	public void drawGrayImage(Graphics g, String name, int x, int y)
	{
		drawImage(g, grayed, name, x, y);
	}

	private void drawImage(Graphics g, Image i, String name, int x, int y)
	{
		if (g == null || name == null)
			return;
		Rectangle bounds = (Rectangle)namesBounds.get(name);
		if (bounds == null)
			return;
		Graphics clippedGraphics = g.create(x, y, bounds.width, bounds.height);
		if (clippedGraphics == null)
			return;
		clippedGraphics.clipRect(0, 0, width, height);
		clippedGraphics.drawImage(i, - bounds.x, - bounds.y, null);
	}

}
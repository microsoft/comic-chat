import java.awt.*;
import java.awt.image.*;
////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	GrayImageFilter is used by class Toolbar to gray out
//	buttons which are currently unavailable.
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
class GrayImageFilter extends RGBImageFilter
{
	int red, green, blue;
	int bright;

	public GrayImageFilter()
	{
		this(null);
	}

	public GrayImageFilter(Color blend)
	{
		canFilterIndexColorModel = true;
		if (blend == null)
			blend = new Color(192, 192, 192);
		red = blend.getRed();
		green = blend.getGreen();
		blue = blend.getBlue();
		bright = (red + green + blue) / 3;
	}
	
	public int filterRGB(int x, int y, int rgb)
	{
		int p = ((rgb >> 16 & 0xff) + (rgb >> 8 & 0xff) + (rgb & 0xff)) / 3;
		p = (p + bright * 4) / 5;
		int r = Math.min(red * p / bright, 0xff);
		int g = Math.min(green * p / bright, 0xff);
		int b = Math.min(blue * p / bright, 0xff);
		return 0xff << 24 | r << 16 | g << 8 | b;
	}

}

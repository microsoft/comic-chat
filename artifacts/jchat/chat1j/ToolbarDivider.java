import java.awt.*;

class ToolbarDivider extends Element
{

	public ToolbarDivider(int width, int height)
	{
		setSize(width, height);
	}

	public void paint(Graphics g)
	{
		Color save = g.getColor();
		g.setColor(getBackground());
		if (bounds.height > bounds.width)
			g.draw3DRect(bounds.width / 2 - 1, 0, 1, bounds.height, false);
		else
			g.draw3DRect(0, bounds.height / 2 - 1, bounds.width, 1, false);
		g.setColor(save);
	}
}
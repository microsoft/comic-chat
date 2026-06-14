
import java.awt.*;
import java.awt.Graphics;

public class BorderWindow extends Window
{
	Component child;

	// insets specify how much room to leave on each side of child component
	public BorderWindow(Frame parent)
	{
		super(parent);
		setBackground(parent.getBackground());
		setLayout(null);
		resize(getPreferredSize());
	}

	public Component add(Component c)
	{
		removeAll();
		child = c;
		resize(getPreferredSize());
		return super.add(c);
	}

	public void layout()
	{
		doLayout();
	}

	public Dimension preferredSize()
	{
		return getPreferredSize();
	}

	public Dimension getPreferredSize()
	{
		if (child == null)
		{
			return new Dimension(200, 400);
		}
		Dimension want = child.preferredSize();
		return new Dimension(want.width + 8, want.height + 8);
	}

	public void doLayout()
	{
		if (child == null)
		{
			return;
		}
		child.reshape(4, 4, size().width - 8, size().height - 8);
	}

	public void update(Graphics g)
	{
		paint(g);
	}

	public void paint(Graphics g)
	{
		int width = size().width;
		int height = size().height;
		Point p0 = new Point(0, 0);
		Point p1 = new Point(width-1, 0);
		Point p2 = new Point(width-1, height-1);
		Point p3 = new Point(0, height-1);
		Point p4 = new Point(1, 1);
		Point p5 = new Point(width-2, 1);
		Point p6 = new Point(width-2, height-2);
		Point p7 = new Point(1, height-2);
		g.setColor(getBackground());
		drawLine(g, p7, p0);
		drawLine(g, p0, p1);
		g.setColor(Color.black);
		drawLine(g, p1, p2);
		drawLine(g, p2, p3);
		g.setColor(getBackground().brighter());
		drawLine(g, p7, p4);
		drawLine(g, p4, p5);
		g.setColor(getBackground().darker());
		drawLine(g, p5, p6);
		drawLine(g, p6, p7);
		g.setColor(getForeground());
	}

	public void drawLine(Graphics g, Point one, Point two)
	{
		g.drawLine(one.x, one.y, two.x, two.y);
	}

}
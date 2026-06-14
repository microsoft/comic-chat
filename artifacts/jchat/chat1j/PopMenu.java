
import java.awt.*;

public class PopMenu extends BorderWindow
{
	MenuPanel menu;
	
	public PopMenu(Frame parent, MenuListener listener, String arg)
	{
		super(parent);
		this.menu = new MenuPanel(listener, arg);
		add(menu);
	}

	public void show(Point location)
	{
		Dimension size = preferredSize();
		if (getWarningString() != null)
			size.height += 20;
		try
		{
			addNotify();
		}
		catch(Exception e){};
		reshape(location.x, location.y, size.width, size.height);
		super.show();
		Dimension ms = menu.preferredSize();
		size.width = Math.max(size.width, ms.width);
		size.height = Math.max(size.height, ms.height);
		reshape(location.x, location.y, size.width, size.height);
		menu.requestFocus();
	}

	public void addMenuItem(String item)
	{
		menu.addMenuItem(item);
	}

	public void addMenuItems(String[] items)
	{
		menu.addMenuItems(items);
	}

	public void setDown(String item, boolean down)
	{
		menu.setDown(item, down);
	}

	public void setEnabled(String item, boolean enabled)
	{
		menu.setEnabled(item, enabled);
	}

	public boolean handleEvent(Event e)
	{
		if (e.id == Event.LOST_FOCUS && e.target == menu)
		{
			hide();
		}
		return super.handleEvent(e);
	}

}
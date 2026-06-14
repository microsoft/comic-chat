////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	ModalDialog
//		this is the parent of every Dialog box in
//		the chatapplet project
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;
import java.util.*;

public class ModalDialog extends Frame
{
	Component disable;
	Vector focuses = new Vector();
	boolean modal = true;
	boolean dispose = true;
	boolean virgin = true;

	public ModalDialog(Frame parent, Component disable)
	{
		this(parent, disable, true);
	}
		
	public ModalDialog(Frame parent, Component disable, boolean modal)
	{
	//	super(parent, true);
		setBackground(disable.getBackground());
		this.disable = disable;
		this.modal = modal;
		loadStrings();
	}

	public void loadStrings()
	{
		//	load internationalization strings here
	}

	//	returns an internationalization string
	public String get(String key)
	{
		if (disable instanceof ChatPanel)
			return ((ChatPanel)disable).get(key);
		else
			return "";
	}

	public void setDispose(boolean dispose)
	{
		this.dispose = dispose;
	}

	public Component add(Component c)
	{
		super.add(c);
		if (c instanceof Container)
			addFocus((Container)c);
		else
			addFocus(c);
		return c;
	}

	public void addFocus(Container c)
	{
		Component[] components = c.getComponents();
		if (components == null)
			return;
		for (int i = 0; i < components.length; i++)
		{
			if (components[i] instanceof Container)
				addFocus((Container)components[i]);
			else
				addFocus(components[i]);
		}
	}

	public void addFocus(Component c)
	{
		if (c instanceof TextField || c instanceof Button || c instanceof Checkbox || c instanceof List || c instanceof Choice)
			focuses.addElement(c);
	}

	public void show()
	{
		if (modal)
			disable.enable(false);
		//	set colors
		color(this);
		pack();
		Static.center(this);

		super.show();

		if (virgin)
		{
			pack();
			virgin = false;
		}
		nextFocus((Component)focuses.lastElement(), true);
	}



	public void color(Container cont)
	{
		if(!(cont instanceof ElementPanel))
			cont.setBackground(getBackground());
		Component[] comps = cont.getComponents();
		for (int i = 0; i < comps.length; i++)
		{
			Component c = comps[i];
			if (!(c instanceof ElementPanel))
			{
				c.setBackground(getBackground());
				if (c instanceof Container)
					color((Container)c);
			}
		}
	}

	public boolean keyDown(Event e, int key)
	{
		if (key == 9 && e.target instanceof Component)
		{
			nextFocus((Component)e.target, !e.shiftDown() || key == 10);
			return true;
		}
		else if (key == 10)
			ok();
		else if (key == 27)
			close();
		return super.keyDown(e, key);
	}

	public void nextFocus(Component target, boolean forward)
	{
		if (!isVisible())
			return;
		int index = focuses.indexOf(target);
		if (index == -1)
			return;
		int size = focuses.size();
		int next = index + (forward?1:-1);
		Component nextComponent = null;
		for (int i = 0; i < size; i++)
		{
			if (next < 0)
				next = size - 1;
			else if (next >= size)
				next = 0;
			Component check = (Component)focuses.elementAt(next);
			if (check.isEnabled() || check == target)
			{
				nextComponent = check;
				break;
			}
			next += (forward?1:-1);
		}
		if (nextComponent == null)
			return;
		nextComponent.requestFocus();
		if (nextComponent instanceof TextField)
		{
			TextField tf = ((TextField)nextComponent);
			if (tf.isEditable())
				tf.selectAll();
		}
	}

	//	override to perform useful function
	public void ok()
	{
	}

	public void close()
	{
		hide();
		disable.enable(true);
		disable.requestFocus();
	}

	public boolean handleEvent(Event e)
	{
		if (e.id == Event.WINDOW_DESTROY)
		{
			close();
			return true;
		}
		return super.handleEvent(e);
	}

}

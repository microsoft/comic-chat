////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	DisposableFrame	is a simple frame which
//	detects WINDOW_DESTROY events and will close itself
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;

public class DisposableFrame extends Frame
{
	public DisposableFrame()
	{
		super("");
	}

	public DisposableFrame(String title)
	{
		super(title);
	}
	
	public boolean handleEvent(Event e)
	{
		if (e.id == Event.WINDOW_DESTROY && e.target == this)
		{
			closeWindow();
		}
		return super.handleEvent(e);
	}

	//	override this method to do something before closing window
	//	return true to indicate window should be closed, false to stay open
	public void closeWindow()
	{
		hide();
		dispose();
	}
}
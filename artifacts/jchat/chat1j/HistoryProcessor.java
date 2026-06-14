////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	a HistoryProcessor processes objects (information)
//	sent to it, stores the objects, and displays the information
//	on an ElementHost object. Different subclasses of HistoryProcessor
//	may process and display the same information in a different manner,
//	for instance a regular ChatHistoryProcess displays Lines of Text,
//	but a ComicChatHistoryProcessor displays Comic Frames instead.	
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;
import java.util.*;

public class HistoryProcessor
{
	//////////////////////////////
	//
	//	protected fields
	//
	//////////////////////////////
	
	//	array of objects to be processed and displayed
	protected Vector history;
	//	display device to show information on
	protected ElementHost display;

	//////////////////////////////
	//
	//	constructor
	//
	//////////////////////////////

	//	the default constructor does nothing
	public HistoryProcessor()
	{

	}
		
	//	this constructor will automatically reprocess all history data
	//	and save the history and display references
	public HistoryProcessor(Vector history, ElementHost display)
	{
		processAll(history, display);
	}

	//////////////////////////////
	//
	//	public methods
	//
	//////////////////////////////

	//	this is the method used to add objects.
	//	it automatically stores them, and processes and displays them
	public void add(Object object)
	{
		history.addElement(object);
		display(object);
	}

	//////////////////////////////
	//
	//	protected methods
	//
	//////////////////////////////

	//
	//	OVERRIDE THIS METHOD IN SUBCLASSES
	//
	//	this method should process an object, create one or more
	//	display Elements, call "display(element)" to add each to
	//	the display device, or return a single element which will
	//	be automatically added to the display device
	protected Element process(Object object)
	{
		return new TextElement(null, null, object.toString());
	}

	// sends the specified Element to the display device
	protected final void display(Element element)
	{
		if (element != null)
		{
			display.add(element);
		}
	}
	
	//////////////////////////////
	//
	//	private methods
	//
	//////////////////////////////

	// clears the display device, reprocesses all history objects
	// and redisplays them
	private final void processAll()
	{
		if (display == null)
		{
			return;
		}
		display.clearElements();
		Enumeration e = history.elements();
		while (e.hasMoreElements())
		{
			display(e.nextElement());
		}
	}

	public void processAll(Vector history, ElementHost display)
	{
		if (history == null || display == null)
		{
			throw new NullPointerException();
		}
		this.history = history;
		this.display = display;
		processAll();
	}
	
	// processes the specified object, and then displays it
	private final void display(Object object)
	{
		if (object != null)
		{
			if (object instanceof Message)
			{
				object = ((Message)object).copy();
			}
			Element element = process(object);
			display(element);
		}
	}

}



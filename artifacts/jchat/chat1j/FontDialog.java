////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	FontDialog allows the user to choose a Font and Color
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.awt.*;

public class FontDialog extends ModalDialog
{
	FontListener parent;
	ElementList fontList = new ElementList(getFontList());
	ElementList styleList = new ElementList(getStyleList());
	ElementList sizeList = new ElementList(getSizeList());
//	ElementList colorList = new ElementList(getColorList());
	// for quickly getting colors
//	Color[] colorArray = getColorArray();
	Panel fontPanel;
	Button okButton;
	Button cancelButton;
	TextField sample;
	int buffer = 10;	// this 10 pixel buffer will surround all components
	String OK, CANCEL, SAMPLE, SELECT_FONT;

	public void loadStrings()
	{
		OK = get("ok");
		CANCEL = get("cancel");
		SAMPLE = get("sample");
		SELECT_FONT = get("select.font");
	}
	
	public FontDialog(Frame frame, ChatPanel parent, Font font)
	{
		super(frame, parent);
		setTitle(SELECT_FONT);
		fontPanel = createFontPanel();
		okButton = new Button(OK);
		cancelButton = new Button(CANCEL);
		sample = new TextField(SAMPLE);
		setBackground(parent.getBackground());
		this.parent = parent;
		setLayout(new GridLayout(1, 1));
			Panel main = new Panel();
			main.setLayout(new BorderLayout(6, 6));
				setFontSelections(font);
			main.add("North", fontPanel);
				updateSample();
				sample.enable(false);
			main.add("Center", sample);
				Panel south = new Panel();
				south.setLayout(new GridLayout(1, 2, 6, 6));
				south.add(okButton);
				south.add(cancelButton);
			main.add("South", south);
		Panel insetPanel = new InsetPanel(6, 6, 6, 6);
		insetPanel.add(main);
		add(insetPanel);
		setResizable(false);
	}

	public void setFontSelections(Font font)
	{
		if (font == null)
		{
			return;
		}
		fontList.select(font.getName());
		sizeList.select(Integer.toString(font.getSize()));
		styleList.select(font.getStyle());
	}

	public Panel createFontPanel()
	{
		fontList.setDeselect(false);
		styleList.setDeselect(false);
		sizeList.setDeselect(false);
		fontList.setBackground(Color.white);
		styleList.setBackground(Color.white);
		sizeList.setBackground(Color.white);
	//	colorList.setDeselect(false);
		Panel p = new Panel();
		p.setLayout(new BorderLayout());
		p.add("West", new TitlePanel(get("font"), fontList, new Insets(6,6,6,6)));
		Panel center = new Panel();
		center.setLayout(new BorderLayout());
		center.add("West", new TitlePanel(get("style"), styleList, new Insets(6,6,6,6)));
		center.add("East", new TitlePanel(get("size"), sizeList, new Insets(6,6,6,6)));
		p.add("Center", center);
	//	setColorListColors();
	//	p.add("East", new TitlePanel(get("color"), colorList, new Insets(6,6,6,6)));
		return p;
	}


/*	public void setColorListColors()
	{
		for (int i = 0; i < colorArray.length; i++)
		{
			Element e = colorList.elementAt(i);
			if (e != null && e instanceof TextElement)
			{
				TextElement te = (TextElement)e;
				te.setColor(colorArray[i]);
			}
		}
	}*/

	public String[] getFontList()
	{
		return Static.getFontList();
	}

	public String[] getStyleList()
	{
		String[] styles = {get("regular"), get("bold"), get("italic"), get("bold.italic")};
		return styles;
	}

	public String[] getSizeList()
	{
		int min = 8;
		int max = 18;
		int length = max - min + 1;
		String[] sizes = new String[length];
		for (int i = 0; i < sizes.length ; i++)
		{
			sizes[i] = Integer.toString(i + min);
		}
		return sizes;
	}

/*	public String[] getColorList()
	{
		String[] colors = {get("black"),
						   get("blue"),
						   get("cyan"),
						   get("gray"),
						   get("green"),
						   get("magenta"),
						   get("orange"),
						   get("pink"),
						   get("red"),
						   get("yellow")};
		return colors;
	}

	public Color[] getColorArray()
	{
		Color[] colors = {Color.black,
						  Color.blue,
						  Color.cyan,
						  Color.gray,
						  Color.green,
						  Color.magenta,
						  Color.orange,
						  Color.pink,
						  Color.red,
						  Color.yellow};
		return colors;
	}*/

	public void ok()
	{
		if (parent != null)
		{
			parent.setFont(getSelectedFont(), null);
		}
		close();
	}

	public void cancel()
	{
		close();		
	}

	public void updateSample()
	{
		Font font = getSelectedFont();
		if (font != null)
		{
			sample.setFont(font);
		}
	//	Color color = getSelectedColor();
	//	if (color != null)
	//	{
	//		sample.setForeground(color);
	//	}
	}

	public Font getSelectedFont()
	{
		String name = fontList.getSelectedItem();
		// the following will work because of the order they were put in
		int style = styleList.getSelectedIndex();
		int size = Integer.parseInt(sizeList.getSelectedItem());
		return new Font(name, style, size);
	}

/*	public Color getSelectedColor()
	{
		int index = colorList.getSelectedIndex();
		if (index >= 0 && index < colorArray.length)
		{
			return colorArray[index];
		}
		return Color.black;
	}*/

	public boolean action(Event e, Object arg)
	{
		if (e.target == okButton)
		{
			ok();
			return true;
		}
		else if (e.target == cancelButton)
		{
			cancel();
			return true;
		}
		return super.action(e, arg);
	}

	public boolean handleEvent(Event e)
	{
		if(e.id == Event.LIST_SELECT)
		{
			updateSample();
		}
		return super.handleEvent(e);
	}

}






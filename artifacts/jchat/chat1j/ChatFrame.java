import java.awt.*;

class ChatFrame extends DisposableFrame
{
	ChatPanel chatPanel;

	public ChatFrame(ChatPanel chatPanel)
	{
		super("Microsoft Chat");
		this.chatPanel = chatPanel;
		setLayout(new GridLayout(1, 1));
		add(chatPanel);
		pack();
		resize(640, 480);
		Static.center(this);
	}

	public void closeWindow()
	{
		if (chatPanel.applet != null)
			chatPanel.dockUndock();
		else
		{
			hide();
			dispose();
			System.exit(0);
		}
	}
}
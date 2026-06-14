#ifndef __UI_H__
#define __UI_H__

#define GetCharSelBodyCam()	((CBodyCam *)cui.GetCharSelBodyCamPv())
#define GetStatusBar()	((CStatusBar *)cui.GetStatusBarPv())
#define GetToolBar()	((CChatToolBar *)cui.GetToolBarPv())
#define GetClientDC()	((CClientDC *)cui.GetClientDCPv())
#define GetChatView()	((CChatView *)cui.GetChatViewPv())
#define GetFrame()		((CFrameWnd *)cui.GetFramePv())
#define GetRoomList()	((CRoomList *)cui.GetRoomListPv())
#define GetUserList()	((CUserList *)cui.GetUserListPv())
#define GetChatDoc()    ((CChatDoc  *)cui.GetChatDocPv())
#define GetPersonalPage() ((CPersonalPage *)cui.GetPersonalPagePv())
#define GetWhisperBox()	((CWhisperBox *)cui.GetWhisperBoxPv())
#define GetNotifBox()	((CNotificationUsers *)cui.GetNotifBoxPv())
#define GetFocusedDoc() ((CChatDoc *) cui.GetFocusedDocPv())
#define GetTabBar()		((CTabBar *) cui.GetTabBarPv())
#define GetOutBuff()	(cui.GetOutBuffSz())
#define GetOutBuffLen()	(cui.GetOutBuffLenN())
#define GetIrcProto()	((CIrcProto *)cui.GetIrcProtoPv())
#define GetDefaultProto() ((CRoomInfo *)cui.GetIrcProtoPv())
#define GetStatusView() ((CWnd *)cui.GetStatusViewPv())

class CUI
{
public:

////////////////////////////////////////////////////////////////
//
	// REVIEW: think about this
    void *	m_pvCharSelBodyCamWnd;
    void *  m_pvStatusBarWnd;
	void *	m_pvToolBarWnd;
	void *	m_pvClientDC;
	void *	m_pvChatView;
	void *	m_pvFrameWnd;
	void *	m_pvRoomList;
	void *  m_pvUserList;
	void *  m_pvChatDoc;
	void *  m_pvPersonalPage;
	void *	m_pvWhisperBox;
	void *	m_pvNotifBox;
	void *	m_pvFocusedDoc;
	void *	m_pvTabBar;
	void *	m_pvIrcProto;
	void *	m_pvStatusView;
	char *  m_szOutBuff;
	short   m_nOutBuffLen;

////////////////////////////////////////////////////////////////
//

	CUI() 
	{
		m_pvWhisperBox		= NULL;
		m_pvNotifBox		= NULL;
		m_pvChatView		= NULL;
		m_pvClientDC		= NULL;
		m_pvFrameWnd		= NULL;
		m_pvStatusBarWnd	= NULL;
		m_szOutBuff			= NULL;
		m_nOutBuffLen		= 0;
	}

	~CUI()
	{
		if (m_pvClientDC)
			delete (CClientDC *) m_pvClientDC;

		if (m_szOutBuff)
			delete [] m_szOutBuff;
	}


//#ifndef _CHAT
	// for ICommObject
//	STDMETHODIMP_(BOOL) Call( BOOL bSynchronous, WORD iCallId, PBYTE pbData, DWORD cb );
//#endif

	BOOL bAllocOutBuff(SHORT nLen)
	{
		if (m_szOutBuff)
			delete [] m_szOutBuff;

		if (m_szOutBuff = new CHAR[nLen])
		{
			m_nOutBuffLen = nLen;
			return TRUE;
		}
		else
		{
			m_nOutBuffLen = 0;
			return FALSE;
		}
	}

	// REVIEW: think about this
    void * GetCharSelBodyCamPv()
	{
		return m_pvCharSelBodyCamWnd;
	}
    void * GetStatusBarPv()
    {
        return m_pvStatusBarWnd;
    }
	void * GetToolBarPv()
	{
		return m_pvToolBarWnd;
	}
	void * GetClientDCPv()
	{
		return m_pvClientDC;
	}
	void * GetChatViewPv()
	{
		return m_pvChatView;
	}
	void * GetFramePv()
	{
		return m_pvFrameWnd;
	}
	void * GetRoomListPv()
	{
		return m_pvRoomList;
	}
	void * GetUserListPv()
	{
		return m_pvUserList;
	}
	void * GetChatDocPv()
	{
		return m_pvChatDoc;
	}

	void * GetPersonalPagePv()
	{
		return m_pvPersonalPage;
	}

	void * GetWhisperBoxPv()
	{
		return m_pvWhisperBox;
	}

	void * GetNotifBoxPv()
	{
		return m_pvNotifBox;
	}

	void * GetFocusedDocPv()
	{
		return m_pvFocusedDoc;
	}

	void * GetTabBarPv()
	{
		return m_pvTabBar;
	}

	void * GetIrcProtoPv()
	{
		return m_pvIrcProto;
	}

	void * GetStatusViewPv()
	{
		return m_pvStatusView;
	}

	char * GetOutBuffSz()
	{
		return m_szOutBuff;
	}

	short GetOutBuffLenN()
	{
		return m_nOutBuffLen;
	}
};

extern CUI cui;

#endif // __UI_H__


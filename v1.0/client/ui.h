#ifndef _UI_H_
#define _UI_H_

//#include "logwnd.h"


#define GetView()		((CPageView *)cui.GetViewPv())
#define GetTextView()	((CTextView *)cui.GetViewPv())
#define GetPrimaryView()((CView *)cui.GetViewPv())
// #define GetLog()		((CLogWnd *)cui.GetLogPv())
#define GetMembers()	((CMemberList *)cui.GetMembersPv())
#define GetSay()		((CSayWnd *)cui.GetSayPv())
#define GetBodyCam()	((CBodyCam *)cui.GetBodyCamPv())
#define GetCharSelBodyCam()	((CBodyCam *)cui.GetCharSelBodyCamPv())
#define GetStatusBar()	((CStatusBar *)cui.GetStatusBarPv())
#define GetClientDC()	((CClientDC *)cui.GetClientDCPv())
#define GetChatView()	((CChatView *)cui.GetChatViewPv())
#define GetFrame()		((CFrameWnd *)cui.GetFramePv())
#define GetRoomList()	((CRoomList *)cui.GetRoomListPv())
#define GetChatApp()    ((CChatApp  *)cui.GetChatAppPv())
#define GetChatDoc()    ((CChatDoc  *)cui.GetChatDocPv())

class CUI
{
public:

////////////////////////////////////////////////////////////////
//
	// REVIEW: think about this
	void *	m_pvClientView;
	void *	m_pvMembersWnd;
	void *	m_pvSayWnd;
	void *	m_pvBodyCamWnd;
    void *	m_pvCharSelBodyCamWnd;
    void *  m_pvStatusBarWnd;
	void *	m_pvClientDC;
	void *	m_pvChatView;
	void *	m_pvFrameWnd;
	void *	m_pvRoomList;
	void *  m_pvChatApp;
	void *  m_pvChatDoc;

////////////////////////////////////////////////////////////////
//

	CUI() { m_pvClientDC = NULL; m_pvFrameWnd = NULL; m_pvStatusBarWnd = NULL; }
	~CUI() { if (m_pvClientDC) delete (CClientDC *) m_pvClientDC; }

	// Appends str to log window
	void Log( CString& str, \
			  int eLogOptions = 0/*Just continue text (no-bold etc.) by default*/, 
			  COLORREF crTextColor = RGB(0,0,0)/*Default text color is black*/);

	// Called when user types string in "say" window
	void Say( CString& str );
	void Think( CString& str );
	void Emote( CString& str );
	void Action( CString & str );

	// Called when user enters partial string in "say" window
	void PreSay(CString& str);

	// Called when user invokes verb
	void DoVerb( CString& strVerb );

#ifndef _CHAT
	// for ICommObject
	STDMETHODIMP_(BOOL) Call( BOOL bSynchronous, WORD iCallId, PBYTE pbData, DWORD cb );
#endif

	// displays error messages
	void Error( UINT nID );
	void Error( CString str );

	// displays warnings
	void Warning( UINT nID );
	void Warning( CString str );

	// REVIEW: think about this
	void * GetViewPv()
	{
		return m_pvClientView;
	}
	void * GetSayPv()
	{
		return m_pvSayWnd;
	}
	void * GetBodyCamPv()
	{
		return m_pvBodyCamWnd;
	}
    void * GetCharSelBodyCamPv()
	{
		return m_pvCharSelBodyCamWnd;
	}
    void * GetStatusBarPv()
    {
        return m_pvStatusBarWnd;
    }
	void * GetMembersPv() 
	{
		return m_pvMembersWnd;
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

	void * GetChatAppPv()
	{
		return m_pvChatApp;
	}

	void * GetChatDocPv()
	{
		return m_pvChatDoc;
	}

	static BOOL FindFileInPath(	const char *	pszFile,
								CString 		strPath,
								CString& 		strFileFound );
};

extern CUI cui;

#endif

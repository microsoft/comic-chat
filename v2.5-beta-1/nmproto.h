#ifdef CB32SUPPORT			// entire files is only read if CB32SUPPORT turned on

// moved from ircproto.h
#define IS_CHANNEL_CHAR(c)	(((c)=='#') || ((c)=='&'))
#define IS_ANNOTATED(s)		(strncmp( s, "(#", 2 ) == 0)



class CCb32CoreNotify : public ICb32CoreNotify
	{
public:
    CCb32CoreNotify(CWnd *pUI);
   ~CCb32CoreNotify( void );

	
	// The methods of IUnknown
    STDMETHODIMP QueryInterface( REFIID riid, void** ppvObj );
    STDMETHODIMP_(ULONG) AddRef( void );
    STDMETHODIMP_(ULONG) Release( void );


	// The methods of ICb32CoreNotify
    STDMETHODIMP ChatStatus( BOOL bInProgress );

    STDMETHODIMP ReceivedMessage( BOOL bEcho,
								  TCHAR *pszMemberName,
								  INmMember *pMember,
								  TCHAR *pszWhisperToName,
								  INmMember *pWhisperTo,
								  ULONG uBuffer,
								  byte *pBuffer, 
								  ULONG uCChatBuffer,
								  byte *pCChatBuffer,
								  HRESULT hr );
        
    STDMETHODIMP MemberJoinedConference( INmMember *pMember, HRESULT hr );
    STDMETHODIMP MemberLeftConference( INmMember *pMember );
    STDMETHODIMP MemberJoinedChat( INmMember *pMember );
    STDMETHODIMP MemberLeftChat( INmMember *pMember );
    STDMETHODIMP SpecialOps( DWORD dwSpecial );

private:
    CWnd *m_pUI;
	ULONG		  m_uRef;
	};


class NmUserInfo : public CUserInfo {
public:
	INmMember *m_pMember;
	NmUserInfo(INmMember *);
	virtual ~NmUserInfo();
	virtual CString &GetScreenName() { return m_fullName; }
	virtual const char *GetQualifiedName() { return (GetScreenName()); }
	TCHAR * BSTR_to_TCHAR( BSTR bs );
	virtual BOOL IsSelf() { return (m_pMember->IsSelf() == S_OK); }
};



class CNmProto : public CRoomInfo {
public:
	CNmProto() { m_iStatus = CX_DISCONNECTED; }

	static ICb32Core *m_pCbCore;
	int m_iStatus;

#if 0
	virtual BOOL ChatSendPrivMesg(const char *addressee, const char *mesg);
	virtual BOOL ChatSendNotice(const char *addressee, const char *mesg);
	virtual BOOL ChatSetTopic(const char *topic);
	virtual BOOL ChatKickUser(const char *nick, const char *reason);
	virtual void ChatKickUser(CUserInfo *pui);
	virtual void ChatBanUser(CUserInfo *pui);
	virtual BOOL ChatBanUser(const char *pattern, BOOL bBan);
	virtual void ChatGetIdentity(CUserInfo *pui); 
	virtual void ChatShowMOTD();
	virtual BOOL ChatSendInvitation(const char *nick);
	virtual BOOL ChatChangeNick(const char *newNick);
	virtual void OnLogin();
#endif
	virtual void ChatPartChannel(CDocument *doc, BOOL hardDisconnect);
	virtual BOOL ChatSendToChannel(const char *szAnnotations, const char *szMesg, char *szNMText = NULL, BYTE byteMode = 0);
	virtual BOOL ChatSendPrivMesg(const char *szAddressee, const char *szAnnotations, const char *szMesg, char *szNMText = NULL, BOOL bAsNotice = FALSE, BYTE byteMode = 0);
	virtual void SendMessageText(char *szAnnotations, char *szMessage, const char *szName = NULL, char *szNMText = NULL);
	static BOOL Initialize();
	static void Uninitialize();
	static void GetNewProto();
	virtual void SetConnectionStatus(int);
	virtual int GetConnectionStatus() { return m_iStatus; }
	virtual int GetType() { return PC_NM; }
	virtual void OnIdle(LONG);
};

#endif CB32SUPPORT


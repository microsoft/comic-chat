#ifndef __USERINFO_H__
#define __USERINFO_H__

#define UF_IGNORED		1
#define UF_COMICUSER	2
#define UF_DEPARTED		4
#define UF_OPERATOR		8
#define UF_EXTERNAL		16
#define UF_SPECTATOR	32
#define UF_REQUESTPING	64
#define UF_HASVOICE		128
#define UF_AWAY			256
#define UF_SCREENNAME	512
#define UF_OWNER		1024
#define UF_AUTODOWNLOAD 16384
#define UF_INTERACTIVEDOWNLOAD 32768

#define RF_PROFILE		0x000F
#define RF_TIME			0x0030
#define RF_EMAIL		0x00C0
#define RF_HOMEPAGE		0x0300
#define RF_NETMEETING	0x0C00
#define RF_VERSION		0x3000

#define PROFILECREDITS	3

class CUserDisplayInfo
{
public:
	CUserDisplayInfo() 
	{
		Reset();
	};

	virtual ~CUserDisplayInfo()
	{
		m_talkTos.RemoveAll();
	};

	void Reset()
	{
		m_chGest = m_chExpr = -1;
		m_chGestE = m_chGestI = m_chExprE = m_chExprI = 0;
		m_bbCooked = m_bbReq = 0;
		m_uModes = 0x0001;	// == BM_SAY
		m_talkTos.RemoveAll();
	}

	CHAR		m_chGest;
	CHAR		m_chExpr;
	CHAR		m_chGestE;
	CHAR		m_chGestI;
	CHAR		m_chExprE;
	CHAR		m_chExprI;
	BYTE		m_bbCooked;
	BYTE		m_bbReq;
	USHORT		m_uModes;
	CDWordArray	m_talkTos;
};

// Forward declaration
class CChatDoc;

class CUserInfo
{
protected:
	CString		m_strName;
	CString		m_fullName;
	CString		m_strScreenName;
	USHORT		m_uRequests;
	USHORT		m_flags;
	USHORT		m_avatarID;
	CString     m_strAvatarRealName;
	CString     m_strAvatarRealURL;
	USHORT		m_uIntervalStart;		// start of current flood interval
	UCHAR		m_uMsgCount;			// number of utterances within that period (moving average)
public:
	CUserInfo();
	CUserInfo(const char *nick, const char *fullName = NULL);
	virtual ~CUserInfo() {};
	CString& GetName() { return m_strName; }
	CString& GetFullName() { return m_fullName; }
	void SetFullName(const char *fName) { m_fullName = fName; }
	virtual CString& GetScreenName();
	virtual const char *GetQualifiedName();
	BOOL IsRequestInfo(USHORT uRequest) {				// BETA1 fix
		return (m_uRequests & uRequest);
	}
	BOOL Ignored() {
		return (m_flags & UF_IGNORED);
	}
	BOOL IsComicUser() {
		return (m_flags & UF_COMICUSER);
	}
	BOOL IsDeparted() {
		return (m_flags & UF_DEPARTED);
	}
	BOOL IsOperator() {
		return (m_flags & UF_OPERATOR);
	}
	BOOL IsOwner() {
		return (m_flags & UF_OWNER);
	}
	BOOL IsExternal() {
		return (m_flags & UF_EXTERNAL);
	}
	BOOL NeedsDownload() {
		return (m_flags & (UF_AUTODOWNLOAD|UF_INTERACTIVEDOWNLOAD));
	}
	BOOL IsDownloadInteractive() {
		return (m_flags & UF_INTERACTIVEDOWNLOAD);
	}
	virtual BOOL IsSelf() {
		const char *GetMyNickName();
		return (strcmp(GetName(), GetMyNickName()) == 0);
	}
	BOOL CheckFlag(USHORT uCheckFlag) {
		return (m_flags & uCheckFlag);
	}
	void SetFlag(USHORT setFlag, BOOL bValue) {
		m_flags = bValue ? (m_flags | setFlag) : (m_flags & ~setFlag);
	}
	BOOL IsSpeaker() {return (!(m_flags & UF_OPERATOR || m_flags & UF_SPECTATOR)); }
	BOOL IsSpectator() { return (m_flags & UF_SPECTATOR); }
	BOOL MatchesNickMask(const char *mask) { return (GetFullName() == mask); }
	void SetName(CString& strNew) { SetName((LPCTSTR) strNew); }
	void SetName(const char *nick) {
		m_strName = nick;
		const char *DecodeNickForScreen(const char *);
		SetScreenName(*nick == '\'' ? DecodeNickForScreen(nick) : NULL);
	}
//	void GetAvatar( CString& strAvatar );
	void Ignore( BOOL bIgnore ) { m_flags = bIgnore ? (m_flags | UF_IGNORED) : (m_flags & ~UF_IGNORED); }
	void ComicUser ( BOOL bComicUser ) { m_flags = bComicUser ? ( m_flags | UF_COMICUSER) : (m_flags & ~UF_COMICUSER); }
	void SetDeparted(BOOL bDeparted) { m_flags = bDeparted ? (m_flags | UF_DEPARTED) : (m_flags & ~UF_DEPARTED); }
	void IncrementRequestInfo(USHORT uRequest);
	void DecrementRequestInfo(USHORT uRequest);
	void SetOperator(BOOL bOp) { m_flags = bOp? (m_flags | UF_OPERATOR) : (m_flags & ~UF_OPERATOR); }
	void SetOwner(BOOL bOwner) { m_flags = bOwner ? (m_flags | UF_OWNER) : (m_flags & ~UF_OWNER); }
	void SetExternal(BOOL bExt) { m_flags = bExt ? (m_flags | UF_EXTERNAL) : (m_flags & ~UF_EXTERNAL); }
	USHORT GetAvatarID() { return m_avatarID; }
	void SetAvatarID(USHORT avID) { m_avatarID = avID; }
	void SetAvatarRealInfo(LPCSTR pszName, LPCSTR pszURL) { m_strAvatarRealName = pszName; m_strAvatarRealURL = pszURL; }
	LPCSTR GetAvatarRealName() { return m_strAvatarRealName; }
	LPCSTR GetAvatarRealURL() { return m_strAvatarRealURL; }
	BOOL IsAvatarReal() { return m_strAvatarRealName.IsEmpty (); }
	void GetAttedNick(CString &, BOOL bScreen = TRUE);
	void ClearTalkTos() { 	m_udi.m_talkTos.RemoveAll(); }
	void SelectInMemberList(CUserInfo *addressee, BOOL select = TRUE, BOOL bExtend = FALSE);
	void UnselectAll();
	BOOL IsFlooding();
	void SetScreenName(const char *);

	BYTE				m_bbValidUDI;
	CUserDisplayInfo	m_udi;
	// CDWordArray		m_talkTos;	// REGISB 11/13/97 using m_udi.m_talkTos instead
};

extern CUserInfo *g_puiSelf;

extern USHORT	ExtractAvatarID(void *vdInfo);
extern void		SetMyPUIAvatarID(UINT avID);
extern void		SetUserAvatarID(CUserInfo *pui, unsigned short avID);
extern void		SetUserAvatarRealInfo(CUserInfo *pui, LPCSTR pszName, LPCSTR pszURL, CChatDoc* pDoc, BOOL bLive = TRUE);
extern void		MListTalkTosToPuiself(CUserInfo *puiSelf);

#endif // __USERINFO_H__

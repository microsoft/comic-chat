#define UF_IGNORED		1
#define UF_COMICUSER	2
#define UF_DEPARTED		4
#define UF_REQUESTINFO	8		// getinfo pending  // BETA1 fix
#define UF_OPERATOR		16

class CUserInfo
{
protected:
	void *		m_clid;
	CString		m_strName;
	CString		m_fullName;
	USHORT		m_flags;
	USHORT		m_avatarID;
public:
	CUserInfo();
	CUserInfo(const CUserInfo &u) {  m_clid = u.m_clid; m_strName = u.m_strName; m_fullName = u.m_fullName; m_flags = u.m_flags; m_avatarID = u.m_avatarID; }
	~CUserInfo();
	BOOL Initialize(void *clid, const char *pch, int cch);
	void *GetClid()
	{
		return m_clid;
	}
	CString& GetName()
	{
		return m_strName;
	}
	CString& GetFullName()
	{
		return m_fullName;
	}
#if 0
	DWORD GetStatus()
	{
		return m_dwStatus;
	}
#endif
	BOOL Ignored() {
		return (m_flags & UF_IGNORED);
	}
	BOOL IsComicUser() {
		return (m_flags & UF_COMICUSER);
	}
	BOOL IsDeparted() {
		return (m_flags & UF_DEPARTED);
	}
	BOOL IsRequestInfo() {				// BETA1 fix
		return (m_flags & UF_REQUESTINFO);
	}
	BOOL IsOperator() {
		return (m_flags & UF_OPERATOR);
	}
	void StopAnimation();
	void SetName(CString& strNew);
	void SetName(const char *strNew) { m_strName = strNew; }
	void GetAvatar( CString& strAvatar );
	void Ignore( BOOL bIgnore ) { m_flags = bIgnore ? (m_flags | UF_IGNORED) : (m_flags & ~UF_IGNORED); }
	void ComicUser ( BOOL bComicUser ) { m_flags = bComicUser ? ( m_flags | UF_COMICUSER) : (m_flags & ~UF_COMICUSER); }
	void SetDeparted(BOOL bDeparted) { m_flags = bDeparted ? (m_flags | UF_DEPARTED) : (m_flags & ~UF_DEPARTED); }
	void SetRequestInfo(BOOL bReq) { m_flags = bReq ? (m_flags | UF_REQUESTINFO) : (m_flags & ~UF_REQUESTINFO); } // BETA1 fix
	void SetOperator(BOOL bOp) { m_flags = bOp? (m_flags | UF_OPERATOR) : (m_flags & ~UF_OPERATOR); }
	USHORT GetAvatarID() { return m_avatarID; }
	void SetAvatarID(USHORT avID) { m_avatarID = avID; }
	void GetAttedNick(CString &);

protected:
	BOOL CreateAvatarFrame( CString& strAvatar, CString& strName );
};

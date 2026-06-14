#define HT_UNSPECED		0
#define HT_AVATARENTRY	1

class HistoryEntry
{
public:
	virtual void Execute(int mode, CChatDoc* pDoc = NULL) = 0;
	virtual void WriteSelf(CArchive &) = 0;
	virtual UINT GetType() { return HT_UNSPECED; }
	virtual ~HistoryEntry() {}
};


class SayEntry : public HistoryEntry 
{
public:
	CUserDisplayInfo	m_udi;
	CDWordArray*		m_prgdwFormatting;
	CUserInfo*			m_pui;
	const char*			m_mesg;
	const char*			m_name;
	char				m_cHighlightType;
	
	SayEntry(CUserInfo *pui, const char *szMesg, CDWordArray *prgdwFormatting, char cHighlightType = -1);
	SayEntry(CString &, CChatDoc *, char cHighlightType = -1);
	virtual ~SayEntry();
	void Execute(int mode, CChatDoc* pDoc = NULL);
	void WriteSelf(CArchive &);
	void FormatOtherArgs(CString &str);
	void ReadOtherArgs(char *, CChatDoc *);
};


class JoinEntry : public HistoryEntry
{
public:
	CUserInfo*	m_pui;
	char*		m_name;
	char*		m_fullName;
	BOOL		m_bTherePrior;
	char		m_cHighlightType;

	JoinEntry(CUserInfo *pui, BOOL bPrior = TRUE, char cHighlightType = -1);
	JoinEntry(CString &str, char cHighlightType = -1);
	virtual ~JoinEntry();
	void Execute(int mode, CChatDoc* pDoc = NULL);
	void WriteSelf(CArchive &);
};


class PartEntry : public HistoryEntry
{
public:
	char*	m_name;
	char	m_cHighlightType;

	PartEntry(const char *szNick, char cHighlightType = -1) 
		{
			m_name = strdup(szNick);
			m_cHighlightType = cHighlightType;
		}
	PartEntry(CString &);
	virtual ~PartEntry();
	void Execute(int mode, CChatDoc* pDoc = NULL);
	void WriteSelf(CArchive &);
};


class ChangeAvatarEntry : public HistoryEntry
{
public:
	CUserInfo *m_pui;
	char *m_name;				// the nick
	unsigned short m_avID;
	char *m_avName;
	char *m_avURL;

	ChangeAvatarEntry(CUserInfo *pui, const char *avName, const char *avURL);
	ChangeAvatarEntry(CString &);
	virtual ~ChangeAvatarEntry();
	void Execute(int mode, CChatDoc* pDoc = NULL);
	void WriteSelf(CArchive &);
	virtual UINT GetType() { return HT_AVATARENTRY; }
};


class GetInfoEntry : public HistoryEntry
{
public:
	CUserInfo *m_pui;
	char *m_info;
	char *m_name;

	GetInfoEntry(CUserInfo *pui, char *info) { m_pui = pui; m_name = strdup(pui->GetName()); m_info = strdup(info); }
	GetInfoEntry(CString &);
	virtual ~GetInfoEntry();
	void Execute(int mode, CChatDoc* pDoc = NULL);
	void WriteSelf(CArchive &);
};


class ComicCharacterEntry : public GetInfoEntry
{
public:
	ComicCharacterEntry(CUserInfo *pui) : GetInfoEntry(pui, NULL) { }
	ComicCharacterEntry(CString &);
	void Execute(int mode, CChatDoc* pDoc = NULL);
	void WriteSelf(CArchive &);
};


class StartHistoryEntry : public HistoryEntry
{
public:
	int m_randStart;
	char *m_title;
	const char *m_avName;
	char *m_name;

	StartHistoryEntry(const char *title, const char *avName, int rand);
	StartHistoryEntry(CString &);
	virtual ~StartHistoryEntry();
	void Execute(int mode, CChatDoc* pDoc = NULL);
	void WriteSelf(CArchive &);
};


class ChangeBackDropEntry : public HistoryEntry
{
public:
	char *m_backName;
	char *m_backURL;

	ChangeBackDropEntry(const char *backName, const char *backURL) { m_backName = strdup(backName); m_backURL = strdup(backURL); }
	ChangeBackDropEntry(CString &);
	virtual ~ChangeBackDropEntry();
	void Execute(int mode, CChatDoc* pDoc = NULL);
	void WriteSelf(CArchive &);
};


class NickEntry : public HistoryEntry
{
public:
	char *m_oldNick;
	char *m_newNick;

	NickEntry(const char *oldNick, const char *newNick) { m_oldNick = strdup(oldNick); m_newNick = strdup(newNick);}
	NickEntry(CString &);
	virtual ~NickEntry();
	void Execute(int mode, CChatDoc* pDoc = NULL);
	void WriteSelf(CArchive &);
};


void AddAndExecute(HistoryEntry *, CDocument * = NULL);

// Execute modes
#define	HM_LIVE		1
#define HM_RELOAD	2
#define HM_LOAD		4

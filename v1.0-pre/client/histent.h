class HistoryEntry {
public:
	virtual void Execute(int mode) = 0;
	virtual void WriteSelf(CArchive &) = 0;
	virtual ~HistoryEntry() {}
};

class SayEntry : public HistoryEntry {
public:
	CUserInfo *m_pui;
	const char *m_mesg;
	const char *m_name;
	BYTE m_cooked;
	BYTE m_mode;
	BYTE m_expr;
	BYTE m_gest;
	BYTE m_exprE;
	BYTE m_exprI;
	BYTE m_gestE;
	BYTE m_gestI;
	BYTE m_req;
	CWordArray m_talkTos;
	CPtrArray *m_links;

	SayEntry(CUserInfo *pui, const char *mesg, BOOL cooked, BYTE mode, BYTE expr, BYTE gest, BYTE exprE, BYTE exprI, BYTE gestE, BYTE gestI, BOOL req, CWordArray &talkTos);
	SayEntry(CString &);
	virtual ~SayEntry();
	void Execute(int mode);
	void WriteSelf(CArchive &);
	void FormatOtherArgs(CString &str);
	void ReadOtherArgs(char *);
};

class JoinEntry : public HistoryEntry {
public:
	CUserInfo *m_pui;
	char *m_name;

	JoinEntry(const char *nick) { m_name = strdup(nick); m_pui = NULL; }
	JoinEntry(CString &);
	virtual ~JoinEntry();
	void Execute(int mode);
	void WriteSelf(CArchive &);
};

class PartEntry : public HistoryEntry {
public:
	char *m_name;

	PartEntry(const char *nick) { m_name = strdup(nick); }
	PartEntry(CString &);
	virtual ~PartEntry();
	void Execute(int mode);
	void WriteSelf(CArchive &);
};

class ChangeAvatarEntry : public HistoryEntry {
public:
	CUserInfo *m_pui;
	char *m_name;				// the nick
	unsigned short m_avID;
	char *m_avName;

	ChangeAvatarEntry(CUserInfo *pui, const char *avName);
	ChangeAvatarEntry(CString &);
	virtual ~ChangeAvatarEntry();
	void Execute(int mode);
	void WriteSelf(CArchive &);
};

class GetInfoEntry : public HistoryEntry {
public:
	CUserInfo *m_pui;
	char *m_info;
	char *m_name;

	GetInfoEntry(CUserInfo *pui, char *info) { m_pui = pui; m_name = strdup(pui->GetName()); m_info = strdup(info); }
	GetInfoEntry(CString &);
	virtual ~GetInfoEntry();
	void Execute(int mode);
	void WriteSelf(CArchive &);
};

class StartHistoryEntry : public HistoryEntry {
public:
	int m_randStart;
	char *m_title;
	const char *m_avName;
	char *m_name;

	StartHistoryEntry(const char *title, const char *avName, int rand);
	StartHistoryEntry(CString &);
	virtual ~StartHistoryEntry();
	void Execute(int mode);
	void WriteSelf(CArchive &);
};

class ChangeBackDropEntry : public HistoryEntry {
public:
	char *m_backName;

	ChangeBackDropEntry(const char *backName);
	ChangeBackDropEntry(CString &);
	virtual ~ChangeBackDropEntry();
	void Execute(int mode);
	void WriteSelf(CArchive &);
};

class NickEntry : public HistoryEntry {
public:
	char *m_oldNick;
	char *m_newNick;

	NickEntry(const char *oldNick, const char *newNick) { m_oldNick = strdup(oldNick); m_newNick = strdup(newNick);}
	NickEntry(CString &);
	virtual ~NickEntry();
	void Execute(int mode);
	void WriteSelf(CArchive &);
};


// Execute modes
#define	HM_LIVE		1
#define HM_RELOAD	2
#define HM_LOAD		4
#define SM_SAY		1
#define SM_WHISPER	2
#define SM_THINK	3
#define SM_SHOUT	4
#define SM_ACTION	5

#define CX_DISCONNECTED		0
#define CX_INCHANNEL		1
#define CX_CONNECTING		2
#define CX_NOCHANNEL		3

extern COLORREF	linkColor;
#define LINKINDEX	252

BOOL ChatInitialize();
BOOL ChatIdle();
BOOL ChatTerminate();
void ChatPartChannel(BOOL hardDisconnect, BOOL removeMembers = TRUE);
BOOL ChatSendText(CString& str, UCHAR mode);
void ChatPreSendText(CString & str, int avID = 0);
// BOOL ChatAvatarChanged();
BOOL ChatGesture(int eGesture);
BOOL ChatGetAvatarName(CString& str);
BOOL ChatSetAvatarName(CString& str);
int  ChatGetMemberCount();
void ChatSetMemberCount(int);
int ChatGetConnectionStatus();
void ChatSetConnectionStatus(int);
void ChatSetNick(const char *nick);
BOOL ChatToggleIgnore( LPARAM lParam );
BOOL ChatFind( LPARAM lParam );
void ChatOnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
void ChatOnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );
void ChatSwitchCamera();
void SetMyAvatar(UINT avatarID);
BOOL SetMyAvatar(const char *avName);
void DrawTalkTos(BOOL clearFirst);
void StartNewPanel();
POINT AccountForScroll(POINT *loc, BOOL mapBack, BOOL logCoords, BOOL isPrinting);
void SetPrintOffset(int x, int y);

BOOL ReplaceToken( CString& str, const CString& strToken, const CString& strValue );

void DrawArc(CDC *dc, POINT& start, POINT& end, int radius, BOOL downStroke);
void DrawArc2(CDC *dc, POINT& start, POINT& end, int altitude);

// new chat server abstraction
BOOL CommunicationInits();
void InitializeServerConnection();
void InitializeChannelConnection();
void ChatAnnounceNewAvatar(const char *avName, const char *addressee = NULL);


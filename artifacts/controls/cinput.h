// 
//   These constants are used for both setting
//   which buttons to create and for getting / setting
//   the enabled / disabled state of the buttons
//
//
#define CI_TBS_SAY      0x01
#define CI_TBS_THINK    0x02
#define CI_TBS_WHISPER  0x04
#define CI_TBS_EMOTE    0x08
#define CI_TBS_WEMOTE   0x10
#define CI_TBS_SOUND    0x20
#define CI_TBS_WSOUND   0x40

#define CI_STYLE_MULTILINE     0x00000001
//#define CI_STYLE_AUTOCOMPLETE  0x00000002

//
//   The WM_CI_NOTIFICATION message is sent to notify
//   the parent of an event. The wParam will contain
//   the notification type (CIN_*).
//
#define WM_CI_NOTIFICATION  (WM_USER + 101)

//
// Toolbar Button Notifications
//
// Must be kept in a sequential, non-interrupted sequence
//
#define CIN_SAY      0x01
#define CIN_THINK    0x02
#define CIN_WHISPER  0x03
#define CIN_EMOTE    0x04
#define CIN_WEMOTE   0x05
#define CIN_SOUND    0x06
#define CIN_WSOUND   0x07

#define CIN_IRC_COMMAND  0x10
#define CIN_INVOKECOLOR  0x11


//
//  Formatting styles to be used with GetFormat / SetFormat
//
#define CI_FORMAT_BOLD       0x01
#define CI_FORMAT_UNDERLINE  0x02
#define CI_FORMAT_ITALIC     0x04


class CChatInput 
{
public:
    //
    // Call Initialize and terminate exactly once
    // to use this class
    //
    static BOOL Initialize(HINSTANCE hInst);
    static void Terminate();

    //
    // Two-step creation. Construct, then Create
    //

    CChatInput();
    ~CChatInput();


    BOOL Create(HWND hWndParent,
           int x,int y,
           int cx,int cy,
           UINT nID,
           DWORD dwWindowStyle,
           DWORD dwExWindowStyle,
           DWORD dwChatStyle,
           DWORD dwToolBarStyle);

    //
    // Destroy will destroy the window but not "this".
    // It is not generally necessary to explicitly call this as the
    // the CChatInput destructor will destroy the window
    //
    void Destroy();

    HWND GetWindow();
    HWND GetRichEditWindow();
    HWND GetToolbar();

    void SetFont(HFONT hFont);
    HFONT GetFont();

    //
    // SetFormat / GetFormat
    //   Set and retrieve the formatting characteristics
    //   of the font
    //
    DWORD SetFormat(DWORD dwStyles);
    DWORD GetFormat();

    //  
    // SetToolbarState / GetToolbarState
    //
    //  For enabling / disabling the toolbar buttons.
    //  For example, if no users are selected in the member
    //  list, then the whisper and e-mail buttons should be
    //  disabled, for example:
    //  SetToolbarState(GetToolbarState() & ~(CI_TBS_WHISPER | CI_TBS_EMAIL));
    //
    void SetToolbarState(DWORD dwToolbarState);
    DWORD GetToolbarState();

    void ShowToolbar(BOOL bShow);
    BOOL IsToolbarVisible();

    //
    //  The WindowProc virtual function is provided for customizing this
    //  window. If you handle the message, then return TRUE to prevent further
    //  processing. Otherwise, return FALSE and the default processing will occur.
    //
    virtual BOOL WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam,LRESULT *pResult);

protected:
    HWND m_hWnd;
    HWND m_hWndRichEdit;
    HWND m_hWndToolbar;

    BOOL CreateMainWindow(HWND hWndParent,
                          int x,int y,
                          int cx,int cy,
                          UINT nID,
                          DWORD dwStyle,
                          DWORD dwExStyle,
                          DWORD dwChatStyle);

    BOOL CreateToolbar(DWORD dwToolbarStyle);
    BOOL CreateRichEditWindow(DWORD dwChatStyle);
    
    void RepositionRichEditWindow(BOOL bFirstCall = FALSE);

    LRESULT HandleToolbarNotification(NMHDR* pnmh);
    LRESULT HandleRichEditNotification(NMHDR* pnmh);
    void AddButton(int nBtnIndex);

    LRESULT InternalWndProc(UINT uMsg,WPARAM wParam,LPARAM lParam);

    friend LRESULT CALLBACK MainWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

#include "cinput.h"

//
//  Windows IDs and command constants
//
   
#define ID_TOOLBAR        139
#define ID_RICHEDIT       140

//
// Note that the button IDs map exactly to the
// notifications, which MUST BE in sequential order.
//
#define IDC_FIRSTBUTTON CIN_SAY
#define IDC_LASTBUTTON  CIN_WSOUND


//
// Window classes and globals
//
static char szMainWindowClass[] = "ChatInputWindowClass";
static char szRICHEDIT[] = "RichEdit";
static char szRICHEDITDLL[] = "riched32.dll";

HINSTANCE ghInst = NULL;
HINSTANCE ghInstRichEdit = NULL;

#define CX_BUTTON_IMAGE 17
#define CY_BUTTON_IMAGE 17
#define CX_BUTTON_SIZE  24
#define CY_BUTTON_SIZE  24

//
// Helpers that can probably be moved to a utility module
//

#define RECTWIDTH(r) (r.right - r.left)
#define RECTHEIGHT(r) (r.bottom - r.top)

void ScreenToClient(HWND hWnd,RECT *pRect)
{
    ScreenToClient(hWnd,(LPPOINT) &pRect->left);
    ScreenToClient(hWnd,(LPPOINT) &pRect->right);
}

typedef struct tagBUTTONSTRUCT
{
    DWORD dwStyle;
    UINT  nIDString;
    TBBUTTON btnInfo;
} BUTTONSTRUCT;

const BUTTONSTRUCT ButtonArray[] = 
{
    { CI_TBS_SAY,    IDS_TOOLTIP_SAY,    { 0, CIN_SAY,     TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0} },
    { CI_TBS_THINK,  IDS_TOOLTIP_THINK,  { 1, CIN_THINK,   TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0} },
    { CI_TBS_WHISPER,IDS_TOOLTIP_WHISPER,{ 2, CIN_WHISPER, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0} },
    { CI_TBS_EMOTE,  IDS_TOOLTIP_EMOTE,  { 3, CIN_EMOTE,   TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0} },
    { CI_TBS_WEMOTE, IDS_TOOLTIP_EMOTE,  { 4, CIN_WEMOTE,  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0} },
    { CI_TBS_SOUND,  IDS_TOOLTIP_SOUND,  { 5, CIN_SOUND,   TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0} },
    { CI_TBS_WSOUND, IDS_TOOLTIP_SOUND,  { 6, CIN_WSOUND,  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0} }
};

const int nButtonEntries = sizeof(ButtonArray) / sizeof(BUTTONSTRUCT);


LRESULT CALLBACK MainWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

static BOOL RegisterWindowClass()
{
    WNDCLASS wc;

    wc.style = 0;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = ghInst;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE+1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szMainWindowClass;

    return RegisterClass(&wc);
}

static void UnregisterWindowClass()
{
    ASSERT(ghInst!=NULL);

    UnregisterClass(szMainWindowClass,ghInst);
}

BOOL CChatInput::Initialize(HINSTANCE hInst)
{
    ghInst = hInst;
    if(!RegisterWindowClass())
        return FALSE;

    //
    // Load the richedit DLL 
    //
    ghInstRichEdit = LoadLibrary(szRICHEDITDLL);
    
    return (ghInstRichEdit!=NULL);
}

void CChatInput::Terminate()
{
    UnregisterWindowClass();
    ghInst = NULL;
    if(ghInstRichEdit!=NULL)
        FreeLibrary(ghInstRichEdit);
}

    
CChatInput::CChatInput()
{
    m_hWnd = NULL;
    m_hWndRichEdit = NULL;
    m_hWndToolbar = NULL;
}

CChatInput::~CChatInput()
{
    Destroy();
}

void CChatInput::Destroy()
{
    if(m_hWndRichEdit!=NULL)
    {
        SetWindowLong(m_hWndRichEdit,GWL_USERDATA,0);
        DestroyWindow(m_hWndRichEdit);
    }
    if(m_hWndToolbar!=NULL)
    {
        SetWindowLong(m_hWndToolbar,GWL_USERDATA,0);
        DestroyWindow(m_hWndToolbar);
    }
    if(m_hWnd!=NULL)
    {
        SetWindowLong(m_hWnd,GWL_USERDATA,0);
        DestroyWindow(m_hWnd);
    }

    m_hWndRichEdit = NULL;
    m_hWndToolbar = NULL;
    m_hWnd = NULL;
}


BOOL CChatInput::Create(HWND hWndParent,
               int x,int y,
               int cx,int cy,
               UINT nID,
               DWORD dwWindowStyle,
               DWORD dwExWindowStyle,
               DWORD dwChatStyle,
               DWORD dwToolBarStyle)
{
    //
    // Don't create twice
    //
    ASSERT(m_hWnd == NULL);
    ASSERT(m_hWndRichEdit == NULL);
    ASSERT(m_hWndToolbar == NULL);

    //
    // Create this window first.
    //
    if(!CreateMainWindow(hWndParent,x,y,cx,cy,nID,dwWindowStyle,dwExWindowStyle,dwChatStyle))
        return FALSE;

    //
    // Then the toolbar window
    //
    if(!CreateToolbar(dwToolBarStyle))
        return FALSE;

    //
    // Lastly the richedit input window
    //
    if(!CreateRichEditWindow(dwChatStyle))
        return FALSE;

    // Th-th-th-th-that's all folks!
    return TRUE;
}


BOOL CChatInput::CreateMainWindow(HWND hWndParent,
                                  int x,int y,
                                  int cx,int cy,
                                  UINT nID,
                                  DWORD dwStyle,
                                  DWORD dwExStyle,
                                  DWORD dwChatStyle)
{
    //
    // Must call Initialize at least once
    //
    ASSERT(ghInst != NULL);


    m_hWnd = CreateWindowEx(dwExStyle,
                     szMainWindowClass,
                     NULL,
                     WS_CHILD | dwStyle,
                     x,y,cx,cy,
                     hWndParent,
                     (HMENU) nID,
                     ghInst,
                     NULL);

    if(m_hWnd == NULL)
        return FALSE;
    
    SetWindowLong(m_hWnd,GWL_USERDATA,(DWORD) this);
    return TRUE;
}


BOOL CChatInput::CreateToolbar(DWORD dwToolbarStyle)
{
    ASSERT(m_hWnd!=NULL);

    DWORD dwStyle = WS_CHILD | WS_VISIBLE;
    dwStyle |= TBSTYLE_TOOLTIPS | CCS_NORESIZE | CCS_NOMOVEY | CCS_NOPARENTALIGN | CCS_NODIVIDER;
    
    //
    // Must call Initialize at least once
    //
    ASSERT(ghInst != NULL);


    m_hWndToolbar = CreateWindowEx(0,
                                    TOOLBARCLASSNAME,
                                    NULL,
                                    dwStyle,
                                    0,0,
                                    0,0,
                                    m_hWnd,
                                    (HMENU) ID_TOOLBAR,
                                    ghInst,
                                    NULL);
    
    if(m_hWndToolbar==NULL)
        return FALSE;

    //
    // Send the TB_BUTTONSTRUCTSIZE message, which is required for 
    // backward compatibility.
    //
    SendMessage(m_hWndToolbar, TB_BUTTONSTRUCTSIZE,
                (WPARAM) sizeof(TBBUTTON), 0); 

    //
    // Set the bitmap size
    //
    SendMessage(m_hWndToolbar,TB_SETBITMAPSIZE,0,MAKELONG(CX_BUTTON_IMAGE,CY_BUTTON_IMAGE));

    //
    // Set the button size
    //
    SendMessage(m_hWndToolbar,TB_SETBUTTONSIZE,0,MAKELONG(CX_BUTTON_SIZE,CY_BUTTON_SIZE));

    //
    // Set the toolbar's button bitmap
    //
    TBADDBITMAP tbAddBitmap;
    tbAddBitmap.hInst = ghInst;
    tbAddBitmap.nID = IDB_CIBUTTONS;

    SendMessage(m_hWndToolbar,TB_ADDBITMAP,nButtonEntries,(LPARAM)&tbAddBitmap);

    // 
    // Now, establish the buttons
    //
    int nButtons=0;

    for(int nBtnIndex=0; nBtnIndex<nButtonEntries; nBtnIndex++)
    {
        if(dwToolbarStyle & ButtonArray[nBtnIndex].dwStyle)
        {
            nButtons++;
            AddButton(nBtnIndex);
        }
    }

    if(nButtons==0)
    {
        //
        // No buttons, just destroy this window and return
        //
        DestroyWindow(m_hWndToolbar);
        m_hWndToolbar = NULL;
        return TRUE;
    }

    //
    // Now, position the toolbar propertly
    //
    RECT rectParent;
    GetClientRect(m_hWnd,&rectParent);

    //
    // Validate the button / image sizes
    //
    ASSERT(CX_BUTTON_SIZE >= CX_BUTTON_IMAGE + 7);
    ASSERT(CY_BUTTON_SIZE >= CY_BUTTON_IMAGE + 6);

    //
    // Re-size the toolbar parent window and the toolbar itself.
    //
    int cx = nButtons * CX_BUTTON_SIZE;  
    int cy = CY_BUTTON_SIZE ;   
    int x  = RECTWIDTH(rectParent) - cx;
    int y  = (RECTHEIGHT(rectParent) - cy) / 2 - 2; // -2 because of the border

    MoveWindow(m_hWndToolbar,x,y,cx,cy + 2,TRUE);  // Win4 adds a top border of 2 to the top of the buttons

    return TRUE;
}

BOOL CChatInput::CreateRichEditWindow(DWORD dwChatStyle)
{
    ASSERT(m_hWndRichEdit==NULL);
    ASSERT(m_hWnd!=NULL);

    DWORD dwStyle = WS_CHILD | WS_VISIBLE;
    
    if(dwChatStyle & CI_STYLE_MULTILINE)
    {
        dwStyle |= ES_MULTILINE;
        dwStyle |= ES_AUTOVSCROLL;
    }
    else
    {
        dwStyle |= ES_AUTOHSCROLL;
    }

    m_hWndRichEdit = CreateWindowEx(0,
                 szRICHEDIT,
                 NULL,
                 dwStyle,
                 0,0,0,0,
                 m_hWnd,
                 (HMENU) ID_RICHEDIT,
                 ghInst,
                 NULL);

    RepositionRichEditWindow(TRUE);

    return (m_hWndRichEdit!=NULL);
}


//
// Resizes the richedit window so it fits all the space EXCEPT for 
// the toolbar. Toolbar must be repositioned first before making this
// call. 
// bFirstCall indicates whether this is being called during window creation.
// This is necessary because the toolbar window will not appear to be visible
// when it is first created and this call is made.
//
void CChatInput::RepositionRichEditWindow(BOOL bFirstCall)
{
    RECT rectParent;
    GetClientRect(m_hWnd,&rectParent);

    RECT rectToolbar = { 0,0,0,0 };

    if( (m_hWndToolbar!=NULL) && (bFirstCall || IsWindowVisible(m_hWndToolbar)) )
       GetClientRect(m_hWndToolbar,&rectToolbar);

    int x = 0;
    int y = 0;

    int cx = RECTWIDTH(rectParent) - RECTWIDTH(rectToolbar);
    int cy = RECTHEIGHT(rectParent);

    // -3 == Leave room for a border between richedit and toolbar
    if( (m_hWndToolbar!=NULL) && (bFirstCall || IsWindowVisible(m_hWndToolbar))  )
        cx -= 3;

    MoveWindow(m_hWndRichEdit,x,y,cx,cy,TRUE);
}

void CChatInput::AddButton(int nBtnIndex)
{
    SendMessage(m_hWndToolbar,TB_ADDBUTTONS,1,(LPARAM) &ButtonArray[nBtnIndex].btnInfo);
}



LRESULT CALLBACK MainWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    CChatInput *pInput = (CChatInput *)GetWindowLong(hWnd,GWL_USERDATA);

    //
    // Some messages will arrive before we have set GWL_USERDATA,
    // (e.g. WM_CREATE), so check for it before assuming it points
    // to a valid object
    //
    if(pInput == NULL)
        return DefWindowProc(hWnd,uMsg,wParam,lParam);

    LRESULT lResult = 0;

    //
    // See if they want to handle the message themselves
    //
    if(pInput->WindowProc(uMsg,wParam,lParam,&lResult))
        return lResult;

    return pInput->InternalWndProc(uMsg,wParam,lParam);
}


LRESULT CChatInput::InternalWndProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_COMMAND:
        {
            //
            // Send the notification to the parent.
            //
            SendMessage(::GetParent(m_hWnd),WM_CI_NOTIFICATION,LOWORD(wParam),0);
            return 0;
        }
        break;

    case WM_NOTIFY:
        {
            NMHDR *pnmh = (NMHDR *) lParam;

            if(pnmh->hwndFrom == m_hWndToolbar)
                return HandleToolbarNotification(pnmh);
            else if(wParam == ID_RICHEDIT)
                return HandleRichEditNotification(pnmh);
            else if((wParam >= IDC_FIRSTBUTTON) && (wParam <= IDC_LASTBUTTON))
            {
                // In the case of toolbar tooltips,
                // idFrom and hwndFrom and wParam point to the actual button
                // ID, so we have to special-case this for tooltips.
                return HandleToolbarNotification(pnmh);
            }
        }
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(m_hWnd,&ps);

            if((m_hWndRichEdit!=NULL) && (m_hWndToolbar!=NULL) && (IsWindowVisible(m_hWndToolbar)))
            {
                RECT rect;
                GetWindowRect(m_hWndRichEdit,&rect);
                ScreenToClient(m_hWnd,&rect);
                rect.right += 2;

                DrawEdge(ps.hdc,&rect,EDGE_SUNKEN,BF_RIGHT);
            }

            EndPaint(m_hWnd,&ps);
        }
        break;
    };

    return DefWindowProc(m_hWnd,uMsg,wParam,lParam);
}
    

LRESULT CChatInput::HandleToolbarNotification(NMHDR* pnmh)
{
    switch(pnmh->code)
    {
    case TTN_NEEDTEXTW:
    case TTN_NEEDTEXTA:
        {
            LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) pnmh;

            //
            // We know there are only a few buttons, so we can get away with
            // a sequential search to find the one that matches (according to 
            // the ID).
            //
            for(int nBtnIndex=0; nBtnIndex<nButtonEntries; nBtnIndex++)
            {
                if((int) lpttt->hdr.idFrom == ButtonArray[nBtnIndex].btnInfo.idCommand)
                {
                    //
                    // We found it. Return the string
                    //
                    lpttt->hinst = ghInst;
                    lpttt->lpszText = MAKEINTRESOURCE(ButtonArray[nBtnIndex].nIDString);
                    break;
                }
            }
        }
        break;
    }

    return 0;
}


LRESULT CChatInput::HandleRichEditNotification(NMHDR* pnmh)
{
    return 0;
}


BOOL CChatInput::WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam,LRESULT *pResult)
{
    return FALSE;
}


void CChatInput::ShowToolbar(BOOL bShow)
{
    if(m_hWndToolbar == NULL)
        return;

    ShowWindow(m_hWndToolbar,bShow ? SW_SHOW : SW_HIDE);

    RepositionRichEditWindow();
}

BOOL CChatInput::IsToolbarVisible()
{
    return (m_hWndToolbar!=NULL) && IsWindowVisible(m_hWndToolbar);
}

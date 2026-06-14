/*
 * FRAMER.H
 * Document Objects Framer
 * Definitions of classes, constants, etc.
 * Includes in-line implementation of CHourGlass, CWindow
 *
 * Copyright (c)1995-1996 Microsoft Corporation, All Rights Reserved
 */
//MODIFIED FOR XCCHAT by Numbers & Co.


#ifndef _FRAMER_H_
#define _FRAMER_H_

#define INC_OLE2
#include <docobj.h>


#include <tchar.h>
#ifdef UNICODE
#include <wchar.h>
#endif

extern "C"
    {
    #include <commdlg.h>
    }


//DeleteInterfaceImp calls 'delete' and NULLs the pointer
#define DeleteInterfaceImp(p)\
            {\
			ATLTRACE( "Deleteing " #p "\n" ); \
            if (NULL!=p)\
                {\
				ATLTRACE( #p " was not NULL\n" ); \
                delete p;\
                p=NULL;\
                }\
            }


//ReleaseInterface calls 'Release' and NULLs the pointer
#define ReleaseInterface(p)\
            {\
			ATLTRACE( "Releaseing " #p "\n" ); \
            IUnknown *pt=(IUnknown *)p;\
            p=NULL;\
            if (NULL!=pt)\
				{ \
				ATLTRACE( #p " was not NULL\n" ); \
                pt->Release();\
				} \
            }



class CHourglass;
class CFrame;
class CSite;
class CImpIOleInPlaceSite;


/**
 ** CFrame encapsulates a main application window
 **/

//FRAMEWIN.CPP:  Standard window procedure and AboutProc
LRESULT APIENTRY FrameWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT APIENTRY ClientWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL    APIENTRY AboutProc(HWND, UINT, WPARAM, LPARAM);

#define CBFRAMEWNDEXTRA     sizeof(LONG)
#define FRAMEWL_STRUCTURE   0

#define CBCLIENTWNDEXTRA    sizeof(LONG)
#define CLIENTWL_STRUCTURE  0

#define SZCLASSFRAME        TEXT("XCChatFrame")
#define SZCLASSCLIENT       TEXT("XCChatClient")


//FRAME.CPP:  Frame object that creates a main window

class CFrame : public IOleInPlaceFrame, public IOleCommandTarget
    {
    //Let our window procedure look in our private variables.
    friend LRESULT APIENTRY FrameWndProc(HWND, UINT, WPARAM, LPARAM);
	friend LRESULT APIENTRY ClientWndProc(HWND, UINT, WPARAM, LPARAM);
	friend CSite;
	friend CImpIOleInPlaceSite;

protected:
    HINSTANCE       m_hInst;            //Task instance
    HWND            m_hWnd;             //Window handle of the window
    ULONG           m_cRef; 

    HINSTANCE       m_hInstPrev;        //WinMain parameters
    LPTSTR          m_pszCmdLine;

    HWND            m_hWndClient;       //Client area window

    IStorage       *m_pIStorage;        //Temp file for all needs
    DWORD           m_dwIDCounter;      //For site IDs

    //Other object needs
	HWND			m_hWndObj;			//UI Active object

    class CSite    *m_pSite;            //Site holding object        
    BORDERWIDTHS    m_bwIP;             //In-place tool allocations
    BOOL            m_fInContextHelp;   //In context help mode?

    IOleInPlaceActiveObject *m_pIOleIPActiveObject;

	// ActiveX container interfaces
	IAdviseSink *m_spAdviseSink;
	IOleClientSite *m_spClientSite;
	IDataAdviseHolder *m_spDataAdviseHolder;
	IOleInPlaceSiteWindowless *m_spInPlaceSite;
	IOleAdviseHolder *m_spOleAdviseHolder;

	HMENU	 m_LasthMenu;
	HOLEMENU m_LasthOLEMenu;
	HWND	 m_LasthWndObj;

	BOOL     m_bShowMenu;
	BOOL	 m_bShowToolbar;

	BOOL	 m_bDelayedOpenNM;

protected:
    BOOL    RegisterAllClasses(void);        
    LRESULT OnCommand(HWND, WPARAM, LPARAM);		
	BOOL CreateObject( LPUNKNOWN pObj, LPSTORAGE pStg );
	void ResizeClientWindow(UINT, UINT, UINT, UINT);

public:
    CFrame(HINSTANCE, HINSTANCE, LPSTR );
    ~CFrame(void);

    inline HINSTANCE Instance(void)
        { return m_hInst; }

    inline HWND      Window(void)
        { return m_hWnd; }        

    inline HWND GUIDWindow(void)
		{
		HWND hwnd = NULL;
		if( m_pIOleIPActiveObject != NULL )
			m_pIOleIPActiveObject->GetWindow( &hwnd );

		return( hwnd );
		}

    BOOL Init( LPUNKNOWN pObj, 
			   HWND hParentWnd, 
			   IStorage *pStg,
			   int x, int y, int cx, int cy,
			   IAdviseSink *spAdviseSink,
			   IOleClientSite *spClientSite,
			   IDataAdviseHolder *spDataAdviseHolder,
			   IOleInPlaceSiteWindowless *spInPlaceSite,
			   IOleAdviseHolder *spOleAdviseHolder );

    IOleInPlaceActiveObject *GetIPActiveObject( void )
		{
		ATLTRACE( "CFrame::GetIPActiveObject" );
		return( m_pIOleIPActiveObject );
		}

	IOleInPlaceObject *GetInPlaceObject( void );
	void    Close();        
	IViewObject *GetViewObject( void );
	IOleDocumentView  *CFrame::GetDocView( void );
	void ResetSharedMenu( void );
	void RemoveSharedMenu( void );

	void ShowMenu( BOOL bShow )
		{m_bShowMenu = bShow;}

	void ShowToolbar( BOOL bShow )
		{m_bShowToolbar = bShow;}


	void DelayedOpenNM( void );

    //Shared IUnknown implementation
    STDMETHODIMP         QueryInterface(REFIID, void **);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

	//IOleInPlaceFrame implementation
    STDMETHODIMP         GetWindow(HWND *);
    STDMETHODIMP         ContextSensitiveHelp(BOOL);
    STDMETHODIMP         GetBorder(LPRECT);
    STDMETHODIMP         RequestBorderSpace(LPCBORDERWIDTHS);
    STDMETHODIMP         SetBorderSpace(LPCBORDERWIDTHS);
    STDMETHODIMP         SetActiveObject(LPOLEINPLACEACTIVEOBJECT, LPCOLESTR);
    STDMETHODIMP         InsertMenus(HMENU, LPOLEMENUGROUPWIDTHS);
    STDMETHODIMP         SetMenu(HMENU, HOLEMENU, HWND);
    STDMETHODIMP         RemoveMenus(HMENU);
    STDMETHODIMP         SetStatusText(LPCOLESTR);
    STDMETHODIMP         EnableModeless(BOOL);
    STDMETHODIMP         TranslateAccelerator(LPMSG, WORD);

	//IOleCommandTarget
    STDMETHODIMP QueryStatus( const GUID *pguidCmdGroup, 
							  ULONG cCmds, 
							  OLECMD prgCmds[], 
							  OLECMDTEXT *pCmdText);
    
    STDMETHODIMP Exec( const GUID *pguidCmdGroup, 
					   DWORD nCmdID, 
					   DWORD nCmdexecopt, 
					   VARIANTARG *pvaIn, 
					   VARIANTARG *pvaOut );            
    };


typedef CFrame *PCFrame;



/**
 ** CHourglass:  Manages the hourglass and mouse capture
 **/

class CHourglass
    {
protected:
    HWND            m_hWndCapture;      //Window with capture
    HCURSOR         m_hCur;             //Cursor held.

public:
    CHourglass(void)
        {
        m_hCur=SetCursor(LoadCursor(NULL, IDC_WAIT));
        m_hWndCapture=NULL;
        return;
        }

    CHourglass(HWND hWnd)
        {
        m_hCur=SetCursor(LoadCursor(NULL, IDC_WAIT));

        if (NULL!=hWnd)
            {
            m_hWndCapture=hWnd;
            SetCapture(hWnd);
            }

        return;
        }

    ~CHourglass(void)
        {
        if (NULL!=m_hWndCapture)
            ReleaseCapture();

        SetCursor(m_hCur);
        return;
        }
    };

typedef CHourglass *PCHourglass;


#include "site.h"

#endif //_FRAMER_H_

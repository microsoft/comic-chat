// filesend.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "filesend.h"
#include <winsock.h>	// for inet_addr, etc...
#include <sys/stat.h>	// for _stat
#include "Process.H"
#include "UserInfo.H"
#include "ChatProt.H"
#include "UI.H"
#include "cderr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CChatApp theApp;
static char fileDCCID[] = {0x01, 'D', 'C', 'C'};
extern BOOL bCanViewUnrated(BOOL = FALSE);

#define WM_STATCHANGE	(WM_USER + 1)
#define WP_CONNECTED		1
#define WP_FILESENT			2
#define WP_BYTESSENT		3
#define WP_CONNECTFAILED	4
#define WP_TIMEOUT			5

#define ACCEPT_TIMEOUT		120000
#define RECV_TIMEOUT		60000
#define POSTLIMIT			100

/////////////////////////////////////////////////////////////////////////////
// CFileProgress dialog


CFileProgress::CFileProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CFileProgress::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileProgress)
	m_bytesSent = _T("");
	m_bytesTotal = _T("");
	m_strStatus = _T("");
	m_strXferredLabel = _T("");
	//}}AFX_DATA_INIT
	m_fileTX = NULL;
}

CFileProgress::~CFileProgress() {
	if (m_fileTX) delete ((FILETXINFO *) m_fileTX);
}


void CFileProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileProgress)
	DDX_Control(pDX, IDC_FILEPROGRESS, m_fileProgress);
	DDX_Text(pDX, IDC_BYTES_SENT, m_bytesSent);
	DDX_Text(pDX, IDC_BYTES_TOTAL, m_bytesTotal);
	DDX_Text(pDX, IDC_CX_STATUS, m_strStatus);
	DDX_Text(pDX, IDC_STATIC_NXFERRED, m_strXferredLabel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileProgress, CDialog)
	//{{AFX_MSG_MAP(CFileProgress)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_STATCHANGE, OnStatChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileProgress message handlers

static CPtrArray fileSendStore;			// array of CProgressFiles yet to be reclaimed

void CleanupFileProgressStore(BOOL bIncludeShowing) {
	int upper = fileSendStore.GetUpperBound();
	for (int i = 0; i <= upper; i++) {
		CFileProgress *fprog = (CFileProgress *) fileSendStore[i];
		if (fprog && (bIncludeShowing || !fprog->m_hWnd || !fprog->IsWindowVisible()) && ((FILETXINFO *)(fprog->m_fileTX))->txThread == 0) {
			delete fprog;
			fileSendStore[i] = NULL;
			TRACE("Cleaning up 1 file progress window\n");
		}
	}
}

void AddToFileProgressStore(CFileProgress *fprog) {
	CleanupFileProgressStore(FALSE);		// don't buildup resources -- reclaim old unused ones
	fileSendStore.Add(fprog);
}

#if 0
void QuoteFileName(CString &fileName) {
	int index;
	while ((index = fileName.Find(' ')) >= 0)
		fileName.SetAt(index, '?');
}

void UnQuoteFileName(CString &fileName) {
	int index;
	while ((index = fileName.Find('?')) >= 0)
		fileName.SetAt(index, ' ');
}
#endif

BOOL FillInFilter(CFileDialog &dlg, char *buff, UINT buffSize) {
	CString filter;
	filter.LoadString(IDS_ALL_FILES);
	if (strlen(filter) < buffSize - 1) {
		strcpy(buff, filter);
		char *bptr = buff;
		while (*bptr) {
			if (*bptr == '\n') *bptr = '\0';
			bptr++;
		}
		dlg.m_ofn.lpstrFilter = buff;
		dlg.m_ofn.nFilterIndex = 1;
		return TRUE;
	} else return FALSE;
}

static int lastPort = 7011;

void CRoomInfo::ChatSendFile(CUserInfo *pui) {
	if (pui && !pui->IsDeparted()) {
		TRACE("Got to send file\n");
		CFileDialog dlg(TRUE);
		CString strTitle;
		strTitle.LoadString(IDS_TITLE_FILEDLG_SEND);
		VERIFY(ReplaceToken(strTitle, CString("%1"), pui->GetScreenName()));
		dlg.m_ofn.lpstrTitle = strTitle;
		char buff[50];
		VERIFY(FillInFilter(dlg, buff, sizeof(buff)));
		if (dlg.DoModal() == IDOK) {
			FILETXINFO *fileTX = new FILETXINFO;
			fileTX->port = lastPort++;
			TRACE("Send %s\n", dlg.GetPathName());
#if 0
			char hostname[100];
			if (gethostname(hostname, sizeof(hostname))) return;
			struct hostent *h2 = gethostbyname(hostname);
			if (!h2 || h2->h_length < 1 || !h2->h_addr_list[0]) return;
			long hostID = *((long *) h2->h_addr_list[0]);
			hostID = ntohl(hostID);
#endif
			long GetMyIP(), hostID;
			if ((hostID = GetMyIP()) == 0) return;

			struct _stat fileStat;
			strcpy(fileTX->pathName, dlg.GetPathName());
			if (_stat(fileTX->pathName, &fileStat) != 0) return;
			CString strQuotedFile = dlg.GetFileName();
			const char *szQuotedFile = strQuotedFile;
			BOOL bNeedFree = CTCPQuoteString(&szQuotedFile);
			sprintf(GetOutBuff(), "%.*s SEND %s %lu %u %lu%c", sizeof(fileDCCID) / sizeof(char), fileDCCID, 
					szQuotedFile, hostID, fileTX->port, (long)fileStat.st_size, 0x01);
			ChatSendPrivMesg(pui->GetName(), NULL /*szAnnotations*/, GetOutBuff() /*szMesg*/);
			if (bNeedFree) free((void *) szQuotedFile);

			CFileProgress *progDlg = fileTX->progDlg = new CFileProgress;
			progDlg->m_fileTX = fileTX;
			progDlg->m_bytesTotal.Format("%d", fileStat.st_size);
			progDlg->m_iBytesTotal = fileStat.st_size;
			progDlg->m_strOtherGuy = pui->GetScreenName();
			progDlg->m_strFileName = dlg.GetFileName();
			progDlg->m_strStatus.LoadString(IDS_AWAITING_ACCEPT);
			progDlg->m_strXferredLabel.LoadString(IDS_BYTES_SENT);
			progDlg->m_bSending = TRUE;
			VERIFY(ReplaceToken(progDlg->m_strStatus, CString("%1"), progDlg->m_strOtherGuy));
			VERIFY(progDlg->Create(IDD_FILE_TRANSFER));
			::SendMessage(progDlg->m_hWnd, WM_STATCHANGE, WP_BYTESSENT, 0); // sets up title & bytes sent
			progDlg->ShowWindow(SW_SHOWNORMAL);
			fileTX->progHwnd = progDlg->m_hWnd;
			fileTX->bCancel = FALSE;
			AddToFileProgressStore(progDlg);

			void _cdecl SendFileThread (void *);
			fileTX->txThread = _beginthread(SendFileThread, 0, (void *) fileTX);
		}
	}
}

void _cdecl SendFileThread (void *arg) {
	FILETXINFO *fileTX = (FILETXINFO *) arg;
	FILE *fp = NULL;
	unsigned long ltrue = 1;
	int totalSent = 0;
	DWORD acceptStart, lastPosted = 0;

	// set up socket and listen
	SOCKET sock0, sock;
	if ((sock0 = socket( AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		TRACE("Bad socket call");
		goto cleanup;
	}

	SOCKADDR_IN local_sin, acc_sin;
	ZeroMemory(&local_sin, sizeof(local_sin));
	local_sin.sin_family = AF_INET;
	local_sin.sin_port = htons(fileTX->port);
	local_sin.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock0, (struct sockaddr FAR *) &local_sin, sizeof(local_sin)) == SOCKET_ERROR) {
		TRACE("Bind failed error: %d", WSAGetLastError());
		goto cleanup;
	}

	if (listen(sock0, 1) < 0) {
		TRACE("listen error %d", WSAGetLastError());
		goto cleanup;
	}

	ioctlsocket(sock0, FIONBIO, &ltrue);  	// make socket non-blocking

	acceptStart = GetTickCount();
	while (TRUE) {
		int acc_sin_len = sizeof(acc_sin);
		sock = accept(sock0,(struct sockaddr FAR *) &acc_sin, (int FAR *) &acc_sin_len);
		if (sock == INVALID_SOCKET) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) goto cleanup;
			else if (fileTX->bCancel) goto cleanup;	// wouldblock and cancel means return
			if (abs(GetTickCount() - acceptStart) >= ACCEPT_TIMEOUT) {
				PostMessage(fileTX->progHwnd, WM_STATCHANGE, WP_TIMEOUT, 0);
				goto cleanup;
			}
			Sleep(500);  // don't chew up compute power here....
		} else break;
	}

	::PostMessage(fileTX->progHwnd, WM_STATCHANGE, WP_CONNECTED, 0);
	TRACE("Got a connection!!!\n");

	char buff[1024];
	if (!(fp = fopen(fileTX->pathName, "rb"))) {
		TRACE("File open failed");
		goto cleanup;
	}

	while (TRUE) {
		size_t nchars = fread(buff, 1, sizeof(buff), fp);
		TRACE("Reading %d\n", nchars);
		if (nchars <= 0) {
			if (feof(fp)) break;	// successful completion
			else if (ferror(fp)) {
				TRACE("Got a file read error");
				goto cleanup;
			}
		} else {
			int nsent = send(sock, buff, nchars, 0);
			TRACE("Sending %d\n", nsent);
			if (nsent > 0) ASSERT(nchars == (size_t) nsent);
			if (nsent == SOCKET_ERROR) {
				TRACE("Got an error on send");
				goto cleanup;
			}
			totalSent += nsent;
			__int32 buffInt, nAcked = 0;
			char mbuff[4];
			do {
				DWORD recvStart = GetTickCount();
				while (TRUE) {
					int n2 = recv(sock, mbuff, sizeof(mbuff), 0);
					if (n2 == -1) {
						if (WSAGetLastError() != WSAEWOULDBLOCK) goto cleanup;
						else if (fileTX->bCancel) goto cleanup;  // wouldblock && cancel means return
					} else if (n2 == sizeof(mbuff)) break;
					if (abs(GetTickCount() - recvStart) >= RECV_TIMEOUT) {
						PostMessage(fileTX->progHwnd, WM_STATCHANGE, WP_TIMEOUT, 0);
						goto cleanup;
					}
					// don't bother sleeping here.  This should hopefully be fast
				}
				buffInt = *((__int32 *) mbuff);
				nAcked = ntohl(buffInt);
				DWORD now = GetTickCount();
				if (abs(now - lastPosted) >= POSTLIMIT || nAcked >= totalSent) {
					::PostMessage(fileTX->progHwnd, WM_STATCHANGE, WP_BYTESSENT, nAcked);
					lastPosted = now;
				}
			} while (nAcked < totalSent);
		}
	}
	::PostMessage(fileTX->progHwnd, WM_STATCHANGE, WP_FILESENT, 0);
cleanup:
	if (fp) fclose(fp);
	closesocket(sock0);
	closesocket(sock);
	TRACE("Done file send");
	fileTX->txThread = 0;		// thread no longer alive
}

#define FILE_RECEIVE_DLG_LIMIT	4

void ChatReceiveFile(CUserInfo *pui, char *mesg) {
	static int iDlgCount = 0;		// poor man's critical section
	if (!theApp.m_bAllowFileTX || !bCanViewUnrated()) return;

	long fileSize;
	CString strSize;
	const char *arg = GetToken(mesg, &mesg, "");
	if (!arg || stricmp(arg, "SEND")!=0) return;	// can't handle DCC Chat yet, just ignore for now
	arg = GetToken(mesg, &mesg, "");
	if (!arg) return;
	BOOL bNeedFree = CTCPUnQuoteString(&arg);
	CString strFileName = arg;
	if (bNeedFree) free((void *)arg);
	arg = GetToken(mesg, &mesg, "");
	if (!arg) return;
	FILETXINFO *fileTX = new FILETXINFO;
	fileTX->hostAddr = atol(arg);
	arg = GetToken(mesg, &mesg, "");
	if (!arg) return;
	fileTX->port = atoi(arg);
	arg = GetToken(mesg, &mesg, "");
	if (!arg) {
		fileSize = -1;
		strSize.LoadString(IDS_FILESIZE_UNKNOWN);
	}
	else {
		fileSize = atol(arg);
		if (fileSize < 1) return;	// only accept positive file sizes!
		strSize.LoadString(IDS_FILESIZE_FORMAT);
		CString strNum;
		strNum.Format("%d", fileSize);
		VERIFY(ReplaceToken(strSize, CString("%1"), strNum));
	}

	CString caption;
	caption.LoadString(IDS_ACCEPT_FILE_MESG);
	VERIFY(ReplaceToken(caption, CString("%1"), pui->GetScreenName()));
	VERIFY(ReplaceToken(caption, CString("%2"), strFileName));
	VERIFY(ReplaceToken(caption, CString("%3"), strSize));

	if (iDlgCount++ < FILE_RECEIVE_DLG_LIMIT && AfxMessageBox(caption, MB_YESNO) == IDYES) {
		CFileDialog dlg(FALSE);
		CString strTitle;
		strTitle.LoadString(IDS_TITLE_FILEDLG_RCV);
		VERIFY(ReplaceToken(strTitle, CString("%1"), pui->GetScreenName()));
		dlg.m_ofn.lpstrTitle = strTitle;
		strncpy(dlg.m_ofn.lpstrFile, strFileName, dlg.m_ofn.nMaxFile);
		dlg.m_ofn.lpstrFile[dlg.m_ofn.nMaxFile-1] = '\0';
		dlg.m_ofn.lpstrInitialDir = theApp.m_strFileTXDir;
		char oldDir[MAX_PATH], newDir[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, oldDir);
		char buff[50];
		VERIFY(FillInFilter(dlg, buff, sizeof(buff)));
		int rval = dlg.DoModal();
		if (rval == IDCANCEL && CommDlgExtendedError() == FNERR_INVALIDFILENAME) { // bad filename can yield an error *before* file dialog is shown
			dlg.m_ofn.lpstrFile[0] = '\0';
			rval = dlg.DoModal();
		}
		if (rval == IDOK) {
			strcpy(fileTX->pathName, dlg.GetPathName());
			fileTX->fileSize = fileSize;
			void _cdecl ReceiveFileThread (void *);

			CFileProgress *progDlg = fileTX->progDlg = new CFileProgress;
			if (fileSize >= 0) progDlg->m_bytesTotal.Format("%d", fileSize);
			else progDlg->m_bytesTotal = strSize;  // "file unknown" or something similar
			progDlg->m_iBytesTotal = fileSize;
			progDlg->m_strOtherGuy = pui->GetScreenName();
			progDlg->m_strFileName = dlg.GetFileName();
			progDlg->m_strStatus.LoadString(IDS_FILE_CONNECTING);
			progDlg->m_strXferredLabel.LoadString(IDS_BYTES_RECEIVED);
			progDlg->m_fileTX = fileTX;
			progDlg->m_bSending = FALSE;
			VERIFY(ReplaceToken(progDlg->m_strStatus, CString("%1"), progDlg->m_strOtherGuy));
			VERIFY(progDlg->Create(IDD_FILE_TRANSFER));
			::SendMessage(progDlg->m_hWnd, WM_STATCHANGE, WP_BYTESSENT, 0); // sets up title & bytes sent
			if (fileSize <= 0)  // disable file progress indicator if we don't file size
				progDlg->GetDlgItem(IDC_FILEPROGRESS)->EnableWindow(FALSE);
			progDlg->ShowWindow(SW_SHOWNORMAL);
			fileTX->progHwnd = progDlg->m_hWnd;
			fileTX->bCancel = FALSE;
			AddToFileProgressStore(progDlg);
			fileTX->txThread = _beginthread(ReceiveFileThread, 0, (void *) fileTX);
			if (GetCurrentDirectory(sizeof(newDir), newDir))
				theApp.m_strFileTXDir = newDir;
			SetCurrentDirectory(oldDir);
		}
	} else delete fileTX;
	iDlgCount--;
}

void _cdecl ReceiveFileThread(void *arg) {
	FILE *fp = NULL;
	unsigned long ltrue = 1;
	int nread, totalRead = 0;
	DWORD lastPosted = 0;
	FILETXINFO *fileTX = (FILETXINFO *) arg;
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
		TRACE("socket call failed");
        goto cleanup;
    }

	SOCKADDR_IN dest_addr;
	ZeroMemory(&dest_addr, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(fileTX->port);
	dest_addr.sin_addr.S_un.S_addr = htonl(fileTX->hostAddr);

	if (connect(sock, (PSOCKADDR) &dest_addr, sizeof(dest_addr)) != 0) {
		::PostMessage(fileTX->progHwnd, WM_STATCHANGE, WP_CONNECTFAILED, 0);
		goto cleanup;
	}

	::PostMessage(fileTX->progHwnd, WM_STATCHANGE, WP_CONNECTED, 0);

	ioctlsocket(sock, FIONBIO, &ltrue);
	
	if (!(fp = fopen(fileTX->pathName, "wb"))) {
		CString mesg;
		mesg.LoadString(ID_ERR_SAVE);
		VERIFY(ReplaceToken(mesg, CString("%1"), fileTX->pathName));
		AfxMessageBox(mesg);
		goto cleanup;
	}

	char buff[1024];
	while (TRUE) {
		DWORD recvStart = GetTickCount();
		while ((nread = recv(sock, buff, sizeof(buff), 0)) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) goto cleanup;
			else if (fileTX->bCancel) goto cleanup;	// wouldblock and cancel means return
			if (abs(GetTickCount() - recvStart) >= RECV_TIMEOUT) {
				PostMessage(fileTX->progHwnd, WM_STATCHANGE, WP_TIMEOUT, 0);
				goto cleanup;
			}
		}
		if (nread > 0) {
			totalRead += nread;
			__int32 nreadBuff = htonl(totalRead);
			int nsent = send(sock, (const char *)&nreadBuff, sizeof(nreadBuff), 0);
			if (totalRead > fileTX->fileSize) nread -= (totalRead - fileTX->fileSize);
			int nwrote = fwrite(buff, 1, nread, fp);
			if (nwrote < nread) {
				AfxMessageBox("Could not completely save file.");
				goto cleanup;
			}
			DWORD now = GetTickCount();
			if (abs(now - lastPosted) >= POSTLIMIT || totalRead >= fileTX->fileSize) {
				::PostMessage(fileTX->progHwnd, WM_STATCHANGE, WP_BYTESSENT, totalRead);
				lastPosted = now;
			}
			if (totalRead >= fileTX->fileSize) break;   // our job is done
		} else goto cleanup;
	}
	TRACE("File saved!!!");
	::PostMessage(fileTX->progHwnd, WM_STATCHANGE, WP_FILESENT, 0);
cleanup:
	if (fp) fclose(fp);
	closesocket(sock);
	fileTX->txThread = 0;   // thread no longer alive
}

#define MAXPOS	100

LRESULT CFileProgress::OnStatChange(WPARAM wParam, LPARAM lParam) {
	CString mesg;
	switch (wParam) {
		case WP_CONNECTED:
			mesg.LoadString(IDS_FILE_CONNECT);
			GetDlgItem(IDC_CX_STATUS)->SetWindowText(mesg);
			break;
		case WP_FILESENT:
			mesg.LoadString(m_bSending ? IDS_FILE_SENT : IDS_FILE_RECEIVED);
			GetDlgItem(IDC_CX_STATUS)->SetWindowText(mesg);
			break;
		case WP_CONNECTFAILED:
			mesg.LoadString(IDS_CONNECTION_FAILED);
			GetDlgItem(IDC_CX_STATUS)->SetWindowText(mesg);
			break;
		case WP_TIMEOUT:
			mesg.LoadString(IDS_FILETIMEOUT);
			GetDlgItem(IDC_CX_STATUS)->SetWindowText(mesg);
			break;
		case WP_BYTESSENT:
			mesg.Format("%d", lParam);
			GetDlgItem(IDC_BYTES_SENT)->SetWindowText(mesg);
			if (m_iBytesTotal > 0) {
				double percent = (double) lParam / (double) m_iBytesTotal;
				int pos = (int)(percent * MAXPOS);
				CProgressCtrl *progCtrl = (CProgressCtrl *)GetDlgItem(IDC_FILEPROGRESS);
				progCtrl->SetPos(pos);
				mesg.LoadString(m_bSending ? IDS_FILESEND_TITLE : IDS_FILEGET_TITLE);
				CString strPercent;
				strPercent.Format("%d", (int)(percent*100));
				VERIFY(ReplaceToken(mesg, CString("%1"), strPercent));
				VERIFY(ReplaceToken(mesg, CString("%2"), m_strFileName));
				VERIFY(ReplaceToken(mesg, CString("%3"), m_strOtherGuy));
				SetWindowText(mesg);
			}
			break;
	}
	return 0;
}


void CFileProgress::OnCancel() 
{
	((FILETXINFO *)m_fileTX)->bCancel = TRUE;
	
	CDialog::OnCancel();
}

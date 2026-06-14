// bodycam.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "bbox.h"
#include "pe.h"
#include "dib.h"
#include "avatar.h"
#include "bodycam.h"

#include "saywnd.h"
#include "userinfo.h"
#include "chatprot.h"
#include "ui.h"
#include "vector2d.h"
#include <math.h>
#include "resource.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "protsupp.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


// Define a nice background color for the image which will replace the ugly color used for transparency in the world.
#define iBackgroundRed	 128
#define iBackgroundGreen 128
#define iBackgroundBlue	 128

extern CChatApp theApp;
void CreateRetainedBitmap(CDC *pDC, HBITMAP *retSec, CDIB **retDib, int width, int height);

/////////////////////////////////////////////////////////////////////////////
// CBodyCam

static CString emotionName[9];

void LoadEmotionStrings() {
	int startID = ID_EM_HAPPY; // assumption: happy is first, contiguous ids
	for (int i = 0; i < 9; i++)
		emotionName[i].LoadString(startID++);
}

UINT lg_icons[] =
{
	IDR_HAPPY,
	IDR_COY,
	IDR_BORED,
	IDR_SCARED,
	IDR_SAD,
	IDR_ANGRY,
	IDR_SHOUT,
	IDR_LAUGH,
};

#define LG_ICON_WIDTH	20
#define LG_ICON_HEIGHT	26
#define SM_ICON_WIDTH	13
#define SM_ICON_HEIGHT	18

// Little class to share icons between bodycam classes.

class CBodyCamIcons
{
public:
	CDIB* GetIcon(UINT nIndex);
protected:
	CDIB m_icons[NEMOTIONS];
};
static class CBodyCamIcons Icons;

CDIB* 
CBodyCamIcons::GetIcon(
UINT nIndex)
{
	if (m_icons[nIndex].GetBitsAddress () == NULL)
	{
		VERIFY (m_icons[nIndex].Load (lg_icons[nIndex]));
	}
	return &m_icons[nIndex];
}

IMPLEMENT_DYNCREATE(CBodyCam, CWnd)

CBodyCam::CBodyCam()
{
	m_avatar = NULL;
	m_palette = NULL;
	m_mouseDown = FALSE;
	m_forcedDelete = TRUE;		// indicates bodycam must be deleted in OnNCDestroy
	m_emotion.Set(0.0, 0.0);	// start off neutral
	m_retDib = NULL;			// allocated later by CreateRetainedBitmap
	m_retSec = NULL;
	m_lastEmotionString = NULL;
//	strcpy(m_toolTipString, "HOWDY!!!");					// ToolTip support
	VERIFY(m_toolTip.Create(this, TTS_ALWAYSTIP));
	VERIFY(m_toolTip.AddTool(this, m_toolTipString));
	EnableToolTips(TRUE);
	m_toolTip.Activate(TRUE);
	m_bEnableDblClk = TRUE;
	m_bGainedFocusImplicitly = FALSE;
	m_hwndTookFocusFrom = NULL;
}

CBodyCam::~CBodyCam()
{
	FreeRetainedPanel();
	// don't need to explicitly free icons since they aren't pointers
}


BEGIN_MESSAGE_MAP(CBodyCam, CWnd)
	//{{AFX_MSG_MAP(CBodyCam)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_BODYCONTEXT_FREEZE, OnBodycontextFreeze)
	ON_COMMAND(ID_BODYCONTEXT_SENDEXPRESSION, OnBodycontextSendexpression)
	ON_WM_SIZE()
	ON_WM_NCDESTROY()
	ON_UPDATE_COMMAND_UI(ID_BODYCONTEXT_FREEZE, OnUpdateBodycontextFreeze)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_GETDLGCODE()
	ON_WM_PALETTECHANGED()
	ON_WM_MENUSELECT()
	ON_WM_ENTERIDLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBodyCam message handlers

BOOL CBodyCam::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	dwStyle &= ~WS_BORDER;

	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void CBodyCam::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rect;
    CPen *ppenOld;
	CBrush *pbrOld;
	CPalette *oldPal;

//	double randfloat();
//	dc.FillSolidRect(0, 0, 1000, 1000, RGB((int)(randfloat()*255), (int)(randfloat()*255), (int)(randfloat()*255)));

	// Use our own palette
	if (oldPal = dc.SelectPalette(&ghPalette, TRUE))
        dc.RealizePalette();

	// figure out rectangle
	GetClientRect(&rect);

	// get background brush
	pbrOld = (CBrush *) dc.SelectStockObject(WHITE_BRUSH);
    ppenOld = (CPen *) dc.SelectStockObject(BLACK_PEN);

	DrawBullsEye(&dc, rect);
	if (!m_bullDisabled) {
		DrawBullsEyeCons(&dc, rect);

		m_cursorPos = GetPointFromEmotion(m_emotion);	// calculate point from emotion in case there was a resize
		DrawCursor(m_cursorPos, &dc);
	}

	// need to clear part of window north of emotion wheel (why isn't this done automatically?
	if (!m_avatar) 
		dc.FillSolidRect(0, 0, rect.right, rect.bottom - m_bullSide, RGB(255, 255, 255));

	// now draw body feedback
	if (m_avatar) {
		CBody *body = m_avatar->m_body;
		if (body) m_bodyRect = DrawBody(&dc, body);
//		DrawTalkTos(m_avatar, &rect);
	}

	// restore
	if (oldPal)
		dc.SelectPalette(oldPal, TRUE);
	dc.SelectObject( pbrOld );
    dc.SelectObject( ppenOld );

	if (::GetFocus () == m_hWnd && !m_bGainedFocusImplicitly)
		dc.DrawFocusRect (rect);
}

#define MAXBULL	159
#define MINBULL	93

// sets m_bullSide and m_bullDisabled
void CBodyCam::CacheBullSide(int width) inline {
	m_bullSide = min(width, MAXBULL);
	if (m_bullSide < MINBULL) {
		m_bullDisabled = TRUE;
		//m_bullSide = MINBULL;
		m_bullSide = 0;
	} else m_bullDisabled = FALSE;
}

void CBodyCam::DrawBullsEye(CDC *dc, RECT &rect) {
	int width = rect.right - rect.left;
	int halfSide = m_bullSide / 2;

	m_bullsEye.x = (rect.left + rect.right) / 2;
	m_bullsEye.y = rect.bottom - halfSide;
	m_bullRadius = halfSide - m_cursorRadius - m_iconHeight;

	if (m_bullDisabled) return;

	// fill widget background w/ gray
	dc->FillSolidRect(rect.left, rect.bottom - m_bullSide, width, m_bullSide, PALETTERGB(210, 210, 210));

	CRect circRect(m_bullsEye.x - m_bullRadius, m_bullsEye.y - m_bullRadius,
				   m_bullsEye.x + m_bullRadius, m_bullsEye.y + m_bullRadius);
	dc->Ellipse(circRect);
	DrawPoint(dc, m_bullsEye, 5);
	m_bullRadius -= m_cursorRadius;	// we do not want the cursor to leave the circle
}

RECT CBodyCam::GetIconRect(int i) const {
	RECT iconRect;
	int offsetFromEye = m_bullRadius + 2*m_cursorRadius + m_iconHeight/2;

	double angle = 2*PI*i / NEMOTIONS;
	POINT iconCenter = dpoint_to_point(point_scalmult(offsetFromEye, angle_to_vector(angle)));
	iconCenter = point_add(iconCenter, m_bullsEye);
	iconRect.top = iconCenter.y + m_iconHeight/2;
	iconRect.bottom = iconCenter.y - m_iconHeight/2;
	iconRect.left = iconCenter.x - m_iconWidth/2;
	iconRect.right = iconCenter.x + m_iconWidth/2;
	return iconRect;
}

void CBodyCam::DrawBullsEyeCons(CDC *dc, RECT &rect) {
	for (int i = 0; i < NEMOTIONS; i++) {
		RECT iconRect = GetIconRect(i);
		Icons.GetIcon (i)->Draw(dc, iconRect.left, iconRect.bottom); // iconRect is flipped to support hit-testing
	}
}


int CBodyCam::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const {
#ifdef IDEDJK  // for djk's interactive compiles
	return -1;
#endif

	// CWnd::OnToolHitTest expects the tool window to be a child of the window to
	//   which the ToolTipCtrl is attached.  Since that's not the case here, we'll
	//   override the OnToolHitTest method to return a TOOLINFO referring to the
	//   CClientView.  -djk
	pTI->hwnd = m_hWnd;
	pTI->uId = (UINT)m_hWnd;
	pTI->uFlags |= (TTF_IDISHWND|TTF_NOTBUTTON);

	if (!m_bullDisabled)
	{
		for (int i = 0; i < NEMOTIONS; i++) {
			RECT iconRect = GetIconRect(i);
			if (inside_bbox(&point, &iconRect)) {
				pTI->lpszText = strdup(emotionName[i]);
				return i;
			}
		}
	}
	return -1;		// no hit
}

CString *StringFromEmotion(CEmotion &em) {
	if (em.m_intensity == 0.0) return &(emotionName[8]);
	if (em.m_emotion >= 7*PI/8 || em.m_emotion < -7*PI/8)
		return &(emotionName[4]);
	else if (em.m_emotion <= -5*PI/8) return &(emotionName[5]);
	else if (em.m_emotion <= -3*PI/8) return &(emotionName[6]);
	else if (em.m_emotion <= -PI/8) return &(emotionName[7]);
	else if (em.m_emotion > 5*PI/8) return &(emotionName[3]);
	else if (em.m_emotion > 3*PI/8) return &(emotionName[2]);
	else if (em.m_emotion > PI/8) return &(emotionName[1]);
	else return &(emotionName[0]);
}


void CBodyCam::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetClientRect (&rect);

	HWND hwndFocus = ::GetFocus ();
	if (hwndFocus != m_hWnd)
	{
		m_bGainedFocusImplicitly = TRUE;
		m_hwndTookFocusFrom = hwndFocus;
	}
	else
		m_bGainedFocusImplicitly = FALSE;
    SetFocus();
	if (m_bullDisabled || point.y < rect.bottom - m_bullSide) return;

	m_mouseDown = TRUE;
	SetCapture();		// continue to receive messages if mouse moves outside window
	CEmotion emotion = GetEmotionFromPoint(point);
	UpdateEmotion(emotion);

	if(m_avatar) // Make sure this didnt get NULLed for some reason
	{
		// Temporarily freeze bodycam
		if (m_avatar->m_freeze == AF_UNFROZEN) {
			m_avatar->m_freeze = AF_TEMPFROZEN;
	//		av->m_body->m_requested = TRUE;		// any way to get rid of two reps of this data?
		}
	}
}

void CBodyCam::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// Make sure it's okay to bring up the options dialog before we try to do so.
	if (m_bEnableDblClk && (!currentRoom || currentRoom->GetConnectionStatus() != CX_CONNECTING))
	{
		CRect rect;
		GetClientRect (&rect);
		if (m_bullDisabled || point.y < rect.bottom - m_bullSide) 
		{
			theApp.DoOptionsDialog (TRUE, IDD_CHARACTERPAGE);
		}
	}
}

void CBodyCam::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_mouseDown) {
		CEmotion emotion = GetEmotionFromPoint(point);
		UpdateEmotion(emotion);
	}
}

void CBodyCam::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_mouseDown = FALSE;
	ReleaseCapture();
	if (GetChatDoc())
	{
		if (CX_INCHANNEL == GetChatDoc()->GetConnectionStatus())
			GetChatDoc()->ResetStatus();
		else
			GetDefaultProto()->UpdateStatus();
		if (m_bGainedFocusImplicitly && ::GetFocus () == m_hWnd && 
				(m_hwndTookFocusFrom == NULL || IsWindow (m_hwndTookFocusFrom)))
			::SetFocus (m_hwndTookFocusFrom);
		m_bGainedFocusImplicitly = FALSE;
		m_hwndTookFocusFrom = NULL;
	}
	else
		GetDefaultProto()->UpdateStatus();	// must still reset status if no doc
}


void CBodyCam::DrawCursor(POINT &point, CDC *dc) {
	CRect cursor(point.x - m_cursorRadius, point.y + m_cursorRadius, point.x + m_cursorRadius, point.y - m_cursorRadius);
	CDC *newDC = NULL;
	if (!dc)
		newDC = dc = new CClientDC(this);

	int oldROP = dc->SetROP2(R2_XORPEN);
	dc->Ellipse(cursor);
	dc->SetROP2(oldROP);
	if (newDC) delete newDC;
}


CEmotion CBodyCam::GetEmotionFromPoint(POINT &point) {
	CEmotion emotion;

	POINT vec = point_sub(point, m_bullsEye);
	emotion.m_intensity = (float)point_magn(vec) / m_bullRadius;
	emotion.m_intensity = (float) min(emotion.m_intensity, 1.0);
	if (emotion.m_intensity < 0.2) emotion.m_intensity = (float)0.0;		// create a detente in the center
	emotion.m_emotion = (emotion.m_intensity == 0) ? 0 : (float) vector_to_angle(vec);

	return emotion;
}

POINT CBodyCam::GetPointFromEmotion(CEmotion &emotion) {
	DPOINT dvec = angle_to_vector(emotion.m_emotion);
	dvec = point_scalmult(m_bullRadius * emotion.m_intensity, dvec);
	POINT vec = dpoint_to_point(dvec);
	vec = point_add(m_bullsEye, vec);
	return vec;
}

void CBodyCam::UpdateEmotion(CEmotion &emotion) {
	POINT newPos = GetPointFromEmotion(emotion);
	if (newPos.x != m_cursorPos.x || newPos.y != m_cursorPos.y/* || !m_body*/) {  // !m_body forces a body change for initialization
		DrawCursor(m_cursorPos);		// erase old point
		DrawCursor(newPos);				// draw new point
		m_cursorPos = newPos;
		m_emotion = emotion;
		if (m_avatar) m_avatar->UpdateBody(m_avatar->GetBodyFromEmotion(emotion));
		if (m_mouseDown) {								// update status bar only if mousedown
			CString *em_str = StringFromEmotion(emotion);
			if (em_str != m_lastEmotionString) {		// update string only if necessary
				CString strEmotion;
				strEmotion.LoadString(ID_EMOTION_IS);
				VERIFY(ReplaceToken(strEmotion, CString("%1"), *em_str));
				ASSERT(AfxGetApp());
				((CChatApp*)AfxGetApp())->SetStatusPaneString(0,strEmotion);
				m_lastEmotionString = em_str;
			}
		}
	}
}



// Stub so that code not knowing details of bodycam can still set the emotion
void UpdateEmotion(CEmotion &emotion) {
	GetBodyCam()->UpdateEmotion(emotion);
}

void CBodyDouble::FlipBodyBox(RECT &fullRect, RECT &headRect, RECT &torsoRect) {
	int headWidth = headRect.right - headRect.left;
	int torsoWidth = torsoRect.right - torsoRect.left;

	headRect.left = fullRect.right - (headRect.left - fullRect.left);
	headRect.right = headRect.left - headWidth;
	
	torsoRect.left = fullRect.right - (torsoRect.left - fullRect.left);
	torsoRect.right = torsoRect.left - torsoWidth;
}

void CBodyCam::GetBodyRect(RECT &rect) inline {
	GetClientRect(&rect);
	rect.bottom -= m_bullSide;				   // subtract out bullseye
}

RECT CBodyCam::DrawBody(CDC *dc, CBody *body) {
	// calculate region for body draw
	RECT rect, rect2, brect;
	GetBodyRect(rect);

	if (rect.bottom >= rect.top) {		// if space shrinks to nothing or inverts, don't draw
		rect.bottom -= rect.top;
		int oldTop = rect.top;
		rect.top = 0;
		rect2 = rect;
		rect2.bottom -= rect2.top;
		rect2.top = 0;
		rect2.left -= 1000;		// so character remains a fixed height, clipped to width
		rect2.right += 1000;

		// set up offscreen bitmap
		CDC memDC;
		VERIFY(memDC.CreateCompatibleDC(dc));
		// RamuM
		CPalette *oldPal;
		CPalette *curPal = dc->GetCurrentPalette();

		if (oldPal = memDC.SelectPalette(curPal, TRUE))
			memDC.RealizePalette();

		POINT point;
		GetBrushOrgEx(dc->GetSafeHdc(),&point);
		int iOldMode = memDC.SetStretchBltMode(STRETCHMODE); //COLORONCOLOR);
		SetBrushOrgEx(memDC.GetSafeHdc(),point.x,point.y,&point);

		CBitmap temp;
		CBitmap *retCBit = temp.FromHandle(m_retSec);
		CBitmap *bmpOld = memDC.SelectObject(retCBit); // must use a CBitmap

		memDC.FillSolidRect(&m_bodyRect, RGB(255, 255, 255));
		brect = body->DrawBody(&memDC, rect2, FALSE);   // note: is the clippath set up right from compatible dc?
		VERIFY(dc->BitBlt(rect.left, oldTop, rect.right, rect.bottom, &memDC, 0, 0, SRCCOPY));
		
		GetBrushOrgEx(memDC.GetSafeHdc(),&point);
		memDC.SetStretchBltMode(iOldMode);
		SetBrushOrgEx(memDC.GetSafeHdc(),point.x,point.y,&point);

		memDC.SelectObject(bmpOld);		// cleanup
		if (oldPal) memDC.SelectPalette(oldPal,TRUE);
	} else {
		brect.left = brect.right = brect.top = brect.bottom = 0;
	}

	return brect;
}


RECT CBodyDouble::DrawBody(CDC *dc, RECT &clientRect, BOOL drawNimbus) {
	RECT fullRect, headRect, torsoRect;
	
	CAvatarComplex *av = (CAvatarComplex*)GetAvatar(m_avatarID);
	int flags = av->m_flags;

	CPose* headPose;
	CPose* torsoPose;

	if (!av->GetPosesFromIDs (m_faceRec->poseID, m_torsoRec->poseID, &headPose, &torsoPose) ||
			headPose == NULL || torsoPose == NULL) {
		SetRectEmpty (&fullRect);
		return fullRect;
	}
	GetBodyBox(headPose, torsoPose, clientRect, fullRect, headRect, torsoRect);
	if (m_flip) FlipBodyBox(fullRect, headRect, torsoRect);

	if (drawNimbus) {
		if (torsoPose->GetAura ())
			torsoPose->GetAura ()->Draw(dc, torsoRect.left, torsoRect.top,
									torsoRect.right - torsoRect.left, torsoRect.bottom - torsoRect.top,
									MERGEPAINT);
		if (headPose->GetAura ())
			headPose->GetAura ()->Draw(dc, headRect.left, headRect.top,
								   headRect.right - headRect.left, headRect.bottom - headRect.top,
								   MERGEPAINT);
	}

	if (flags & TORSOFIRST) {
		if ((flags & TORSOMASK) && torsoPose->GetMask ())
			torsoPose->GetMask ()->Draw(dc, torsoRect.left, torsoRect.top,
									torsoRect.right - torsoRect.left, torsoRect.bottom - torsoRect.top,
									MERGEPAINT);
		if (torsoPose->GetDrawing ())
			torsoPose->GetDrawing ()->Draw(dc, torsoRect.left, torsoRect.top,
								   torsoRect.right - torsoRect.left, torsoRect.bottom - torsoRect.top,
								   SRCAND);
	}

	if ((flags & HEADMASK) && headPose->GetMask ())
		headPose->GetMask ()->Draw(dc, headRect.left, headRect.top,
							   headRect.right - headRect.left, headRect.bottom - headRect.top,
						       MERGEPAINT);
	if (headPose->GetDrawing ())
		headPose->GetDrawing ()->Draw(dc, headRect.left, headRect.top,
	     					   headRect.right - headRect.left, headRect.bottom - headRect.top,
		        			   SRCAND);

	if (!(flags & TORSOFIRST)) {
		if ((flags & TORSOMASK) && torsoPose->GetMask ())
			torsoPose->GetMask ()->Draw(dc, torsoRect.left, torsoRect.top,
									torsoRect.right - torsoRect.left, torsoRect.bottom - torsoRect.top,
									MERGEPAINT);
		if (torsoPose->GetDrawing ())
			torsoPose->GetDrawing ()->Draw(dc, torsoRect.left, torsoRect.top,
								   torsoRect.right - torsoRect.left, torsoRect.bottom - torsoRect.top,
								   SRCAND);
	}

	return fullRect;
}

void CBodyDouble::Draw(CDC *dc, POINT *ul, RECT *dmgRect) {
	// for now, ignore ul and dmgRect
	DrawBody(dc, SRECTToRECT(m_bbox), TRUE);
}

void CBodySingle::FlipBodyBox(RECT &fullBox) {
	int temp = fullBox.left;
	fullBox.left = fullBox.right;
	fullBox.right = temp;
}

RECT CBodySingle::DrawBody(CDC *dc, RECT &clientRect, BOOL drawNimbus) {
	RECT fullRect;

	CAvatarSimple *av = (CAvatarSimple*)GetAvatar(m_avatarID);

	CPose *pose;
	if (!av->GetPoseFromID(GetPoseID(), &pose) || pose == NULL) {
		SetRectEmpty (&fullRect);
		return fullRect;
	}
	GetBodyBox(pose, clientRect, fullRect);
	if (m_flip) FlipBodyBox(fullRect);

	if (drawNimbus && pose->GetAura ())
		pose->GetAura ()->Draw(dc, fullRect.left, fullRect.top,
						   fullRect.right - fullRect.left, fullRect.bottom - fullRect.top,
						   MERGEPAINT);
	if (pose->GetDrawing ())
		pose->GetDrawing ()->Draw(dc, fullRect.left, fullRect.top,
					   fullRect.right - fullRect.left, fullRect.bottom - fullRect.top,
					   SRCAND);

	return fullRect;
}

void CBodySingle::Draw(CDC *dc, POINT *ul, RECT *dmgRect) {
	DrawBody(dc, SRECTToRECT(m_bbox), TRUE);
}


void CBodyDouble::GetBodyBox(CPose *headPose, CPose *torsoPose, RECT &clientRect, RECT &fullRect, RECT &headRect, RECT &torsoRect) {

	int xOffset = m_torsoRec->xCX + m_faceRec->delta_xCX - m_faceRec->xCX;
	int yOffset = m_torsoRec->yCX + m_faceRec->delta_yCX - m_faceRec->yCX;

	RECT bitRect;
	bitRect.left = min(0, xOffset);
	bitRect.right = max(torsoPose->GetDrawing ()->GetWidth(), xOffset + headPose->GetDrawing ()->GetWidth());
	bitRect.top = min(0, yOffset);
	bitRect.bottom = max(torsoPose->GetDrawing ()->GetHeight(), yOffset + headPose->GetDrawing ()->GetHeight());


	int bitWidth = bitRect.right - bitRect.left;
	int bitHeight = bitRect.bottom - bitRect.top;
	int heightSign = (clientRect.bottom > clientRect.top) ? 1 : -1;  // heightSign < 0 if MM_TWIPS
	int clientWidth = clientRect.right - clientRect.left;
	int clientHeight = heightSign * (clientRect.bottom - clientRect.top);

	double widthScale = (double)clientWidth / bitWidth;
	double heightScale = (double)clientHeight / bitHeight;
	double scale = min(widthScale, heightScale);

	int fullHeight = ROUND(scale * bitHeight);
	int fullWidth = ROUND(scale * bitWidth);

	fullRect.left = clientRect.left + (clientWidth - fullWidth) / 2;
	fullRect.top = clientRect.top + (clientHeight - fullHeight);       // centered on bottom
	fullRect.bottom = fullRect.top + heightSign * fullHeight;
	fullRect.right = fullRect.left + fullWidth;

	headRect.left = ROUND((xOffset - bitRect.left) * scale) + fullRect.left;
	headRect.right = headRect.left + ROUND(headPose->GetDrawing ()->GetWidth() * scale) + 1;
	headRect.top = ROUND((yOffset - bitRect.top) * scale) + fullRect.top;
	headRect.bottom = headRect.top + heightSign * (ROUND(headPose->GetDrawing ()->GetHeight() * scale) + 1);

	torsoRect.left = ROUND((0 - bitRect.left) * scale) + fullRect.left;
	torsoRect.right = torsoRect.left + ROUND(torsoPose->GetDrawing ()->GetWidth() * scale) + 1;
	torsoRect.top = ROUND((0 - bitRect.top) * scale) * heightSign + fullRect.top;
	torsoRect.bottom = torsoRect.top + heightSign * (ROUND(torsoPose->GetDrawing ()->GetHeight() * scale) + 1);
}

void CBodySingle::GetBodyBox(CPose *pose, RECT &clientRect, RECT &fullRect) {
	int bitWidth = pose->GetDrawing ()->GetWidth();
	int bitHeight = pose->GetDrawing ()->GetHeight();
	int heightSign = (clientRect.bottom > clientRect.top) ? 1 : -1; // heightSign < 0 if MM_TWIPS
	int clientWidth = clientRect.right - clientRect.left;
	int clientHeight = heightSign * (clientRect.bottom - clientRect.top);

	double widthScale = (double)clientWidth / bitWidth;
	double heightScale = (double)clientHeight / bitHeight;

	int fullHeight, fullWidth;
	if (widthScale <= heightScale) {
		fullWidth = clientWidth;
		fullHeight = (int) (widthScale * bitHeight);
	} else {
		fullHeight = clientHeight;
		fullWidth = (int) (heightScale * bitWidth);
	}

	fullRect.left = clientRect.left + (clientWidth - fullWidth) / 2;
	fullRect.top = clientRect.top + (clientHeight - fullHeight);
	fullRect.bottom = fullRect.top + heightSign * fullHeight;
	fullRect.right = fullRect.left + fullWidth;
}

BOOL CBodyDouble::IsSame(CBody *other) {
	if (!other || GetClass() != other->GetClass()) return FALSE;
	CBodyDouble *b = (CBodyDouble *) other;	// OK, since they are of the same class
	return (m_faceRec == b->m_faceRec && m_torsoRec == b->m_torsoRec);
}

BOOL CBodySingle::IsSame(CBody *other) {
	if (!other || GetClass() != other->GetClass()) return FALSE;
	CBodySingle *b = (CBodySingle *) other;	// OK, since they are of the same class
	return (GetPoseID() == b->GetPoseID());
}


void CBodyCam::EraseRect(CDC *dc, RECT *rect) {
	dc->FillSolidRect(rect, RGB(255, 255, 255));
}

short CBodyCam::m_cursorRadius = 5;
short CBodyCam::m_iconWidth = LG_ICON_WIDTH;
short CBodyCam::m_iconHeight = LG_ICON_HEIGHT;


extern int testbody;

void CBodyCam::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar != VK_TAB)
		ForwardToSayWnd(nChar);
	else if (nChar == VK_TAB && !m_bGainedFocusImplicitly && m_forcedDelete)
		GetChatDoc ()->CycleFocus (CHATFOCUS_EMOTIONWND, GetKeyState(VK_SHIFT) & 0x8000);
}

void CBodyCam::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_mouseDown || m_bullDisabled)
		return;

	BOOL bCtrl = (GetKeyState (VK_CONTROL) & 0x8000) != 0;
	switch (nChar)
	{
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
		case VK_HOME:
			break;
		default:
			// These keys are not handled.
			return;
	}

	CRect rectBull;
	GetClientRect (&rectBull);
	rectBull.top = rectBull.bottom - m_bullSide;

	if (!bCtrl && nChar != VK_HOME)
	{
		static float fMagnitudes[3];	// We use floats, because CEmotion does too
		static float fAngles[8];
		static BOOL bMagnitudesInitialized = FALSE;
		static BOOL bAnglesInitialized = FALSE;

		// LEFT/RIGHT cycle through the 8 preset angles. UP/DOWN go from 0 to 50 to 100
		// percent on the intensity scale. This is all done with lookup tables, and
		// a general snap-to algorithm that uses interval comparisons to figure out what value
		// to set. Here, we make sure the right lookup array is initialized, and set up 
		// variables for lookup.
		CEmotion emotion = m_emotion;
		float * pCurr;	
		float * pCompare;
		int nCompareCount;
		BOOL bDecrease = FALSE;
		BOOL bCircular;
		switch (nChar)
		{
			case VK_DOWN:
				bDecrease = TRUE;
				bCircular = FALSE;
				// FALLTHRU
			case VK_UP:
				if (!bMagnitudesInitialized)
				{
					fMagnitudes[0] = 0;
					for (int i = 1; i < _countof(fMagnitudes); i++)
						fMagnitudes[i] = i * 0.5f;
					bMagnitudesInitialized = TRUE;
				}
				pCurr = &emotion.m_intensity;
				pCompare = fMagnitudes;
				nCompareCount = _countof(fMagnitudes);
				bCircular = FALSE;	// doesn't cycle through values
				break;
			case VK_LEFT:
				bDecrease = TRUE;
				// FALLTHRU
			case VK_RIGHT:
				if (!bAnglesInitialized)
				{
					for (int i = 0; i < _countof(fAngles); i++)
						fAngles[i] = (float)i * (PI / 4) - PI;
					bAnglesInitialized = TRUE;
				}
				pCurr = &emotion.m_emotion;
				pCompare = fAngles;
				nCompareCount = _countof(fAngles);
				bCircular = TRUE; // does cycle through values
				break;
			default:
				return;
		}

		// This snap-to algorithm relies on the fact that the numbers in the snap
		// array are in increasing order. It goes through them finding which interval
		// the number is in, and then snaps the number up or down, depending on
		// bDecrease.
		int iGoTo;
		if (bCircular)
			iGoTo = bDecrease ? nCompareCount - 1 : 1;
		else
			iGoTo = bDecrease ? nCompareCount - 2 : nCompareCount;
		for (int i = 0; i < nCompareCount; i++)
		{
			if (*pCurr < pCompare[i])
			{
				iGoTo = bDecrease ? i - 1 : i;
				break;
			}
			else if (*pCurr == pCompare[i])
			{
				iGoTo = bDecrease ? i - 1 : i + 1;
				break;
			}
		}
		
		if (iGoTo == -1)
			iGoTo = bCircular ? nCompareCount - 1 : 0;
		else if (iGoTo == nCompareCount)
			iGoTo = bCircular ? 0 : nCompareCount - 1;

		*pCurr = pCompare[iGoTo];

		UpdateEmotion (emotion);
	}
	else
	{
		BOOL bShift = (GetKeyState (VK_SHIFT) & 0x8000) != 0;
		POINT ptOld, ptCur;
		ptOld = ptCur = GetPointFromEmotion (m_emotion);
		int nStepAmount = (ptCur.x == m_bullsEye.x && ptCur.y == m_bullsEye.y) ? (m_bullRadius + 4) / 5 : 1;
		switch (nChar)
		{
			case VK_UP:
				ptCur.y = bShift ? rectBull.top : ptCur.y - nStepAmount;
				break;
			case VK_LEFT:
				ptCur.x = bShift ? rectBull.left : ptCur.x - nStepAmount;
				break;
			case VK_DOWN:
				ptCur.y = bShift ? rectBull.bottom : ptCur.y + nStepAmount;
				break;
			case VK_RIGHT:
				ptCur.x = bShift ? rectBull.right : ptCur.x + nStepAmount;
				break;
			case VK_HOME:
				ptCur = m_bullsEye;
				break;
		}
		if (ptOld.x != ptCur.x || ptOld.y != ptCur.y)
		{
			UpdateEmotion (GetEmotionFromPoint (ptCur));
		}
	}
}


void RefreshBodyCam(CAvatarX *av = NULL) {
	CBodyCam *bcam = GetBodyCam();
	if (av) bcam->m_avatar = av;
	if (!theApp.m_bNoRefresh)
		bcam->RefreshBody();
}

// When MyAvatarID() == 0, bodycam logically should be detached.
void DetachBodyCamAvatar() {
	CBodyCam *bcam = GetBodyCam();
	if (bcam)
		GetBodyCam()->m_avatar = NULL;
}

BOOL RefreshBodyPreview(CAvatarX *av) {
	CBodyCam *bcam = GetCharSelBodyCam();
	if (bcam && av == bcam->m_avatar) {
		bcam->RefreshBody();
		return TRUE;
	}
	return FALSE;
}

void CBodyCam::RefreshBody() {
	CClientDC dc(this);
	CPalette *oldPal = dc.SelectPalette(&ghPalette, TRUE);

	if (::GetFocus () == m_hWnd && !m_bGainedFocusImplicitly)
	{
		CRect rectClient, rectBody;
		GetClientRect (&rectClient);
		rectClient.InflateRect (-1, -1);
		dc.IntersectClipRect (rectClient);
	}

	m_bodyRect = DrawBody(&dc, m_avatar->m_body);

	if (oldPal) dc.SelectPalette(oldPal, TRUE);
}


CPalette *CBodyCam::InstallPalette() 
{
	m_palette = &ghPalette;
	return (m_palette);
}

#define DIBSEC

void
GetOptimalDibSectionInfo(
CDC*		 pDC,
BITMAPINFO * pBMI)
{
	static struct
	{
		BITMAPINFOHEADER bmih;
		DWORD dwBitFields[3];
	} bmiSaved;
	static BOOL bOptimalFound = FALSE;

	if (!bOptimalFound)
	{
		ZeroMemory (&bmiSaved.bmih, sizeof(bmiSaved.bmih));
		CBitmap bm;
		bm.CreateCompatibleBitmap (pDC, 1, 1);
		bmiSaved.bmih.biSize = sizeof(BITMAPINFOHEADER);
		bmiSaved.bmih.biBitCount = 0; // So it will return just the header.
		::GetDIBits (pDC->m_hDC, (HBITMAP)bm, 0, 1, 
			NULL, (BITMAPINFO*)&bmiSaved, DIB_RGB_COLORS);
	
		if (bmiSaved.bmih.biCompression == BI_BITFIELDS)
		{
			// Get the bitfield data.
			::GetDIBits (pDC->m_hDC, (HBITMAP)bm, 0, 1, 
				NULL, (BITMAPINFO*)&bmiSaved, DIB_RGB_COLORS);
		}

		bOptimalFound = TRUE;
	}

	memcpy (pBMI, &bmiSaved, sizeof(bmiSaved));
	return;
}

void 
CreateRetainedBitmap(
CDC *pDC, 
HBITMAP *retSec, 
CDIB **retDib, 
int width, 
int height) 
{
   #ifdef DIBSEC
	BYTE *pBits = NULL;
	BITMAPINFO *b = (BITMAPINFO *) malloc (sizeof(BITMAPINFOHEADER)
											   + 256*sizeof(RGBQUAD));
	BITMAPINFOHEADER *infoHdr = &(b->bmiHeader);
	ZeroMemory (infoHdr, sizeof(*infoHdr));
	infoHdr->biSize = sizeof(BITMAPINFOHEADER);

	BOOL bPrinting = pDC->IsPrinting ();
	int nBitDepth = pDC->GetDeviceCaps (BITSPIXEL);
	if (!bPrinting && (pDC->GetDeviceCaps (RASTERCAPS) & RC_PALETTE) != 0)
	{
		// Palette based device - create 8 bit DIB section and draw to it.

		infoHdr->biWidth = width;
		infoHdr->biHeight = height;
		infoHdr->biPlanes = 1;
		infoHdr->biBitCount = 8;
		infoHdr->biCompression = BI_RGB;
		infoHdr->biSizeImage = 0;
		infoHdr->biXPelsPerMeter = 0;
		infoHdr->biYPelsPerMeter = 0;
		infoHdr->biClrUsed = gpLogPal->palNumEntries ; // 256;
		infoHdr->biClrImportant = gpLogPal->palNumEntries ; // 256;
		RGBQUAD *rgbq = (LPRGBQUAD)((BYTE*)b + sizeof(BITMAPINFOHEADER));
		for (int i = 0; i < gpLogPal->palNumEntries; i++) 
		{
			rgbq->rgbRed = gpLogPal->palPalEntry[i].peRed;
			rgbq->rgbBlue = gpLogPal->palPalEntry[i].peBlue;
			rgbq->rgbGreen = gpLogPal->palPalEntry[i].peGreen;
			rgbq->rgbReserved = 0; //gpLogPal->palPalEntry[i].peFlags;
			rgbq++;
		}
	}
	else if (bPrinting || nBitDepth == 24 || nBitDepth == 1)
	{
		// Full 24-bit device or monochrome, take advantage of it.
		infoHdr->biWidth = width;
		infoHdr->biHeight = height;
		infoHdr->biPlanes = 1;
		infoHdr->biBitCount = bPrinting ? 24 : nBitDepth;
		infoHdr->biCompression = BI_RGB;
	}
	else
	{
		// Somewhere in the merry land between 8 and 24. Determine optimal
		// configuration of DIB section and use it to create one.

		GetOptimalDibSectionInfo (pDC, b);
		infoHdr->biWidth = width;
		infoHdr->biHeight = height;
	}
	*retSec = CreateDIBSection(pDC->GetSafeHdc(), b,
							   DIB_RGB_COLORS, (VOID **) &pBits,
							   NULL, 0);
	ASSERT(*retSec);
	ASSERT(pBits);
	*retDib = new CDIB;
	VERIFY((*retDib)->Create(b, pBits));
	free(b);
   #else
	*retSec = CreateCompatibleBitmap(pDC->GetSafeHdc(),infoHdr->biWidth, infoHdr->biHeight );
   #endif
}


void CBodyCam::OnContextMenu(CWnd* pWnd, CPoint screenPoint) 
{
	if (!m_forcedDelete) return;   // ie, embedded in PropertyPage, so no context menu

	CMenu menu;
	void UpdateFreezeUI(CMenu &);
	if(screenPoint.x == -1 && screenPoint.y == -1)  // then we must be invoking menu with Shift+F10
	{
		CRect rect; // so lets set up our own point
		GetClientRect(&rect);
		screenPoint.x = (rect.right - rect.left)/2;
		screenPoint.y = (rect.bottom - rect.top)/2;
		ClientToScreen(&screenPoint);
	}	
	menu.LoadMenu(IDR_BODYCONTEXT);
	UpdateFreezeUI(menu);
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
									   screenPoint.x, screenPoint.y, this);
	
}

// now toggles
void CBodyCam::OnBodycontextFreeze() 
{
	if(m_avatar)
	{
		if (m_avatar->m_freeze == AF_FROZEN)
			m_avatar->m_freeze = AF_UNFROZEN;
		else m_avatar->m_freeze = AF_FROZEN;
	//	av->m_body->m_requested = TRUE;
	}
	
}

void UpdateFreezeUI(CMenu &menu) {
	if(GetBodyCam()->m_avatar)  // check for NULLed pointer
	{
		BOOL checked = (GetBodyCam()->m_avatar->m_freeze == AF_FROZEN);
		int flag = checked ? MF_CHECKED : MF_UNCHECKED;
		menu.CheckMenuItem(ID_BODYCONTEXT_FREEZE, flag | MF_BYCOMMAND);
	}
}


void CBodyCam::OnBodycontextSendexpression() 
{
	BOOL bLegalToSend(BOOL = FALSE);
	if (!bLegalToSend()) return;

	bChatSendText(CString("<Chr>"), BM_SAY);	
}

void CBodyCam::OnSize(UINT nType, int cx, int cy) 
{
	CacheBullSide(cx);							// necessary for accurate RecalcRetainedBMP
	RecalcRetainedBMP();

	m_bodyRect.left = m_bodyRect.top = 0;		// initially will fill entire canvas w/ white
	m_bodyRect.right = cx;
	m_bodyRect.bottom = cy;

	CWnd::OnSize(nType, cx, cy);
}

void CBodyCam::RecalcRetainedBMP() {		
	if (m_retSec) {				// cleanup old versions
		FreeRetainedPanel();
	}
	
	RECT r;
	GetBodyRect(r);
	int height = r.bottom - r.top;
	int width = r.right - r.left;
	if (width <= 0) width = 1;
	if (height <= 0) height = 1;

	CClientDC dc(this);
	CreateRetainedBitmap(&dc, &m_retSec, &m_retDib, width, height);
}

void CBodyCam::OnNcDestroy() 
{
	CWnd::OnNcDestroy();
	
	// for some reason, the main bodycam window is not destroyed automatically (why?).
	//    hence we check if it's the main window (m_forcedDelete = TRUE), and
	//	  delete it here.
	if (m_forcedDelete) {
//		cui.m_pvBodyCamWnd = NULL;		// clear out bodycam pointer
		delete this;	// Otherwise destructor not called. Why is main bodycam special?
	}
}

void CBodyCam::OnUpdateBodycontextFreeze(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	TRACE("CALLED BODYCONTEXT UPDATE\n");
	pCmdUI->SetCheck(TRUE);
}
extern HWND hgPrevFocus;
void CBodyCam::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);

	hgPrevFocus = m_hWnd;

	if (!m_bGainedFocusImplicitly)
	{
		CRect rect;
		GetClientRect (&rect);
		CClientDC dc (this);
		dc.DrawFocusRect (&rect);
	}
}

void CBodyCam::OnSetFocus(CWnd* pNewWnd) 
{
	CWnd::OnSetFocus(pNewWnd);
	if (!m_bGainedFocusImplicitly)
	{
		CRect rect;
		GetClientRect (&rect);
		CClientDC dc (this);
		dc.DrawFocusRect (&rect);
	}
}

UINT CBodyCam::OnGetDlgCode()
{
	return m_forcedDelete ? DLGC_WANTALLKEYS : DLGC_WANTARROWS;
}

void 
CBodyCam::OnPaletteChanged(
CWnd* pFocusWnd)
{
	if (pFocusWnd != this)
		Invalidate ();
}

void 
CBodyCam::OnMenuSelect(
UINT nItemID, 
UINT nFlags, 
HMENU hSysMenu)
{
	// Let the main window deal with it, so we get status bar help.
	::AfxGetMainWnd ()->SendMessage (WM_MENUSELECT, (WPARAM)MAKELONG(nItemID, nFlags), (LPARAM)hSysMenu); 
}
	
void 
CBodyCam::OnEnterIdle(
UINT nWhy, 
CWnd* pWho)
{
	CWnd::OnEnterIdle (nWhy, pWho);
	// Let the main window deal with it, so we get status bar help.
	::AfxGetMainWnd ()->SendMessage (WM_ENTERIDLE, nWhy, (LPARAM)pWho->GetSafeHwnd ());
}

// AvatarFilerDoc.cpp : implementation of the CAvatarFilerDoc class
//

#include "stdafx.h"
#include "AvatarFiler.h"


#include "pseudoAvatar.h"

#include "AvatarFilerDoc.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int StringToEmType(const char *);

/////////////////////////////////////////////////////////////////////////////
// CAvatarFilerDoc

IMPLEMENT_DYNCREATE(CAvatarFilerDoc, CDocument)

BEGIN_MESSAGE_MAP(CAvatarFilerDoc, CDocument)
	//{{AFX_MSG_MAP(CAvatarFilerDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAvatarFilerDoc construction/destruction

CAvatarFilerDoc::CAvatarFilerDoc()
{
	m_avatar = NULL;

}

CAvatarFilerDoc::~CAvatarFilerDoc()
{
}

BOOL CAvatarFilerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAvatarFilerDoc serialization

void CAvatarFilerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAvatarFilerDoc diagnostics

#ifdef _DEBUG
void CAvatarFilerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAvatarFilerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAvatarFilerDoc commands

BOOL CAvatarFilerDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// Get a path name, for later use.
	for (LPCTSTR pszSlash = lpszPathName + lstrlen (lpszPathName) - 1;
		 *pszSlash != '\\';
		 pszSlash--);
	m_strPath = CString (lpszPathName, pszSlash - lpszPathName + 1);

	FILE *fp;	
	if ((fp = fopen(lpszPathName, "rb")) == NULL) {
		CString mesg;
		mesg.Format("Could not open %s\n", lpszPathName);
		AfxMessageBox(mesg);
		return FALSE;
	}

	char header[20];
	int avtype, version;
	fscanf(fp, "%s %d %d\n", header, &avtype, &version);

	if (!(strcmp(header, "#AvFile") == 0)) {
		AfxMessageBox("Not an AvFile");
		fclose(fp);
		return FALSE;
	}

	switch (avtype) {
	case 1:
	case 2:
		LoadBinaryAvatar(fp, avtype, version);		// binary handles Complex and Simple
		break;
	default:
		AfxMessageBox("Unknown avatar type");
		fclose(fp);
		return FALSE;
	}
	
	fclose(fp);
	return TRUE;
}

void write32(FILE *fp, INT32 val32) inline {
	fwrite(&val32, sizeof(val32), 1, fp);
}

void write16(FILE *fp, INT16 val16) inline {
	fwrite(&val16, sizeof(val16), 1, fp);
}

void write8(FILE *fp, INT8 val8) inline {
	fwrite(&val8, sizeof(val8), 1, fp);
}

BOOL CAvatarFilerDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	BOOL bRet = FALSE;
	CAvatarFileStream stream (lpszPathName, TRUE);
	if (stream.Open ()) {
		bRet = m_avatar->Save (&stream);
		stream.Close ();
	}
	if (!bRet) {
		CString mesg;
		mesg.Format("Could not write %s", lpszPathName);
		AfxMessageBox(mesg);
	}
	return bRet;
}

static char inBuff[100];

// modified version from chat project -- '.' not considered a word delimeter, but \t is.
char *GetToken(char *start, char **nextStart) {
	static char buff[100];
	while (*start && (isspace(*start) || strchr(",\t)", *start))) start++;
	if (!*start) return NULL;
	char *endPtr = start;
	while (*endPtr && !isspace(*endPtr) && !strchr(",\t)", *endPtr)) endPtr++;
	int nchars = endPtr - start;
	ASSERT(nchars);
	strncpy(buff, start, nchars);
	buff[nchars] = '\0';
	*nextStart = endPtr;
	return buff;
}

static double scale = 1.0;

int Scale(char *val) {
	int num = atoi(val);
	return ((int) floor (num * scale + 0.5));
}

static LPSTR pszUsageTypes[] = {
	"free",
	"limited"
};

void CAvatarFilerDoc::LoadBinaryAvatar(FILE *fp, int avType, int version) {
	CAvatarX *av;

	if (avType == 1)
		av = new CAvatarComplex;
	else if (avType == 2)
		av = new CAvatarSimple;
	else ASSERT(0);

	while(fgets(inBuff, sizeof(inBuff), fp)) {
		char *bptr = inBuff;
		char *token;
		if (!(token = GetToken(bptr, &bptr))) continue;  // blank line
		if (!strcmp(token, "Name")) {
			char *name = GetToken(bptr, &bptr);
			av->m_name = strdup(name);
		} else if (!strcmp(token,"Copyright")) {
			// The rest of the line is used as the Copyright message
			char * pszCopyright = bptr;
			while (*pszCopyright && (isspace(*pszCopyright) || strchr(",\t)", *pszCopyright))) pszCopyright++;
			av->m_pszCopyright = strdup (pszCopyright);
		} else if (!strcmp(token,"URL")) {
			char * pszURL = GetToken(bptr, &bptr);
			av->m_pszOriginalURL = strdup (pszURL);
		} else if (!strcmp(token,"Permissions")) {
			char * pszUsage = GetToken(bptr, &bptr);
			int iType;
			for (iType = 0; iType < sizeof(pszUsageTypes)/sizeof(pszUsageTypes[0]); iType++) {
				if (!stricmp (pszUsage, pszUsageTypes[iType])) {
					av->m_byUsageFlags = (BYTE)iType;
					break;
				}
			}
		} else if (!strcmp(token, "Style")) {
			char *style = GetToken(bptr, &bptr);
			av->m_style = atoi(style);
		} else if (!strcmp(token, "Flags")) {
			int flags = 0;
			while (token = GetToken(bptr, &bptr)) {
				if (!strcmp(token, "TORSOFIRST")) flags |= TORSOFIRST;
				else if (!strcmp(token, "TORSOMASK")) flags |= TORSOMASK;
				else if (!strcmp(token, "HEADMASK")) flags |= HEADMASK;
				else {
					char line[80];
					sprintf(line, "Unknown flag: %s", token);
					AfxMessageBox(line);
				}
			}
			av->m_flags = flags;
		} else if (!strcmp(token, "Scale")) {
			scale = atof(GetToken(bptr, &bptr));
		} else if (!strcmp(token, "BeginFaces")) {
			LoadFaces(fp, version, (CAvatarComplex *) av);
		} else if (!strcmp(token, "BeginTorsos")) {
			LoadTorsos(fp, version, (CAvatarComplex *) av);
		} else if (!strcmp(token, "Icon")) {
			av->m_icon = av->CreatePose (m_strPath + CString(GetToken(bptr, &bptr)));
		} else if (!strcmp(token, "BeginBodies")) {
			LoadBodies(fp, version, (CAvatarSimple *) av);
		} else if (!strcmp(token, "End")) {
			break;
		}
	}
	m_avatar = av;		// Can create memory leaks.  Must free old one (need to support deletes properly)
}

void CAvatarFilerDoc::LoadFaces(FILE *fp, int version, CAvatarComplex *av) {
	FACEREC fRec[200], *newRec = fRec-1;
	USHORT sLastID = 0;

	while (fgets(inBuff, sizeof(inBuff), fp)) {
		char *bptr = inBuff;
		char *token;
		if (!(token = GetToken(bptr, &bptr))) continue;		// blank line
		if (!strcmp(token, "FOREGROUND")) {				// Foreground (must precede masks)
			newRec++;
			sLastID = newRec->poseID = av->CreatePose (m_strPath + CString (GetToken (bptr, &bptr)));
			char *emType = GetToken(bptr, &bptr);
			newRec->emotion = StringToEmType(emType);
			char *intensity = GetToken(bptr, &bptr);
			newRec->intensity = (UCHAR) (atof(intensity) * 255);

			char *val = GetToken(bptr, &bptr);		// xCX & yCX
			newRec->xCX = Scale(val);
			val = GetToken(bptr, &bptr);
			newRec->yCX = Scale(val);

			val = GetToken(bptr, &bptr);			// delta_xCX & delta_yCX
			newRec->delta_xCX = Scale(val);
			val = GetToken(bptr, &bptr);
			newRec->delta_yCX = Scale(val);

			val = GetToken(bptr, &bptr);			// faceX and faceY
			newRec->faceX = Scale(val);
			val = GetToken(bptr, &bptr);
			newRec->faceY = Scale(val);
		} else if (!strcmp(token, "DITTO")) {		// repeat of prior
			newRec++;
			*newRec = *(newRec-1);
			newRec->poseID = sLastID;
			char *emType = GetToken(bptr, &bptr);
			newRec->emotion = StringToEmType(emType);
			char *intensity = GetToken(bptr, &bptr);
			newRec->intensity = (UCHAR) (atof(intensity) * 255);
		} else if (!strcmp(token, "TRANSPARENCY")) {
			CPose * pPose = av->GetPoseFromID (newRec->poseID);
			ASSERT (pPose != NULL);
			pPose->m_strFiles[1] = m_strPath + CString (GetToken(bptr, &bptr));
		} else if (!strcmp(token, "AURA")) {
			CPose * pPose = av->GetPoseFromID (newRec->poseID);
			ASSERT (pPose != NULL);
			pPose->m_strFiles[2] = m_strPath + CString (GetToken(bptr, &bptr));
		} else if (!strcmp(token, "EndFaces")) {
			break;
		}
	}

	// now malloc this and install
	av->nFaces = newRec - fRec + 1;
	av->fRec = (FACEREC *) malloc (sizeof(FACEREC) * av->nFaces);
	for (int i = 0; i < av->nFaces; i++) {
		av->fRec[i] = fRec[i];
	}
}

// similar to LoadFaces, but we deal w/ TorsoRecs instead of FaceRecs (only one x, y pair)
void CAvatarFilerDoc::LoadTorsos(FILE *fp, int version, CAvatarComplex *av) {
	BODYREC tRec[200], *newRec = tRec-1;
	USHORT sLastID = 0;

	while (fgets(inBuff, sizeof(inBuff), fp)) {
		char *bptr = inBuff;
		char *token;
		if (!(token = GetToken(bptr, &bptr))) continue;		// blank line
		if (!strcmp(token, "FOREGROUND")) {				// Foreground (must precede masks)
			newRec++;
			sLastID = newRec->poseID = av->CreatePose (m_strPath + CString (GetToken (bptr, &bptr)));
			char *emType = GetToken(bptr, &bptr);
			newRec->emotion = StringToEmType(emType);
			char *intensity = GetToken(bptr, &bptr);
			newRec->intensity = (UCHAR) (atof(intensity) * 255);

			char *val = GetToken(bptr, &bptr);			// xCX and yCX
			newRec->xCX = Scale(val);
			val = GetToken(bptr, &bptr);
			newRec->yCX = Scale(val);
		} else if (!strcmp(token, "DITTO")) {			// repeat of prior
			newRec++;
			*newRec = *(newRec-1);
			newRec->poseID = sLastID;
			char *emType = GetToken(bptr, &bptr);
			newRec->emotion = StringToEmType(emType);
			char *intensity = GetToken(bptr, &bptr);
			newRec->intensity = (UCHAR) (atof(intensity) * 255);
		} else if (!strcmp(token, "TRANSPARENCY")) {
			CPose * pPose = av->GetPoseFromID (newRec->poseID);
			ASSERT (pPose != NULL);
			pPose->m_strFiles[1] = m_strPath + CString (GetToken(bptr, &bptr));
		} else if (!strcmp(token, "AURA")) {
			CPose * pPose = av->GetPoseFromID (newRec->poseID);
			ASSERT (pPose != NULL);
			pPose->m_strFiles[2] = m_strPath + CString (GetToken(bptr, &bptr));
		} else if (!strcmp(token, "EndTorsos")) {
			break;
		}
	}

	// now malloc this and install
	av->nTorsos = newRec - tRec + 1;
	av->bRec = (BODYREC *) malloc (sizeof(BODYREC) * av->nTorsos);
	for (int i = 0; i < av->nTorsos; i++) {
		av->bRec[i] = tRec[i];
	}
}

// similar to LoadFaces, but we deal w/ only bodies
void CAvatarFilerDoc::LoadBodies(FILE *fp, int version, CAvatarSimple *av) {
	RBODYREC bRec[200], *newRec = bRec-1;
	USHORT sLastID = 0;

	while (fgets(inBuff, sizeof(inBuff), fp)) {
		char *bptr = inBuff;
		char *token;
		if (!(token = GetToken(bptr, &bptr))) continue;		// blank line
		if (!strcmp(token, "FOREGROUND")) {				// Foreground (must precede masks)
			newRec++;
			sLastID = newRec->poseID = av->CreatePose (m_strPath + CString (GetToken (bptr, &bptr)));
			char *emType = GetToken(bptr, &bptr);
			newRec->emotion = StringToEmType(emType);
			char *intensity = GetToken(bptr, &bptr);
			newRec->intensity = (UCHAR) (atof(intensity) * 255);

			char *val = GetToken(bptr, &bptr);			// faceX and faceY
			newRec->faceX = Scale(val);
			val = GetToken(bptr, &bptr);
			newRec->faceY = Scale(val);
		} else if (!strcmp(token, "DITTO")) {			// repeat of prior
			newRec++;
			*newRec = *(newRec-1);
			newRec->poseID = sLastID;
			char *emType = GetToken(bptr, &bptr);
			newRec->emotion = StringToEmType(emType);
			char *intensity = GetToken(bptr, &bptr);
			newRec->intensity = (UCHAR) (atof(intensity) * 255);
		} else if (!strcmp(token, "TRANSPARENCY")) {
			CPose * pPose = av->GetPoseFromID (newRec->poseID);
			ASSERT (pPose != NULL);
			pPose->m_strFiles[1] = m_strPath + CString (GetToken(bptr, &bptr));
		} else if (!strcmp(token, "AURA")) {
			CPose * pPose = av->GetPoseFromID (newRec->poseID);
			ASSERT (pPose != NULL);
			pPose->m_strFiles[2] = m_strPath + CString (GetToken(bptr, &bptr));
		} else if (!strcmp(token, "EndTorsos")) {
			break;
		}
	}

	// now malloc this and install
	av->m_nBodies = newRec - bRec + 1;
	av->bRec = (RBODYREC *) malloc (sizeof(RBODYREC) * av->m_nBodies);
	for (int i = 0; i < av->m_nBodies; i++) {
		av->bRec[i] = bRec[i];
	}
}


char *emStrings[] = {
	NULL,
	"EM_HAPPY",
	"EM_COY",
	"EM_BORED",
	"EM_SCARED",
	"EM_SAD",
	"EM_ANGRY",
	"EM_SHOUT",
	"EM_LAUGH",
	"EM_NEUTRAL",
	"EM_WAVE",
	"EM_POINTOTHER",
	"EM_POINTSELF",
	"EM_DOUBLEPOINT",
	"EM_SHRUG",
	"EM_3QRWALK",
	"EM_SIDEWALK",
	"EM_3QFWALK",
};

int StringToEmType(char const *emType) {
	int nTypes = sizeof(emStrings) / sizeof(const char *);
	for (int i = 1; i < nTypes; i++) {
		if (!strcmp(emType, emStrings[i])) return i;
	}

	return 0;
}


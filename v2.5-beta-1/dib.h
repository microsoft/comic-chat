// dib.h : header file
//
// Originally from "Animation Techniques in Win32", Nigel Thomson, MS Press
// Made non-256-color-compatible and then stripped down, ShankuN, Feb. 98
// CDIB class
//

#ifndef __DIB__
#define __DIB__

extern UINT DIBStorageWidth(UINT nWidth, UINT nBitCount);
extern int NumDIBColorEntries(BITMAPINFO* pBmpInfo);

class CDIB : public CObject
{
   #if 0
    DECLARE_SERIAL(CDIB)
   #endif
public:
    CDIB();
    ~CDIB();

    BITMAPINFO* GetBitmapInfoAddress()
        {return m_pBMI;}                        // Pointer to bitmap info
    void* GetBitsAddress()
        {return m_pBits;}                       // Pointer to the bits
    RGBQUAD* GetClrTabAddress()
        {return (LPRGBQUAD)(((BYTE*)(m_pBMI)) 
            + m_pBMI->bmiHeader.biSize);}      // Pointer to color table
    int GetNumClrEntries();                     // Number of color table entries
    BOOL Create(BITMAPINFO* pBMI, BYTE* pBits); // Create from existing mem
    virtual BOOL Load(WORD wResid);             // Load DIB from resource
    virtual void Draw(CDC* pDC, int x, int y);
	virtual void Draw(CDC* pDC,
					  int x, int y,
					  int destWidth, int destHeight,
					  int rop = SRCCOPY);
	virtual void Draw(CDC* pDC, int destX, int destY, int destWidth, int destHeight, 
					  int srcX, int srcY, int srcWidth, int srcHeight, int rop);
    virtual BOOL Save(const char* pszFileName = NULL);// Save DIB to disk file
    virtual BOOL Save(CFile* fp);               // Save to file
	void Clear()
		{ free (m_pBMI); m_pBMI = NULL; if (m_bMyBits) free (m_pBits); m_pBits = NULL; };
	void TransferFrom(CDIB* pDIB)
		{ Clear (); 
		  m_pBMI = pDIB->m_pBMI; m_pBits = pDIB->m_pBits; m_bMyBits = pDIB->m_bMyBits; 
		  pDIB->m_pBMI = NULL; pDIB->m_pBits = NULL; }
   #if 0
    BOOL Create(int width, int height);         // Create a new DIB
    void* GetPixelAddress(int x, int y);
    virtual BOOL Load(CFile* fp);               // Load from file
	virtual BOOL Load(FILE *fp);				// Load from file
    virtual BOOL Load(const char* pszFileName = NULL);// Load DIB from disk file
    virtual void Serialize(CArchive& ar);
    virtual BOOL MapColorsToPalette(CPalette* pPal);
    virtual void GetRect(CRect* pRect);
    virtual void CopyBits(CDIB* pDIB, 
                          int xd, int yd,
                          int w,  int h,
                          int xs, int ys,
                          COLORREF clrTrans = 0xFFFFFFFF);
   #endif
    virtual int GetWidth() {return this != NULL ? DibWidth() : 0;}   // Image width
    virtual int GetHeight() {return this != NULL ? DibHeight() : 0;} // Image height
	virtual void Convert8ToNonRLE();
	virtual void Convert4ToNonRLE();
	virtual void ConvertToNonRLE();
    int DibWidth()
        {return m_pBMI->bmiHeader.biWidth;}
    int DibHeight() 
        {return m_pBMI->bmiHeader.biHeight;}
    int StorageWidth()
        {return DIBStorageWidth (m_pBMI->bmiHeader.biWidth, m_pBMI->bmiHeader.biBitCount); }

protected:
    BITMAPINFO* m_pBMI;         // Pointer to BITMAPINFO struct
    BYTE* m_pBits;              // Pointer to the bits
    BOOL  m_bMyBits;            // TRUE if DIB owns Bits memory

};


#endif // __DIB__

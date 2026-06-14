#ifndef __PE_H__
#define __PE_H__

#include "bbox.h"

#define PE_UNKNOWN	0
#define PE_BALLOON	1
#define PE_BOX		2		// technically, also of type balloon

class CPanelElement {
public:
	SRECT m_bbox;

	CPanelElement() { m_bbox.Left = m_bbox.Right = -1; }  // empty
	CPanelElement(const CPanelElement&);
	virtual ~CPanelElement() {};
	virtual void Draw(CDC* dc, POINT* ul, RECT *rect) = 0;
	virtual BOOL SetBBox(int left, int bottom, int right, int top);
	virtual void GetBBox(RECT *);
	virtual int GetType() { return PE_UNKNOWN; }
};

#endif // __PE_H__
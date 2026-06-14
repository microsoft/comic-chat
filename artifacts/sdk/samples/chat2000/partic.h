//****************************************************************************
//  Module:     Chat2000.EXE
//  File:       Partic.h
//              
//
//  Copyright (c) Microsoft Corporation 1995-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#ifndef _PARTIC_H_
#define _PARTIC_H_

#include "globals.h"

//class participant
class CParticipant : public CObject
{
private:
	short		m_iID;
	CString		m_szName;
	CString		m_szAlias;
	short		m_iStatus;

public:
	CParticipant();
	CParticipant(	long ParticipantID, 
					LPCTSTR Alias=NULL, 
					short Status=PARTICIPANT_STATE_PARTICIPANT);
	void operator =( const CParticipant& src );
	bool operator== ( const CParticipant &src ) const;

	//Set Attributes
	void SetAlias(LPCTSTR Alias_p);
	void SetStatus (short Status_p);
	void SetName(LPCTSTR Name_p);
	//get attributes
	short	GetID ();
	CString GetAlias();
	short	GetStatus ();
	CString GetName();
};

#endif _PARTIC_H_
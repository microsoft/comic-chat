//****************************************************************************
//  Module:     Chat2000.EXE
//  File:       Partic.cpp
//              
//
//  Copyright (c) Microsoft Corporation 1995-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#include "stdafx.h"
#include "partic.h"

CParticipant::CParticipant()
{
}

CParticipant::CParticipant(long ParticipantID, LPCTSTR Alias, short Status)
{
	m_iID = (short) ParticipantID;
	//m_szName = Name;
	m_szAlias = Alias;
	m_iStatus = Status;
}


bool CParticipant::operator== ( const CParticipant &src ) const
{
	if ((m_iID		== src.m_iID))		//&&
		//(szName	== src.szName)	&&
		//(szAlias	== src.szAlias) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CParticipant::operator= ( const CParticipant& src )
{
	m_iID		= src.m_iID;
	m_szName	= src.m_szName;
	m_szAlias	= src.m_szAlias;
	m_iStatus	= src.m_iStatus;
}


//Set Attributes
void CParticipant::SetAlias(LPCTSTR Alias_p)
{
	m_szAlias = Alias_p;
}

void CParticipant::SetStatus (short Status_p)
{
	m_iStatus = Status_p;
}

void CParticipant::SetName(LPCTSTR Name_p)
{
	m_szName = Name_p;
}


//get attributes
short CParticipant::GetID ()
{
	return m_iID;
}

CString CParticipant::GetAlias()
{
	return m_szAlias;
}

short CParticipant::GetStatus ()
{
	return m_iStatus;
}

CString CParticipant::GetName()
{
	return m_szName;
}

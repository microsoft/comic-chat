//****************************************************************************
//  Module:     Chappy.EXE
//  File:       MsChatX.cpp
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "stdafx.h"
#include "mschatx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// IChatItems properties

/////////////////////////////////////////////////////////////////////////////
// IChatItems operations

CString IChatItems::GetAssociatedType()
{
	CString result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void IChatItems::SetAssociatedType(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x1, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

VARIANT IChatItems::GetValidItems()
{
	VARIANT result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&result, NULL);
	return result;
}

long IChatItems::GetItemValid(LPCTSTR ItemName)
{
	long result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms,
		ItemName);
	return result;
}

VARIANT IChatItems::GetItem(LPCTSTR ItemName)
{
	VARIANT result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&result, parms,
		ItemName);
	return result;
}

void IChatItems::SetItem(LPCTSTR ItemName, const VARIANT& newValue)
{
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 ItemName, &newValue);
}

void IChatItems::InvalidateItem(LPCTSTR ItemName)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x37, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 ItemName);
}


/////////////////////////////////////////////////////////////////////////////
// IChannelObject properties

/////////////////////////////////////////////////////////////////////////////
// IChannelObject operations

long IChannelObject::GetIndex()
{
	long result;
	InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

long IChannelObject::GetChannelState()
{
	long result;
	InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

CString IChannelObject::GetChannelName()
{
	CString result;
	InvokeHelper(0xc, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void IChannelObject::SetChannelName(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0xc, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

CString IChannelObject::GetChannelTopic()
{
	CString result;
	InvokeHelper(0xd, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

long IChannelObject::GetMemberCount()
{
	long result;
	InvokeHelper(0xe, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

CString IChannelObject::GetMemberIdentity(const VARIANT& vMemberNickname)
{
	CString result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0xf, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, parms,
		&vMemberNickname);
	return result;
}

long IChannelObject::GetMemberModes(const VARIANT& vMemberNickname)
{
	long result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x10, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms,
		&vMemberNickname);
	return result;
}

LPDISPATCH IChannelObject::GetChannelProperty(LPCTSTR ChannelPropertyName)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x11, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		ChannelPropertyName);
	return result;
}

LPDISPATCH IChannelObject::GetMemberProperty(LPCTSTR MemberPropertyName, const VARIANT& vMemberNickname)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x12, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		MemberPropertyName, &vMemberNickname);
	return result;
}

void IChannelObject::CreateChannel(const VARIANT& ChannelName, const VARIANT& ChannelKeyword, const VARIANT& ChannelModes, const VARIANT& MaxMemberCount)
{
	static BYTE parms[] =
		VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0x3c, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &ChannelName, &ChannelKeyword, &ChannelModes, &MaxMemberCount);
}

void IChannelObject::JoinChannel(const VARIANT& ChannelName, const VARIANT& ChannelKeyword)
{
	static BYTE parms[] =
		VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0x3d, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &ChannelName, &ChannelKeyword);
}

void IChannelObject::LeaveChannel()
{
	InvokeHelper(0x3e, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IChannelObject::SendMessage(long MessageType, const VARIANT& Message, const VARIANT& RecipientNicknames, const VARIANT& DataMessageTag)
{
	static BYTE parms[] =
		VTS_I4 VTS_VARIANT VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0x3f, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 MessageType, &Message, &RecipientNicknames, &DataMessageTag);
}

void IChannelObject::SendInvitation(LPCTSTR Nickname)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x40, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Nickname);
}

void IChannelObject::ListMembers(LPDISPATCH MemberQueryItems)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x41, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 MemberQueryItems);
}

void IChannelObject::QueryMemberProperty(LPCTSTR MemberPropertyName, const VARIANT& MemberNickname)
{
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x42, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 MemberPropertyName, &MemberNickname);
}

void IChannelObject::ChangeMemberProperty(LPCTSTR MemberPropertyName, const VARIANT& NewMemberProperty, const VARIANT& MemberNickname)
{
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0x43, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 MemberPropertyName, &NewMemberProperty, &MemberNickname);
}

void IChannelObject::BanMembers(LPDISPATCH MemberItems, long Set)
{
	static BYTE parms[] =
		VTS_DISPATCH VTS_I4;
	InvokeHelper(0x44, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 MemberItems, Set);
}

void IChannelObject::KickMember(LPCTSTR MemberNickname, const VARIANT& Reason)
{
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x45, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 MemberNickname, &Reason);
}

void IChannelObject::QueryChannelProperty(LPCTSTR ChannelPropertyName)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x46, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 ChannelPropertyName);
}

void IChannelObject::ChangeChannelProperty(LPCTSTR ChannelPropertyName, const VARIANT& NewChannelProperty)
{
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x47, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 ChannelPropertyName, &NewChannelProperty);
}


/////////////////////////////////////////////////////////////////////////////
// IChannelsCollection properties

/////////////////////////////////////////////////////////////////////////////
// IChannelsCollection operations

LPDISPATCH IChannelsCollection::GetItem(const VARIANT& vIndex)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		&vIndex);
	return result;
}

LPUNKNOWN IChannelsCollection::Get_NewEnum()
{
	LPUNKNOWN result;
	InvokeHelper(0xfffffffc, DISPATCH_PROPERTYGET, VT_UNKNOWN, (void*)&result, NULL);
	return result;
}

long IChannelsCollection::GetCount()
{
	long result;
	InvokeHelper(0x1e, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH IChannelsCollection::Add(const VARIANT& vIndex, const VARIANT& vKey)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT VTS_VARIANT;
	InvokeHelper(0x50, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		&vIndex, &vKey);
	return result;
}

void IChannelsCollection::Remove(const VARIANT& vIndex)
{
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x51, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 &vIndex);
}



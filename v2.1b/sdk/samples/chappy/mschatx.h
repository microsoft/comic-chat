//****************************************************************************
//  Module:     Chappy.EXE
//  File:       MsChatX.H
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#ifndef __MSCHATX_H__
#define __MSCHATX_H__
// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// IChatItems wrapper class

class IChatItems : public COleDispatchDriver
{
public:
	IChatItems() {}		// Calls COleDispatchDriver default constructor
	IChatItems(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IChatItems(const IChatItems& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetAssociatedType();
	void SetAssociatedType(LPCTSTR lpszNewValue);
	VARIANT GetValidItems();
	long GetItemValid(LPCTSTR ItemName);
	VARIANT GetItem(LPCTSTR ItemName);
	void SetItem(LPCTSTR ItemName, const VARIANT& newValue);
	void InvalidateItem(LPCTSTR ItemName);
};
/////////////////////////////////////////////////////////////////////////////
// IChannelObject wrapper class

class IChannelObject : public COleDispatchDriver
{
public:
	IChannelObject() {}		// Calls COleDispatchDriver default constructor
	IChannelObject(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IChannelObject(const IChannelObject& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long GetIndex();
	long GetChannelState();
	CString GetChannelName();
	void SetChannelName(LPCTSTR lpszNewValue);
	CString GetChannelTopic();
	long GetMemberCount();
	CString GetMemberIdentity(const VARIANT& vMemberNickname);
	long GetMemberModes(const VARIANT& vMemberNickname);
	LPDISPATCH GetChannelProperty(LPCTSTR ChannelPropertyName);
	LPDISPATCH GetMemberProperty(LPCTSTR MemberPropertyName, const VARIANT& vMemberNickname);
	void CreateChannel(const VARIANT& ChannelName, const VARIANT& ChannelKeyword, const VARIANT& ChannelModes, const VARIANT& MaxMemberCount);
	void JoinChannel(const VARIANT& ChannelName, const VARIANT& ChannelKeyword);
	void LeaveChannel();
	void SendMessage(long MessageType, const VARIANT& Message, const VARIANT& RecipientNicknames, const VARIANT& DataMessageTag);
	void SendInvitation(LPCTSTR Nickname);
	void ListMembers(LPDISPATCH MemberQueryItems);
	void QueryMemberProperty(LPCTSTR MemberPropertyName, const VARIANT& MemberNickname);
	void ChangeMemberProperty(LPCTSTR MemberPropertyName, const VARIANT& NewMemberProperty, const VARIANT& MemberNickname);
	void BanMembers(LPDISPATCH MemberItems, long Set);
	void KickMember(LPCTSTR MemberNickname, const VARIANT& Reason);
	void QueryChannelProperty(LPCTSTR ChannelPropertyName);
	void ChangeChannelProperty(LPCTSTR ChannelPropertyName, const VARIANT& NewChannelProperty);
};
/////////////////////////////////////////////////////////////////////////////
// IChannelsCollection wrapper class

class IChannelsCollection : public COleDispatchDriver
{
public:
	IChannelsCollection() {}		// Calls COleDispatchDriver default constructor
	IChannelsCollection(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IChannelsCollection(const IChannelsCollection& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetItem(const VARIANT& vIndex);
	LPUNKNOWN Get_NewEnum();
	long GetCount();
	LPDISPATCH Add(const VARIANT& vIndex, const VARIANT& vKey);
	void Remove(const VARIANT& vIndex);
};
/////////////////////////////////////////////////////////////////////////////
// IMsChatPr wrapper class

class IMsChatPr : public COleDispatchDriver
{
public:
	IMsChatPr() {}		// Calls COleDispatchDriver default constructor
	IMsChatPr(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IMsChatPr(const IMsChatPr& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long GetConnectionState();
	long GetConnectionType();
	long GetOpeningChannels();
	long GetPropertyAccessTimeOut();
	void SetPropertyAccessTimeOut(long nNewValue);
	long GetProcessProtocolMessages();
	void SetProcessProtocolMessages(long nNewValue);
	CString GetConvertedString(long ConversionType, long ConversionSource, LPCTSTR Source, long PostProcess);
	LPDISPATCH GetChannels();
	LPDISPATCH GetNewChatItems();
	long GetMaxStringLength(long StringType);
	CString GetServerName();
	CString GetNickname();
	LPDISPATCH GetUserProperty(LPCTSTR UserPropertyName, const VARIANT& Nickname);
	LPDISPATCH GetChannelProperty(LPCTSTR ChannelPropertyName, const VARIANT& ChannelName);
	LPDISPATCH GetServerProperty(LPCTSTR ServerPropertyName);
	void AboutBox();
	void Connect(LPCTSTR ServerName);
	void Login(LPCTSTR ServerName, LPCTSTR Nickname, const VARIANT& UserName, const VARIANT& RealName, const VARIANT& Password, const VARIANT& SecurityPackages);
	void Disconnect();
	void CancelConnectLogin();
	void ListChannels(LPDISPATCH ChannelQueryItems);
	void QueryChannelProperty(LPCTSTR ChannelPropertyName, const VARIANT& ChannelName);
	void KillChannel(LPCTSTR ChannelName, const VARIANT& Reason);
	void ListUsers(LPDISPATCH UserQueryItems);
	void QueryUserProperty(LPCTSTR UserPropertyName, const VARIANT& Nickname);
	void ChangeUserProperty(LPCTSTR UserPropertyName, const VARIANT& NewUserProperty, const VARIANT& Nickname);
	void IgnoreUsers(LPDISPATCH UserItems, long Set);
	void BanUsers(LPDISPATCH UserItems, long Set, const VARIANT& Reason, const VARIANT& Duration);
	void KickUser(LPCTSTR Nickname, const VARIANT& Reason);
	void ChangeNickname(LPCTSTR NewNickname);
	void SetAway(long Away, const VARIANT& TextMessage);
	void QueryServerProperty(LPCTSTR ServerPropertyName);
	void SendInvitation(LPCTSTR Nickname, const VARIANT& ChannelName);
	void SendPrivateMessage(long PrivateMessageType, const VARIANT& Message, const VARIANT& RecipientNicknames, const VARIANT& DataMessageTag);
	void SendProtocolMessage(LPCTSTR TextMessage);
};
/////////////////////////////////////////////////////////////////////////////
// _IMsChatPrEvent wrapper class

class _IMsChatPrEvent : public COleDispatchDriver
{
public:
	_IMsChatPrEvent() {}		// Calls COleDispatchDriver default constructor
	_IMsChatPrEvent(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	_IMsChatPrEvent(const _IMsChatPrEvent& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	void OnConnectionState(long NewConnectionState);
	void OnConnectionError(long ErrorCode, LPCTSTR Description, const VARIANT& ChannelName);
	void OnChannelState(LPDISPATCH Channel, long NewChannelState);
	void OnChannelError(LPDISPATCH Channel, long ErrorCode, LPCTSTR Description);
	void OnBeginEnumeration(LPDISPATCH Channel, long EnumType);
	void OnEndEnumeration(LPDISPATCH Channel, long EnumType);
	void OnAddMember(LPDISPATCH Channel, LPCTSTR Nickname, LPDISPATCH MemberItems);
	void OnDelMember(LPDISPATCH Channel, LPCTSTR Nickname);
	void OnUserProperty(LPDISPATCH UserItems);
	void OnUserPropertyChanged(LPCTSTR ModifiedNickname, LPCTSTR ModifierNickname, LPCTSTR UserPropertyName, const VARIANT& OldUserProperty, const VARIANT& NewUserProperty);
	void OnMemberProperty(LPDISPATCH Channel, LPDISPATCH MemberItems);
	void OnMemberPropertyChanged(LPDISPATCH Channel, LPCTSTR ModifiedNickname, LPCTSTR ModifierNickname, LPCTSTR MemberPropertyName, const VARIANT& OldMemberProperty, const VARIANT& NewMemberProperty);
	void OnMemberKicked(LPDISPATCH Channel, LPCTSTR KickedNickname, LPCTSTR KickerNickname, LPCTSTR Reason);
	void OnChannelProperty(LPDISPATCH Channel, LPDISPATCH ChannelItems);
	void OnChannelPropertyChanged(LPDISPATCH Channel, LPCTSTR ModifierNickname, LPCTSTR ChannelPropertyName, const VARIANT& OldChannelProperty, const VARIANT& NewChannelProperty);
	void OnServerProperty(LPDISPATCH ServerItems);
	void OnInvitation(LPCTSTR ChannelName, LPDISPATCH InviterItems);
	void OnKnock(LPDISPATCH Channel, LPDISPATCH KnockerItems, long ErrorCode);
	void OnPrivateMessage(LPCTSTR SenderNickname, long PrivateMessageType, const VARIANT& Message, const VARIANT& DataMessageTag);
	void OnServerTextMessage(long ServerMessageType, LPCTSTR Text);
	void OnMessage(LPDISPATCH Channel, LPCTSTR SenderNickname, long MessageType, const VARIANT& Message, const VARIANT& RecipientNicknames, const VARIANT& DataMessageTag);
	void OnProtocolMessage(LPCTSTR MessagePrefix, LPCTSTR MessageCommand, const VARIANT& MessageParameters, BOOL* EnableDefault);
};

#endif //__MSCHATX_H__
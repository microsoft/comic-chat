Attribute VB_Name = "utils"
Option Explicit


' User Type
Global Const utHost = 0
Global Const utModerator = 1
Global Const utGuest = 2


Global Const colorSay = &HFF0000
Global Const colorWhisper = &H80&
Global Const colorJoin = &H8000&
Global Const colorLeave = &HFF&
Global Const colorKick = &HC000C0
Global Const colorMessage = &H0&

Global Const colorForwardedToModerator = &HC0&
Global Const colorForwardedToGuest = &HC0&
Global Const colorAnswered = &HC0&


' Server properties
Global Const spnServerAnonymousAllowed = "AnonymousAllowed"
Global Const spnServerChannelCount = "ChannelCount"
Global Const spnServerIgnoredUsers = "IgnoredUsers"
Global Const spnServerInfo = "Info"
Global Const spnServerMaxMessageLength = "MaxMessageLength"
Global Const spnServerName = "Name"
Global Const spnServerNetInvisibleCount = "NetInvisibleCount"
Global Const spnServerNetServerCount = "NetServerCount"
Global Const spnServerNetUserCount = "NetUserCount"
Global Const spnServerNodeServerCount = "NodeServerCount"
Global Const spnServerNodeUserCount = "NodeUserCount"
Global Const spnServerSecurityPackages = "SecurityPackages"
Global Const spnServerSysopCount = "SysopCount"
Global Const spnServerUnknownConnectionCount = "UnknownConnectionCount"



' Member properties
Global Const pnIdentity = "Identity"
Global Const pnIPAddress = "IPAddress"
Global Const pnIPAddressOp = "IPAddressOp"
Global Const pnModes = "Modes"
Global Const pnNickName = "NickName"
Global Const pnNickNameOp = "NicknameOp"
Global Const pnObjectId = "ObjectId"
Global Const pnUserName = "UserName"
Global Const pnUserNameOp = "UserNameOp"

' user properties
Global Const pnAway = "Away"
Global Const pnChannels = "Channels"
Global Const pnHostInChannels = "HostInChannels"
' Global Const pnIdentity = "Identity"
' Global Const pnIdleTime = "IdleTime"
'IPAddress
'IPAddressOp
Global Const pnMask = "Mask"
Global Const pnMaskOp = "MaskOp"
' Modes"
'NickName ","
'NicknameOp ","
Global Const pnOwnerOfChannels = "OwnerOfChannels"
Global Const pnRealName = "RealName"
Global Const pnServerInfo = "ServerInfo"
Global Const pnServerName = "ServerName"
Global Const pnSignOnTime = "SignOnTime"
'UserName ","
'UserNameOp ","
Global Const pnVoiceInChannels = "VoiceInChannels"


' Channel properties
Global Const cpnAccount = "Account"
Global Const cpnBannedList = "BannedList"
Global Const cpnChannelAgeMax = "ChannelAgeMax"
Global Const cpnChannelAgeMin = "ChannelAgeMin"
Global Const cpnClientData = "ClientData"
Global Const cpnClientGuid = "ClientGuid"
Global Const cpnCloneList = "CloneList"
Global Const cpnCreationTime = "CreationTime"
Global Const cpnHostKey = "HostKey"
Global Const cpnKeyword = "Keyword"
Global Const cpnLag = "Lag"
Global Const cpnLanguage = "Language"
Global Const cpnLanguageOp = "LanguageOp"
Global Const cpnListCount = "ListCount"
Global Const cpnMaxMemberCount = "MaxMemberCount"
Global Const cpnMemberCount = "MemberCount"
Global Const cpnMemberCountMax = "MemberCountMax"
Global Const cpnMemberCountMin = "MemberCountMin"
Global Const cpnModes = "Modes"
Global Const cpnName = "Name"
Global Const cpnNameOp = "NameOp"
Global Const cpnObjectId = "ObjectId"
Global Const cpnOnJoin = "OnJoin"
Global Const cpnOnPart = "OnPart"
Global Const cpnOwnerKey = "OwnerKey"
Global Const cpnRating = "Rating"
Global Const cpnRegistered = "Registered"
Global Const cpnServicePath = "ServicePath"
Global Const cpnSubject = "Subject"
Global Const cpnSubjectOp = "SubjectOp"
Global Const cpnTopic = "Topic"
Global Const cpnTopicAgeMax = "TopicAgeMax"
Global Const cpnTopicAgeMin = "TopicAgeMin"
Global Const cpnTopicOp = "TopicOp"


Public strChannelState(chsClosed To chsOpen) As String

Public Declare Function OSWinHelp% Lib "user32" Alias "WinHelpA" (ByVal hwnd&, ByVal HelpFile$, ByVal wCommand%, dwData As Any)
Public Declare Function GetUserName Lib "advapi32.dll" Alias "GetUserNameA" (ByVal lpbuffer As String, nSize As Long) As Long



Function VPropertyFromChatItems(ByRef ci As ChatItems, ByRef strPropertyName As String) As Variant
    If ci Is Nothing Then
        VPropertyFromChatItems = Empty
        MsgBox "ChatItems is invalid in VPropertyFromChatItems"
        Exit Function
    End If
    
    If (ci.ItemValid(strPropertyName)) Then
        VPropertyFromChatItems = ci.item(strPropertyName)
    Else
        VPropertyFromChatItems = Empty
    End If
End Function



' This helper function can used to set a check box
Function nBitwiseAnd(ByVal a As Long, ByVal b As Long) As Long
    If (a And b) = 0 Then
        nBitwiseAnd = 0
    Else
        nBitwiseAnd = 1
    End If
End Function


Function IsSeperatorChar(ByVal AscCh As Integer)
    IsSeperatorChar = AscCh < 65 Or AscCh > 90
End Function


Function IsFlagSet(ByVal v As Long, ByVal mask As Long) As Boolean
    If (v And mask) = 0 Then
        IsFlagSet = False
    Else
        IsFlagSet = True
    End If
End Function


Sub SetFlag(ByRef v As Long, ByVal mask As Long)
    v = v Or mask
End Sub


Sub UnsetFlag(ByRef v As Long, ByVal mask As Long)
    v = v And (Not mask)
End Sub


Sub SetFlagIf(ByRef v As Long, ByVal mask As Long, ByVal condition As Boolean)
    If condition = False Then
        v = v And (Not mask)
    Else
        v = v Or mask
    End If
End Sub


Function IsHost() As Boolean
    Dim varNick
    
    On Error GoTo ErrorExit
    varNick = fMainForm.MsChatPr1.NickName
    
    IsHost = VPropertyFromChatItems(fMainForm.ChatUI1.Channel1.MemberProperty(pnModes, varNick), pnModes) And mmHost
    Exit Function
    
ErrorExit:
    IsHost = False
End Function


Function Max(ByVal a As Integer, ByVal b As Integer) As Integer
    If a > b Then
        Max = a
    Else
        Max = b
    End If
End Function

Function Min(ByVal a As Integer, ByVal b As Integer) As Integer
    If a > b Then
        Min = b
    Else
        Min = a
    End If
End Function


VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.1#0"; "COMDLG32.OCX"
Object = "{6B7E6392-850A-101B-AFC0-4210102A8DA7}#1.1#0"; "COMCTL32.OCX"
Object = "{9FE8A900-96AC-11CF-9086-00AA006C3FBC}#2.0#0"; "MsChatpr.OCX"
Object = "{F96522A1-FC37-11D0-99CB-00C04FD64497}#1.0#0"; "nmstart.ocx"
Begin VB.Form frmMain 
   AutoRedraw      =   -1  'True
   Caption         =   "ExecChat2"
   ClientHeight    =   5655
   ClientLeft      =   165
   ClientTop       =   735
   ClientWidth     =   8700
   LinkTopic       =   "Form1"
   ScaleHeight     =   5655
   ScaleWidth      =   8700
   StartUpPosition =   3  'Windows Default
   Begin ExecChat2.ChatUI ChatUI1 
      Height          =   3255
      Left            =   120
      TabIndex        =   1
      Top             =   480
      Width           =   7095
      _ExtentX        =   17383
      _ExtentY        =   9340
   End
   Begin ComctlLib.StatusBar sbStatusBar 
      Align           =   2  'Align Bottom
      Height          =   270
      Left            =   0
      TabIndex        =   0
      Top             =   5385
      Width           =   8700
      _ExtentX        =   15346
      _ExtentY        =   476
      SimpleText      =   ""
      _Version        =   327682
      BeginProperty Panels {0713E89E-850A-101B-AFC0-4210102A8DA7} 
         NumPanels       =   3
         BeginProperty Panel1 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            AutoSize        =   1
            Object.Width           =   9710
            Text            =   "Status: Disconnected."
            TextSave        =   "Status: Disconnected."
            Object.Tag             =   ""
         EndProperty
         BeginProperty Panel2 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            Style           =   6
            AutoSize        =   2
            TextSave        =   "1/7/98"
            Object.Tag             =   ""
         EndProperty
         BeginProperty Panel3 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            Style           =   5
            AutoSize        =   2
            TextSave        =   "6:17 PM"
            Object.Tag             =   ""
         EndProperty
      EndProperty
   End
   Begin MSComDlg.CommonDialog dlgCommonDialog 
      Left            =   1320
      Top             =   4080
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   327680
   End
   Begin VB.Label lblRoomName 
      BackColor       =   &H00FFFF00&
      Height          =   255
      Left            =   4080
      TabIndex        =   5
      Top             =   120
      Width           =   1335
   End
   Begin VB.Label Label2 
      Caption         =   "Current Room:"
      Height          =   255
      Left            =   2880
      TabIndex        =   4
      Top             =   120
      Width           =   1095
   End
   Begin VB.Label Label1 
      Caption         =   "Nick Name:"
      Height          =   255
      Left            =   120
      TabIndex        =   3
      Top             =   120
      Width           =   855
   End
   Begin VB.Label lblNickName 
      BackColor       =   &H00FFFF00&
      Height          =   255
      Left            =   1080
      TabIndex        =   2
      Top             =   120
      Width           =   1455
   End
   Begin NMSTARTLibCtl.NMStart NMStart1 
      Left            =   240
      OleObjectBlob   =   "Main.frx":0000
      Top             =   4200
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr1 
      Left            =   720
      OleObjectBlob   =   "Main.frx":0024
      Top             =   4200
   End
   Begin ComctlLib.ImageList imlIcons 
      Left            =   1920
      Top             =   4080
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   12632256
      _Version        =   327682
      BeginProperty Images {0713E8C2-850A-101B-AFC0-4210102A8DA7} 
         NumListImages   =   13
         BeginProperty ListImage1 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "Main.frx":0048
            Key             =   ""
         EndProperty
         BeginProperty ListImage2 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "Main.frx":039A
            Key             =   ""
         EndProperty
         BeginProperty ListImage3 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "Main.frx":06EC
            Key             =   ""
         EndProperty
         BeginProperty ListImage4 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "Main.frx":0A3E
            Key             =   ""
         EndProperty
         BeginProperty ListImage5 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "Main.frx":0D90
            Key             =   ""
         EndProperty
         BeginProperty ListImage6 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "Main.frx":10E2
            Key             =   ""
         EndProperty
         BeginProperty ListImage7 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "Main.frx":1434
            Key             =   ""
         EndProperty
         BeginProperty ListImage8 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "Main.frx":1786
            Key             =   ""
         EndProperty
         BeginProperty ListImage9 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "Main.frx":1AD8
            Key             =   ""
         EndProperty
         BeginProperty ListImage10 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "Main.frx":1E2A
            Key             =   ""
         EndProperty
         BeginProperty ListImage11 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "Main.frx":217C
            Key             =   ""
         EndProperty
         BeginProperty ListImage12 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "Main.frx":24CE
            Key             =   ""
         EndProperty
         BeginProperty ListImage13 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "Main.frx":2820
            Key             =   ""
         EndProperty
      EndProperty
   End
   Begin VB.Menu mnuConnection 
      Caption         =   "&Connection"
      Begin VB.Menu mnuConnectionConnect 
         Caption         =   "Co&nnect"
      End
      Begin VB.Menu mnuConnectionDisconnect 
         Caption         =   "&Disconnect"
      End
   End
   Begin VB.Menu mnuRooms 
      Caption         =   "&Rooms"
      Begin VB.Menu mnuRoomsEnter 
         Caption         =   "&Enter a Room"
      End
      Begin VB.Menu mnuRoomsCreate 
         Caption         =   "&Create a New Room"
      End
      Begin VB.Menu mnuRoomsLeave 
         Caption         =   "&Leave Current Room"
      End
      Begin VB.Menu mnuRoomsList 
         Caption         =   "Room &List"
      End
   End
   Begin VB.Menu mnuRoom 
      Caption         =   "C&urrent Room"
      Begin VB.Menu mnuRoomProperties 
         Caption         =   "&Properties"
      End
      Begin VB.Menu mnuRoomClearHistory 
         Caption         =   "C&lear History"
      End
      Begin VB.Menu mnuRoomSeperator1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuRoomRules 
         Caption         =   "&Rules"
      End
      Begin VB.Menu mnuRoomWarnings 
         Caption         =   "&Warnings"
      End
      Begin VB.Menu mnuRoomBannedUsers 
         Caption         =   "&Banned Users"
      End
   End
   Begin VB.Menu mnuTranscript 
      Caption         =   "&Transcript"
      Begin VB.Menu mnuTranscriptStart 
         Caption         =   "&Start"
      End
      Begin VB.Menu mnuTranscriptStop 
         Caption         =   "Sto&p"
      End
      Begin VB.Menu mnuTranscriptOptions 
         Caption         =   "&Options"
      End
   End
   Begin VB.Menu mnuText 
      Caption         =   "&Prepared Text"
      Begin VB.Menu mnuTextView 
         Caption         =   "&View text"
      End
   End
   Begin VB.Menu mnuView 
      Caption         =   "&View"
      Begin VB.Menu mnuViewStatusBar 
         Caption         =   "Status &Bar"
         Checked         =   -1  'True
      End
      Begin VB.Menu mnuViewBar2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuViewHost 
         Caption         =   "&Host"
      End
      Begin VB.Menu mnuViewModerator 
         Caption         =   "&Moderator"
      End
      Begin VB.Menu mnuViewGuest 
         Caption         =   "Special &Guest"
         Checked         =   -1  'True
      End
   End
   Begin VB.Menu mnuHelp 
      Caption         =   "&Help"
      Begin VB.Menu mnuHelpAbout 
         Caption         =   "&About ExecChat2..."
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'****************************************************************************
'
'  Module:     Exec2.exe
'  File:       Main.frm
'
'  Copyright (c) Microsoft Corporation 1996-1998
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************

Option Explicit

Const strEnterRoom = "&Enter a Room"
Const strLeaveRoom = "Leave Room"
Const strConnect = "Connect"
Const strDisconnect = "Disconnect"

Public UserType As Integer

Public NickModerator As String
Public NickSpecialGuest As String


Private bStartingNetMeetingListen As Boolean
Private strNMPartnerNick As String


Private fRooms As frmRooms
Public fNewRoomProp As frmRoomProp




Public Sub SetUserType(utUserType As Integer)
    UserType = utUserType
    SetControlStates
End Sub






Private Sub SetControlStates()
    Dim bChannelOpen As Boolean
    Dim bConnected As Boolean
    Dim bLogged As Boolean
    
    bConnected = MsChatPr1.ConnectionState = csConnected
    bLogged = MsChatPr1.ConnectionState = csLogged
    
    bChannelOpen = bLogged And (ChatUI1.Channel1.ChannelState = chsOpen)
    
    ChatUI1.SetUserType UserType
    mnuViewHost.Checked = UserType = utHost
    mnuViewModerator.Checked = UserType = utModerator
    mnuViewGuest.Checked = UserType = utGuest
    
    mnuView.Visible = UserType = utHost
    'mnuRooms.Visible = UserType = utHost
    'mnuRoom.Visible = UserType = utHost
    mnuTranscript.Visible = (UserType = utHost) Or (UserType = utModerator)
    
    mnuTranscriptStart.Enabled = bChannelOpen
    mnuTranscriptStop.Enabled = bChannelOpen
    mnuTranscriptOptions.Enabled = bChannelOpen
    
    mnuRoomProperties.Enabled = bChannelOpen
    mnuRoomClearHistory.Enabled = bChannelOpen
    
    mnuRoomRules.Enabled = bChannelOpen And (Not UserType = utGuest)
    mnuRoomWarnings.Enabled = bChannelOpen And (Not UserType = utGuest)
    mnuRoomBannedUsers.Enabled = bChannelOpen And (Not UserType = utGuest)
    
    mnuRoomsEnter.Enabled = (Not bChannelOpen) And bLogged
    mnuRoomsCreate.Enabled = (Not bChannelOpen) And bLogged
    mnuRoomsLeave.Enabled = bChannelOpen
    mnuRoomsList.Enabled = bLogged
    
    mnuTextView.Enabled = bLogged
    
    mnuConnectionConnect.Enabled = MsChatPr1.ConnectionState = csDisconnected
    mnuConnectionDisconnect.Enabled = bConnected Or bLogged
    
End Sub




Public Sub StartNetMeetingWith(ByRef strNick As String)
    'Make sure NetMeeting is installed
    If NMStart1.NetMeetingInstalled = False Then
        If vbYes = MsgBox("NetMeeting is not installed on this machine. Do you wish to learn how to install NetMeeting?", vbYesNo) Then
            NMStart1.InstallNetMeeting
        End If
        Exit Sub
    End If
    
    If bStartingNetMeetingListen Then
        MsgBox "Please wait while NetMeeting is starting and try again later."
    Else
        If strNick = "" Then
            MsgBox "Please select the nickname of the other party in the text box next to the button and try again."
        Else
            'Start NetMeeting locally
            strNMPartnerNick = strNick
            bStartingNetMeetingListen = True
            NMStart1.StartNetMeetingListen
        End If
    End If
End Sub


Private Sub EnumRooms()
    fRooms.lblRoomToEnter.Visible = False
    fRooms.txtRoomName.Visible = False
    fRooms.cmdOK.Caption = "OK"
    
    fRooms.BeginEnumChannels
    fRooms.Show vbModal, Me
    
    fRooms.lblRoomToEnter.Visible = True
    fRooms.txtRoomName.Visible = True
    fRooms.cmdOK.Caption = "&Enter"
End Sub


Private Sub ShowRoomProp()
    Dim fRoomProp As New frmRoomProp
    
    fRoomProp.txtRoomName = ChatUI1.RoomName
    fRoomProp.bHost = (ChatUI1.Channel1.MemberModes And mmHost) Or (ChatUI1.Channel1.MemberModes And mmOwner)
    
    If fRoomProp.GetProperties = True Then
        fRoomProp.Show vbModal, Me
    End If
    
    Unload fRoomProp
End Sub





Private Sub Form_Load()
    Me.Left = GetSetting(App.Title, "Settings", "MainLeft", 1000)
    Me.Top = GetSetting(App.Title, "Settings", "MainTop", 1000)
    Me.width = GetSetting(App.Title, "Settings", "MainWidth", 6500)
    Me.height = GetSetting(App.Title, "Settings", "MainHeight", 6500)
    
    ChatUI1.Initialize 2, MsChatPr1   'in special guest mode by default
    SetControlStates
    
    Set fRooms = New frmRooms
    
End Sub


Private Sub Form_Unload(Cancel As Integer)
    Dim i As Integer


    'close all sub forms
    For i = Forms.count - 1 To 1 Step -1
        Unload Forms(i)
    Next
    
    ChatUI1.Destroy
    
    If Me.WindowState <> vbMinimized Then
        SaveSetting App.Title, "Settings", "MainLeft", Me.Left
        SaveSetting App.Title, "Settings", "MainTop", Me.Top
        SaveSetting App.Title, "Settings", "MainWidth", Me.width
        SaveSetting App.Title, "Settings", "MainHeight", Me.height
    End If
    
    Set fRooms = Nothing
End Sub


Private Sub Form_Resize()
    ' recaculate ChatUI size and position
    Dim width
    Dim height
    
    width = ScaleWidth - ChatUI1.Left - 120
    height = ScaleHeight - ChatUI1.Top - 500
    
    If (width < 0) Then
        width = 0
    End If
    
    If (height < 0) Then
        height = 0
    End If
    
    ChatUI1.width = width
    ChatUI1.height = height
    
End Sub




Private Sub mnuConnectionConnect_Click()
    Dim fLogin As New frmLogin
    fLogin.txtNick = lblNickName
    fLogin.bTryLogin = True
    fLogin.Show vbModal
    If Not fLogin.OK Then
        'User canceled
        Exit Sub
        End
    End If
    
    SetUserType fLogin.UserType
End Sub

Private Sub mnuConnectionDisconnect_Click()
    MsChatPr1.Disconnect
End Sub

Private Sub mnuHelpAbout_Click()
    frmAbout.Show vbModal, Me
End Sub



Private Sub mnuRoomBannedUsers_Click()
    ChatUI1.fBanUsers.Show vbModal, Me
End Sub

Private Sub mnuRoomProperties_Click()
    ShowRoomProp
End Sub

Private Sub mnuRoomClearHistory_Click()
    ChatUI1.ClearHistory
End Sub


Private Sub mnuRoomRules_Click()
    ChatUI1.ShowRules
End Sub

Private Sub mnuRoomsCreate_Click()
    Dim fRoomProp As New frmRoomProp
    fRoomProp.bCreate = True
    fRoomProp.bHost = True
    fRoomProp.txtRoomName = "#ExecChat2"
    
    fRoomProp.Show vbModal, Me
    
    If fRoomProp.bCreated Then
        lblRoomName = fRoomProp.txtRoomName
        Set fMainForm.fNewRoomProp = fRoomProp
        ChatUI1.EnterRoom fRoomProp.txtRoomName
    Else
        Unload fRoomProp
        Set fRoomProp = Nothing
    End If
End Sub

Private Sub mnuRoomsEnter_Click()
    fRooms.BeginEnumChannels
    fRooms.Show vbModal, Me

    If fRooms.bEnter = True Then
        ChatUI1.EnterRoom fRooms.txtRoomName
        lblRoomName = fRooms.txtRoomName
    End If
End Sub

Private Sub mnuRoomsLeave_Click()
    ChatUI1.LeaveRoom
    lblRoomName = ""
End Sub

Private Sub mnuRoomsList_Click()
    EnumRooms
End Sub

Private Sub mnuRoomWarnings_Click()
    ChatUI1.fWarnings.Show vbModal, Me
End Sub


Private Sub mnuTextView_Click()
    ChatUI1.ViewPreparedText
End Sub

Private Sub mnuTranscriptOptions_Click()
    ChatUI1.ChangeTranscriptOption
End Sub

Private Sub mnuTranscriptStart_Click()
    ChatUI1.StartTranscript
End Sub

Private Sub mnuTranscriptStop_Click()
    ChatUI1.StopTranscript
End Sub

Private Sub mnuViewGuest_Click()
    SetUserType utGuest
End Sub

Private Sub mnuViewHost_Click()
    SetUserType utHost
End Sub

Private Sub mnuViewModerator_Click()
    SetUserType utModerator
End Sub


Private Sub mnuViewStatusBar_Click()
    If mnuViewStatusBar.Checked Then
        sbStatusBar.Visible = False
        mnuViewStatusBar.Checked = False
    Else
        sbStatusBar.Visible = True
        mnuViewStatusBar.Checked = True
    End If
End Sub



Private Sub NMStart1_OnListenDone(ByVal Success As Boolean)
    bStartingNetMeetingListen = False
    If Success Then
        If MsChatPr1.ConnectionState = 4 Then
            MsChatPr1.SendPrivateMessage 2049, "NETMEET " + CStr(NMStart1.IPAddress), strNMPartnerNick
        Else
            MsgBox "You are not logged to a chat server and therefore cannot start a NetMeeting conference."
        End If
    Else
        MsgBox "NetMeeting could not be started locally."
    End If
End Sub


Private Sub NMStart1_OnConnectDone(ByVal Success As Boolean)
    If Success = False Then
        MsgBox "The NetMeeting conference couldn't be started successfully."
    End If
End Sub






















Function GetConnectionStateString(ByVal NewConnectionState As enumConnectionState) As String
    Select Case NewConnectionState
        Case 0
            GetConnectionStateString = "Disconnected."
        Case 1
            GetConnectionStateString = "Connecting..."
        Case 2
            GetConnectionStateString = "Connected."
        Case 3
            GetConnectionStateString = "Logging..."
        Case 4
            GetConnectionStateString = "Logged."
        Case 5
            GetConnectionStateString = "Disconnecting."
        Case Else
            GetConnectionStateString = "Unknown."
        
    End Select

End Function






Private Sub AppendHistory(ByVal strHistory As String, ByVal nIndent As Integer, _
                        ByVal nCR As Integer, Optional ByRef color As Variant = colorMessage)
    ChatUI1.AppendHistory strHistory, nIndent, nCR, color
End Sub


Private Sub LogEvent(ByRef strText)
    'fEvents.lstEvents.AddItem strText
    Debug.Print strText
End Sub


















'##########################################################################
'# MSChat Protocol OCX Events
'##########################################################################
Sub MsChatPr1_OnAddMember(ByVal Channel As Channel, ByVal NickName As String, ByVal MemberItems As ChatItems)
    LogEvent "  !!! OnAddMember " & NickName & " To: " & Channel.ChannelName
    
    If Channel.ChannelName = ChatUI1.RoomName Then
        ChatUI1.AddMember NickName, MemberItems
    End If
End Sub

Sub MsChatPr1_OnBeginEnumeration(ByVal Channel As Channel, ByVal EnumType As enumEnumType)
    LogEvent "Begin Enumeration " & EnumType
    LogEvent "----------------------------------------------------------"
   
    If EnumType = etInitialMembers Then
        ChatUI1.bEnumerationDone = False
    End If
End Sub

Sub MsChatPr1_OnChannelError(ByVal Channel As Channel, ByVal ErrorCode As Long, ByVal Description As String)
   MsgBox "OnChannelError " + Description + ", Err: " + CStr(ErrorCode)
   LogEvent "OnChannelError " + Description + ", Err: " + CStr(ErrorCode)
   LogEvent "----------------------------------------------------------"
End Sub

Sub MsChatPr1_OnChannelProperty(ByVal Channel As Channel, ByVal ChannelItems As ChatItems)
    LogEvent "  OnChannelProperty " & VPropertyFromChatItems(ChannelItems, "Name")
    fRooms.AddChannel ChannelItems
End Sub

Sub MsChatPr1_OnChannelPropertyChanged(ByVal Channel As Channel, ByVal ModifierNickname As String, _
        ByVal ChannelPropertyName As String, ByVal OldChannelProperty As Variant, ByVal NewChannelProperty As Variant)

    If ChannelPropertyName = cpnTopic Then
        If CStr(OldChannelProperty) = "" Then
            ' This event is fired when entering a channel with non-empty topic
            'MsgBox ModifierNickname + " has changed the " + ChannelPropertyName + " to '" + CStr(NewChannelProperty) + "'."
        Else
            If CStr(NewChannelProperty) = "" Then
                MsgBox ModifierNickname + " has changed the " + ChannelPropertyName + " from '" + CStr(OldChannelProperty) + "' to (no topic)."
            Else
                MsgBox ModifierNickname + " has changed the " + ChannelPropertyName + " from '" + CStr(OldChannelProperty) + "' to '" + CStr(NewChannelProperty) + "'."
            End If
        End If
    ElseIf ChannelPropertyName = cpnModes Then
        ChatUI1.OnChannelModeChange
    End If
End Sub

Sub MsChatPr1_OnChannelState(ByVal Channel As Channel, ByVal NewChannelState As enumChannelState)
    LogEvent "   OnChannelState: " & Channel.ChannelName & "  New State: " & NewChannelState
    LogEvent "----------------------------------------------------------"
    ChatUI1.OnChannelStateChange NewChannelState
    
    If NewChannelState = chsOpen Then
        If Not fNewRoomProp Is Nothing Then
            fNewRoomProp.ChangeChannelProperty
            Unload fNewRoomProp
            Set fNewRoomProp = Nothing
        End If
    End If
    
    ' may want to set status bar
    SetControlStates
End Sub

Sub MsChatPr1_OnConnectionError(ByVal ErrorCode As Long, ByVal Description As String, ByVal ChannelName As Variant)
    If ErrorCode = 3133 Then
        LogEvent "OnChannel Error " + Description + "Err:" + CStr(ErrorCode) + ")"
        Exit Sub
    End If
    
    Select Case VarType(ChannelName)
        Case 0:
            MsgBox "OnConnectionError: " + Description + " (Err:" + CStr(ErrorCode) + ")"
            LogEvent "OnConnectionError: " + Description + " (Err:" + CStr(ErrorCode) + ")"
        Case 8:
            MsgBox "OnConnectionError CN:" + ChannelName + ", " + Description + " (Err:" + CStr(ErrorCode) + ")"
            LogEvent "OnConnectionError CN:" + ChannelName + ", " + Description + " (Err:" + CStr(ErrorCode) + ")"
        Case Else
            MsgBox "OnConnectionError: " + Description + " (Err:" + CStr(ErrorCode) + ", UNEXPECTED ChannelName vartype!)"
            LogEvent "OnConnectionError CN:" + ChannelName + ", " + Description + " (Err:" + CStr(ErrorCode) + ")"
    End Select
    
    LogEvent "----------------------------------------------------------"
End Sub

Sub MsChatPr1_OnConnectionState(ByVal NewConnectionState As enumChannelState)
    Dim StrState As String
    
    StrState = GetConnectionStateString(NewConnectionState)
    sbStatusBar.Panels.item(1).Text = StrState
    
    LogEvent "---- OnConnectionState " & NewConnectionState
    
    SetControlStates
End Sub

Sub MsChatPr1_OnDelMember(ByVal Channel As Channel, ByVal NickName As String)
    LogEvent "!!! OnDelMember " & NickName & " from: " & Channel.ChannelName

    If Not Channel.ChannelName = ChatUI1.RoomName Then
        Exit Sub
    End If

    ChatUI1.DelMember NickName
End Sub

Sub MsChatPr1_OnEndEnumeration(ByVal Channel As Channel, ByVal EnumType As enumEnumType)
    LogEvent "----------------------------------------------------------"
    LogEvent "End Enumeration " & EnumType
    Select Case EnumType
        Case etChannels
            fRooms.EndEnumChannels
        Case etMembers, etInitialMembers, etUsers
            ChatUI1.EndEnumMembers
    End Select
End Sub

Sub MsChatPr1_OnInvitation(ByVal ChannelName As String, ByVal InviterItems As ChatItems)
    Dim NickName
    NickName = "Nickname"

    MsgBox CStr(VPropertyFromChatItems(InviterItems, CStr(NickName))) + " has invited you to the room '" + ChannelName + "'."
End Sub

Sub MsChatPr1_OnMemberKicked(ByVal Channel As Channel, ByVal KickedNickname As String, _
                             ByVal KickerNickname As String, ByVal Reason As String)
    Dim strMsg As String
    If Reason = "" Then
        strMsg = KickerNickname + " kicked " + KickedNickname + " out of the chat room."
    Else
        strMsg = KickerNickname + " kicked " + KickedNickname + " out of the chat room, saying '" + Reason + "'."
    End If
    
    AppendHistory strMsg, 0, 2, colorKick
    'MsgBox strMsg
    
End Sub

Sub MsChatPr1_OnMemberProperty(ByVal Channel As Channel, ByVal MemberItem As ChatItems)
    LogEvent "  OnMemberProperty: " & VPropertyFromChatItems(MemberItem, pnNickName) & " of " & Channel.ChannelName
    
    If Channel.ChannelName = ChatUI1.RoomName Then
        ChatUI1.AddMember VPropertyFromChatItems(MemberItem, pnNickName), MemberItem
    End If
End Sub

Sub MsChatPr1_OnMemberPropertyChanged(ByVal Channel As Channel, ByVal ModifiedNickname As String, _
                                      ByVal ModifierNickname As String, ByVal MemberPropertyName As String, _
                                      ByVal OldMemberProperty As Variant, ByVal NewMemberProperty As Variant)
    If MemberPropertyName = pnNickName Then
        Dim strMsg As String
        strMsg = CStr(OldMemberProperty) + " is now known as " + NewMemberProperty + "."
        MsgBox strMsg
        AppendHistory strMsg, 0, 2
        
        ChatUI1.OnNicknameChange OldMemberProperty, NewMemberProperty
    End If

    If MemberPropertyName = pnModes Then
        ModeChanges ModifiedNickname, OldMemberProperty, NewMemberProperty
    End If
End Sub

Sub ModeChanges(ModifiedNickname, OldMemberProperty, NewMemberProperty)
    If ((OldMemberProperty And mmNoVoice) And Not (NewMemberProperty And mmNoVoice)) Then
        MsgBox CStr(ModifiedNickname) + " has been made a speaker."
    End If
    
    If ((OldMemberProperty And mmVoice) And Not (NewMemberProperty And mmVoice)) Then
        MsgBox CStr(ModifiedNickname) + " has been made a spectator."
    End If
    
    If ((OldMemberProperty And 2048) And Not (NewMemberProperty And 2048)) Then
        ' MsgBox CStr(ModifiedNickname) + " has been made a host."
    End If
    
    ChatUI1.OnMemberModeChange "", ModifiedNickname, OldMemberProperty, NewMemberProperty
End Sub


Sub MsChatPr1_OnMessage(ByVal Channel As Channel, ByVal SenderNickname As String, ByVal MessageType As Long, _
                        ByVal Message As Variant, ByVal RecipientNicknames As Variant, ByVal DataMessageTag As Variant)
    
    Rem valid MessageType's:
    Rem     msgtNormal,msgtNotice,msgtData,msgtDataRequest,msgtDataReply,msgtDataRaw
    Rem     msgtWhisper,msgtThought,msgtAction,msgtSound,msgtCTCP,msgtInfo
    
    If ChatUI1.UserType = utModerator Or ChatUI1.UserType = utGuest Then
        Dim Mode
        Mode = VPropertyFromChatItems(ChatUI1.Channel1.MemberProperty(pnModes, CStr(SenderNickname)), pnModes)
        If Not (CBool(Mode And mmHost) Or CBool(Mode And mmOwner)) Then
            ' moderator and guest doesn't care about anything from the audience
            Exit Sub
        End If
    End If
    
    Select Case MessageType
        Case msgtNormal:
            AppendHistory SenderNickname + " says:", 0, 2, colorSay
            AppendHistory Message, 1, 1, colorMessage
        
        Case msgtNotice:
            AppendHistory SenderNickname + " says (as a notice):", 0, 2, colorSay
            AppendHistory Message, 1, 1, colorMessage
            
        Case msgtWhisper:
            ' ExtractRecipients RecipientNicknames
            Dim recpt
            Dim strRecipient As String
            strRecipient = ""
            For Each recpt In RecipientNicknames
                If strRecipient = "" Then
                    strRecipient = recpt
                Else
                    strRecipient = strRecipient + ", " + recpt
                End If
            Next
            
            AppendHistory SenderNickname + " whispers to " + strRecipient + ":", 0, 2, colorWhisper
            AppendHistory Message, 1, 1, colorMessage
            
        Case msgtThought:
            AppendHistory SenderNickname + " thinks:", 0, 2
            AppendHistory Message, 1, 1, colorMessage
            
        Case msgtAction:
            AppendHistory SenderNickname + " " + Message, 0, 2
        
        Case msgtSound:
            AppendHistory SenderNickname + " sends sound:", 0, 2
        
        Case msgtCTCP:
            AppendHistory SenderNickname + " sends CTCP:", 0, 2, colorWhisper
            AppendHistory Message, 1, 1, colorMessage
            
        Case Else
            Exit Sub
    End Select
    
    fMainForm.ChatUI1.fRoomRules.CheckRules SenderNickname, CStr(Message)
End Sub

Sub MsChatPr1_OnPrivateMessage(ByVal SenderNickname As String, ByVal PrivateMessageType As Long, _
                               ByVal Message As Variant, ByVal DataMessageTag As Variant)

    Rem valid PrivateMessageType's:
    Rem     pmtNormal,pmtNotice,pmtData,pmtDataRequest,pmtDataReply,pmtDataRaw,pmtAway
    Rem     pmtVersion,pmtLagTime,pmtLocalTime,pmtAction,pmtSound,pmtCTCP,pmtInfo

    Select Case PrivateMessageType
        Case pmtAway:
            AppendHistory SenderNickname + " is away: " + Message, 0, 2
        
        Case pmtVersion:
            AppendHistory SenderNickname + " asked for version information.", 0, 2
        
        Case pmtLagTime:
            AppendHistory SenderNickname + " asked for lag time.", 0, 2
        
        Case pmtLocalTime:
            AppendHistory SenderNickname + " asked for local time.", 0, 2
        
        Case pmtNormal, pmtNotice, pmtAction, pmtInfo, pmtCTCP:
            AppendHistory SenderNickname + " sends private message:", 0, 2
            AppendHistory Message, 1, 1
        
        
        Case 2049:
            If Left(Message, 7) = "NETMEET" Then
                Dim strParam
                strParam = Mid(Message, 9)
                Select Case strParam
                    Case "REFUSED":
                        MsgBox "The other party refused to open a NetMeeting conference."
                    Case "NOHAVE":
                        MsgBox "The other party does not have NetMeeting properly installed."
                    Case Else
                        If NMStart1.AcceptNetMeetingCalls = False Then
                            MsChatPr1.SendPrivateMessage 2049, "NETMEET REFUSED", SenderNickname
                        Else
                            If NMStart1.NetMeetingInstalled = False Then
                                MsChatPr1.SendPrivateMessage 2049, "NETMEET NOHAVE", SenderNickname
                            Else
                                If MsgBox(SenderNickname + " would like to have a NetMeeting conversation with you. Do you accept?", 4) = 6 Then
                                    NMStart1.OpenNetMeetingConference strParam
                                Else
                                    MsChatPr1.SendPrivateMessage 2049, "NETMEET REFUSED", SenderNickname
                                End If
                            End If
                        End If
                End Select
                
            ElseIf Left(Message, 8) = "EXECCHAT" Then
                Dim Mode
                Mode = VPropertyFromChatItems(ChatUI1.Channel1.MemberProperty(pnModes, CStr(SenderNickname)), pnModes)
                If Not (CBool(Mode And mmHost) Or CBool(Mode And mmOwner)) Then
                    MsgBox SenderNickname & " sends bogus ExecChat message: " & Message
                    Exit Sub
                End If
                
                strParam = Mid(Message, 10)
                
                If "+MODERATOR " = Left(strParam, 11) Then
                    ChatUI1.OnAddModerator Mid(strParam, 12)
                
                ElseIf "-MODERATOR " = Left(strParam, 11) Then
                    ChatUI1.OnRemoveModerator Mid(strParam, 12)
                
                ElseIf "+GUEST " = Left(strParam, 7) Then
                    ChatUI1.OnAddGuest Mid(strParam, 8)
                    
                ElseIf "-GUEST " = Left(strParam, 7) Then
                    ChatUI1.OnRemoveGuest Mid(strParam, 8)
                
                ElseIf "TOMODERATOR " = Left(strParam, 12) Then
                    ' Forwarding notification
                    If ChatUI1.UserType = utHost Then
                        Dim Question As String
                        Question = Mid(strParam, 13)
                        ChatUI1.OnQuestionForwardedToModerator Question
                    End If
                    
                ElseIf "TOGUEST " = Left(strParam, 8) Then
                    ' Forwarding notification
                    If ChatUI1.UserType = utModerator Then
                        Question = Mid(strParam, 9)
                        ChatUI1.OnQuestionForwardedToGuest Question
                    End If
                                        
                Else
                    MsgBox "Unknown ExecChat private message: " & Message
                End If
                
            Else
                AppendHistory SenderNickname + " sends private message:", 0, 2
                AppendHistory Message, 1, 1
                
            End If
      
        
        Case Else   ' data and sound
            Exit Sub
    End Select
    

    fMainForm.ChatUI1.fRoomRules.CheckRules SenderNickname, CStr(Message)
    
End Sub

Sub MsChatPr1_OnProtocolMessage1(ByVal MessagePrefix As String, ByVal MessageCommand As String, _
                                 ByVal MessageParameters As Variant, ByVal EnableDefault As Boolean)
    Dim i
    Dim strParams
    strParams = ""

    Select Case VarType(MessageParameters)
        Case 8:
            strParams = CStr(MessageParameters)
        Case 8204:
            For i = LBound(MessageParameters) To UBound(MessageParameters)
                strParams = strParams + "<" + CStr(MessageParameters(i)) + "> "
            Next
        Case Else
            strParams = "UNEXPECTED VARIANT TYPE"
    End Select
    LogEvent "OnProtocolMessage==> Prefix: " + MessagePrefix + " Command: " + MessageCommand + " Paramaters: " + strParams
    'MsgBox "OnProtocolMessage==> Prefix: " + MessagePrefix + " Command: " + MessageCommand + " Parameters: " + strParams

End Sub

Sub MsChatPr1_OnServerProperty(ByVal ServerItems As ChatItems)
End Sub

Sub MsChatPr1_OnServerTextMessage(ByVal ServerMessageType As enumServerMessageType, ByVal Text As String)
    Rem valid server message types are:
    Rem     smtNormal,smtNotice,smtMessageOfTheDay,smtServerInfo,smtError
    
    Select Case ServerMessageType
        Case smtNormal:
            LogEvent "Normal Server Message: " + Text
            LogEvent "----------------------------------------------------------"
        Case smtNotice:
            LogEvent "Notice Server Message: " + Text
            LogEvent "----------------------------------------------------------"
        Case smtMessageOfTheDay:
            LogEvent "Message of the Day: " + Text
            LogEvent "----------------------------------------------------------"
        Case smtServerInfo:
            LogEvent "Server Info: " + Text
            LogEvent "----------------------------------------------------------"
        Case smtError:
            LogEvent "Server Error Message: " + Text
            LogEvent "----------------------------------------------------------"
        Case Else
            LogEvent "Unknown server message: " + Text
            LogEvent "----------------------------------------------------------"
    End Select
End Sub

Sub MsChatPr1_OnUserProperty(ByVal UserItem As ChatItems)
    LogEvent "  OnUserProperty: " & VPropertyFromChatItems(UserItem, "NickName")
    ChatUI1.AddMember "", UserItem
End Sub

Sub MsChatPr1_OnUserPropertyChanged(ByVal ModifiedNickname As String, ByVal ModifierNickname As String, _
                                    ByVal UserPropertyName As String, ByVal OldUserProperty As Variant, _
                                    ByVal NewUserProperty As Variant)
    
    LogEvent ModifierNickname & " changed " & ModifiedNickname & "'s " & UserPropertyName & _
        " property from " & OldUserProperty & " to " & NewUserProperty
End Sub

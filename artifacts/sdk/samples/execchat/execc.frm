VERSION 4.00
'****************************************************************************
'
'  Module:     ExecChat.EXE
'  File:       Execc.frm
'
'  Copyright (c) Microsoft Corporation 1996-1997
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************
Begin VB.Form ExecC 
   BackColor       =   &H00FFFFC0&
   Caption         =   "Executive Chat Monitoring Tool"
   ClientHeight    =   8310
   ClientLeft      =   990
   ClientTop       =   405
   ClientWidth     =   8850
   Height          =   8715
   Left            =   930
   LinkTopic       =   "ExecC"
   ScaleHeight     =   8310
   ScaleWidth      =   8850
   Top             =   60
   Width           =   8970
   Begin VB.CheckBox chkAutoPaste 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Paste Clipboard Content into Send text boxes?"
      Height          =   615
      Left            =   3840
      TabIndex        =   27
      Top             =   720
      Width           =   2055
   End
   Begin VB.ListBox lstPartID 
      Enabled         =   0   'False
      Height          =   255
      Left            =   7920
      TabIndex        =   22
      Top             =   120
      Visible         =   0   'False
      Width           =   855
   End
   Begin VB.Frame frmGuest 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Special Guest Functions"
      Enabled         =   0   'False
      Height          =   615
      Left            =   120
      TabIndex        =   17
      Top             =   3600
      Width           =   8655
      Begin VB.CommandButton cmdGWhisperToHosts 
         Caption         =   "Whisper To Hosts"
         Enabled         =   0   'False
         Height          =   255
         Left            =   4560
         TabIndex        =   29
         Top             =   240
         Width           =   1935
      End
      Begin VB.CommandButton cmdGWhisperToModerator 
         Caption         =   "Whisper To Moderator"
         Enabled         =   0   'False
         Height          =   255
         Left            =   6600
         TabIndex        =   23
         Top             =   240
         Width           =   1935
      End
   End
   Begin VB.Frame frmModerator 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Moderator Functions"
      Enabled         =   0   'False
      Height          =   975
      Left            =   120
      TabIndex        =   16
      Top             =   2520
      Visible         =   0   'False
      Width           =   8655
      Begin VB.CommandButton cmdMWhisperToHosts 
         Caption         =   "Whisper To Hosts"
         Enabled         =   0   'False
         Height          =   255
         Left            =   6720
         TabIndex        =   28
         Top             =   240
         Width           =   1815
      End
      Begin VB.CommandButton cmdMSendToEntireRoom 
         Caption         =   "Send To Entire Room"
         Enabled         =   0   'False
         Height          =   615
         Left            =   4680
         TabIndex        =   21
         Top             =   240
         Width           =   1935
      End
      Begin VB.CommandButton cmdWhisperToGuest 
         Caption         =   "Whisper To Guest"
         Enabled         =   0   'False
         Height          =   255
         Left            =   6720
         TabIndex        =   20
         Top             =   600
         Width           =   1815
      End
      Begin VB.CommandButton cmdSetGuestID 
         Caption         =   "Identify Special Guest"
         Enabled         =   0   'False
         Height          =   255
         Left            =   120
         TabIndex        =   18
         Top             =   240
         Width           =   1815
      End
      Begin VB.Label lblGuestAlias 
         BackColor       =   &H00FFFF00&
         Caption         =   "<Special Guest Alias>"
         Height          =   255
         Left            =   2040
         TabIndex        =   19
         Top             =   240
         Width           =   2415
      End
   End
   Begin VB.Frame frmHost 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Host Functions"
      Enabled         =   0   'False
      Height          =   975
      Left            =   120
      TabIndex        =   12
      Top             =   1440
      Visible         =   0   'False
      Width           =   8655
      Begin VB.CommandButton cmdHWhisperToHosts 
         Caption         =   "Whisper To Hosts"
         Enabled         =   0   'False
         Height          =   255
         Left            =   6720
         TabIndex        =   26
         Top             =   240
         Width           =   1815
      End
      Begin VB.CheckBox chkSpeakerView 
         BackColor       =   &H00FFFFC0&
         Caption         =   "Enable Send and Whisper to Room Features?"
         Height          =   255
         Left            =   120
         TabIndex        =   25
         Top             =   600
         Width           =   3855
      End
      Begin VB.CommandButton cmdShowHostPanel 
         Caption         =   "Show Host Panel"
         Enabled         =   0   'False
         Height          =   255
         Left            =   6720
         TabIndex        =   24
         Top             =   600
         Width           =   1815
      End
      Begin VB.CommandButton cmdHWhisperToModerator 
         Caption         =   "Whisper To Moderator"
         Enabled         =   0   'False
         Height          =   615
         Left            =   4680
         TabIndex        =   15
         Top             =   240
         Width           =   1935
      End
      Begin VB.CommandButton cmdSetModeratorID 
         Caption         =   "Identify Moderator"
         Enabled         =   0   'False
         Height          =   255
         Left            =   120
         TabIndex        =   13
         Top             =   240
         Width           =   1575
      End
      Begin VB.Label lblModeratorAlias 
         BackColor       =   &H00FFFF00&
         Caption         =   "<Moderator Alias>"
         Height          =   255
         Left            =   1800
         TabIndex        =   14
         Top             =   240
         Width           =   2175
      End
   End
   Begin VB.CommandButton cmdClearHistory 
      Caption         =   "Clear History"
      Height          =   375
      Left            =   6000
      TabIndex        =   11
      Top             =   840
      Width           =   1215
   End
   Begin VB.Frame Frame1 
      BackColor       =   &H00FFFF00&
      Caption         =   "User Mode"
      Height          =   615
      Left            =   120
      TabIndex        =   7
      Top             =   720
      Width           =   3630
      Begin VB.OptionButton optGuest 
         BackColor       =   &H00FFFF00&
         Caption         =   "Special Guest"
         Height          =   255
         Left            =   2160
         TabIndex        =   10
         Top             =   240
         Value           =   -1  'True
         Width           =   1320
      End
      Begin VB.OptionButton optModerator 
         BackColor       =   &H00FFFF00&
         Caption         =   "Moderator"
         Height          =   255
         Left            =   960
         TabIndex        =   9
         Top             =   240
         Width           =   1455
      End
      Begin VB.OptionButton optHost 
         BackColor       =   &H00FFFF00&
         Caption         =   "Host"
         Height          =   255
         Left            =   120
         TabIndex        =   8
         Top             =   240
         Width           =   735
      End
   End
   Begin VB.CommandButton cmdFlux 
      Caption         =   "Enter Chat"
      Height          =   375
      Left            =   7320
      TabIndex        =   6
      Top             =   840
      Width           =   1455
   End
   Begin ComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   255
      Left            =   0
      TabIndex        =   31
      Top             =   8055
      Width           =   8850
      _Version        =   65536
      _ExtentX        =   15610
      _ExtentY        =   450
      _StockProps     =   68
      AlignSet        =   -1  'True
      SimpleText      =   ""
      NumPanels       =   3
      i1              =   "execc.frx":0000
      i2              =   "execc.frx":00FD
      i3              =   "execc.frx":0202
   End
   Begin MSChatObjectsCtl.MSChat MSChat 
      Height          =   3615
      Left            =   120
      TabIndex        =   30
      Top             =   4320
      Width           =   8655
      _ExtentX        =   15266
      _ExtentY        =   6376
      BorderStyle     =   0
      UIOption        =   4093
      BackColor       =   192
   End
   Begin VB.Label lblRoomTopic 
      BackColor       =   &H00FFFF00&
      Height          =   255
      Left            =   3840
      TabIndex        =   5
      Top             =   360
      Width           =   4935
   End
   Begin VB.Label lblUserName 
      BackColor       =   &H00FFFF00&
      Height          =   255
      Left            =   2040
      TabIndex        =   4
      Top             =   360
      Width           =   1695
   End
   Begin VB.Label lblAlias 
      BackColor       =   &H00FFFF00&
      Height          =   255
      Left            =   120
      TabIndex        =   3
      Top             =   360
      Width           =   1815
   End
   Begin VB.Label Label10 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Name"
      Height          =   255
      Left            =   2040
      TabIndex        =   2
      Top             =   120
      Width           =   1695
   End
   Begin VB.Label Label8 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Room Topic"
      Height          =   255
      Left            =   3840
      TabIndex        =   1
      Top             =   120
      Width           =   4935
   End
   Begin VB.Label Label7 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Alias"
      Height          =   255
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   1815
   End
End
Attribute VB_Name = "ExecC"
Attribute VB_Creatable = False
Attribute VB_Exposed = False


Option Explicit



Private Sub chkSpeakerView_Click()
    On Error GoTo errSpeakerView
    If (chkSpeakerView.Value = 1) Then
        MSChat.UIOption = MSChat.UIOption + 19
    Else
        MSChat.UIOption = MSChat.UIOption - 19
    End If
    Exit Sub
    
errSpeakerView:
    DisplayError "chkSpeakerView_Click"
End Sub

Private Sub cmdFlux_Click()
    On Error GoTo errFlux
    
    Select Case (MSChat.State)
        Case 1: 'disconnected
            EnterOptions.Show
        Case 2: 'connecting
            MSChat.CancelEntering
            WriteLogFile "Cancelling EnterRoom operation..."
        Case 3: 'connected
            MSChat.ExitRoom
            WriteLogFile "Exiting Room."
    End Select
    Exit Sub
    
errFlux:
    DisplayError "Flux_Click"
End Sub


Sub GetMemberCount()
    On Error Resume Next
    StatusBar.Panels(1).Text = CStr(MSChat.MemberCount) + " Members"
End Sub




Sub SetState(NewState As Integer)
    On Error GoTo errSetState
    
    Select Case NewState
    Case 1:
        lModeratorID = 0
        WriteLogFile "lModeratorID set to 0 in SetState(1)"
        lGuestID = 0
        WriteLogFile "lGuestID set to 0 in SetState(1)"
        nRealNameType = 0
        
        lstPartID.Clear
        WhispToHosts.lstHosts.Clear
        
        lblModeratorAlias.Caption = "<Moderator Alias>"
        lblGuestAlias.Caption = "<Special Guest Alias>"
        
        cmdFlux.Caption = "Enter Room"
        StatusBar.Panels(1).Text = "0 Members"
        StatusBar.Panels(2).Text = "Disconnected"
        StatusBar.Panels(3).Text = "Not in a room"
        lblAlias.Caption = ""
        lblUserName.Caption = ""
        lblRoomTopic.Caption = ""
        
        MSChat.BackColor = &HC0&
        
        optHost.Enabled = True
        optModerator.Enabled = True
        optGuest.Enabled = True
        
        frmHost.Enabled = False
        frmModerator.Enabled = False
        frmGuest.Enabled = False
        
        cmdSetModeratorID.Enabled = False
        cmdSetGuestID.Enabled = False
        
        cmdGWhisperToHosts.Enabled = False
        cmdMWhisperToHosts.Enabled = False
        cmdHWhisperToHosts.Enabled = False
        
        cmdGWhisperToModerator.Enabled = False
        cmdHWhisperToModerator.Enabled = False
        cmdWhisperToGuest.Enabled = False
        
        cmdMSendToEntireRoom.Enabled = False
        cmdShowHostPanel.Enabled = False
        WriteLogFile ""

    Case 2:
        cmdFlux.Caption = "Cancel entering"
        StatusBar.Panels(2).Text = "Connecting..."
        MSChat.BackColor = &H80FF&
        optHost.Enabled = False
        optModerator.Enabled = False
        optGuest.Enabled = False
    
    Case 3:
        cmdFlux.Caption = "Leave Room"
        StatusBar.Panels(2).Text = "Connected"
        MSChat.BackColor = &HC000&
        
        frmHost.Enabled = True
        frmModerator.Enabled = True
        frmGuest.Enabled = True
        
        cmdSetModeratorID.Enabled = True
        cmdSetGuestID.Enabled = True
        
        cmdGWhisperToHosts.Enabled = True
        cmdMWhisperToHosts.Enabled = True
        cmdHWhisperToHosts.Enabled = True
        
        cmdMSendToEntireRoom.Enabled = True
        cmdShowHostPanel.Enabled = True
    End Select
    
    Exit Sub

errSetState:
    DisplayError "SetState"
End Sub


Private Sub cmdGWhisperToHosts_Click()
    WhispToHosts.Show
    If (chkAutoPaste.Value = 1) Then
        WhispToHosts.txtMessage.Text = Clipboard.GetText()
    End If
End Sub

Private Sub cmdGWhisperToModerator_Click()
    On Error Resume Next
    WhispToMod.Show
    If (chkAutoPaste.Value = 1) Then
        WhispToMod.txtMessage.Text = Clipboard.GetText()
    End If
End Sub



Private Sub cmdHWhisperToHosts_Click()
    WhispToHosts.Show
    If (chkAutoPaste.Value = 1) Then
        WhispToHosts.txtMessage.Text = Clipboard.GetText()
    End If
End Sub

Private Sub cmdMSendToEntireRoom_Click()
    SendToRoom.Show
    If (chkAutoPaste.Value = 1) Then
        SendToRoom.txtMessage.Text = Clipboard.GetText()
    End If
End Sub


Private Sub cmdMWhisperToHosts_Click()
    WhispToHosts.Show
    If (chkAutoPaste.Value = 1) Then
        WhispToHosts.txtMessage.Text = Clipboard.GetText()
    End If
End Sub

Private Sub cmdSetGuestID_Click()
    Dim txtRealName As String
    
    On Error GoTo errSetGuestID
    
    nRealNameType = RN_GUEST
    MSChat.GetParticipantRealName -1, "", False, txtRealName
    Exit Sub
    
errSetGuestID:
    DisplayError "cmdSetGuestID_Click"
End Sub

Private Sub cmdSetModeratorID_Click()
    Dim txtRealName As String
    
    On Error GoTo errSetModeratorID
    
    nRealNameType = RN_MODERATOR
    MSChat.GetParticipantRealName -1, "", False, txtRealName
    Exit Sub
    
errSetModeratorID:
    DisplayError "cmdSetModeratorID"
End Sub

Private Sub cmdShowHostPanel_Click()
    On Error Resume Next
    HostPanel.Show
End Sub

Private Sub cmdWhisperToGuest_Click()
    WhispToGuest.Show
    If (chkAutoPaste.Value = 1) Then
        WhispToGuest.txtMessage.Text = Clipboard.GetText()
    End If
End Sub

Private Sub cmdHWhisperToModerator_Click()
    On Error Resume Next
    WhispToMod.Show
    If (chkAutoPaste.Value = 1) Then
        WhispToMod.txtMessage.Text = Clipboard.GetText()
    End If
End Sub



Private Sub Form_Load()
    MSChat.Top = MSCHAT_TOP
   
    frmHost.Top = FUNCTIONS_TOP
    frmModerator.Top = FUNCTIONS_TOP
    frmGuest.Top = FUNCTIONS_TOP
    lModeratorID = 0
    lGuestID = 0
    nRealNameType = 0
    Load WhispToHosts
    
    OpenLogFile
End Sub

Private Sub Form_Resize()
    On Error Resume Next
    MSChat.Height = ExecC.Height - StatusBar.Height - MSChat.Top - 500
    MSChat.Width = ExecC.Width - 350
End Sub


Private Sub Form_Unload(Cancel As Integer)
    Unload EnterOptions
    Unload HostPanel
    Unload KickBan
    Unload UISettings
    Unload WhispToMod
    Unload WhispToGuest
    Unload WhispToHosts
    Unload SendToRoom
    
    CloseLogFile
End Sub













Private Sub cmdClearHistory_Click()
    On Error Resume Next
    MSChat.ClearHistory
End Sub







































Private Sub MSChat_OnEndEnumeration()
    On Error GoTo errOnEndEnumeration
    
    ' For the Moderator and the Guest, ignore all none-host members
    If (Not optHost.Value) Then
        Dim nCnt As Integer
        Dim nStatus As Integer
        Dim nAliasLen As Integer
    
        For nCnt = 0 To lstPartID.ListCount - 1
            nAliasLen = InStr(lstPartID.List(nCnt), ",") - 1
            nStatus = CInt(Mid(lstPartID.List(nCnt), nAliasLen + 2))
            If (((nStatus And STAT_PARTICIPANT) Or (nStatus And STAT_SPECTATOR)) And (lstPartID.ItemData(nCnt) <> MSChat.ThisParticipantID)) Then
                MSChat.SetParticipantStatus lstPartID.ItemData(nCnt), MASK_IGNORED, STAT_IGNORED
            End If
        Next nCnt
    End If
    
    lblRoomTopic.Caption = MSChat.RoomTopic
    lblUserName.Caption = MSChat.ThisParticipantName
    StatusBar.Panels(3).Text = MSChat.RoomPath
    
    GetMemberCount
    Exit Sub
    
errOnEndEnumeration:
    DisplayError "OnEndEnumeration"
End Sub

Private Sub MSChat_OnEnterParticipant(ByVal ParticipantID As Long, ByVal Alias As String, ByVal Status As Integer)
    On Error GoTo errOnEnterParticipant
    
    GetMemberCount
    
    lstPartID.AddItem Alias + "," + CStr(Status)
    lstPartID.ItemData(lstPartID.NewIndex) = ParticipantID
    
    If (Status And STAT_HOST) Then
        WhispToHosts.lstHosts.AddItem Alias
        WhispToHosts.lstHosts.ItemData(WhispToHosts.lstHosts.NewIndex) = ParticipantID
    End If
    
    If ((MSChat.ThisParticipantID <> 0) And (optGuest.Value Or optModerator.Value) And ((Status And STAT_PARTICIPANT) Or (Status And STAT_SPECTATOR))) Then
        ' Guest and Moderator ignore anybody that is not a host
        MSChat.SetParticipantStatus ParticipantID, MASK_IGNORED, STAT_IGNORED
    End If
    
    Exit Sub
    
errOnEnterParticipant:
    DisplayError "OnEnterParticipant"
End Sub


Private Sub MSChat_OnExitParticipant(ByVal ParticipantID As Long)
    
    On Error GoTo errOnExitPart
    
    GetMemberCount
    
    Dim nLstPos As Integer
   
    nLstPos = nFindParticipantIndex(ParticipantID)
    If nLstPos >= 0 Then
        lstPartID.RemoveItem nLstPos
    Else
        WriteLogFile "Couldn't find participant in OnExitParticipant(" + CStr(ParticipantID) + ")"
    End If
    
    nLstPos = nFindHostIndex(ParticipantID)
    If nLstPos >= 0 Then
        WhispToHosts.lstHosts.RemoveItem nLstPos
    End If
    
    If lModeratorID = ParticipantID Then
        lModeratorID = 0
        WriteLogFile "lModeratorID set to 0 in OnExitParticipant because Moderator left the room."
        lblModeratorAlias.Caption = "<Moderator Alias>"
        cmdHWhisperToModerator.Enabled = False
        cmdGWhisperToModerator.Enabled = False
    End If
    If lGuestID = ParticipantID Then
        lGuestID = 0
        WriteLogFile "lGuestID set to 0 in OnExitParticipant because Guest left the room."
        lblGuestAlias.Caption = "<Guest Alias>"
        cmdWhisperToGuest.Enabled = False
    End If
    
    Exit Sub
    
errOnExitPart:
    DisplayError "OnExitParticipant"
End Sub

Private Function nFindParticipantIndex(ParticipantID As Long) As Integer
    Dim nCnt As Integer

    On Error GoTo errFindPartIndex
    
    nFindParticipantIndex = -1
    For nCnt = 0 To lstPartID.ListCount - 1
        If lstPartID.ItemData(nCnt) = ParticipantID Then
            nFindParticipantIndex = nCnt
            Exit For
        End If
    Next nCnt
    Exit Function
    
errFindPartIndex:
    DisplayError "nFindPArticipantIndex"
End Function

Private Function nFindHostIndex(ParticipantID As Long) As Integer
    Dim nCnt As Integer

    On Error GoTo errFindHostIndex
    
    nFindHostIndex = -1
    For nCnt = 0 To WhispToHosts.lstHosts.ListCount - 1
        If WhispToHosts.lstHosts.ItemData(nCnt) = ParticipantID Then
            nFindHostIndex = nCnt
            Exit For
        End If
    Next nCnt
    Exit Function
    
errFindHostIndex:
    DisplayError "nFindHostIndex"
End Function


Private Function lFindParticipantID(Alias As String) As Long
    Dim nCnt As Integer
    Dim nAliasLen As Integer

    On Error GoTo errFindPartID
    
    lFindParticipantID = 0
    For nCnt = 0 To lstPartID.ListCount - 1
        nAliasLen = InStr(lstPartID.List(nCnt), ",") - 1
        If Left(lstPartID.List(nCnt), nAliasLen) = Alias Then
            lFindParticipantID = lstPartID.ItemData(nCnt)
            Exit For
        End If
    Next nCnt
    Exit Function
    
errFindPartID:
    DisplayError "lFindParticipantID"
End Function

Private Function nFindStatus(Alias As String) As Integer
    Dim nCnt As Integer
    Dim nAliasLen As Integer

    On Error GoTo errFindStatus
    
    nFindStatus = 0
    For nCnt = 0 To lstPartID.ListCount - 1
        nAliasLen = InStr(lstPartID.List(nCnt), ",") - 1
        If Left(lstPartID.List(nCnt), nAliasLen) = Alias Then
            nFindStatus = CInt(Mid(lstPartID.List(nCnt), nAliasLen + 2))
            Exit For
        End If
    Next nCnt
    Exit Function
    
errFindStatus:
    DisplayError "nFindStatus"
End Function
Private Sub MSChat_OnMessage(ByVal SenderID As Long, ByVal Message As Variant, ByVal MessageType As Integer)
    On Error GoTo errOnMessage
    
    If (optGuest.Value) Then
        If (VarType(Message) = vbInteger) Then
            If (Message = 1984) Then
                lModeratorID = SenderID
                WriteLogFile "lModeratorID set to " + CStr(SenderID) + " in OnMessage handler."
                cmdGWhisperToModerator.Enabled = True
            End If
        End If
    End If
    
    Exit Sub
    
errOnMessage:
    DisplayError "OnMessage"
End Sub

Private Sub MSChat_OnParticipantAliasChanged(ByVal ParticipantID As Long, ByVal OldAlias As String, ByVal NewAlias As String)
    On Error GoTo errOnPartAliasChanged
        
    Dim nLstPos As Integer
        
    nLstPos = nFindParticipantIndex(ParticipantID)
    If (nLstPos <> -1) Then
        lstPartID.List(nLstPos) = NewAlias + "," + Mid(lstPartID.List(nLstPos), Len(OldAlias) + 2)
    Else
        WriteLogFile "Couldn't find participant in OnParticipantAliasChanged(" + CStr(ParticipantID) + "," + OldAlias + "," + NewAlias + ")"
    End If
    
    nLstPos = nFindHostIndex(ParticipantID)
    If nLstPos >= 0 Then
        WhispToHosts.lstHosts.RemoveItem nLstPos
        WhispToHosts.lstHosts.AddItem NewAlias
        WhispToHosts.lstHosts.ItemData(WhispToHosts.lstHosts.NewIndex) = ParticipantID
    End If
    
    Exit Sub
    
errOnPartAliasChanged:
    DisplayError "OnParticipantAliasChanged"
End Sub

Private Sub MSChat_OnParticipantRealName(ByVal Alias As String, ByVal RealName As String)
    
    On Error GoTo errOnPartRealName
    
    Select Case nRealNameType
    Case RN_MODERATOR:
        lModeratorID = lFindParticipantID(Alias)
        If (lModeratorID <> 0) Then
            If (lModeratorID <> MSChat.ThisParticipantID) Then
                WriteLogFile "lModeratorID set to " + CStr(lModeratorID) + " (" + Alias + ") in OnParticipantRealName handler."
                lblModeratorAlias.Caption = Alias
                cmdHWhisperToModerator.Enabled = True
            Else
                MsgBox "You cannot select yourself as the Moderator. Please select another member.", vbOKOnly, "Executive Chat Monitoring Tool"
                lModeratorID = 0
            End If
        Else
            WriteLogFile "Couldn't find Moderator in OnParticipantRealName(" + Alias + "," + RealName + ")"
        End If
        
    Case RN_GUEST:
        lGuestID = lFindParticipantID(Alias)
        If (lGuestID <> 0) Then
            If (lGuestID <> MSChat.ThisParticipantID) Then
                cmdWhisperToGuest.Enabled = True
                lblGuestAlias.Caption = Alias
                MSChat.SendMessage lGuestID, 1, CVar(1984)
            Else
                MsgBox "You cannot select yourself as the Special Guest. Please select another member.", vbOKOnly, "Executive Chat Monitoring Tool"
                lGuestID = 0
            End If
        Else
            WriteLogFile "Couldn't find Guest in OnParticipantRealName(" + Alias + "," + RealName + ")"
        End If
    
    Case RN_PARTICIPANT:
        HostPanel.lblAsyncAlias.Caption = Alias
        HostPanel.lblAsyncRealName.Caption = RealName
    End Select
    
    Exit Sub

errOnPartRealName:
    DisplayError "OnParticipantRealName"
End Sub


Private Sub MSChat_OnParticipantStatusChanged(ByVal ParticipantID As Long, ByVal Status As Integer)
        
    On Error GoTo errOnPartStatusChanged
    
    Dim nLstPos As Integer
    Dim nAliasLen As Integer
    Dim strAlias As String
   
    nLstPos = nFindParticipantIndex(ParticipantID)
    If nLstPos >= 0 Then
        nAliasLen = InStr(lstPartID.List(nLstPos), ",") - 1
        If (nAliasLen) Then
            strAlias = Left(lstPartID.List(nLstPos), nAliasLen)
            lstPartID.List(nLstPos) = strAlias + "," + CStr(Status)
        End If
    Else
        WriteLogFile "Couldn't find participant in OnParticipantStatusChanged(" + CStr(ParticipantID) + "," + CStr(Status) + ")"
    End If
    
    nLstPos = nFindHostIndex(ParticipantID)
    If (nLstPos >= 0) And (Not (Status And STAT_HOST)) Then
        WhispToHosts.lstHosts.RemoveItem nLstPos
    End If
    
    If (Status And STAT_HOST) And (nLstPos = -1) Then
        WhispToHosts.lstHosts.AddItem strAlias
        WhispToHosts.lstHosts.ItemData(WhispToHosts.lstHosts.NewIndex) = ParticipantID
    End If
    Exit Sub
    
errOnPartStatusChanged:
    DisplayError "OnParticipantStatusChanged"
End Sub

Private Sub MSChat_OnRoomTopicChanged(ByVal NewRoomTopic As String)
    On Error Resume Next
    lblRoomTopic.Caption = NewRoomTopic
End Sub


Private Sub MSChat_OnStateChanged(ByVal NewState As Integer)
    SetState NewState
End Sub



Private Sub optGuest_Click()
    On Error Resume Next
    HostPanel.Hide
    MSChat.UIOption = UIOPT_GUEST
    MSChat.Top = frmGuest.Top + frmGuest.Height + 100
    MSChat.Height = ExecC.Height - StatusBar.Height - MSChat.Top - 500
    frmHost.Visible = False
    frmModerator.Visible = False
    frmGuest.Visible = True
End Sub

Private Sub optHost_Click()
    On Error Resume Next
    HostPanel.Show
    MSChat.UIOption = UIOPT_HOST
    MSChat.Top = frmHost.Top + frmHost.Height + 100
    MSChat.Height = ExecC.Height - StatusBar.Height - MSChat.Top - 500
    frmHost.Visible = True
    frmModerator.Visible = False
    frmGuest.Visible = False
End Sub


Private Sub optModerator_Click()
    On Error Resume Next
    HostPanel.Hide
    MSChat.UIOption = UIOPT_MODERATOR
    MSChat.Top = frmModerator.Top + frmModerator.Height + 100
    MSChat.Height = ExecC.Height - StatusBar.Height - MSChat.Top - 500
    frmHost.Visible = False
    frmModerator.Visible = True
    frmGuest.Visible = False
End Sub


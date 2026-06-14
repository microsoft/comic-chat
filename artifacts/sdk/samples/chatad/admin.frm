VERSION 4.00
'****************************************************************************
'
'  Module:     CHATAD.EXE
'  File:       Admin.frm
'
'  Copyright (c) Microsoft Corporation 1996-1997
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************
Begin VB.Form Main 
   BackColor       =   &H00FFFFC0&
   Caption         =   "MS Chat Monitoring Tool"
   ClientHeight    =   8415
   ClientLeft      =   1125
   ClientTop       =   1845
   ClientWidth     =   11655
   Height          =   8820
   Left            =   1065
   LinkTopic       =   "Main"
   ScaleHeight     =   8415
   ScaleWidth      =   11655
   Top             =   1500
   Width           =   11775
   Begin VB.CommandButton cmdClearErr 
      Caption         =   "Clear Err."
      Height          =   255
      Left            =   120
      TabIndex        =   47
      Top             =   8040
      Width           =   855
   End
   Begin VB.Frame Frame5 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Enter/Exit Room"
      Height          =   735
      Left            =   6600
      TabIndex        =   46
      Top             =   6120
      Width           =   4935
      Begin VB.CommandButton cmdExitRoom 
         Caption         =   "Exit Room"
         Height          =   255
         Left            =   3360
         TabIndex        =   2
         Top             =   360
         Width           =   1455
      End
      Begin VB.CommandButton cmdCancelEntering 
         Caption         =   "Cancel Entering"
         Height          =   255
         Left            =   1680
         TabIndex        =   1
         Top             =   360
         Width           =   1575
      End
      Begin VB.CommandButton cmdEnterRoom 
         Caption         =   "Enter Room"
         Default         =   -1  'True
         Height          =   255
         Left            =   120
         TabIndex        =   0
         Top             =   360
         Width           =   1455
      End
   End
   Begin VB.Frame Frame4 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Ignore Participant"
      Height          =   975
      Left            =   6600
      TabIndex        =   44
      Top             =   4920
      Width           =   4935
      Begin VB.CommandButton cmdDoNotIgnore 
         Caption         =   "Don't Ignore"
         Height          =   255
         Left            =   2520
         TabIndex        =   22
         Top             =   600
         Width           =   2175
      End
      Begin VB.CommandButton cmdIgnore 
         Caption         =   "Ignore"
         Height          =   255
         Left            =   240
         TabIndex        =   21
         Top             =   600
         Width           =   2175
      End
      Begin VB.Label Label5 
         BackColor       =   &H00FFFFC0&
         Caption         =   "Ignore or not the selected member."
         Height          =   255
         Left            =   1320
         TabIndex        =   45
         Top             =   240
         Width           =   2535
      End
   End
   Begin VB.Frame Frame3 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Participant Privileges"
      Height          =   1095
      Left            =   6600
      TabIndex        =   42
      Top             =   3600
      Width           =   4935
      Begin VB.CommandButton cmdMakeHost 
         Caption         =   "Make Host"
         Height          =   255
         Left            =   3360
         TabIndex        =   20
         Top             =   720
         Width           =   1455
      End
      Begin VB.CommandButton cmdMakeParticipant 
         Caption         =   "Make Participant"
         Height          =   255
         Left            =   1800
         TabIndex        =   19
         Top             =   720
         Width           =   1455
      End
      Begin VB.CommandButton cmdMakeSpectator 
         Caption         =   "Make Spectator"
         Height          =   255
         Left            =   120
         TabIndex        =   18
         Top             =   720
         Width           =   1575
      End
      Begin VB.Label Label4 
         BackColor       =   &H00FFFFC0&
         Caption         =   "Change the status of the selected member."
         Height          =   255
         Left            =   1080
         TabIndex        =   43
         Top             =   360
         Width           =   3255
      End
   End
   Begin VB.Frame Frame1 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Participant Real Name"
      Height          =   2295
      Left            =   6600
      TabIndex        =   36
      Top             =   1080
      Width           =   4935
      Begin VB.CommandButton cmdGetRealName 
         Caption         =   "Get Real Name"
         Height          =   975
         Left            =   4080
         TabIndex        =   17
         Top             =   480
         Width           =   735
      End
      Begin VB.Frame Frame2 
         BackColor       =   &H00FFFFC0&
         Caption         =   "Get Real Name from"
         Height          =   1095
         Left            =   120
         TabIndex        =   37
         Top             =   360
         Width           =   3735
         Begin VB.TextBox txtAlias 
            Enabled         =   0   'False
            Height          =   285
            Left            =   1320
            TabIndex        =   16
            Top             =   720
            Width           =   2295
         End
         Begin VB.OptionButton optAlias 
            BackColor       =   &H00FFFFC0&
            Caption         =   "This Alias"
            Height          =   255
            Left            =   120
            TabIndex        =   15
            Top             =   720
            Width           =   1095
         End
         Begin VB.OptionButton optSelection 
            BackColor       =   &H00FFFFC0&
            Caption         =   "Selected member"
            Height          =   255
            Left            =   120
            TabIndex        =   14
            Top             =   360
            Value           =   -1  'True
            Width           =   1815
         End
      End
      Begin VB.Label lblAsyncRealName 
         Height          =   255
         Left            =   1680
         TabIndex        =   41
         Top             =   1920
         Width           =   3135
      End
      Begin VB.Label lblAsyncAlias 
         Height          =   255
         Left            =   120
         TabIndex        =   40
         Top             =   1920
         Width           =   1335
      End
      Begin VB.Label Label2 
         BackColor       =   &H00FFFFC0&
         Caption         =   "Real Name:"
         Height          =   255
         Left            =   1680
         TabIndex        =   39
         Top             =   1680
         Width           =   1215
      End
      Begin VB.Label Label1 
         BackColor       =   &H00FFFFC0&
         Caption         =   "Alias:"
         Height          =   255
         Left            =   120
         TabIndex        =   38
         Top             =   1680
         Width           =   735
      End
   End
   Begin VB.CommandButton cmdKickBan 
      Caption         =   "Kick, Ban and Re-instate Participant"
      Height          =   255
      Left            =   6600
      TabIndex        =   6
      Top             =   7560
      Width           =   3255
   End
   Begin VB.CommandButton cmdChangeControlUI 
      Caption         =   "Change Control UI"
      Height          =   255
      Left            =   9840
      TabIndex        =   5
      Top             =   7080
      Width           =   1695
   End
   Begin VB.CommandButton cmdSetMaxMembers 
      Caption         =   "Set MaxMembers"
      Height          =   255
      Left            =   3240
      TabIndex        =   12
      Top             =   1800
      Width           =   1455
   End
   Begin VB.CommandButton cmdGetMaxMembers 
      Caption         =   "Get MaxMembers"
      Height          =   255
      Left            =   4800
      TabIndex        =   13
      Top             =   1800
      Width           =   1455
   End
   Begin VB.TextBox txtMaxMembers 
      Height          =   285
      Left            =   1680
      TabIndex        =   11
      Top             =   1800
      Width           =   975
   End
   Begin VB.TextBox txtError 
      BackColor       =   &H000080FF&
      Height          =   285
      Left            =   1080
      TabIndex        =   31
      Top             =   8040
      Width           =   10455
   End
   Begin VB.TextBox txtState 
      BackColor       =   &H000000C0&
      Height          =   285
      Left            =   10440
      TabIndex        =   30
      Text            =   "Disconnected"
      Top             =   360
      Width           =   1095
   End
   Begin VB.CommandButton cmdGetMemberCount 
      Caption         =   "Get MemberCount"
      Height          =   255
      Left            =   4800
      TabIndex        =   10
      Top             =   1440
      Width           =   1455
   End
   Begin VB.TextBox txtMemberCount 
      Height          =   285
      Left            =   1680
      TabIndex        =   9
      Top             =   1440
      Width           =   975
   End
   Begin VB.CommandButton cmdWhisperNoWhisper 
      Caption         =   "No/Whisper"
      Height          =   255
      Left            =   6600
      TabIndex        =   3
      Top             =   7080
      Width           =   1455
   End
   Begin VB.CommandButton cmdSetTopic 
      Caption         =   "Set Topic"
      Height          =   255
      Left            =   4800
      TabIndex        =   8
      Top             =   1080
      Width           =   1455
   End
   Begin VB.TextBox txtTopic 
      Height          =   285
      Left            =   1680
      TabIndex        =   7
      Top             =   1080
      Width           =   3015
   End
   Begin VB.CommandButton cmdClearHistory 
      Caption         =   "Clear History"
      Height          =   255
      Left            =   8280
      TabIndex        =   4
      Top             =   7080
      Width           =   1335
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   255
      Left            =   10080
      TabIndex        =   23
      Top             =   7560
      Width           =   1455
   End
   Begin MSChatObjectsCtl.MSChat MSChat1 
      Height          =   5775
      Left            =   120
      TabIndex        =   48
      Top             =   2160
      Width           =   6375
      _ExtentX        =   11245
      _ExtentY        =   10186
      BorderStyle     =   0
      UIOption        =   4095
      BackColor       =   192
   End
   Begin VB.Label lblRoomPath 
      Height          =   255
      Left            =   4800
      TabIndex        =   35
      Top             =   360
      Width           =   5175
   End
   Begin VB.Label lblUserName 
      Height          =   255
      Left            =   2520
      TabIndex        =   34
      Top             =   360
      Width           =   1815
   End
   Begin VB.Label lblAlias 
      Height          =   255
      Left            =   120
      TabIndex        =   33
      Top             =   360
      Width           =   1935
   End
   Begin VB.Label lblMaxMembers 
      Alignment       =   1  'Right Justify
      BackColor       =   &H00FFFFC0&
      Caption         =   "MaxMembers"
      Height          =   255
      Left            =   480
      TabIndex        =   32
      Top             =   1800
      Width           =   975
   End
   Begin VB.Label Label18 
      BackColor       =   &H00FFFFC0&
      Caption         =   "State"
      Height          =   255
      Left            =   10440
      TabIndex        =   29
      Top             =   120
      Width           =   615
   End
   Begin VB.Label Label15 
      Alignment       =   1  'Right Justify
      BackColor       =   &H00FFFFC0&
      Caption         =   "Member Count"
      Height          =   255
      Left            =   360
      TabIndex        =   28
      Top             =   1440
      Width           =   1095
   End
   Begin VB.Label Label13 
      Alignment       =   1  'Right Justify
      BackColor       =   &H00FFFFC0&
      Caption         =   "Chat Room Topic"
      Height          =   255
      Left            =   120
      TabIndex        =   27
      Top             =   1080
      Width           =   1335
   End
   Begin VB.Label Label10 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Name"
      Height          =   255
      Left            =   2520
      TabIndex        =   26
      Top             =   120
      Width           =   1815
   End
   Begin VB.Label Label8 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Room Path"
      Height          =   255
      Left            =   4800
      TabIndex        =   25
      Top             =   120
      Width           =   5175
   End
   Begin VB.Label Label7 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Alias "
      Height          =   255
      Left            =   120
      TabIndex        =   24
      Top             =   120
      Width           =   1935
   End
End
Attribute VB_Name = "Main"
Attribute VB_Creatable = False
Attribute VB_Exposed = False

Option Explicit

Const MASK_PRIVILEGES = 1
Const MASK_NOWHISPER = 2
Const MASK_IGNORED = 4

Const STAT_HOST = 1
Const STAT_PARTICIPANT = 2
Const STAT_SPECTATOR = 4
Const STAT_NOWHISPER = 8
Const STAT_IGNORED = 16

Const CAP_NOWHISPER = "Deny Whispers"
Const CAP_WHISPER = "Accept Whispers"


Sub SetState(NewState As Integer)
    Select Case NewState
    Case 1:
        cmdEnterRoom.Enabled = True
        cmdCancelEntering.Enabled = False
        cmdExitRoom.Enabled = False
        txtState.Text = "Disconnected"
        txtState.BackColor = &HC0&
        MSChat1.BackColor = &HC0&
    Case 2:
        cmdEnterRoom.Enabled = False
        cmdCancelEntering.Enabled = True
        cmdExitRoom.Enabled = False
        txtState.Text = "Connecting..."
        txtState.BackColor = &H80FF&
        MSChat1.BackColor = &H80FF&
    Case 3:
        cmdEnterRoom.Enabled = False
        cmdCancelEntering.Enabled = False
        cmdExitRoom.Enabled = True
        txtState.Text = "Connected"
        txtState.BackColor = &HC000&
        MSChat1.BackColor = &HC000&
    End Select
End Sub







Private Sub cmdChangeControlUI_Click()
    UISettings.Show
End Sub

Private Sub cmdClearErr_Click()
    txtError.Text = ""
End Sub





Private Sub cmdGetMaxMembers_Click()
    On Error Resume Next
    txtMaxMembers.Text = CStr(MSChat1.MaxMembers)
End Sub

Private Sub cmdGetRealName_Click()
    On Error Resume Next
    
    Dim txtReal As String
    
    lblAsyncAlias.Caption = ""
    lblAsyncRealName.Caption = ""
    
    If (optSelection.Value) Then
        MSChat1.GetParticipantRealName -1, "", False, txtReal
    Else
        MSChat1.GetParticipantRealName 0, txtAlias.Text, False, txtReal
    End If
End Sub



Private Sub cmdGetMemberCount_Click()
    On Error Resume Next
    txtMemberCount.Text = CStr(MSChat1.MemberCount)
End Sub

Private Sub cmdKickBan_Click()
    KickBan.Show
End Sub


Private Sub cmdMakeHost_Click()
    On Error Resume Next
    MSChat1.SetParticipantStatus -1, MASK_PRIVILEGES, STAT_HOST
End Sub

Private Sub cmdMakeParticipant_Click()
    On Error Resume Next
    MSChat1.SetParticipantStatus -1, MASK_PRIVILEGES, STAT_PARTICIPANT
End Sub

Private Sub cmdMakeSpectator_Click()
    On Error Resume Next
    MSChat1.SetParticipantStatus -1, MASK_PRIVILEGES, STAT_SPECTATOR
End Sub

Private Sub cmdSetMaxMembers_Click()
    On Error Resume Next
    MSChat1.MaxMembers = CLng(txtMaxMembers.Text)
End Sub



Private Sub Form_Unload(Cancel As Integer)
    Unload UISettings
    Unload EnterOptions
    Unload KickBan
End Sub

Private Sub MSChat1_OnParticipantRealName(ByVal Alias As String, ByVal RealName As String)
    On Error Resume Next
    lblAsyncAlias.Caption = Alias
    lblAsyncRealName.Caption = RealName
End Sub

Private Sub MSChat1_OnRoomTopicChanged(ByVal NewRoomTopic As String)
    On Error Resume Next
    txtTopic.Text = NewRoomTopic
End Sub


Private Sub MSChat1_OnStateChanged(ByVal NewState As Integer)
    SetState NewState
End Sub














Private Sub cmdCancelEntering_Click()
    On Error Resume Next
    MSChat1.CancelEntering
End Sub

Private Sub cmdClearHistory_Click()
    On Error Resume Next
    MSChat1.ClearHistory
End Sub


Private Sub cmdDoNotIgnore_Click()
    On Error Resume Next
    MSChat1.SetParticipantStatus -1, MASK_IGNORED, 0
End Sub

Private Sub cmdEnterRoom_Click()
    EnterOptions.Show
End Sub



Private Sub cmdExit_Click()
    End
End Sub


Private Sub cmdExitRoom_Click()
    On Error Resume Next
    MSChat1.ExitRoom
End Sub













Private Sub cmdIgnore_Click()
    On Error Resume Next
    MSChat1.SetParticipantStatus -1, MASK_IGNORED, STAT_IGNORED
End Sub


























Private Sub cmdSetTopic_Click()
    On Error Resume Next
    MSChat1.RoomTopic = txtTopic.Text
End Sub






Private Sub cmdWhisperNoWhisper_Click()
    On Error Resume Next
    If (cmdWhisperNoWhisper.Caption = CAP_NOWHISPER) Then
        MSChat1.SetParticipantStatus MSChat1.ThisParticipantID, MASK_NOWHISPER, STAT_NOWHISPER
        cmdWhisperNoWhisper.Caption = CAP_WHISPER
    Else
        MSChat1.SetParticipantStatus MSChat1.ThisParticipantID, MASK_NOWHISPER, 0
        cmdWhisperNoWhisper.Caption = CAP_NOWHISPER
    End If
End Sub
Private Sub Form_Load()
    cmdWhisperNoWhisper.Caption = CAP_NOWHISPER
End Sub




Private Sub MSChat1_OnEndEnumeration()
    
    On Error Resume Next
    txtTopic.Text = MSChat1.RoomTopic
    lblUserName.Caption = MSChat1.ThisParticipantName
    lblRoomPath.Caption = MSChat1.RoomPath
    
    cmdGetMemberCount_Click
    cmdGetMaxMembers_Click
End Sub


Private Sub MSChat1_OnError(ByVal ErrorCode As Integer, ByVal Description As String)
    On Error Resume Next
    txtError.Text = "MSChat1 error " + CStr(ErrorCode) + ", Desc: " + Description
End Sub







Private Sub optAlias_Click()
    txtAlias.Enabled = True
End Sub

Private Sub optSelection_Click()
    txtAlias.Enabled = False
End Sub





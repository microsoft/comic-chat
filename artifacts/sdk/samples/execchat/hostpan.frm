VERSION 4.00
'****************************************************************************
'
'  Module:     ExecChat.EXE
'  File:       HostPan.frm
'
'  Copyright (c) Microsoft Corporation 1996-1997
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************
Begin VB.Form HostPanel 
   BackColor       =   &H00C0FFC0&
   Caption         =   "Host Monitoring Panel"
   ClientHeight    =   2985
   ClientLeft      =   1095
   ClientTop       =   5325
   ClientWidth     =   9765
   Height          =   3390
   Left            =   1035
   LinkTopic       =   "HostPanel"
   ScaleHeight     =   2985
   ScaleWidth      =   9765
   Top             =   4980
   Width           =   9885
   Begin VB.Frame Frame1 
      BackColor       =   &H00C0FFC0&
      Caption         =   "Privileges of the Selected Member"
      Height          =   615
      Left            =   4440
      TabIndex        =   22
      Top             =   120
      Width           =   5295
      Begin VB.CommandButton cmdMakeSpectator 
         Caption         =   "Make Spectator"
         Height          =   255
         Left            =   120
         TabIndex        =   25
         Top             =   240
         Width           =   1695
      End
      Begin VB.CommandButton cmdMakeParticipant 
         Caption         =   "Make Participant"
         Height          =   255
         Left            =   1920
         TabIndex        =   24
         Top             =   240
         Width           =   1695
      End
      Begin VB.CommandButton cmdMakeHost 
         Caption         =   "Make Host"
         Height          =   255
         Left            =   3720
         TabIndex        =   23
         Top             =   240
         Width           =   1455
      End
   End
   Begin VB.Frame Frame2 
      BackColor       =   &H00C0FFC0&
      Caption         =   "Filtering of the Selected Member"
      Height          =   615
      Left            =   4440
      TabIndex        =   19
      Top             =   840
      Width           =   5295
      Begin VB.CommandButton cmdIgnore 
         Caption         =   "Ignore Member"
         Height          =   255
         Left            =   120
         TabIndex        =   21
         Top             =   240
         Width           =   2415
      End
      Begin VB.CommandButton cmdDoNotIgnore 
         Caption         =   "Don't Ignore Member"
         Height          =   255
         Left            =   2640
         TabIndex        =   20
         Top             =   240
         Width           =   2535
      End
   End
   Begin VB.Frame Frame3 
      BackColor       =   &H00C0FFC0&
      Caption         =   "Set New Room Topic"
      ForeColor       =   &H00000000&
      Height          =   615
      Left            =   120
      TabIndex        =   16
      Top             =   1560
      Width           =   4215
      Begin VB.TextBox txtRoomTopic 
         Height          =   285
         Left            =   120
         TabIndex        =   18
         Top             =   240
         Width           =   3015
      End
      Begin VB.CommandButton cmdSetTopic 
         Caption         =   "Set Topic"
         Height          =   285
         Left            =   3240
         TabIndex        =   17
         Top             =   240
         Width           =   855
      End
   End
   Begin VB.Frame Frame4 
      BackColor       =   &H00C0FFC0&
      Caption         =   "Room Capacity"
      Height          =   615
      Left            =   4440
      TabIndex        =   12
      Top             =   1560
      Width           =   5295
      Begin VB.TextBox txtMaxMembers 
         Height          =   285
         Left            =   120
         TabIndex        =   15
         Top             =   240
         Width           =   975
      End
      Begin VB.CommandButton cmdSetMaxMembers 
         Caption         =   "Set New Capacity"
         Height          =   285
         Left            =   1200
         TabIndex        =   14
         Top             =   240
         Width           =   1695
      End
      Begin VB.CommandButton cmdGetMaxMembers 
         Caption         =   "Get Current Capacity"
         Height          =   285
         Left            =   3000
         TabIndex        =   13
         Top             =   240
         Width           =   2175
      End
   End
   Begin VB.Frame Frame5 
      BackColor       =   &H00C0FFC0&
      Caption         =   "Police Functions"
      Height          =   615
      Left            =   120
      TabIndex        =   10
      Top             =   2280
      Width           =   4215
      Begin VB.CommandButton cmdKickBan 
         Caption         =   "Kick, Ban or Re-instate the Selected Member"
         Height          =   255
         Left            =   120
         TabIndex        =   11
         Top             =   240
         Width           =   3975
      End
   End
   Begin VB.Frame Frame7 
      BackColor       =   &H00C0FFC0&
      Caption         =   "Control UI Settings"
      Height          =   615
      Left            =   4440
      TabIndex        =   7
      Top             =   2280
      Width           =   5295
      Begin VB.CommandButton cmdClearHistory 
         Caption         =   "Clear History"
         Height          =   255
         Left            =   120
         TabIndex        =   9
         Top             =   240
         Width           =   2535
      End
      Begin VB.CommandButton cmdChangeControlUI 
         Caption         =   "Change Control UI"
         Height          =   255
         Left            =   2760
         TabIndex        =   8
         Top             =   240
         Width           =   2415
      End
   End
   Begin VB.Frame Frame6 
      BackColor       =   &H00C0FFC0&
      Caption         =   "Real Name of"
      Height          =   1335
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   4215
      Begin VB.CommandButton cmdGetRealName 
         Caption         =   "Get    Real Name"
         Height          =   640
         Left            =   3240
         TabIndex        =   4
         Top             =   240
         Width           =   855
      End
      Begin VB.OptionButton optSelection 
         BackColor       =   &H00C0FFC0&
         Caption         =   "the Selected Member"
         Height          =   255
         Left            =   120
         TabIndex        =   3
         Top             =   240
         Value           =   -1  'True
         Width           =   3015
      End
      Begin VB.OptionButton optAlias 
         BackColor       =   &H00C0FFC0&
         Caption         =   "this Alias"
         Height          =   255
         Left            =   120
         TabIndex        =   2
         Top             =   500
         Width           =   1095
      End
      Begin VB.TextBox txtAlias 
         Height          =   285
         Left            =   1200
         TabIndex        =   1
         Top             =   480
         Width           =   1935
      End
      Begin VB.Label lblAsyncAlias 
         BackColor       =   &H00FFFFFF&
         Caption         =   "Alias appears here"
         BeginProperty Font 
            name            =   "Arial Narrow"
            charset         =   0
            weight          =   400
            size            =   8.25
            underline       =   0   'False
            italic          =   0   'False
            strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   6
         Top             =   960
         Width           =   1335
      End
      Begin VB.Label lblAsyncRealName 
         BackColor       =   &H00FFFFFF&
         Caption         =   "Real name appears here"
         BeginProperty Font 
            name            =   "Arial Narrow"
            charset         =   0
            weight          =   400
            size            =   8.25
            underline       =   0   'False
            italic          =   0   'False
            strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   1560
         TabIndex        =   5
         Top             =   960
         Width           =   2535
      End
   End
End
Attribute VB_Name = "HostPanel"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
Option Explicit

Sub cmdGetMaxMembers_Click()
    On Error Resume Next
    txtMaxMembers.Text = CStr(ExecC.MSChat.MaxMembers)
End Sub

Private Sub cmdChangeControlUI_Click()
    UISettings.Show
End Sub


Private Sub cmdDoNotIgnore_Click()
    On Error Resume Next
    ExecC.MSChat.SetParticipantStatus -1, MASK_IGNORED, 0
End Sub


Private Sub cmdGetRealName_Click()
    On Error GoTo errGetRealName
    
    Dim txtReal As String
    
    lblAsyncAlias.Caption = "Alias appears here"
    lblAsyncRealName.Caption = "Real name appears here"
    
    nRealNameType = RN_PARTICIPANT
    If (optSelection.Value) Then
        ExecC.MSChat.GetParticipantRealName -1, "", False, txtReal
    Else
        ExecC.MSChat.GetParticipantRealName 0, txtAlias.Text, False, txtReal
    End If
    
    Exit Sub

errGetRealName:
    DisplayError "cmdGetRealName_Click"
End Sub

Private Sub cmdIgnore_Click()
    On Error Resume Next
    ExecC.MSChat.SetParticipantStatus -1, MASK_IGNORED, STAT_IGNORED
End Sub


Private Sub cmdKickBan_Click()
    KickBan.Show
End Sub

Private Sub cmdMakeHost_Click()
    On Error Resume Next
    ExecC.MSChat.SetParticipantStatus -1, MASK_PRIVILEGES, STAT_HOST
End Sub

Private Sub cmdMakeParticipant_Click()
    On Error Resume Next
    ExecC.MSChat.SetParticipantStatus -1, MASK_PRIVILEGES, STAT_PARTICIPANT
End Sub


Private Sub cmdMakeSpectator_Click()
    On Error Resume Next
    ExecC.MSChat.SetParticipantStatus -1, MASK_PRIVILEGES, STAT_SPECTATOR
End Sub

Private Sub cmdSetMaxMembers_Click()
    On Error Resume Next
    ExecC.MSChat.MaxMembers = CLng(txtMaxMembers.Text)
End Sub

Private Sub cmdSetTopic_Click()
    On Error Resume Next
    ExecC.MSChat.RoomTopic = txtRoomTopic.Text
End Sub

Private Sub optAlias_Click()
    txtAlias.Enabled = True
End Sub

Private Sub optSelection_Click()
    txtAlias.Enabled = False
End Sub


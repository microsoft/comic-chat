VERSION 4.00
'****************************************************************************
'
'  Module:     ExecChat.Exe
'  File:       ECenter.frm
'
'  Copyright (c) Microsoft Corporation 1996-1997
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************
Begin VB.Form EnterOptions 
   BackColor       =   &H00FFFFC0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Enter Room Options"
   ClientHeight    =   2325
   ClientLeft      =   1890
   ClientTop       =   1980
   ClientWidth     =   6945
   Height          =   2730
   Left            =   1830
   LinkTopic       =   "EnterOptions"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2325
   ScaleWidth      =   6945
   ShowInTaskbar   =   0   'False
   Top             =   1635
   Width           =   7065
   Begin VB.CheckBox chkMicOnly 
      BackColor       =   &H00FFFFC0&
      Caption         =   "MIC only room?"
      Height          =   255
      Left            =   120
      TabIndex        =   8
      Top             =   1800
      Width           =   1455
   End
   Begin VB.TextBox txtRoomPath 
      Height          =   285
      Left            =   120
      TabIndex        =   2
      Top             =   1200
      Width           =   4095
   End
   Begin VB.TextBox txtAlias 
      Height          =   285
      Left            =   120
      TabIndex        =   0
      Top             =   360
      Width           =   4095
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   4440
      TabIndex        =   3
      Top             =   1800
      Width           =   1095
   End
   Begin VB.CommandButton cmdCancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   5640
      TabIndex        =   4
      Top             =   1800
      Width           =   1095
   End
   Begin VB.TextBox txtRoomPwd 
      Height          =   285
      Left            =   4800
      TabIndex        =   1
      Top             =   360
      Width           =   1935
   End
   Begin VB.Label Label7 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Type your alias:"
      Height          =   255
      Left            =   120
      TabIndex        =   7
      Top             =   120
      Width           =   3975
   End
   Begin VB.Label Label8 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Type the chat room path:"
      Height          =   255
      Index           =   0
      Left            =   120
      TabIndex        =   6
      Top             =   960
      Width           =   3975
   End
   Begin VB.Label lblRoomPwd 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Type the optional host key:"
      Height          =   255
      Left            =   4800
      TabIndex        =   5
      Top             =   120
      Width           =   1935
   End
End
Attribute VB_Name = "EnterOptions"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
Option Explicit

' Channel creation modes
Const OCX_CHANNEL_CREATE_JOIN = 1

Const OCX_CHANNEL_FLAG_LOCAL = 2     ' channel not exposed to other servers on net
Const OCX_CHANNEL_FLAG_FEED = 4      ' news feeds, stock quotes..
Const OCX_CHANNEL_FLAG_MICONLY = 8   ' NO IRC - NO NO NO IRC
Const OCX_CHANNEL_FLAG_AUTHONLY = 16 ' permit authenticated members only
Const OCX_CHANNEL_FLAG_AUTHTALK = 32 ' permit only authenticated members to talk

Const OCX_CHANNEL_PUBLIC = 1         ' open to anybody and anything
Const OCX_CHANNEL_TOPICOP = 2        ' topic can be changed only by Host
Const OCX_CHANNEL_NOREMOTE = 4       ' only allow users who logged into THIS server..to enter chat
Const OCX_CHANNEL_NOWHISPER = 8      ' no whispers or \msg allowed
Const OCX_CHANNEL_AUDITORIUM = 16    ' auditorium chat room
Const OCX_CHANNEL_MODERATED = 32     ' join as spectator by default


Dim nFlags As Integer
Dim nType As Integer

Private Sub cmdCancel_Click()
    Hide
End Sub


Private Sub cmdOK_Click()
        
    On Error Resume Next
    
    Dim nFlags As Integer
    
    If (chkMicOnly.Value = 1) Then
        nFlags = 9  'Mic only room
    Else
        nFlags = 1  'Mic and Irc room
    End If
    
    ExecC.MSChat.EnterRoom txtRoomPath.Text, txtRoomPwd.Text, txtAlias.Text, "NTLM;ANON", nFlags, 0
    
    If (Err.Number = 0) Then
        ExecC.lblAlias.Caption = txtAlias.Text
        ExecC.StatusBar.Panels(3).Text = txtRoomPath.Text
        If (ExecC.optHost.Value) Then
            WriteLogFile "Entering room " + txtRoomPath.Text + " as host."
        Else
            If (ExecC.optModerator.Value) Then
                WriteLogFile "Entering room " + txtRoomPath.Text + " as moderator."
            Else
                WriteLogFile "Entering room " + txtRoomPath.Text + " as guest."
            End If
        End If
        WriteLogFile "Alias: " + txtAlias.Text
    End If
    
    Hide
End Sub


Private Sub Form_Load()
    
    txtRoomPath.Text = VBGetPrivateProfileString("Common Default Entries", "RoomPath", "//Comicsrv1.microsoft.com/#ExecChat", App.Path + "\ExecChat.ini")
    txtRoomPwd.Text = VBGetPrivateProfileString("Common Default Entries", "HostKey", "", App.Path + "\ExecChat.ini")
    
    If (ExecC.optHost.Value) Then
        txtAlias.Text = VBGetPrivateProfileString("Host Default Entries", "HostAlias", "Host", App.Path + "\ExecChat.ini")
    End If
    If (ExecC.optModerator.Value) Then
        txtAlias.Text = VBGetPrivateProfileString("Moderator Default Entries", "ModeratorAlias", "Moderator", App.Path + "\ExecChat.ini")
    End If
    If (ExecC.optGuest.Value) Then
        txtAlias.Text = VBGetPrivateProfileString("Guest Default Entries", "GuestAlias", "Guest", App.Path + "\ExecChat.ini")
    End If
    
    If (InStr(txtRoomPath.Text, "/#") Or InStr(txtRoomPath.Text, "\#")) Then
        chkMicOnly.Value = 0
    Else
        chkMicOnly.Value = 1
    End If
End Sub

Private Sub txtRoomPath_Change()
    If (InStr(txtRoomPath.Text, "/#") Or InStr(txtRoomPath.Text, "\#")) Then
        chkMicOnly.Value = 0
    Else
        chkMicOnly.Value = 1
    End If
End Sub



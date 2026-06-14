VERSION 4.00
'****************************************************************************
'
'  Module:     CHATAD.EXE
'  File:       EnterOpt.frm
'
'  Copyright (c) Microsoft Corporation 1996-1997
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************
Begin VB.Form EnterOptions 
   BackColor       =   &H00C0FFC0&
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Enter Room Options"
   ClientHeight    =   5775
   ClientLeft      =   1785
   ClientTop       =   3300
   ClientWidth     =   6870
   Height          =   6180
   Left            =   1725
   LinkTopic       =   "EnterOptions"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5775
   ScaleWidth      =   6870
   ShowInTaskbar   =   0   'False
   Top             =   2955
   Width           =   6990
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   4320
      TabIndex        =   26
      Top             =   5280
      Width           =   1095
   End
   Begin VB.CommandButton cmdCancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   5640
      TabIndex        =   25
      Top             =   5280
      Width           =   1095
   End
   Begin VB.ComboBox ComboSecPackages 
      Height          =   315
      Left            =   4800
      TabIndex        =   24
      Top             =   1200
      Width           =   1935
   End
   Begin VB.Frame Frame2 
      BackColor       =   &H00C0FFC0&
      Height          =   2415
      Left            =   120
      TabIndex        =   11
      Top             =   2640
      Width           =   6615
      Begin VB.CheckBox chkAuthTalk 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Only Authenticated Members Can Talk?"
         Height          =   195
         Left            =   2880
         TabIndex        =   23
         Top             =   2040
         Width           =   3615
      End
      Begin VB.CheckBox chkModerated 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Moderated Room?"
         Height          =   195
         Left            =   2880
         TabIndex        =   22
         Top             =   1320
         Width           =   3615
      End
      Begin VB.CheckBox chkAuthOnly 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Accept Authenticated Members Only?"
         Height          =   195
         Left            =   2880
         TabIndex        =   21
         Top             =   1680
         Width           =   3615
      End
      Begin VB.CheckBox chkAllowWhispers 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Allow Whispers in Room?"
         Height          =   195
         Left            =   2880
         TabIndex        =   20
         Top             =   600
         Value           =   1  'Checked
         Width           =   3615
      End
      Begin VB.CheckBox chkTopicRO 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Topic read only for non-hosts?"
         Height          =   195
         Left            =   120
         TabIndex        =   19
         Top             =   960
         Width           =   2655
      End
      Begin VB.CheckBox chkLocal 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Local Room?"
         Height          =   195
         Left            =   120
         TabIndex        =   18
         Top             =   1680
         Width           =   2655
      End
      Begin VB.CheckBox chkNoRemote 
         BackColor       =   &H00C0FFC0&
         Caption         =   "NoRemote Room?"
         Height          =   195
         Left            =   120
         TabIndex        =   17
         Top             =   1320
         Width           =   2655
      End
      Begin VB.CheckBox chkMicOnly 
         BackColor       =   &H00C0FFC0&
         Caption         =   "MIC Only Room?"
         Height          =   195
         Left            =   120
         TabIndex        =   16
         Top             =   240
         Width           =   2655
      End
      Begin VB.CheckBox chkPublic 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Public Room?"
         Height          =   195
         Left            =   120
         TabIndex        =   15
         Top             =   600
         Width           =   2655
      End
      Begin VB.CheckBox chkFeed 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Feed Room?"
         Height          =   195
         Left            =   120
         TabIndex        =   14
         Top             =   2040
         Width           =   2655
      End
      Begin VB.CheckBox chkCreateJoin 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Join if room exists?"
         Height          =   195
         Left            =   2880
         TabIndex        =   13
         Top             =   240
         Value           =   1  'Checked
         Width           =   3615
      End
      Begin VB.CheckBox chkAuditorium 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Auditorium Room?"
         Height          =   195
         Left            =   2880
         TabIndex        =   12
         Top             =   960
         Width           =   3615
      End
   End
   Begin VB.TextBox txtTopic 
      Height          =   285
      Left            =   120
      TabIndex        =   9
      Top             =   2040
      Width           =   3975
   End
   Begin VB.CommandButton cmdGetTopic 
      Caption         =   "Get Topic"
      Height          =   255
      Left            =   2160
      TabIndex        =   8
      Top             =   1800
      Width           =   975
   End
   Begin VB.CommandButton cmdSetTopic 
      Caption         =   "Set Topic"
      Height          =   255
      Left            =   3120
      TabIndex        =   7
      Top             =   1800
      Width           =   975
   End
   Begin VB.TextBox txtRoomPwd 
      Height          =   285
      Left            =   4800
      TabIndex        =   2
      Top             =   360
      Width           =   1935
   End
   Begin VB.ComboBox ComboRoomPath 
      Height          =   315
      Left            =   120
      TabIndex        =   1
      Top             =   1200
      Width           =   3975
   End
   Begin VB.ComboBox ComboAlias 
      Height          =   315
      Left            =   120
      TabIndex        =   0
      Top             =   360
      Width           =   3975
   End
   Begin VB.Label Label13 
      BackColor       =   &H00C0FFC0&
      Caption         =   "Type the chat room topic:"
      Height          =   255
      Left            =   120
      TabIndex        =   10
      Top             =   1800
      Width           =   2175
   End
   Begin VB.Label Label7 
      BackColor       =   &H00C0FFC0&
      Caption         =   "Select or type your Alias:"
      Height          =   255
      Left            =   120
      TabIndex        =   6
      Top             =   120
      Width           =   3975
   End
   Begin VB.Label Label8 
      BackColor       =   &H00C0FFC0&
      Caption         =   "Select or type the chat room:"
      Height          =   255
      Left            =   120
      TabIndex        =   5
      Top             =   960
      Width           =   3975
   End
   Begin VB.Label lblSecurityPackage 
      BackColor       =   &H00C0FFC0&
      Caption         =   "Select or type the security packages to use:"
      Height          =   495
      Left            =   4800
      TabIndex        =   4
      Top             =   800
      Width           =   1935
   End
   Begin VB.Label lblRoomPwd 
      BackColor       =   &H00C0FFC0&
      Caption         =   "Type the optional host key:"
      Height          =   255
      Left            =   4800
      TabIndex        =   3
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
    
    SetFlagsAndType
    
    cmdSetTopic_Click

    Main.lblAlias.Caption = ComboAlias.Text
    Main.lblRoomPath.Caption = ComboRoomPath.Text
    Main.MSChat1.EnterRoom ComboRoomPath.Text, txtRoomPwd.Text, ComboAlias.Text, ComboSecPackages.Text, nFlags, nType
    
    Hide
End Sub

Sub SetFlagsAndType()
    nFlags = 0
    nType = 0
    If (chkCreateJoin.Value = 1) Then
        nFlags = nFlags + OCX_CHANNEL_CREATE_JOIN
    End If
    If (chkLocal.Value = 1) Then
        nFlags = nFlags + OCX_CHANNEL_FLAG_LOCAL
    End If
    If (chkFeed.Value = 1) Then
        nFlags = nFlags + OCX_CHANNEL_FLAG_FEED
    End If
    If (chkMicOnly.Value = 1) Then
        nFlags = nFlags + OCX_CHANNEL_FLAG_MICONLY
    End If
    If (chkAuthOnly.Value = 1) Then
        nFlags = nFlags + OCX_CHANNEL_FLAG_AUTHONLY
    End If
    If (chkAuthTalk.Value = 1) Then
        nFlags = nFlags + OCX_CHANNEL_FLAG_AUTHTALK
    End If
        
    If (chkPublic.Value = 1) Then
        nType = nType + OCX_CHANNEL_PUBLIC
    End If
    If (chkTopicRO.Value = 1) Then
        nType = nType + OCX_CHANNEL_TOPICOP
    End If
    If (chkNoRemote.Value = 1) Then
        nType = nType + OCX_CHANNEL_NOREMOTE
    End If
    If (chkAllowWhispers.Value = 0) Then
        nType = nType + OCX_CHANNEL_NOWHISPER
    End If
    If (chkAuditorium.Value = 1) Then
        nType = nType + OCX_CHANNEL_AUDITORIUM
    End If
    If (chkModerated.Value = 1) Then
        nType = nType + OCX_CHANNEL_MODERATED
    End If
End Sub


Private Sub cmdSetTopic_Click()
    On Error Resume Next
    
    Main.MSChat1.RoomTopic = txtTopic.Text
End Sub

Private Sub Form_Load()
    ComboRoomPath.AddItem "//ComicSrv1.Microsoft.Com/#Comic_Chat"
    ComboRoomPath.AddItem "//ComicSrv1.Microsoft.Com/#Francais"
    ComboRoomPath.AddItem "//ComicSrv1.Microsoft.Com/#Newbies"
    ComboRoomPath.AddItem "//ComicSrv1.Microsoft.Com/#Singles_Chat"
    
    ComboRoomPath.AddItem "//irc.dal.net/#newbies"
    ComboRoomPath.AddItem "//irc.mtv.com/#newbies"
    ComboRoomPath.AddItem "//irc.eskimo.com/#newbies"
    ComboRoomPath.AddItem "//irc.univ-lyon1.fr/#newbies"
    
    ComboRoomPath.ListIndex = 0
    
    ComboAlias.AddItem "Nickname1"
    ComboAlias.AddItem "Nickname2"
    
    ComboAlias.ListIndex = 0

    ComboSecPackages.AddItem "ANON"
    ComboSecPackages.AddItem "ALL"
    ComboSecPackages.AddItem "MSN;DPA;NTLM;ANON"
    ComboSecPackages.AddItem "MSN;DPA;ANON"
    ComboSecPackages.AddItem "MSN;DPA"
    ComboSecPackages.AddItem "NTLM;ANON"
    ComboSecPackages.AddItem "NTLM"
    
    ComboSecPackages.ListIndex = 0
End Sub


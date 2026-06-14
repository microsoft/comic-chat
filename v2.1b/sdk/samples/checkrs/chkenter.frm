VERSION 4.00
'****************************************************************************
'
'  Module:     CHECKER.EXE
'  File:       Chkenter.frm
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
   Caption         =   "Join Game Options"
   ClientHeight    =   3060
   ClientLeft      =   1785
   ClientTop       =   3300
   ClientWidth     =   4245
   Height          =   3465
   Left            =   1725
   LinkTopic       =   "EnterOptions"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3060
   ScaleWidth      =   4245
   ShowInTaskbar   =   0   'False
   Top             =   2955
   Width           =   4365
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   1680
      TabIndex        =   9
      Top             =   2520
      Width           =   1095
   End
   Begin VB.CommandButton cmdCancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   3000
      TabIndex        =   8
      Top             =   2520
      Width           =   1095
   End
   Begin VB.TextBox txtTopic 
      Height          =   285
      Left            =   120
      TabIndex        =   6
      Top             =   2040
      Width           =   3975
   End
   Begin VB.CommandButton cmdGetTopic 
      Caption         =   "Get Topic"
      Height          =   255
      Left            =   2160
      TabIndex        =   5
      Top             =   1800
      Width           =   975
   End
   Begin VB.CommandButton cmdSetTopic 
      Caption         =   "Set Topic"
      Height          =   255
      Left            =   3120
      TabIndex        =   4
      Top             =   1800
      Width           =   975
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
      TabIndex        =   7
      Top             =   1800
      Width           =   2175
   End
   Begin VB.Label Label7 
      BackColor       =   &H00C0FFC0&
      Caption         =   "Select or type your Alias:"
      Height          =   255
      Left            =   120
      TabIndex        =   3
      Top             =   120
      Width           =   3975
   End
   Begin VB.Label Label8 
      BackColor       =   &H00C0FFC0&
      Caption         =   "Select or type the chat room:"
      Height          =   255
      Left            =   120
      TabIndex        =   2
      Top             =   960
      Width           =   3975
   End
End
Attribute VB_Name = "EnterOptions"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
Option Explicit

' Channel creation modes
Const OCX_CHANNEL_CREATE_JOIN = 1

Const OCX_CHANNEL_TOPICOP = 2        ' topic can be changed only by Host

Const SEC_PACKAGE = "ANON;ALL"
Private Sub cmdCancel_Click()
    Hide
End Sub

Private Sub cmdGetTopic_Click()
    On Error Resume Next
    txtTopic.Text = Main.MSChat1.RoomTopic
End Sub


Private Sub cmdOK_Click()
        
    On Error Resume Next
    
    cmdSetTopic_Click

    Main.lblAlias.Caption = ComboAlias.Text
    Main.lblRoomPath.Caption = ComboRoomPath.Text
    Main.MSChat1.EnterRoom ComboRoomPath.Text, "", ComboAlias.Text, SEC_PACKAGE, OCX_CHANNEL_CREATE_JOIN, OCX_CHANNEL_TOPICOP
    
    Hide
End Sub


Private Sub cmdSetTopic_Click()
    On Error Resume Next
    
    Main.MSChat1.RoomTopic = txtTopic.Text
End Sub


Private Sub Form_Load()
    ComboRoomPath.AddItem "//ComicSrv1.Microsoft.Com/#Checkers"
    ComboRoomPath.AddItem "//ComicSrv2.Microsoft.Com/#Checkers"
    
    ComboRoomPath.ListIndex = 0
    
    ComboAlias.AddItem "Player1"
    ComboAlias.AddItem "Player2"
    
    ComboAlias.ListIndex = 0
End Sub



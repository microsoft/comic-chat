VERSION 4.00
'****************************************************************************
'
'  Module:     ExecChat.EXE
'  File:       WhispToG.frm
'
'  Copyright (c) Microsoft Corporation 1996-1997
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************
Begin VB.Form WhispToGuest 
   BackColor       =   &H00FFFFC0&
   Caption         =   "Whisper to the Special Guest"
   ClientHeight    =   3240
   ClientLeft      =   2025
   ClientTop       =   1980
   ClientWidth     =   6690
   Height          =   3645
   Left            =   1965
   LinkTopic       =   "WhispToGuest"
   ScaleHeight     =   3240
   ScaleWidth      =   6690
   Top             =   1635
   Width           =   6810
   Begin VB.TextBox txtMessage 
      BeginProperty Font 
         name            =   "MS Sans Serif"
         charset         =   0
         weight          =   400
         size            =   9.75
         underline       =   0   'False
         italic          =   0   'False
         strikethrough   =   0   'False
      EndProperty
      Height          =   2295
      Left            =   120
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   0
      Top             =   360
      Width           =   6495
   End
   Begin VB.CommandButton cmdCancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   5760
      TabIndex        =   2
      Top             =   2760
      Width           =   855
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Height          =   375
      Left            =   4680
      TabIndex        =   1
      Top             =   2760
      Width           =   855
   End
   Begin VB.Label Label1 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Message to whisper:"
      Height          =   205
      Left            =   120
      TabIndex        =   3
      Top             =   120
      Width           =   1575
   End
End
Attribute VB_Name = "WhispToGuest"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
Option Explicit
Private Sub cmdCancel_Click()
    Unload WhispToGuest
End Sub

Private Sub cmdOK_Click()
    On Error GoTo errOK
    If (txtMessage.Text <> "") Then
        ExecC.MSChat.SendMessage lGuestID, 1, Trim(txtMessage.Text)
    End If
    Unload WhispToGuest
    Exit Sub

errOK:
    DisplayError "WhisperToGuest.cmdOK_Click"
End Sub



Private Sub Form_Load()
    On Error Resume Next
    txtMessage.SetFocus
End Sub



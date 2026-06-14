VERSION 4.00
'****************************************************************************
'
'  Module:     ExecChat.EXE
'  File:       WhispToH.frm
'
'  Copyright (c) Microsoft Corporation 1996-1997
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************
Begin VB.Form WhispToHosts 
   BackColor       =   &H00FFFFC0&
   Caption         =   "Whisper to Hosts"
   ClientHeight    =   3855
   ClientLeft      =   2055
   ClientTop       =   2010
   ClientWidth     =   7125
   Height          =   4260
   Left            =   1995
   LinkTopic       =   "Form1"
   ScaleHeight     =   3855
   ScaleWidth      =   7125
   Top             =   1665
   Width           =   7245
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
      Height          =   2895
      Left            =   2640
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   0
      Top             =   360
      Width           =   4335
   End
   Begin VB.CommandButton Cancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   6120
      TabIndex        =   3
      Top             =   3360
      Width           =   855
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Height          =   375
      Left            =   5040
      TabIndex        =   2
      Top             =   3360
      Width           =   855
   End
   Begin VB.ListBox lstHosts 
      Height          =   3375
      Left            =   120
      MultiSelect     =   2  'Extended
      TabIndex        =   1
      Top             =   360
      Width           =   2295
   End
   Begin VB.Label Label2 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Message To Whisper:"
      Height          =   210
      Left            =   2640
      TabIndex        =   5
      Top             =   120
      Width           =   1695
   End
   Begin VB.Label Label1 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Host List:"
      Height          =   205
      Left            =   120
      TabIndex        =   4
      Top             =   120
      Width           =   735
   End
End
Attribute VB_Name = "WhispToHosts"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
Option Explicit
Private Sub Cancel_Click()
    txtMessage.Text = ""
    Hide
End Sub


Private Sub cmdOK_Click()
    On Error Resume Next
    
    ' get the selected hosts and construct an array of longs
    Dim rglHostIDs(0 To 15) As Long
    Dim nCnt As Integer
    Dim nArrayIndex As Integer
    
    nArrayIndex = 0
    For nCnt = 0 To lstHosts.ListCount - 1
        If lstHosts.Selected(nCnt) Then
            If nArrayIndex > 15 Then
                MsgBox "Too many hosts are selected. You are limited to 10 hosts.", vbOKOnly, "Executive Chat Monitoring Tool"
                Exit For
            End If
            If (lstHosts.ItemData(nCnt) <> ExecC.MSChat.ThisParticipantID) Then
                rglHostIDs(nArrayIndex) = lstHosts.ItemData(nCnt)
                nArrayIndex = nArrayIndex + 1
            End If
        End If
    Next nCnt
    
    If (ExecC.MSChat.State = 3 And nArrayIndex > 0 And txtMessage.Text <> "") Then
        ExecC.MSChat.SendMessage rglHostIDs, nArrayIndex, Trim(txtMessage.Text)
    End If
    
    txtMessage.Text = ""
    Hide
End Sub



Private Sub Form_Activate()
    On Error Resume Next
    txtMessage.SetFocus
End Sub


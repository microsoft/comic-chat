VERSION 4.00
'****************************************************************************
'
'  Module:     CHATAD.EXE
'  File:       UISet.frm
'
'  Copyright (c) Microsoft Corporation 1996-1997
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************
Begin VB.Form UISettings 
   BackColor       =   &H00C0FFC0&
   Caption         =   "MSChat Control UI Setting"
   ClientHeight    =   5070
   ClientLeft      =   6840
   ClientTop       =   2730
   ClientWidth     =   4800
   Height          =   5475
   Left            =   6780
   LinkTopic       =   "UISettings"
   ScaleHeight     =   5070
   ScaleWidth      =   4800
   Top             =   2385
   Width           =   4920
   Begin VB.CommandButton cmdApply 
      Caption         =   "Apply"
      Height          =   375
      Left            =   3480
      TabIndex        =   14
      Top             =   4560
      Width           =   1215
   End
   Begin VB.CommandButton cmdCancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   2160
      TabIndex        =   13
      Top             =   4560
      Width           =   1215
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   840
      TabIndex        =   0
      Top             =   4560
      Width           =   1215
   End
   Begin VB.Frame Frame1 
      BackColor       =   &H00C0FFC0&
      Height          =   4215
      Left            =   120
      TabIndex        =   1
      Top             =   120
      Width           =   4575
      Begin VB.CheckBox chkBut 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Display Send Button?"
         Height          =   255
         Left            =   120
         TabIndex        =   12
         TabStop         =   0   'False
         Top             =   3480
         Width           =   4335
      End
      Begin VB.CheckBox chkMem 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Display Member ListBox?"
         Height          =   255
         Left            =   120
         TabIndex        =   11
         TabStop         =   0   'False
         Top             =   2400
         Width           =   4335
      End
      Begin VB.CheckBox chkRec 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Display Receive TextBox?"
         Height          =   255
         Left            =   120
         TabIndex        =   10
         TabStop         =   0   'False
         Top             =   2760
         Width           =   4335
      End
      Begin VB.CheckBox chkSen 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Display Send TextBox?"
         Height          =   255
         Left            =   120
         TabIndex        =   9
         TabStop         =   0   'False
         Top             =   3840
         Width           =   4335
      End
      Begin VB.CheckBox chkDisplayError 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Display error messages?"
         Height          =   255
         Left            =   120
         TabIndex        =   8
         Top             =   1680
         Width           =   4335
      End
      Begin VB.CheckBox chkSensitiveURLs 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Sensitive URLs in history window?"
         Height          =   255
         Left            =   120
         TabIndex        =   7
         Top             =   2040
         Width           =   4335
      End
      Begin VB.CheckBox chkMemChg 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Inform when privilege, alias changed or member kicked?"
         Height          =   255
         Left            =   120
         TabIndex        =   6
         Top             =   1320
         Width           =   4335
      End
      Begin VB.CheckBox chkBlank 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Insert blank line between revd messages?"
         Height          =   255
         Left            =   120
         TabIndex        =   5
         TabStop         =   0   'False
         Top             =   240
         Width           =   4335
      End
      Begin VB.CheckBox chkEnter 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Inform when participant enters the room?"
         Height          =   255
         Left            =   120
         TabIndex        =   4
         TabStop         =   0   'False
         Top             =   600
         Width           =   4335
      End
      Begin VB.CheckBox chkLeave 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Inform when participant leaves the room?"
         Height          =   255
         Left            =   120
         TabIndex        =   3
         TabStop         =   0   'False
         Top             =   960
         Width           =   4335
      End
      Begin VB.CheckBox chkWhi 
         BackColor       =   &H00C0FFC0&
         Caption         =   "Display Whisper Button?"
         Height          =   255
         Left            =   120
         TabIndex        =   2
         TabStop         =   0   'False
         Top             =   3120
         Width           =   4335
      End
   End
End
Attribute VB_Name = "UISettings"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
Option Explicit

Const SEND_BUTTON = 1
Const WHISPER_BUTTON = 2
Const PARTICIPANT_LISTBOX = 4
Const RECEIVE_TEXTBOX = 8
Const SEND_TEXTBOX = 16
Const INFORM_JOIN = 32
Const INFORM_LEAVE = 64
Const INFORM_MEMCHG = 128
Const INSERT_BLANK = 256
Const DISPLAY_ERRORMESSAGES = 512
Const ALLOW_RESIZING = 1024
Const SENSITIVE_URLS = 2048

Dim nUIOption As Integer
Dim nOriginalUIOption As Integer
Sub UpdateUIOption()
    
    On Error GoTo UpdateUIOptionErr
    
    nUIOption = chkBut.Value * SEND_BUTTON + chkWhi.Value * WHISPER_BUTTON + chkMem.Value * PARTICIPANT_LISTBOX + chkRec.Value * RECEIVE_TEXTBOX + chkSen.Value * SEND_TEXTBOX + chkEnter.Value * INFORM_JOIN + chkLeave.Value * INFORM_LEAVE + chkMemChg.Value * INFORM_MEMCHG + chkBlank.Value * INSERT_BLANK + chkDisplayError.Value * DISPLAY_ERRORMESSAGES + chkSensitiveURLs.Value * SENSITIVE_URLS
    Exit Sub
    
UpdateUIOptionErr:
    MsgBox Err.Description
End Sub

Private Sub chkBlank_Click()
    UpdateUIOption
End Sub

Private Sub chkBut_Click()
    UpdateUIOption
End Sub


Private Sub chkDisplayError_Click()
    UpdateUIOption
End Sub


Private Sub chkEnter_Click()
    UpdateUIOption
End Sub


Private Sub chkLeave_Click()
    UpdateUIOption
End Sub


Private Sub chkMem_Click()
    UpdateUIOption
End Sub


Private Sub chkMemChg_Click()
    UpdateUIOption
End Sub


Private Sub chkRec_Click()
    UpdateUIOption
End Sub


Private Sub chkSen_Click()
    UpdateUIOption
End Sub


Private Sub chkSensitiveURLs_Click()
    UpdateUIOption
End Sub


Private Sub chkWhi_Click()
    UpdateUIOption
End Sub


Private Sub cmdApply_Click()
    On Error GoTo errApply_Click
    Main.MSChat1.UIOption = nUIOption
    Exit Sub
    
errApply_Click:
    Main.MSChat1.UIOption = nOriginalUIOption
End Sub


Private Sub cmdCancel_Click()
    Main.MSChat1.UIOption = nOriginalUIOption
    Hide
End Sub

Private Sub cmdOK_Click()
    On Error GoTo errOK_Click
    Main.MSChat1.UIOption = nUIOption
    Hide
    Exit Sub
    
errOK_Click:
    Main.MSChat1.UIOption = nOriginalUIOption
End Sub

Private Sub Form_Load()
    
    nOriginalUIOption = Main.MSChat1.UIOption
    If (nOriginalUIOption And SEND_BUTTON) Then
        chkBut.Value = 1
    Else
        chkBut.Value = 0
    End If
    If (nOriginalUIOption And WHISPER_BUTTON) Then
        chkWhi.Value = 1
    Else
        chkWhi.Value = 0
    End If
    If (nOriginalUIOption And PARTICIPANT_LISTBOX) Then
        chkMem.Value = 1
    Else
        chkMem.Value = 0
    End If
    If (nOriginalUIOption And RECEIVE_TEXTBOX) Then
        chkRec.Value = 1
    Else
        chkRec.Value = 0
    End If
    If (nOriginalUIOption And SEND_TEXTBOX) Then
        chkSen.Value = 1
    Else
        chkSen.Value = 0
    End If
    If (nOriginalUIOption And INFORM_JOIN) Then
        chkEnter.Value = 1
    Else
        chkEnter.Value = 0
    End If
    If (nOriginalUIOption And INFORM_LEAVE) Then
        chkLeave.Value = 1
    Else
        chkLeave.Value = 0
    End If
    If (nOriginalUIOption And INFORM_MEMCHG) Then
        chkMemChg.Value = 1
    Else
        chkMemChg.Value = 0
    End If
    If (nOriginalUIOption And INSERT_BLANK) Then
        chkBlank.Value = 1
    Else
        chkBlank.Value = 0
    End If
    If (nOriginalUIOption And DISPLAY_ERRORMESSAGES) Then
        chkDisplayError.Value = 1
    Else
        chkDisplayError.Value = 0
    End If
    If (nOriginalUIOption And SENSITIVE_URLS) Then
        chkSensitiveURLs.Value = 1
    Else
        chkSensitiveURLs.Value = 0
    End If
End Sub


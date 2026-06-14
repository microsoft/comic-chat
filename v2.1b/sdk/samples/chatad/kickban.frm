VERSION 4.00
'****************************************************************************
'
'  Module:     CHATAD.EXE
'  File:       KickBan.frm
'
'  Copyright (c) Microsoft Corporation 1996-1997
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************
Begin VB.Form KickBan 
   BackColor       =   &H00C0FFC0&
   Caption         =   "Kick, Ban and Re-instate Participant"
   ClientHeight    =   6690
   ClientLeft      =   2595
   ClientTop       =   1905
   ClientWidth     =   5745
   Height          =   7095
   Left            =   2535
   LinkTopic       =   "KickBan"
   ScaleHeight     =   6690
   ScaleWidth      =   5745
   Top             =   1560
   Width           =   5865
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Height          =   375
      Left            =   4440
      TabIndex        =   14
      Top             =   6240
      Width           =   1215
   End
   Begin VB.CommandButton cmdReinstate 
      Caption         =   "Re-instate Participant"
      Height          =   375
      Left            =   3720
      TabIndex        =   13
      Top             =   5640
      Width           =   1935
   End
   Begin VB.TextBox txtAlias 
      Height          =   405
      Left            =   120
      TabIndex        =   12
      Top             =   5040
      Width           =   5535
   End
   Begin VB.CommandButton cmdBanParticipant 
      Caption         =   "Ban Participant"
      Height          =   375
      Left            =   1560
      TabIndex        =   10
      Top             =   5640
      Width           =   1935
   End
   Begin VB.TextBox txtKickReason 
      Height          =   375
      Left            =   120
      TabIndex        =   5
      Top             =   2040
      Width           =   5535
   End
   Begin VB.CommandButton cmdKickParticipant 
      Caption         =   "Kick Participant"
      Height          =   375
      Left            =   3720
      TabIndex        =   0
      Top             =   2640
      Width           =   1935
   End
   Begin VB.Line Line2 
      X1              =   5640
      X2              =   120
      Y1              =   6120
      Y2              =   6120
   End
   Begin VB.Label Label9 
      BackColor       =   &H00C0FFC0&
      Caption         =   "Participant Alias:"
      Height          =   255
      Left            =   120
      TabIndex        =   11
      Top             =   4800
      Width           =   1335
   End
   Begin VB.Label Label8 
      BackColor       =   &H00C0FFC0&
      Caption         =   "2. Click the 'Ban Participant' or 'Re-instate Participant' button."
      Height          =   255
      Left            =   120
      TabIndex        =   9
      Top             =   4320
      Width           =   5535
   End
   Begin VB.Label Label7 
      BackColor       =   &H00C0FFC0&
      Caption         =   "1. Type the alias of the participant to ban or re-instate."
      Height          =   255
      Left            =   120
      TabIndex        =   8
      Top             =   3840
      Width           =   5535
   End
   Begin VB.Label Label6 
      BackColor       =   &H00C0FFC0&
      Caption         =   "To ban or re-instate a participant:"
      BeginProperty Font 
         name            =   "MS Sans Serif"
         charset         =   0
         weight          =   700
         size            =   8.25
         underline       =   0   'False
         italic          =   0   'False
         strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   120
      TabIndex        =   7
      Top             =   3360
      Width           =   5535
   End
   Begin VB.Label Label5 
      BackColor       =   &H00C0FFC0&
      Caption         =   "To kick a participant:"
      BeginProperty Font 
         name            =   "MS Sans Serif"
         charset         =   0
         weight          =   700
         size            =   8.25
         underline       =   0   'False
         italic          =   0   'False
         strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   120
      TabIndex        =   6
      Top             =   120
      Width           =   5535
   End
   Begin VB.Line Line1 
      X1              =   5640
      X2              =   120
      Y1              =   3120
      Y2              =   3120
   End
   Begin VB.Label Label4 
      BackColor       =   &H00C0FFC0&
      Caption         =   "Kick Reason:"
      Height          =   255
      Left            =   120
      TabIndex        =   4
      Top             =   1800
      Width           =   1095
   End
   Begin VB.Label Label3 
      BackColor       =   &H00C0FFC0&
      Caption         =   "3. Click the 'Kick Participant' button."
      Height          =   255
      Left            =   120
      TabIndex        =   3
      Top             =   1320
      Width           =   5535
   End
   Begin VB.Label Label2 
      BackColor       =   &H00C0FFC0&
      Caption         =   "2. Enter the optinal kick reason."
      Height          =   255
      Left            =   120
      TabIndex        =   2
      Top             =   960
      Width           =   5535
   End
   Begin VB.Label Label1 
      BackColor       =   &H00C0FFC0&
      Caption         =   "1. Select the participant you want to kick in the control's member list box."
      Height          =   255
      Left            =   120
      TabIndex        =   1
      Top             =   600
      Width           =   5535
   End
End
Attribute VB_Name = "KickBan"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
Private Sub cmdBanParticipant_Click()
    On Error GoTo errBan
    If (txtAlias.Text <> "") Then
        Main.MSChat1.BanParticipant txtAlias.Text, True
    Else
        MsgBox "Please enter the alias of the participant to ban and try again."
    End If
    Exit Sub
    
errBan:
    Hide
End Sub

Private Sub cmdKickParticipant_Click()
    On Error GoTo errKick
    Main.MSChat1.KickParticipant -1, txtKickReason.Text
    Exit Sub
    
errKick:
    Hide
End Sub

Private Sub cmdOK_Click()
    Hide
End Sub


Private Sub Command1_Click()

End Sub


Private Sub cmdReinstate_Click()
    On Error GoTo errReinstate
    If (txtAlias.Text <> "") Then
        Main.MSChat1.BanParticipant txtAlias.Text, False
    Else
        MsgBox "Please enter the alias of the participant to re-instate and try again."
    End If
    Exit Sub
    
errReinstate:
    Hide
End Sub



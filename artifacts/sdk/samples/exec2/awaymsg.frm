VERSION 5.00
Begin VB.Form frmAwayMessage 
   Caption         =   "Away message"
   ClientHeight    =   3195
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   4770
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3195
   ScaleWidth      =   4770
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox txtAwayMessage 
      Height          =   1695
      Left            =   240
      TabIndex        =   3
      Text            =   "Will be back soon."
      Top             =   600
      Width           =   4215
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   360
      Left            =   2640
      TabIndex        =   1
      Tag             =   "Cancel"
      Top             =   2520
      Width           =   1140
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   360
      Left            =   1200
      TabIndex        =   0
      Tag             =   "OK"
      Top             =   2520
      Width           =   1140
   End
   Begin VB.Label Label1 
      Caption         =   "Away message:"
      Height          =   255
      Left            =   240
      TabIndex        =   2
      Top             =   240
      Width           =   1215
   End
End
Attribute VB_Name = "frmAwayMessage"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'****************************************************************************
'
'  Module:     Exec2.exe
'  File:       AwayMsg.frm
'
'  Copyright (c) Microsoft Corporation 1996-1998
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************

Option Explicit


Public OK As Boolean


Private Sub cmdCancel_Click()
    OK = False
    Me.Hide
End Sub


Private Sub cmdOK_Click()
    OK = True
    Me.Hide
End Sub



VERSION 5.00
Begin VB.Form frmWhyKick 
   Caption         =   "Reason to Kick"
   ClientHeight    =   2625
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   6030
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2625
   ScaleWidth      =   6030
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   360
      Left            =   2040
      TabIndex        =   3
      Tag             =   "OK"
      Top             =   2040
      Width           =   1140
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   360
      Left            =   3480
      TabIndex        =   2
      Tag             =   "Cancel"
      Top             =   2040
      Width           =   1140
   End
   Begin VB.TextBox txtReason 
      Height          =   645
      Left            =   120
      TabIndex        =   1
      Text            =   "You are not welcome."
      Top             =   600
      Width           =   5655
   End
   Begin VB.Label Label1 
      Caption         =   "Reason to kick:"
      Height          =   255
      Left            =   120
      TabIndex        =   0
      Top             =   240
      Width           =   1215
   End
End
Attribute VB_Name = "frmWhyKick"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
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




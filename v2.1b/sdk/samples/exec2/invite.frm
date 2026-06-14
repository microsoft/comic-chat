VERSION 5.00
Begin VB.Form frmInvite 
   Caption         =   "Invite Others"
   ClientHeight    =   1575
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   4935
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1575
   ScaleWidth      =   4935
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton btnCancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   2520
      TabIndex        =   3
      Top             =   1080
      Width           =   1335
   End
   Begin VB.CommandButton btnOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   840
      TabIndex        =   2
      Top             =   1080
      Width           =   1335
   End
   Begin VB.TextBox txtNick 
      Height          =   285
      Left            =   120
      TabIndex        =   1
      Text            =   "NickName"
      Top             =   480
      Width           =   4335
   End
   Begin VB.Label Label1 
      Caption         =   "Invite this person to the chat room:"
      Height          =   255
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   2775
   End
End
Attribute VB_Name = "frmInvite"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Public OK As Boolean


Private Sub btnCancel_Click()
    OK = False
    Me.Hide
End Sub


Private Sub btnOK_Click()
    If txtNick = "" Then
        MsgBox "Please input the person's nick name."
        Exit Sub
    End If
    
    OK = True
    Me.Hide
End Sub

VERSION 5.00
Begin VB.Form frmEvents 
   Caption         =   "Events"
   ClientHeight    =   4155
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   7110
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4155
   ScaleWidth      =   7110
   StartUpPosition =   3  'Windows Default
   Begin VB.ListBox lstEvents 
      Height          =   2985
      Left            =   120
      TabIndex        =   1
      Top             =   720
      Width           =   6855
   End
   Begin VB.CommandButton btnClearEvent 
      Caption         =   "Clear Event"
      Height          =   375
      Left            =   120
      TabIndex        =   0
      Top             =   240
      Width           =   1575
   End
End
Attribute VB_Name = "frmEvents"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

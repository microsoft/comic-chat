VERSION 5.00
Begin VB.Form frmRuleEdit 
   Caption         =   "Edit Room Rule"
   ClientHeight    =   1830
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   6270
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1830
   ScaleWidth      =   6270
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton btnOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   1560
      TabIndex        =   5
      Top             =   1320
      Width           =   1215
   End
   Begin VB.CommandButton btnCancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   3360
      TabIndex        =   4
      Top             =   1320
      Width           =   1215
   End
   Begin VB.TextBox txtWord 
      Height          =   285
      Left            =   2640
      TabIndex        =   1
      Text            =   "BadWord"
      Top             =   240
      Width           =   3255
   End
   Begin VB.TextBox txtPoint 
      Height          =   285
      Left            =   2640
      TabIndex        =   0
      Text            =   "50"
      Top             =   600
      Width           =   1575
   End
   Begin VB.Label Label1 
      Caption         =   "Word To Watch:"
      Height          =   255
      Left            =   120
      TabIndex        =   3
      Top             =   240
      Width           =   1335
   End
   Begin VB.Label Label2 
      Caption         =   "Points cost on each occurrence:"
      Height          =   255
      Left            =   120
      TabIndex        =   2
      Top             =   600
      Width           =   2415
   End
End
Attribute VB_Name = "frmRuleEdit"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public OK As Boolean
Public lvwRules As ListView
Public CurrentItem As ListItem



Private Sub Form_Unload(Cancel As Integer)
    Set lvwRules = Nothing
    Set CurrentItem = Nothing
End Sub


Private Sub btnCancel_Click()
    OK = False
    Me.Hide
End Sub


Private Sub btnOK_Click()
    If txtWord = "" Then
        MsgBox "You must enter a word to watch."
        Exit Sub
    End If
    
    Dim item As ListItem
    If Not lvwRules Is Nothing Then
        Set item = lvwRules.FindItem(txtWord)
        
        If (item Is Nothing) Then
            GoTo OK_Here
        End If
        
        If (CurrentItem Is Nothing) Then
            MsgBox txtWord & " is already in the list."
            GoTo Cleanup
        End If
        
        If (Not item.index = CurrentItem.index) Then
            MsgBox txtWord & " is already in the list."
            GoTo Cleanup
        End If
        
    End If

OK_Here:
    OK = True
    Me.Hide
    
Cleanup:
    Set item = Nothing
End Sub

VERSION 5.00
Object = "{3B7C8863-D78F-101B-B9B5-04021C009402}#1.1#0"; "richtx32.OCX"
Begin VB.Form frmAnswer 
   Caption         =   "Answer Question"
   ClientHeight    =   5235
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   6030
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5235
   ScaleWidth      =   6030
   StartUpPosition =   3  'Windows Default
   Begin RichTextLib.RichTextBox rtbAnswer 
      Height          =   2535
      Left            =   120
      TabIndex        =   0
      Top             =   1920
      Width           =   5775
      _ExtentX        =   10186
      _ExtentY        =   4471
      _Version        =   327681
      AutoVerbMenu    =   -1  'True
      TextRTF         =   $"Answer.frx":0000
   End
   Begin VB.CommandButton btnCancel 
      Caption         =   "Cancel"
      Default         =   -1  'True
      Height          =   375
      Left            =   3000
      TabIndex        =   2
      Top             =   4680
      Width           =   1215
   End
   Begin VB.CommandButton btnSend 
      Caption         =   "&Send"
      Height          =   375
      Left            =   1560
      TabIndex        =   1
      Top             =   4680
      Width           =   1215
   End
   Begin RichTextLib.RichTextBox rtbQuestion 
      Height          =   1095
      Left            =   120
      TabIndex        =   4
      Top             =   360
      Width           =   5775
      _ExtentX        =   10186
      _ExtentY        =   1931
      _Version        =   327681
      ReadOnly        =   -1  'True
      AutoVerbMenu    =   -1  'True
      TextRTF         =   $"Answer.frx":012F
   End
   Begin VB.Label Label2 
      Caption         =   "Answer:"
      Height          =   255
      Left            =   120
      TabIndex        =   5
      Top             =   1680
      Width           =   615
   End
   Begin VB.Label Label1 
      Caption         =   "Question:"
      Height          =   255
      Left            =   120
      TabIndex        =   3
      Top             =   120
      Width           =   735
   End
End
Attribute VB_Name = "frmAnswer"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public bSent As Boolean


Private Sub btnCancel_Click()
    bSent = False
    Me.Hide
End Sub


Private Sub btnSend_Click()
    If rtbQuestion.Text = "" Then
        MsgBox "There's no question."
        Exit Sub
    End If
    
    If rtbAnswer.Text = "" Then
        MsgBox "Please answer the question."
        Exit Sub
    End If
    
    Dim strMsg As String
    strMsg = "Question: " & rtbQuestion.Text & vbCrLf & vbCrLf & "Answer: " & rtbAnswer.Text
    
    On Error Resume Next
    fMainForm.ChatUI1.Channel1.SendMessage msgtNormal, strMsg
    
    If Not Err = 0 Then
        MsgBox "Error in sending message: " & Err.Description
        bSent = False
        Exit Sub
    End If
    
    bSent = True
    Me.Hide
End Sub

Private Sub Form_Load()
'    rtbAnswer.SetFocus
End Sub

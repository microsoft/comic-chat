VERSION 5.00
Object = "{6B7E6392-850A-101B-AFC0-4210102A8DA7}#1.1#0"; "COMCTL32.OCX"
Begin VB.Form frmWarnings 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Warnings"
   ClientHeight    =   5235
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   6630
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5235
   ScaleWidth      =   6630
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton btnCancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   4200
      TabIndex        =   3
      Top             =   4560
      Width           =   1215
   End
   Begin VB.CommandButton btnOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   2400
      TabIndex        =   2
      Top             =   4560
      Width           =   1215
   End
   Begin VB.CommandButton btnRemove 
      Caption         =   "Remove"
      Height          =   375
      Left            =   720
      TabIndex        =   1
      Top             =   4560
      Width           =   1215
   End
   Begin ComctlLib.ListView lvwWarnings 
      Height          =   3255
      Left            =   120
      TabIndex        =   0
      Top             =   720
      Width           =   6375
      _ExtentX        =   11245
      _ExtentY        =   5741
      View            =   3
      LabelWrap       =   -1  'True
      HideSelection   =   -1  'True
      _Version        =   327680
      ForeColor       =   -2147483640
      BackColor       =   -2147483643
      BorderStyle     =   1
      Appearance      =   1
      MouseIcon       =   "Warnings.frx":0000
      NumItems        =   4
      BeginProperty ColumnHeader(1) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
         Key             =   ""
         Object.Tag             =   ""
         Text            =   "Nick Name"
         Object.Width           =   2540
      EndProperty
      BeginProperty ColumnHeader(2) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
         SubItemIndex    =   1
         Key             =   ""
         Object.Tag             =   ""
         Text            =   "User Name"
         Object.Width           =   2540
      EndProperty
      BeginProperty ColumnHeader(3) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
         SubItemIndex    =   2
         Key             =   ""
         Object.Tag             =   ""
         Text            =   "IP Address"
         Object.Width           =   2540
      EndProperty
      BeginProperty ColumnHeader(4) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
         SubItemIndex    =   3
         Key             =   ""
         Object.Tag             =   ""
         Text            =   "Points"
         Object.Width           =   1270
      EndProperty
   End
   Begin VB.Label Label1 
      Caption         =   "Current warning status:"
      Height          =   255
      Left            =   120
      TabIndex        =   4
      Top             =   360
      Width           =   1815
   End
End
Attribute VB_Name = "frmWarnings"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit


Public Function AddPoints(ByRef strNick As String, ByRef strUser As String, _
                        ByRef strIP As String, ByVal point As Long) As Long

    Dim item As ListItem
    Dim i As Long
    
    For i = 1 To lvwWarnings.ListItems.count
        Set item = lvwWarnings.ListItems.item(i)
        If strUser = item.SubItems(1) And strIP = item.SubItems(2) Then
            item.SubItems(3) = item.SubItems(3) + point
            AddPoints = item.SubItems(3)
            Set item = Nothing
            Exit Function
        End If
        
        Set item = Nothing
    Next
    
    Set item = lvwWarnings.ListItems.Add
    item.Text = strNick
    item.SubItems(1) = strUser
    item.SubItems(2) = strIP
    item.SubItems(3) = point
    
    Set item = Nothing
    
    AddPoints = point
    
End Function



Private Sub btnCancel_Click()
    Me.Hide
End Sub


Private Sub btnOK_Click()
    Me.Hide
End Sub

Private Sub btnRemove_Click()
    If Not lvwWarnings.SelectedItem Is Nothing Then
        lvwWarnings.ListItems.Remove lvwWarnings.SelectedItem.Index
    End If
End Sub


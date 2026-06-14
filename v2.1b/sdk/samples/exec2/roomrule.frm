VERSION 5.00
Object = "{6B7E6392-850A-101B-AFC0-4210102A8DA7}#1.2#0"; "COMCTL32.OCX"
Begin VB.Form frmRoomRule 
   Caption         =   "Room rules"
   ClientHeight    =   5895
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   6885
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5895
   ScaleWidth      =   6885
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox txtKickReason 
      Height          =   285
      Left            =   1920
      TabIndex        =   18
      Text            =   "You are kicked because you said too many bad words."
      Top             =   4440
      Width           =   4455
   End
   Begin VB.TextBox txtWarningMessage 
      Height          =   285
      Left            =   1920
      TabIndex        =   16
      Text            =   "You are warned."
      Top             =   4080
      Width           =   4455
   End
   Begin VB.TextBox txtBanSeconds 
      Height          =   285
      Left            =   3720
      TabIndex        =   13
      Text            =   "0"
      Top             =   3720
      Width           =   855
   End
   Begin VB.CommandButton btnWarnings 
      Caption         =   "Warnings"
      Height          =   375
      Left            =   5520
      TabIndex        =   12
      Top             =   2520
      Width           =   1095
   End
   Begin VB.CommandButton btnOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   2160
      TabIndex        =   11
      Top             =   5400
      Width           =   1215
   End
   Begin VB.CommandButton btnCancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   3960
      TabIndex        =   10
      Top             =   5400
      Width           =   1215
   End
   Begin VB.TextBox txtBanMinutes 
      Height          =   285
      Left            =   1920
      TabIndex        =   8
      Text            =   "60"
      Top             =   3720
      Width           =   855
   End
   Begin VB.TextBox txtKickPoint 
      Height          =   285
      Left            =   1920
      TabIndex        =   7
      Text            =   "100"
      Top             =   3360
      Width           =   1335
   End
   Begin VB.CommandButton btnRemove 
      Caption         =   "&Remove"
      Height          =   375
      Left            =   5520
      TabIndex        =   4
      Top             =   1320
      Width           =   1095
   End
   Begin VB.CommandButton btnEdit 
      Caption         =   "&Edit"
      Height          =   375
      Left            =   5520
      TabIndex        =   3
      Top             =   1800
      Width           =   1095
   End
   Begin ComctlLib.ListView lvwRules 
      Height          =   2535
      Left            =   120
      TabIndex        =   1
      Top             =   480
      Width           =   5055
      _ExtentX        =   8916
      _ExtentY        =   4471
      View            =   3
      Sorted          =   -1  'True
      LabelWrap       =   -1  'True
      HideSelection   =   -1  'True
      _Version        =   327682
      ForeColor       =   -2147483640
      BackColor       =   -2147483643
      BorderStyle     =   1
      Appearance      =   1
      NumItems        =   2
      BeginProperty ColumnHeader(1) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
         Key             =   ""
         Object.Tag             =   ""
         Text            =   "Words"
         Object.Width           =   5080
      EndProperty
      BeginProperty ColumnHeader(2) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
         SubItemIndex    =   1
         Key             =   "SortKey"
         Object.Tag             =   ""
         Text            =   "Point"
         Object.Width           =   2540
      EndProperty
   End
   Begin VB.CommandButton btnAddWord 
      Caption         =   "&Add"
      Height          =   375
      Left            =   5520
      TabIndex        =   0
      Top             =   840
      Width           =   1095
   End
   Begin VB.Label Label7 
      Caption         =   "Kick reason:"
      Height          =   255
      Left            =   120
      TabIndex        =   17
      Top             =   4440
      Width           =   975
   End
   Begin VB.Label Label6 
      Caption         =   "Warning message:"
      Height          =   255
      Left            =   120
      TabIndex        =   15
      Top             =   4080
      Width           =   1335
   End
   Begin VB.Label Label5 
      Caption         =   "seconds"
      Height          =   255
      Left            =   4800
      TabIndex        =   14
      Top             =   3720
      Width           =   615
   End
   Begin VB.Label Label4 
      Caption         =   "minutes"
      Height          =   255
      Left            =   3000
      TabIndex        =   9
      Top             =   3720
      Width           =   615
   End
   Begin VB.Label Label3 
      Caption         =   "Ban duration after kick:"
      Height          =   255
      Left            =   120
      TabIndex        =   6
      Top             =   3720
      Width           =   1695
   End
   Begin VB.Label Label2 
      Caption         =   "Points to kick:"
      Height          =   255
      Left            =   120
      TabIndex        =   5
      Top             =   3360
      Width           =   1095
   End
   Begin VB.Label Label1 
      Caption         =   "Words to watch:"
      Height          =   255
      Left            =   120
      TabIndex        =   2
      Top             =   120
      Width           =   1335
   End
End
Attribute VB_Name = "frmRoomRule"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public lBanDurationSeconds As Long




Private Sub btnWarnings_Click()
    fMainForm.ChatUI1.fWarnings.Show vbModal, Me
End Sub



Public Sub CheckRules(ByRef strNick As String, ByRef strMesg As String)
    Dim i As Long
    Dim item As ListItem
    Dim point As Long
    Dim total As Long
    Dim varNick
    Dim strUser As String
    Dim strIP As String
    
    point = 0
    
    For i = 1 To lvwRules.ListItems.count
        Set item = lvwRules.ListItems.item(i)
        If FindWord(strMesg, item.Text) Then
            point = point + item.SubItems(1)
        End If
        
        Set item = Nothing
    Next
    
    If point = 0 Then
        Exit Sub
    End If
    
    varNick = strNick
    strUser = VPropertyFromChatItems(fMainForm.MsChatPr1.UserProperty(pnUserName, varNick), pnUserName)
    strIP = VPropertyFromChatItems(fMainForm.MsChatPr1.UserProperty(pnIPAddress, varNick), pnIPAddress)
    
    total = fMainForm.ChatUI1.fWarnings.AddPoints(strNick, strUser, strIP, point)
    
    If total >= txtKickPoint Then
        ' kick and ban
        fMainForm.ChatUI1.Channel1.KickMember strNick, txtKickReason
        fMainForm.ChatUI1.fBanUsers.BanMember strNick, strUser, strIP, lBanDurationSeconds
    Else
        ' send warning message
        fMainForm.ChatUI1.Channel1.SendMessage msgtNormal, txtWarningMessage
    End If
    
End Sub


Function FindWord(ByRef strMesg As String, ByRef strWord As String, Optional ByVal nHow As Long = 0) As Boolean
    If strWord = "" Then
        MsgBox "Error: Empty word to find in a string!"
        FindWord = False
        Exit Function
    End If
    
    Dim StartPos As Long
    Dim Pos As Long
    Dim LeftPos As Long
    Dim RightPos As Long
    Dim LeftCh As Integer
    Dim RightCh As Integer
    
    StartPos = 1
    
    Do While StartPos < Len(strMesg)
        Pos = InStr(StartPos, strMesg, strWord, vbTextCompare)
        If Pos = 0 Then
            FindWord = False
            Exit Function
        End If
        
        FindWord = True
        
        Rem check the left and right char
        LeftPos = Pos - 1
        RightPos = Pos + Len(strWord)
        
        If LeftPos > 0 Then
            LeftCh = Asc(Mid(strMesg, LeftPos, 1))
        Else
            LeftCh = 0
        End If
        
        If RightPos <= Len(strMesg) Then
            RightCh = Asc(Mid(strMesg, RightPos, 1))
        Else
            RightCh = 0
        End If
        
        If IsSeperatorChar(LeftCh) And IsSeperatorChar(RightCh) Then
            FindWord = True
            Exit Function
        End If
        
        StartPos = Pos + 1
    Loop
    
    FindWord = False
End Function



Private Sub btnAddWord_Click()
    Dim fRuleEdit As New frmRuleEdit
    Set fRuleEdit.lvwRules = lvwRules
    Set fRuleEdit.CurrentItem = Nothing
    fRuleEdit.Show vbModal, Me
    
    If fRuleEdit.OK = True Then
        Dim item As ListItem
        Set item = lvwRules.ListItems.Add
        item.Text = fRuleEdit.txtWord
        item.SubItems(1) = fRuleEdit.txtPoint
    End If
    
    Unload fRuleEdit
    Set fRuleEdit = Nothing
End Sub


Private Sub btnCancel_Click()
    Me.Hide
End Sub


Private Sub btnEdit_Click()
    Dim item As ListItem
    Set item = lvwRules.SelectedItem
    If item Is Nothing Then
        Exit Sub
    End If
    
    Dim fRuleEdit As New frmRuleEdit
    Set fRuleEdit.lvwRules = lvwRules
    Set fRuleEdit.CurrentItem = item
    fRuleEdit.txtPoint = item.SubItems(1)
    fRuleEdit.txtWord = item.Text
    fRuleEdit.Show vbModal, Me
    
    If fRuleEdit.OK = True Then
        item.Text = fRuleEdit.txtWord
        item.SubItems(1) = fRuleEdit.txtPoint
    End If
    
    Unload fRuleEdit
    Set fRuleEdit = Nothing
    Set item = Nothing
End Sub


Private Sub btnOK_Click()
    lBanDurationSeconds = 0
    
    If Not txtBanMinutes = "" Then
        lBanDurationSeconds = lBanDurationSeconds + txtBanMinutes * 60
    End If
    
    If Not txtBanSeconds = "" Then
        lBanDurationSeconds = lBanDurationSeconds + txtBanSeconds
    End If
    
    Me.Hide
End Sub


Private Sub btnRemove_Click()
    If Not lvwRules.SelectedItem Is Nothing Then
        lvwRules.ListItems.Remove lvwRules.SelectedItem.index
    End If
End Sub

Private Sub lvwRules_DblClick()
    btnEdit_Click
End Sub


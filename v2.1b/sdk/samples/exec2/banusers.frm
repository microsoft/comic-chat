VERSION 5.00
Object = "{6B7E6392-850A-101B-AFC0-4210102A8DA7}#1.2#0"; "COMCTL32.OCX"
Begin VB.Form frmBanUsers 
   Caption         =   "Ban/Unban users"
   ClientHeight    =   6015
   ClientLeft      =   165
   ClientTop       =   450
   ClientWidth     =   7110
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   6015
   ScaleWidth      =   7110
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton btnUnban 
      Caption         =   "&UnBan"
      Height          =   360
      Left            =   840
      TabIndex        =   16
      Top             =   5520
      Width           =   1140
   End
   Begin VB.TextBox txtDurationSeconds 
      Height          =   285
      Left            =   5760
      TabIndex        =   14
      Top             =   600
      Width           =   615
   End
   Begin VB.Timer TimerUnban 
      Left            =   6240
      Top             =   1560
   End
   Begin VB.TextBox txtDurationMinutes 
      Height          =   285
      Left            =   4440
      TabIndex        =   12
      Top             =   600
      Width           =   615
   End
   Begin VB.CommandButton btnBan 
      Caption         =   "Ban This User "
      Height          =   375
      Left            =   240
      TabIndex        =   9
      Top             =   1080
      Width           =   1815
   End
   Begin VB.TextBox txtIPAddress 
      Height          =   285
      Left            =   1200
      TabIndex        =   8
      Top             =   600
      Width           =   1695
   End
   Begin VB.TextBox txtUserName 
      Height          =   285
      Left            =   4440
      TabIndex        =   6
      Top             =   120
      Width           =   2295
   End
   Begin VB.TextBox txtNick 
      Height          =   285
      Left            =   1200
      TabIndex        =   4
      Top             =   120
      Width           =   1695
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   360
      Left            =   3840
      TabIndex        =   2
      Tag             =   "Cancel"
      Top             =   5520
      Width           =   1140
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   360
      Left            =   2400
      TabIndex        =   1
      Tag             =   "OK"
      Top             =   5520
      Width           =   1140
   End
   Begin ComctlLib.ListView lvwBannedUsers 
      Height          =   2895
      Left            =   120
      TabIndex        =   0
      Top             =   2400
      Width           =   6855
      _ExtentX        =   12091
      _ExtentY        =   5106
      View            =   3
      LabelWrap       =   -1  'True
      HideSelection   =   -1  'True
      _Version        =   327682
      ForeColor       =   -2147483640
      BackColor       =   -2147483643
      BorderStyle     =   1
      Appearance      =   1
      NumItems        =   4
      BeginProperty ColumnHeader(1) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
         Key             =   "NickName"
         Object.Tag             =   ""
         Text            =   "Nick Name"
         Object.Width           =   2117
      EndProperty
      BeginProperty ColumnHeader(2) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
         SubItemIndex    =   1
         Key             =   "UserName"
         Object.Tag             =   ""
         Text            =   "User Name"
         Object.Width           =   2540
      EndProperty
      BeginProperty ColumnHeader(3) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
         SubItemIndex    =   2
         Key             =   "IPAddress"
         Object.Tag             =   ""
         Text            =   "IP Address"
         Object.Width           =   2540
      EndProperty
      BeginProperty ColumnHeader(4) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
         SubItemIndex    =   3
         Key             =   "TimeToUnban"
         Object.Tag             =   ""
         Text            =   "Time to Unban"
         Object.Width           =   2752
      EndProperty
   End
   Begin VB.Label Label7 
      Caption         =   "seconds"
      Height          =   255
      Left            =   6480
      TabIndex        =   15
      Top             =   600
      Width           =   615
   End
   Begin VB.Label Label6 
      Caption         =   "minutes"
      Height          =   255
      Left            =   5160
      TabIndex        =   13
      Top             =   600
      Width           =   615
   End
   Begin VB.Label Label5 
      Caption         =   "For:"
      Height          =   255
      Left            =   3480
      TabIndex        =   11
      Top             =   600
      Width           =   855
   End
   Begin VB.Label Label4 
      Caption         =   "Currently banned users:"
      Height          =   255
      Left            =   120
      TabIndex        =   10
      Top             =   2040
      Width           =   1815
   End
   Begin VB.Label Label3 
      Caption         =   "IP Address:"
      Height          =   255
      Left            =   240
      TabIndex        =   7
      Top             =   600
      Width           =   855
   End
   Begin VB.Label Label2 
      Caption         =   "User Name:"
      Height          =   255
      Left            =   3480
      TabIndex        =   5
      Top             =   120
      Width           =   855
   End
   Begin VB.Label Label1 
      Caption         =   "Nick Name:"
      Height          =   255
      Left            =   240
      TabIndex        =   3
      Top             =   120
      Width           =   855
   End
   Begin VB.Menu mnuBanned 
      Caption         =   "Banned Member Menu"
      Visible         =   0   'False
      Begin VB.Menu mnuBannedUnban 
         Caption         =   "&Unban"
      End
      Begin VB.Menu mnuBannedTest 
         Caption         =   "Test"
      End
   End
End
Attribute VB_Name = "frmBanUsers"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit


Const DefaultUnbanTimerInterval As Long = 10 * 1000     ' 10 seconds



Public Sub BanMember(ByRef strNickName As String, ByRef strUserName As String, _
                     ByRef strIPAddress As String, Optional ByVal lDurationSeconds As Long = 0)
    
    On Error GoTo ExitHere
    OcxBanMember strUserName, strIPAddress
    
    Dim item As ListItem
    
    Set item = lvwBannedUsers.ListItems.Add
    item.Text = txtNick
    item.SubItems(1) = txtUserName
    item.SubItems(2) = txtIPAddress
    
    If Not lDurationSeconds = 0 Then
        item.SubItems(3) = CStr(DateAdd("s", lDurationSeconds, Now))
    Else
        item.SubItems(3) = ""
    End If
    Exit Sub
    
ExitHere:
    If Not Err = 0 Then
        MsgBox "Error in BanMember " & strNickName & ": " & Err.Description
        Err.Clear
    End If
End Sub



Private Sub btnBan_Click()
    Dim lDurationSeconds As Long
    lDurationSeconds = 0
    
    If Not IsHost() Then
        MsgBox "You are not a host in this room."
        Exit Sub
    End If
    
    If txtUserName = "" Or txtIPAddress = "" Then
        MsgBox "You must enter both user name and IP address to ban a user."
        Exit Sub
    End If
    
    If Not txtDurationMinutes = "" Then
        lDurationSeconds = CLng(txtDurationMinutes) * 60
    End If
    
    If Not txtDurationSeconds = "" Then
        lDurationSeconds = lDurationSeconds + CLng(txtDurationSeconds)
    End If
    
    BanMember txtNick, txtUserName, txtIPAddress, lDurationSeconds
End Sub


Private Sub btnUnban_Click()
    If Not (lvwBannedUsers.SelectedItem Is Nothing) Then
        If Not IsHost() Then
            MsgBox "You are not a host in this room."
            Exit Sub
        End If
        
        OcxUnBanMember lvwBannedUsers.SelectedItem.SubItems(1), lvwBannedUsers.SelectedItem.SubItems(2)
        lvwBannedUsers.ListItems.Remove lvwBannedUsers.SelectedItem.index
    End If
End Sub


Private Sub lvwBannedUsers_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    If Button = vbRightButton Then
        If Not (lvwBannedUsers.SelectedItem Is Nothing) Then
            PopupMenu mnuBanned
        End If
    End If
End Sub


Private Sub mnuBannedUnban_Click()
    If Not (lvwBannedUsers.SelectedItem Is Nothing) Then
        OcxUnBanMember lvwBannedUsers.SelectedItem.SubItems(1), lvwBannedUsers.SelectedItem.SubItems(2)
        lvwBannedUsers.ListItems.Remove lvwBannedUsers.SelectedItem.index
    End If
End Sub


Private Sub Form_Load()
    TimerUnban.Interval = DefaultUnbanTimerInterval
    TimerUnban.Enabled = True
End Sub

Private Sub cmdOK_Click()
    Me.Hide
End Sub

Private Sub cmdCancel_Click()
    Me.Hide
End Sub


Private Sub TimerUnban_timer()
    Dim i As Long
    Dim count As Long
    Dim item As ListItem
    Dim dateNow As Date
    Dim strMembers As String
    
    dateNow = Now
    count = lvwBannedUsers.ListItems.count
    
    i = 1
    Do While i <= count
        Set item = lvwBannedUsers.ListItems.item(i)
        If Not (item.SubItems(3) = "") Then
            If (CDate(item.SubItems(3)) <= dateNow) Then
                OcxUnBanMember item.SubItems(1), item.SubItems(2)
                lvwBannedUsers.ListItems.Remove i
                i = i - 1
                count = count - 1
            End If
        End If
        
        i = i + 1
        Set item = Nothing
    Loop

End Sub


Private Sub OcxBanMember(ByRef strUserName As String, ByRef strIPAddress As String)
    OcxBanOrUnbanMember "", strUserName, strIPAddress, True
End Sub


Private Sub OcxUnBanMember(ByRef strUserName As String, ByRef strIPAddress As String)
    OcxBanOrUnbanMember "", strUserName, strIPAddress, False
End Sub


Private Sub OcxBanOrUnbanMember(ByRef strNickName As String, ByRef strUserName As String, _
                                ByRef strIPAddress As String, ByVal bBan As Boolean)
    
    If strNickName = "" And strUserName = "" And strIPAddress = "" Then
         MsgBox "Error in Ban/Unban user: all fields are empty!"
         Exit Sub
    End If
    
    Dim mi As New ChatItems
    mi.AssociatedType = "Query"
    
    If Not strNickName = "" Then
        mi.item(pnNickName) = strNickName
        'mi.item(pnNickNameOp) = "StartsWith"
    End If
    
    If Not strUserName = "" Then
        mi.item(pnUserName) = strUserName
        mi.item(pnUserNameOp) = "Contains"
        'mi.item(pnUserNameOp) = "StartsWith"
    End If
    
    If Not strIPAddress = "" Then
        mi.item(pnIPAddress) = strIPAddress
        'mi.item(pnIPAddressOp) = "Contains"
        'mi.item(pnIPAddressOp) = "StartsWith"
    End If
    
    fMainForm.ChatUI1.Channel1.BanMembers mi, bBan
    
    Set mi = Nothing
End Sub

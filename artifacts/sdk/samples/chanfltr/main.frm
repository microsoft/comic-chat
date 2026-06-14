VERSION 5.00
Object = "{9FE8A900-96AC-11CF-9086-00AA006C3FBC}#2.0#0"; "MSCHATPR.OCX"
Object = "{6B7E6392-850A-101B-AFC0-4210102A8DA7}#1.2#0"; "COMCTL32.OCX"
Begin VB.Form Main 
   Caption         =   "Channel Filter VB50  Application"
   ClientHeight    =   7965
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   10200
   Icon            =   "Main.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   7965
   ScaleWidth      =   10200
   StartUpPosition =   3  'Windows Default
   Begin ComctlLib.StatusBar StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   34
      Top             =   7590
      Width           =   10200
      _ExtentX        =   17992
      _ExtentY        =   661
      SimpleText      =   ""
      _Version        =   327680
      BeginProperty Panels {0713E89E-850A-101B-AFC0-4210102A8DA7} 
         NumPanels       =   2
         BeginProperty Panel1 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            Object.Width           =   5292
            MinWidth        =   5292
            Text            =   "New and Modified Channel Count: 0"
            TextSave        =   "New and Modified Channel Count: 0"
            Key             =   ""
            Object.Tag             =   ""
         EndProperty
         BeginProperty Panel2 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            Object.Width           =   3351
            MinWidth        =   3351
            Text            =   "All Channel Count: 0"
            TextSave        =   "All Channel Count: 0"
            Key             =   ""
            Object.Tag             =   ""
         EndProperty
      EndProperty
      MouseIcon       =   "Main.frx":0442
   End
   Begin VB.Frame frmListViews 
      BorderStyle     =   0  'None
      Caption         =   "AllChannels"
      Height          =   3735
      Index           =   2
      Left            =   5160
      TabIndex        =   31
      Top             =   2280
      Width           =   4695
      Begin ComctlLib.ListView lstAllChannels 
         Height          =   3135
         Left            =   360
         TabIndex        =   33
         Top             =   360
         Width           =   3975
         _ExtentX        =   7011
         _ExtentY        =   5530
         View            =   3
         MultiSelect     =   -1  'True
         LabelWrap       =   -1  'True
         HideSelection   =   0   'False
         _Version        =   327680
         ForeColor       =   -2147483640
         BackColor       =   -2147483643
         BorderStyle     =   1
         Appearance      =   1
         MouseIcon       =   "Main.frx":045E
         NumItems        =   6
         BeginProperty ColumnHeader(1) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
            Key             =   ""
            Object.Tag             =   ""
            Text            =   "Name"
            Object.Width           =   2540
         EndProperty
         BeginProperty ColumnHeader(2) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
            SubItemIndex    =   1
            Key             =   ""
            Object.Tag             =   ""
            Text            =   "Count"
            Object.Width           =   1058
         EndProperty
         BeginProperty ColumnHeader(3) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
            SubItemIndex    =   2
            Key             =   ""
            Object.Tag             =   ""
            Text            =   "Topic"
            Object.Width           =   3528
         EndProperty
         BeginProperty ColumnHeader(4) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
            SubItemIndex    =   3
            Key             =   ""
            Object.Tag             =   ""
            Text            =   "OnJoin"
            Object.Width           =   2540
         EndProperty
         BeginProperty ColumnHeader(5) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
            SubItemIndex    =   4
            Key             =   ""
            Object.Tag             =   ""
            Text            =   "OnPart"
            Object.Width           =   2540
         EndProperty
         BeginProperty ColumnHeader(6) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
            SubItemIndex    =   5
            Key             =   ""
            Object.Tag             =   ""
            Text            =   "Modes"
            Object.Width           =   2540
         EndProperty
      End
   End
   Begin VB.Frame frmListViews 
      BorderStyle     =   0  'None
      Caption         =   "NewChannels"
      Height          =   3735
      Index           =   1
      Left            =   360
      TabIndex        =   30
      Top             =   2280
      Width           =   4575
      Begin ComctlLib.ListView lstNewChannels 
         Height          =   3135
         Left            =   420
         TabIndex        =   32
         Top             =   360
         Width           =   3975
         _ExtentX        =   7011
         _ExtentY        =   5530
         View            =   3
         MultiSelect     =   -1  'True
         LabelWrap       =   -1  'True
         HideSelection   =   0   'False
         _Version        =   327680
         SmallIcons      =   "imglstNewChannels"
         ForeColor       =   -2147483640
         BackColor       =   -2147483643
         BorderStyle     =   1
         Appearance      =   1
         MouseIcon       =   "Main.frx":047A
         NumItems        =   8
         BeginProperty ColumnHeader(1) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
            Key             =   ""
            Object.Tag             =   ""
            Text            =   "Name"
            Object.Width           =   2540
         EndProperty
         BeginProperty ColumnHeader(2) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
            SubItemIndex    =   1
            Key             =   ""
            Object.Tag             =   ""
            Text            =   "Count"
            Object.Width           =   1058
         EndProperty
         BeginProperty ColumnHeader(3) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
            SubItemIndex    =   2
            Key             =   ""
            Object.Tag             =   ""
            Text            =   "Topic"
            Object.Width           =   3528
         EndProperty
         BeginProperty ColumnHeader(4) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
            SubItemIndex    =   3
            Key             =   ""
            Object.Tag             =   ""
            Text            =   "OnJoin"
            Object.Width           =   2540
         EndProperty
         BeginProperty ColumnHeader(5) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
            SubItemIndex    =   4
            Key             =   ""
            Object.Tag             =   ""
            Text            =   "OnPart"
            Object.Width           =   2540
         EndProperty
         BeginProperty ColumnHeader(6) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
            SubItemIndex    =   5
            Key             =   ""
            Object.Tag             =   ""
            Text            =   "Modes"
            Object.Width           =   2540
         EndProperty
         BeginProperty ColumnHeader(7) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
            SubItemIndex    =   6
            Key             =   ""
            Object.Tag             =   ""
            Text            =   "Creator Identity"
            Object.Width           =   2540
         EndProperty
         BeginProperty ColumnHeader(8) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
            SubItemIndex    =   7
            Key             =   ""
            Object.Tag             =   ""
            Text            =   "Creator IP Address"
            Object.Width           =   2540
         EndProperty
      End
   End
   Begin VB.Frame frmChannelFilter 
      Caption         =   "Channel Filtering"
      Height          =   5775
      Left            =   120
      TabIndex        =   24
      Top             =   1680
      Width           =   9975
      Begin VB.CheckBox chkRefreshOnJoinOnPart 
         Alignment       =   1  'Right Justify
         Caption         =   "Refresh OnJoin && OnPart fields too?"
         Height          =   375
         Left            =   8160
         TabIndex        =   11
         Top             =   5000
         Width           =   1695
      End
      Begin ComctlLib.TabStrip TabStrip1 
         Height          =   4215
         Left            =   120
         TabIndex        =   5
         Top             =   240
         Width           =   9735
         _ExtentX        =   17171
         _ExtentY        =   7435
         _Version        =   327680
         BeginProperty Tabs {0713E432-850A-101B-AFC0-4210102A8DA7} 
            NumTabs         =   2
            BeginProperty Tab1 {0713F341-850A-101B-AFC0-4210102A8DA7} 
               Caption         =   "New or Modified Channels"
               Key             =   ""
               Object.Tag             =   ""
               ImageVarType    =   2
            EndProperty
            BeginProperty Tab2 {0713F341-850A-101B-AFC0-4210102A8DA7} 
               Caption         =   "All Channels"
               Key             =   ""
               Object.Tag             =   ""
               ImageVarType    =   2
            EndProperty
         EndProperty
         MouseIcon       =   "Main.frx":0496
      End
      Begin VB.TextBox txtReason 
         Height          =   285
         Left            =   6720
         TabIndex        =   9
         Top             =   5400
         Width           =   3135
      End
      Begin VB.CommandButton cmdRefresh 
         Caption         =   "Refresh"
         Enabled         =   0   'False
         Height          =   375
         Left            =   7680
         TabIndex        =   12
         Top             =   4560
         Width           =   2175
      End
      Begin VB.CommandButton cmdStopFiltering 
         Caption         =   "Stop listening"
         Enabled         =   0   'False
         Height          =   375
         Left            =   3480
         TabIndex        =   7
         Top             =   4560
         Width           =   1215
      End
      Begin VB.CommandButton cmdStartFiltering 
         Caption         =   "Start listening to new and modified channels"
         Enabled         =   0   'False
         Height          =   375
         Left            =   120
         TabIndex        =   6
         Top             =   4560
         Width           =   3375
      End
      Begin VB.CommandButton cmdKill 
         Caption         =   "Kill"
         Enabled         =   0   'False
         Height          =   375
         Left            =   6720
         TabIndex        =   10
         Top             =   4560
         Width           =   975
      End
      Begin VB.CommandButton cmdAccept 
         Caption         =   "Accept"
         Enabled         =   0   'False
         Height          =   375
         Left            =   5760
         TabIndex        =   8
         Top             =   4560
         Width           =   975
      End
      Begin VB.Label lblKillReason 
         Caption         =   "Kill Reason:"
         Height          =   255
         Left            =   5760
         TabIndex        =   29
         Top             =   5475
         Width           =   855
      End
      Begin VB.Label lblFilteringStatus 
         Alignment       =   2  'Center
         Caption         =   "Filtering Status: Disconnected"
         Height          =   495
         Left            =   120
         TabIndex        =   25
         Top             =   5040
         Width           =   4575
      End
   End
   Begin VB.Frame frmConnection 
      Caption         =   "Connection/Disconnection"
      Height          =   1455
      Left            =   120
      TabIndex        =   13
      Top             =   120
      Width           =   9975
      Begin VB.TextBox cmbServers 
         Height          =   285
         Left            =   720
         TabIndex        =   0
         Top             =   240
         Width           =   3855
      End
      Begin VB.CommandButton cmdDisconnect 
         Caption         =   "Disconnect"
         Height          =   255
         Left            =   3480
         TabIndex        =   21
         Top             =   600
         Width           =   1095
      End
      Begin VB.CommandButton cmdCancelConnectLogin 
         Caption         =   "Cancel Connect/Login"
         Height          =   255
         Left            =   1680
         TabIndex        =   20
         Top             =   600
         Width           =   1815
      End
      Begin VB.CommandButton cmdLogin 
         Caption         =   "Login"
         Height          =   255
         Left            =   960
         TabIndex        =   19
         Top             =   600
         Width           =   735
      End
      Begin VB.CommandButton cmdConnect 
         Caption         =   "Connect"
         Height          =   255
         Left            =   120
         TabIndex        =   18
         Top             =   600
         Width           =   855
      End
      Begin VB.ComboBox cmbSecurityPackages 
         Height          =   315
         ItemData        =   "Main.frx":04B2
         Left            =   8640
         List            =   "Main.frx":04CB
         TabIndex        =   4
         Text            =   "Anon"
         Top             =   600
         Width           =   1215
      End
      Begin VB.TextBox txtPassword 
         Height          =   285
         IMEMode         =   3  'DISABLE
         Left            =   5640
         PasswordChar    =   "*"
         TabIndex        =   3
         Top             =   600
         Width           =   1215
      End
      Begin VB.TextBox txtUserName 
         Height          =   300
         Left            =   8640
         TabIndex        =   2
         Top             =   230
         Width           =   1215
      End
      Begin VB.TextBox txtNickname 
         Height          =   300
         Left            =   5640
         TabIndex        =   1
         Top             =   230
         Width           =   1215
      End
      Begin VB.Label lblLastConnectionError 
         BackColor       =   &H00FFFFFF&
         Height          =   255
         Left            =   5280
         TabIndex        =   28
         Top             =   1040
         Width           =   4575
      End
      Begin VB.Label lblConnectionError 
         Caption         =   "Last Connection Error:"
         Height          =   255
         Left            =   3600
         TabIndex        =   27
         Top             =   1080
         Width           =   1575
      End
      Begin VB.Label Label1 
         Caption         =   "Connection Status:"
         Height          =   255
         Left            =   120
         TabIndex        =   26
         Top             =   1080
         Width           =   1455
      End
      Begin VB.Label lblServer 
         Caption         =   "Server:"
         Height          =   300
         Left            =   120
         TabIndex        =   23
         Top             =   280
         Width           =   495
      End
      Begin VB.Label lblConnectionState 
         Caption         =   "Disconnected"
         Height          =   255
         Left            =   1680
         TabIndex        =   22
         Top             =   1080
         Width           =   1215
      End
      Begin VB.Label lblSecurityPackages 
         Alignment       =   1  'Right Justify
         Caption         =   "Security Packages:"
         Height          =   255
         Left            =   7120
         TabIndex        =   17
         Top             =   640
         Width           =   1500
      End
      Begin VB.Label lblPassword 
         Alignment       =   1  'Right Justify
         Caption         =   "Password:"
         Height          =   255
         Left            =   4700
         TabIndex        =   16
         Top             =   640
         Width           =   900
      End
      Begin VB.Label lblUserName 
         Alignment       =   1  'Right Justify
         Caption         =   "User Name:"
         Height          =   300
         Left            =   7120
         TabIndex        =   15
         Top             =   280
         Width           =   1500
      End
      Begin VB.Label lblNickname 
         Alignment       =   1  'Right Justify
         Caption         =   "Nickname:"
         Height          =   300
         Left            =   4700
         TabIndex        =   14
         Top             =   280
         Width           =   900
      End
   End
   Begin ComctlLib.ImageList imglstNewChannels 
      Left            =   9960
      Top             =   1680
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   16777215
      _Version        =   327680
      BeginProperty Images {0713E8C2-850A-101B-AFC0-4210102A8DA7} 
         NumListImages   =   4
         BeginProperty ListImage1 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "Main.frx":0513
            Key             =   "NewChannel"
         EndProperty
         BeginProperty ListImage2 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "Main.frx":0A41
            Key             =   "NewTopic"
         EndProperty
         BeginProperty ListImage3 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "Main.frx":0F6F
            Key             =   "NewOnJoin"
         EndProperty
         BeginProperty ListImage4 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "Main.frx":149D
            Key             =   "NewOnPart"
         EndProperty
      EndProperty
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr1 
      Left            =   9600
      OleObjectBlob   =   "Main.frx":19CB
      Top             =   1800
   End
End
Attribute VB_Name = "Main"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Const RPL_EVENTADD = "806"
Const RPL_EVENTDEL = "807"
Const RPL_SYSOPADMIN = "386"
Const RPL_SYSOP = "381"
Const CMD_EVENT = "EVENT"

Const ICON_NEWCHANNEL = 1
Const ICON_NEWTOPIC = 2
Const ICON_NEWONJOIN = 3
Const ICON_NEWONPART = 4

Const MIN_NEWREFRESHPERCYCLE = 8

Const SB_NEWCHANNELS = "New and Modified Channel Count: "
Const SB_AllCHANNELS = "All Channel Count: "

'Channel Property Names
Const cpnAccount = "Account"
Const cpnBannedList = "BannedList"
Const cpnChannelAgeMax = "ChannelAgeMax"
Const cpnChannelAgeMin = "ChannelAgeMin"
Const cpnClientData = "ClientData"
Const cpnClientGuid = "ClientGuid"
Const cpnCloneList = "CloneList"
Const cpnCreationTime = "CreationTime"
Const cpnHostKey = "HostKey"
Const cpnKeyword = "Keyword"
Const cpnLag = "Lag"
Const cpnLanguage = "Language"
Const cpnLanguageOp = "LanguageOp"
Const cpnListCount = "ListCount"
Const cpnMaxMemberCount = "MaxMemberCount"
Const cpnMemberCount = "MemberCount"
Const cpnMemberCountMax = "MemberCountMax"
Const cpnMemberCountMin = "MemberCountMin"
Const cpnModes = "Modes"
Const cpnName = "Name"
Const cpnNameOp = "NameOp"
Const cpnObjectId = "ObjectId"
Const cpnOnJoin = "OnJoin"
Const cpnOnPart = "OnPart"
Const cpnOwnerKey = "OwnerKey"
Const cpnRating = "Rating"
Const cpnRegistered = "Registered"
Const cpnServicePath = "ServicePath"
Const cpnSubject = "Subject"
Const cpnTopic = "Topic"
Const cpnTopicOp = "TopicOp"
Const cpnTopicAgeMax = "TopicAgeMax"
Const cpnTopicAgeMin = "TopicAgeMin"

Dim bInChannelListing As Boolean
Dim cRefreshingAllChannels As Integer
Dim lAllChannelCount As Long
Dim lNewChannelCount As Long
Dim nLastNewChannelIndexRefreshed As Integer
Dim ct As enumConnectionType
Dim nCurFrame As Integer

Private Sub cmdAccept_Click()
    On Error GoTo errAccept
    
    Dim i As Integer
    Dim cnt As Integer
    
    cnt = lstNewChannels.ListItems.Count
    i = 1
    While (i <= cnt)
        If (lstNewChannels.ListItems.Item(i).Selected) Then
            lstNewChannels.ListItems.Remove i
            cnt = cnt - 1
        Else
            i = i + 1
        End If
    Wend
    
    cmdAccept.Enabled = False
    cmdKill.Enabled = False
    cmdRefresh.Enabled = False
    chkRefreshOnJoinOnPart.Enabled = cmdRefresh.Enabled
      
    Exit Sub

errAccept:
    DisplayError "Accept"
End Sub

Private Sub cmdCancelConnectLogin_Click()
    On Error GoTo errCancelConnectLogin
    
    MsChatPr1.CancelConnectLogin
    Exit Sub

errCancelConnectLogin:
    DisplayError "CancelConnectLogin"
End Sub

Private Sub cmdConnect_Click()
    On Error GoTo errConnect
    
    MsChatPr1.Connect cmbServers.Text
    Exit Sub
    
errConnect:
    DisplayError "Connect"
End Sub

Sub DisplayError(strCallerName As String)
    MsgBox Err.Description, vbOKOnly, "ChannelFilter Error " + CStr(Err.Number) + " in " + strCallerName
End Sub

Private Sub cmdDisconnect_Click()
    On Error GoTo errDisconnect
    
    MsChatPr1.Disconnect
    Exit Sub
    
errDisconnect:
    DisplayError "Disconnect"
End Sub

Private Sub cmdKill_Click()
    On Error GoTo errKill
    
    Dim i As Integer
    Dim lst As ListView
    
    If (nCurFrame = 1) Then
        Set lst = lstNewChannels
    Else
        Set lst = lstAllChannels
    End If
    
    For i = 1 To lst.ListItems.Count
        If (lst.ListItems.Item(i).Selected) Then
            MsChatPr1.KillChannel lst.ListItems.Item(i).Text, txtReason.Text
        End If
    Next i
    
    Set lst = Nothing
    Exit Sub

errKill:
    DisplayError "Kill"
End Sub

Private Sub cmdLogin_Click()
    On Error GoTo errLogin
    
    MsChatPr1.Login cmbServers.Text, txtNickname.Text, txtUserName.Text, txtUserName.Text, txtPassword.Text, cmbSecurityPackages.Text
    Exit Sub
    
errLogin:
    DisplayError "Login"
End Sub
Sub UpdateConnectionState(NewConnectionState As MsChatPrCtl.enumConnectionState)
    Select Case NewConnectionState
        Case csDisconnected:
            lblConnectionState.BackColor = &HFF&
            cmbServers.Enabled = True
            cmbSecurityPackages.Enabled = True
            txtNickname.Enabled = True
            txtUserName.Enabled = True
            txtPassword.Enabled = True
            
            cmdConnect.Enabled = True
            cmdLogin.Enabled = True
            cmdCancelConnectLogin.Caption = "Cancel Connect/Login"
            cmdCancelConnectLogin.Enabled = False
            cmdDisconnect.Enabled = False
            
            cmdAccept.Enabled = False
            cmdKill.Enabled = False
            cmdRefresh.Enabled = False
            chkRefreshOnJoinOnPart.Enabled = cmdRefresh.Enabled

            cmdStartFiltering.Enabled = False
            cmdStopFiltering.Enabled = False
            lblFilteringStatus.Caption = "Listening Status: Disconnected"
            
            nLastNewChannelIndexRefreshed = 0
        Case csConnecting:
            lblConnectionState.BackColor = &H80FF&
            cmbServers.Enabled = False
            cmbSecurityPackages.Enabled = False
            txtNickname.Enabled = True
            txtUserName.Enabled = True
            txtPassword.Enabled = True

            cmdConnect.Enabled = False
            cmdLogin.Enabled = False
            cmdCancelConnectLogin.Caption = "Cancel Connect"
            cmdCancelConnectLogin.Enabled = True
            cmdDisconnect.Enabled = False
            
            lblLastConnectionError.Caption = ""
            
            StatusBar.Panels.Item(1).Text = SB_NEWCHANNELS + "0"
            StatusBar.Panels.Item(2).Text = SB_AllCHANNELS + "0"
            
            lNewChannelCount = 0
            lAllChannelCount = 0
            
            lstNewChannels.ListItems.Clear
            lstAllChannels.ListItems.Clear
            
        Case csConnected:
            lblConnectionState.BackColor = &HFFFF&
            txtNickname.Enabled = True
            txtUserName.Enabled = True
            txtPassword.Enabled = True

            cmdConnect.Enabled = False
            cmdLogin.Enabled = True
            cmdCancelConnectLogin.Caption = "Cancel Connect/Login"
            cmdCancelConnectLogin.Enabled = False
            cmdDisconnect.Enabled = True
        Case csLogging:
            ct = MsChatPr1.ConnectionType
            
            lblConnectionState.BackColor = &H80FF80
            cmbServers.Enabled = False
            cmbSecurityPackages.Enabled = False
            txtNickname.Enabled = False
            txtUserName.Enabled = False
            txtPassword.Enabled = False

            cmdConnect.Enabled = False
            cmdLogin.Enabled = False
            cmdCancelConnectLogin.Caption = "Cancel Login"
            cmdCancelConnectLogin.Enabled = True
            cmdDisconnect.Enabled = False
            
            lblLastConnectionError.Caption = ""
        Case csLogged:
            lblConnectionState.BackColor = &HFF00&

            cmdConnect.Enabled = False
            cmdLogin.Enabled = False
            cmdCancelConnectLogin.Caption = "Cancel Login/Cancel"
            cmdCancelConnectLogin.Enabled = False
            cmdDisconnect.Enabled = True
            cmdRefresh.Enabled = (nCurFrame = 2)
            chkRefreshOnJoinOnPart.Enabled = cmdRefresh.Enabled

            txtPassword.Text = ""

            If (MsChatPr1.ConnectionType = ctIRCX) Then
                lblFilteringStatus.Caption = "Listening Status: Connected to an IRCX server but you don't have the privileges to listen"
            Else
                lblFilteringStatus.Caption = "Listening Status: Disabled on IRC Servers"
            End If
            txtPassword.Text = ""
        
        Case csDisconnecting:
            lblConnectionState.BackColor = &HC0C0&

            cmdConnect.Enabled = False
            cmdLogin.Enabled = False
            cmdCancelConnectLogin.Caption = "Cancel Login/Cancel"
            cmdCancelConnectLogin.Enabled = False
            cmdDisconnect.Enabled = False
        End Select
    lblConnectionState.Caption = StrConnectionNameFromState(NewConnectionState)
End Sub

Function StrConnectionNameFromState(ConnectionState As MsChatPrCtl.enumConnectionState) As String
    Select Case ConnectionState
        Case csDisconnected:
            StrConnectionNameFromState = "Disconnected"
        Case csConnecting:
            StrConnectionNameFromState = "Connecting..."
        Case csConnected:
            StrConnectionNameFromState = "Connected"
        Case csLogging:
            StrConnectionNameFromState = "Logging..."
        Case csLogged:
            StrConnectionNameFromState = "Logged"
        Case csDisconnecting:
            StrConnectionNameFromState = "Disconnecting..."
    End Select
End Function

Private Sub cmdRefresh_Click()
    On Error GoTo errRefresh
    
    If (nCurFrame = 1) Then
        Dim i As Integer
        
        For i = 1 To lstNewChannels.ListItems.Count
            If (lstNewChannels.ListItems.Item(i).Selected) Then
                MsChatPr1.QueryChannelProperty cpnTopic, lstNewChannels.ListItems.Item(i).Text
                If (chkRefreshOnJoinOnPart.Value = 1) Then
                    MsChatPr1.QueryChannelProperty cpnOnJoin, lstNewChannels.ListItems.Item(i).Text
                    MsChatPr1.QueryChannelProperty cpnOnPart, lstNewChannels.ListItems.Item(i).Text
                End If
            End If
        Next i
    Else
        Dim ChannelQueryItems As ChatItems
        
        cRefreshingAllChannels = cRefreshingAllChannels + 1
        
        Set ChannelQueryItems = New ChatItems
        ChannelQueryItems.AssociatedType = "Query"
        MsChatPr1.ListChannels ChannelQueryItems
        Set ChannelQueryItems = Nothing
    End If
    Exit Sub

errRefresh:
    DisplayError "Refresh"
End Sub

Private Sub cmdStartFiltering_Click()
    On Error GoTo errStartFiltering
    MsChatPr1.SendProtocolMessage "EVENT ADD CHANNEL" + vbCrLf
    Exit Sub
    
errStartFiltering:
    DisplayError "StartFiltering"
End Sub

Private Sub cmdStopFiltering_Click()
    On Error GoTo errStopFiltering
    MsChatPr1.SendProtocolMessage "EVENT DELETE CHANNEL" + vbCrLf
    Exit Sub
    
errStopFiltering:
    DisplayError "StopFiltering"
End Sub

Private Sub Form_Load()
    cRefreshingAllChannels = 0
    bInChannelListing = False
    UpdateConnectionState csDisconnected
    nCurFrame = 2
    TabStrip1_Click
    lNewChannelCount = 0
    lAllChannelCount = 0
    nLastNewChannelIndexRefreshed = 0
End Sub

Private Sub Form_Resize()
    frmChannelFilter.Width = Width - 350
    
    TabStrip1.Width = frmChannelFilter.Width - 250
    
    frmListViews(1).Left = frmChannelFilter.Left + TabStrip1.Left + 100
    frmListViews(1).Top = frmChannelFilter.Top + TabStrip1.Top + 400
    frmListViews(1).Width = TabStrip1.Width - 200
    frmListViews(1).Height = TabStrip1.Height - 500
    
    frmListViews(2).Left = frmChannelFilter.Left + TabStrip1.Left + 100
    frmListViews(2).Top = frmChannelFilter.Top + TabStrip1.Top + 400
    frmListViews(2).Width = TabStrip1.Width - 200
    frmListViews(2).Height = TabStrip1.Height - 500
        
    lstNewChannels.Left = 0
    lstNewChannels.Top = 0
    lstNewChannels.Width = frmListViews(1).Width
    lstNewChannels.Height = frmListViews(1).Height
    
    lstAllChannels.Left = 0
    lstAllChannels.Top = 0
    lstAllChannels.Width = frmListViews(2).Width
    lstAllChannels.Height = frmListViews(2).Height
End Sub

Private Sub Form_Unload(Cancel As Integer)
    lstNewChannels.ListItems.Clear
    lstAllChannels.ListItems.Clear
    'Unload ChannelList
End Sub

Private Sub lstNewChannels_Click()
    cmdAccept.Enabled = (SelectionCount(1) <> 0) And (MsChatPr1.ConnectionState = csLogged)
    cmdKill.Enabled = cmdAccept.Enabled
    cmdRefresh.Enabled = cmdAccept.Enabled
    chkRefreshOnJoinOnPart.Enabled = cmdRefresh.Enabled
End Sub

Private Sub lstNewChannels_ColumnClick(ByVal ColumnHeader As ComctlLib.ColumnHeader)
    cmdAccept.Enabled = (SelectionCount(1) <> 0) And (MsChatPr1.ConnectionState = csLogged)
    cmdKill.Enabled = cmdAccept.Enabled
    cmdRefresh.Enabled = cmdAccept.Enabled
    chkRefreshOnJoinOnPart.Enabled = cmdRefresh.Enabled
    
    If (lstNewChannels.SortOrder = lvwDescending) Then
        lstNewChannels.SortOrder = lvwAscending
    Else
        lstNewChannels.SortOrder = lvwDescending
    End If
    
    lstNewChannels.SortKey = ColumnHeader.Index - 1
    lstNewChannels.Sorted = True
End Sub

Private Sub lstNewChannels_ItemClick(ByVal Item As ComctlLib.ListItem)
    cmdAccept.Enabled = (SelectionCount(1) <> 0) And (MsChatPr1.ConnectionState = csLogged)
    cmdKill.Enabled = cmdAccept.Enabled
    cmdRefresh.Enabled = cmdAccept.Enabled
    chkRefreshOnJoinOnPart.Enabled = cmdRefresh.Enabled
End Sub

Private Sub MsChatPr1_OnBeginEnumeration(ByVal Channel As MsChatPrCtl.Channel, ByVal EnumType As MsChatPrCtl.enumEnumType)
    On Error GoTo errOnBeginEnumeration
    
    If (cRefreshingAllChannels > 0) Then
        StatusBar.Panels.Item(2).Text = SB_AllCHANNELS + "0"
        lAllChannelCount = 0
        lstAllChannels.ListItems.Clear
    End If
    
    If (Channel Is Nothing And EnumType = etChannels) Then
        bInChannelListing = True
    End If
    Exit Sub
    
errOnBeginEnumeration:
    DisplayError "OnBeginEnumeration"
End Sub

Private Sub MsChatPr1_OnConnectionError(ByVal ErrorCode As Long, ByVal Description As String, ByVal ChannelName As Variant)
    On Error GoTo errOnConnectionError
  
    Select Case VarType(ChannelName)
        Case vbEmpty:
            lblLastConnectionError.Caption = Description + " (" + CStr(ErrorCode) + ")"
        Case vbString:
            lblLastConnectionError.Caption = "For channel '" + ChannelName + "': " + Description + " (" + CStr(ErrorCode) + ")"
    End Select
    Exit Sub
    
errOnConnectionError:
    DisplayError "OnConnectionError"
End Sub

Private Sub MsChatPr1_OnConnectionState(ByVal NewConnectionState As MsChatPrCtl.enumConnectionState)
    On Error GoTo errOnConnectionState
    
    UpdateConnectionState NewConnectionState
    Exit Sub
    
errOnConnectionState:
    DisplayError "OnConnectionState"
End Sub

Private Sub MsChatPr1_OnProtocolMessage(ByVal MessagePrefix As String, ByVal MessageCommand As String, ByVal MessageParameters As Variant, EnableDefault As stdole.OLE_ENABLEDEFAULTBOOL)
    On Error GoTo errOnProtocolMessage
    
    Select Case MessageCommand
        Case RPL_EVENTADD:
            lblFilteringStatus.Caption = "Listening Status: Showing new and modified channels"
            cmdStartFiltering.Enabled = False
            cmdStopFiltering.Enabled = True
        Case RPL_EVENTDEL:
            lblFilteringStatus.Caption = "Listening Status: Not showing new and modified channels but you have the privileges to do so"
            cmdStartFiltering.Enabled = True
            cmdStopFiltering.Enabled = False
        Case RPL_SYSOPADMIN, RPL_SYSOP:
            lblFilteringStatus.Caption = "Listening Status: Not showing new and modified channels but you have the privileges to do so"
            cmdStartFiltering.Enabled = True
            cmdStartFiltering_Click
        Case CMD_EVENT:
            TreatChannelEvent MessageParameters
    End Select
    
    'Dim strText As String
    
    'strText = "Prefix: '" + MessagePrefix + "', Command:'" + MessageCommand + "', Params:'" + StrParamsFromVariant(MessageParameters) + "', EnableDefault:'" + CStr(EnableDefault) + "'"
    'MsgBox strText
    
    Exit Sub
    
errOnProtocolMessage:
    DisplayError "OnProtocolMessage"
End Sub

Function StrParamsFromVariant(MessageParameters As Variant) As String

    Dim i As Integer
    
    StrParamsFromVariant = ""
    Select Case VarType(MessageParameters)
        Case vbString:
            StrParamsFromVariant = CStr(MessageParameters)
        Case vbString + vbArray:
            For i = LBound(MessageParameters) To UBound(MessageParameters)
                StrParamsFromVariant = StrParamsFromVariant + "<" + MessageParameters(i) + "> "
            Next i
        Case vbVariant + vbArray:
            For i = LBound(MessageParameters) To UBound(MessageParameters)
                StrParamsFromVariant = StrParamsFromVariant + "<" + CStr(MessageParameters(i)) + "> "
            Next i
        Case Else
            StrParamsFromVariant = "UNEXPECTED VARIANT TYPE"
    End Select
End Function

Private Sub MsChatPr1_OnEndEnumeration(ByVal Channel As Channel, ByVal EnumType As MsChatPrCtl.enumEnumType)
    On Error GoTo errOnEndEnumeration
    
    If (Channel Is Nothing And (EnumType = etChannels Or EnumType = etChannelsTruncated)) Then
        bInChannelListing = False
        cRefreshingAllChannels = cRefreshingAllChannels - 1
    End If
    Exit Sub
    
errOnEndEnumeration:
    DisplayError "OnEndEnumeration"
End Sub

Private Sub MsChatPr1_OnChannelProperty(ByVal Channel As MsChatPrCtl.Channel, ByVal ChannelItems As MsChatPrCtl.ChatItems)
    On Error GoTo errOnChannelProperty
    
    If (Channel Is Nothing) Then
        Dim bExtendedChannel As Boolean
        Dim itmX As ListItem
        Dim strChannelName As String
        Dim strEncodedChannelName As String
            
        strChannelName = CStr(VPropertyFromChatItems(ChannelItems, cpnName))
        bExtendedChannel = (Left(strChannelName, 1) = "%")
        If (bExtendedChannel) Then
            strEncodedChannelName = MsChatPr1.ConvertedString(cvtToUTF8, cvsChannelName, strChannelName, True)
        Else
            strEncodedChannelName = strChannelName
        End If
        
        If (bInChannelListing) Then
            If (cRefreshingAllChannels > 0) Then
                Set itmX = lstAllChannels.ListItems.Add(, strEncodedChannelName, strChannelName)
                itmX.SubItems(1) = CStr(VPropertyFromChatItems(ChannelItems, cpnMemberCount))
                itmX.SubItems(2) = CStr(VPropertyFromChatItems(ChannelItems, cpnTopic))
                If (ct = ctIRCX) Then
                    itmX.SubItems(5) = StrChannelModesFromBits(VPropertyFromChatItems(ChannelItems, cpnModes))
                    If (chkRefreshOnJoinOnPart.Value = 1) Then
                        MsChatPr1.QueryChannelProperty cpnOnJoin, strChannelName
                        MsChatPr1.QueryChannelProperty cpnOnPart, strChannelName
                    End If
                End If
                lAllChannelCount = lAllChannelCount + 1
                StatusBar.Panels.Item(2).Text = SB_AllCHANNELS + CStr(lAllChannelCount)
            End If
        Else
            ' this must be a request answer
            Dim strMemberCount As String
            Dim strTopic As String
            Dim strOnJoin As String
            Dim strOnPart As String
            Dim strModes As String
            Dim bMemberCountValid As Boolean
            Dim bTopicValid As Boolean
            Dim bOnJoinValid As Boolean
            Dim bOnPartValid As Boolean
            Dim bModesValid As Boolean
            
            bMemberCountValid = ChannelItems.ItemValid(cpnMemberCount)
            bTopicValid = ChannelItems.ItemValid(cpnTopic)
            bOnJoinValid = ChannelItems.ItemValid(cpnOnJoin)
            bOnPartValid = ChannelItems.ItemValid(cpnOnPart)
            bModesValid = ChannelItems.ItemValid(cpnModes)
            
            If (bMemberCountValid) Then strMemberCount = CStr(VPropertyFromChatItems(ChannelItems, cpnMemberCount))
            If (bTopicValid) Then strTopic = CStr(VPropertyFromChatItems(ChannelItems, cpnTopic))
            If (bOnJoinValid) Then strOnJoin = CStr(VPropertyFromChatItems(ChannelItems, cpnOnJoin))
            If (bOnPartValid) Then strOnPart = CStr(VPropertyFromChatItems(ChannelItems, cpnOnPart))
            If (bModesValid) Then strModes = StrChannelModesFromBits(VPropertyFromChatItems(ChannelItems, cpnModes))
            
            Set itmX = lstNewChannels.FindItem(strChannelName, lvwText)
            If (Not (itmX Is Nothing)) Then
                If (bMemberCountValid) Then itmX.SubItems(1) = strMemberCount
                If (bTopicValid) Then itmX.SubItems(2) = strTopic
                If (bOnJoinValid) Then itmX.SubItems(3) = strOnJoin
                If (bOnPartValid) Then itmX.SubItems(4) = strOnPart
                If (bModesValid) Then itmX.SubItems(5) = strModes
            End If
            
            Set itmX = lstAllChannels.FindItem(strChannelName, lvwText)
            If (Not (itmX Is Nothing)) Then
                If (bMemberCountValid) Then itmX.SubItems(1) = strMemberCount
                If (bTopicValid) Then itmX.SubItems(2) = strTopic
                If (bOnJoinValid) Then itmX.SubItems(3) = strOnJoin
                If (bOnPartValid) Then itmX.SubItems(4) = strOnPart
                If (bModesValid) Then itmX.SubItems(5) = strModes
            End If
            
            Set itmX = Nothing
        End If
    End If
    Exit Sub
    
errOnChannelProperty:
    DisplayError "OnChannelProperty"
End Sub

Function VPropertyFromChatItems(ci As ChatItems, strPropertyName As String) As Variant

    On Error GoTo errVPropFromItems
    If (ci.ItemValid(strPropertyName)) Then
        VPropertyFromChatItems = ci.Item(strPropertyName)
    Else
        VPropertyFromChatItems = Empty
    End If
    Exit Function
    
errVPropFromItems:
    DisplayError "VPropertyFromItems"
End Function

Function StrChannelModesFromBits(Modes As Long) As String
    
               ' cmNone              = 0x00000000,
               ' cmPublic            = 0x00000000,
               ' cmPrivate           = 0x00000001,
               ' cmHidden            = 0x00000002,
               ' cmSecret            = 0x00000004,
               ' cmModerated         = 0x00000008,
               ' cmNoExtern          = 0x00000010,
               ' cmTopicop           = 0x00000020,
               ' cmInvite            = 0x00000040,
               ' cmKnock             = 0x00000080,
               ' cmNoWhisper         = 0x00000100,
               ' cmRegistered        = 0x00000200,
               ' cmService           = 0x00000400,
               ' cmAuthOnly          = 0x00000800,
               ' cmCloneable         = 0x00001000,
               ' cmClone             = 0x00002000,
               ' cmAuditorium        = 0x00004000,
               ' cmNoFormat          = 0x00008000

    StrChannelModesFromBits = ""
    If (Modes And cmPrivate) Then
        If (StrChannelModesFromBits <> "") Then
            StrChannelModesFromBits = StrChannelModesFromBits + "+"
        End If
        StrChannelModesFromBits = StrChannelModesFromBits + "Private"
    End If
    If (Modes And cmHidden) Then
        If (StrChannelModesFromBits <> "") Then
            StrChannelModesFromBits = StrChannelModesFromBits + "+"
        End If
        StrChannelModesFromBits = StrChannelModesFromBits + "Hidden"
    End If
    If (Modes And cmSecret) Then
        If (StrChannelModesFromBits <> "") Then
            StrChannelModesFromBits = StrChannelModesFromBits + "+"
        End If
        StrChannelModesFromBits = StrChannelModesFromBits + "Secret"
    End If
    If (Modes And cmModerated) Then
        If (StrChannelModesFromBits <> "") Then
            StrChannelModesFromBits = StrChannelModesFromBits + "+"
        End If
        StrChannelModesFromBits = StrChannelModesFromBits + "Moderated"
    End If
    If (Modes And cmNoExtern) Then
        If (StrChannelModesFromBits <> "") Then
            StrChannelModesFromBits = StrChannelModesFromBits + "+"
        End If
        StrChannelModesFromBits = StrChannelModesFromBits + "NoExtern"
    End If
    If (Modes And cmTopicop) Then
        If (StrChannelModesFromBits <> "") Then
            StrChannelModesFromBits = StrChannelModesFromBits + "+"
        End If
        StrChannelModesFromBits = StrChannelModesFromBits + "Topicop"
    End If
    If (Modes And cmInvite) Then
        If (StrChannelModesFromBits <> "") Then
            StrChannelModesFromBits = StrChannelModesFromBits + "+"
        End If
        StrChannelModesFromBits = StrChannelModesFromBits + "Invite"
    End If
    If (Modes And cmKnock) Then
        If (StrChannelModesFromBits <> "") Then
            StrChannelModesFromBits = StrChannelModesFromBits + "+"
        End If
        StrChannelModesFromBits = StrChannelModesFromBits + "Knock"
    End If
    If (Modes And cmNoWhisper) Then
        If (StrChannelModesFromBits <> "") Then
            StrChannelModesFromBits = StrChannelModesFromBits + "+"
        End If
        StrChannelModesFromBits = StrChannelModesFromBits + "NoWhisper"
    End If
    If (Modes And cmRegistered) Then
        If (StrChannelModesFromBits <> "") Then
            StrChannelModesFromBits = StrChannelModesFromBits + "+"
        End If
        StrChannelModesFromBits = StrChannelModesFromBits + "Registered"
    End If
    If (Modes And cmService) Then
        If (StrChannelModesFromBits <> "") Then
            StrChannelModesFromBits = StrChannelModesFromBits + "+"
        End If
        StrChannelModesFromBits = StrChannelModesFromBits + "Service"
    End If
    If (Modes And cmAuthOnly) Then
        If (StrChannelModesFromBits <> "") Then
            StrChannelModesFromBits = StrChannelModesFromBits + "+"
        End If
        StrChannelModesFromBits = StrChannelModesFromBits + "AuthOnly"
    End If
    If (Modes And cmCloneable) Then
        If (StrChannelModesFromBits <> "") Then
            StrChannelModesFromBits = StrChannelModesFromBits + "+"
        End If
        StrChannelModesFromBits = StrChannelModesFromBits + "Cloneable"
    End If
    If (Modes And cmClone) Then
        If (StrChannelModesFromBits <> "") Then
            StrChannelModesFromBits = StrChannelModesFromBits + "+"
        End If
        StrChannelModesFromBits = StrChannelModesFromBits + "Clone"
    End If
    If (Modes And cmAuditorium) Then
        If (StrChannelModesFromBits <> "") Then
            StrChannelModesFromBits = StrChannelModesFromBits + "+"
        End If
        StrChannelModesFromBits = StrChannelModesFromBits + "Auditorium"
    End If
    If (Modes And cmNoFormat) Then
        If (StrChannelModesFromBits <> "") Then
            StrChannelModesFromBits = StrChannelModesFromBits + "+"
        End If
        StrChannelModesFromBits = StrChannelModesFromBits + "NoFormat"
    End If
End Function

Private Sub TreatChannelEvent(ByVal MessageParameters As Variant)

    Dim bExtendedChannel As Boolean
    Dim nParams As Integer
    Dim strChannelName As String
    Dim strDecodedChannelName As String
    Dim strChannelModes As String
    Dim strCreatorIdentity As String
    Dim strCreatorIPAddress As String
    Dim strNewTopic As String
    Dim strNewModes As String
        
    On Error GoTo errTreatChannelEvent
    
    If (MessageParameters(1) <> "CHANNEL") Then Exit Sub
    
    strChannelName = CStr(MessageParameters(3))
    bExtendedChannel = (Left(strChannelName, 1) = "%")
    If (bExtendedChannel) Then
        strDecodedChannelName = MsChatPr1.ConvertedString(cvtToUnicode, cvsChannelName, strChannelName, True)
    Else
        strDecodedChannelName = strChannelName
    End If
    
    Dim itmX As ListItem
    
    Select Case MessageParameters(2)
    Case "CREATE":
        ' new channel creation
        strChannelModes = MessageParameters(4)
        If (Left(MessageParameters(5), 1) = "'") Then
            strCreatorIdentity = MsChatPr1.ConvertedString(cvtToUnicode, cvsNickname, CStr(MessageParameters(5)), False)
        Else
            strCreatorIdentity = MessageParameters(5)
        End If
        strCreatorIPAddress = MessageParameters(6)
        
        Set itmX = lstNewChannels.ListItems.Add(, strChannelName, strDecodedChannelName)
        'Debug.Print "New Add: " + strChannelName + " " + strDecodedChannelName
        itmX.SubItems(1) = "1"
        itmX.SubItems(5) = StrChannelModesFromIRCChars(strChannelModes)
        itmX.SubItems(6) = strCreatorIdentity
        itmX.SubItems(7) = strCreatorIPAddress
        itmX.SmallIcon = ICON_NEWCHANNEL
        lNewChannelCount = lNewChannelCount + 1
        StatusBar.Panels.Item(1).Text = SB_NEWCHANNELS + CStr(lNewChannelCount)
       
        Set itmX = lstAllChannels.ListItems.Add(, strChannelName, strDecodedChannelName)
        'Debug.Print "All Add: " + strChannelName + " " + strDecodedChannelName
        itmX.SubItems(1) = "1"
        itmX.SubItems(5) = StrChannelModesFromIRCChars(strChannelModes)
        lAllChannelCount = lAllChannelCount + 1
        StatusBar.Panels.Item(2).Text = SB_AllCHANNELS + CStr(lAllChannelCount)
        
    Case "DESTROY", "COLLISION":
        ' channel death
        Set itmX = lstNewChannels.FindItem(strDecodedChannelName, lvwText)
        If (Not (itmX Is Nothing)) Then
            lstNewChannels.ListItems.Remove strChannelName
            'Debug.Print "New Del/Collision: " + strChannelName + " " + strDecodedChannelName
            lNewChannelCount = lNewChannelCount - 1
            StatusBar.Panels.Item(1).Text = SB_NEWCHANNELS + CStr(lNewChannelCount)
        End If
    
        Set itmX = lstAllChannels.FindItem(strDecodedChannelName, lvwText)
        If (Not (itmX Is Nothing)) Then
            lstAllChannels.ListItems.Remove strChannelName
            'Debug.Print "All Del/Collision: " + strChannelName + " " + strDecodedChannelName
            lAllChannelCount = lAllChannelCount - 1
            StatusBar.Panels.Item(2).Text = SB_AllCHANNELS + CStr(lAllChannelCount)
        End If
        
    Case "TOPIC":
        ' new topic
        Set itmX = lstNewChannels.FindItem(strDecodedChannelName, lvwText)
        If (itmX Is Nothing) Then
            If (Left(MessageParameters(4), 1) = "'") Then
                strCreatorIdentity = MsChatPr1.ConvertedString(cvtToUnicode, cvsNickname, CStr(MessageParameters(4)), False)
            Else
                strCreatorIdentity = MessageParameters(4)
            End If
            strCreatorIPAddress = MessageParameters(5)
            
            Set itmX = lstNewChannels.ListItems.Add(, strChannelName, strDecodedChannelName)
            'Debug.Print "New Add: " + strChannelName + " " + strDecodedChannelName
            itmX.SubItems(6) = strCreatorIdentity
            itmX.SubItems(7) = strCreatorIPAddress
            itmX.SmallIcon = ICON_NEWTOPIC
            lNewChannelCount = lNewChannelCount + 1
            StatusBar.Panels.Item(1).Text = SB_NEWCHANNELS + CStr(lNewChannelCount)
            'since we don't know the channel's modes and member count, we request them
            MsChatPr1.QueryChannelProperty cpnMemberCount, strDecodedChannelName
            MsChatPr1.QueryChannelProperty cpnModes, strDecodedChannelName
            MsChatPr1.QueryChannelProperty cpnOnJoin, strDecodedChannelName
            MsChatPr1.QueryChannelProperty cpnOnPart, strDecodedChannelName
        End If
        If (bExtendedChannel) Then
            itmX.SubItems(2) = MsChatPr1.ConvertedString(cvtToUnicode, cvsNormal, CStr(MessageParameters(6)), False)
        Else
            itmX.SubItems(2) = CStr(MessageParameters(6))
        End If
        
        Set itmX = lstAllChannels.FindItem(strDecodedChannelName, lvwText)
        If (Not (itmX Is Nothing)) Then
            If (bExtendedChannel) Then
                itmX.SubItems(2) = MsChatPr1.ConvertedString(cvtToUnicode, cvsNormal, MessageParameters(6), False)
            Else
                itmX.SubItems(2) = CStr(MessageParameters(6))
            End If
        End If
    
    Case "MODE":
        ' new modes
        Set itmX = lstNewChannels.FindItem(strDecodedChannelName, lvwText)
        If (Not (itmX Is Nothing)) Then
            itmX.SubItems(5) = StrChannelModesFromIRCChars(CStr(MessageParameters(4)))
        End If
        
        Set itmX = lstAllChannels.FindItem(strDecodedChannelName, lvwText)
        If (Not (itmX Is Nothing)) Then
            itmX.SubItems(5) = StrChannelModesFromIRCChars(CStr(MessageParameters(4)))
        End If
    End Select
    
    cmdAccept.Enabled = (SelectionCount(nCurFrame) <> 0) And (nCurFrame = 1)
    cmdKill.Enabled = SelectionCount(nCurFrame) <> 0
    cmdRefresh.Enabled = cmdKill.Enabled Or (nCurFrame = 2)
    chkRefreshOnJoinOnPart.Enabled = cmdRefresh.Enabled
    
    Exit Sub
    
errTreatChannelEvent:
    Debug.Print "TreatChannelEvent, CN=" + strChannelName + ", DCN=" + strDecodedChannelName
End Sub

Private Function SelectionCount(nWhichList As Integer) As Integer
    Dim i As Integer
    Dim lst As ListView
    
    SelectionCount = 0
    If (nWhichList = 1) Then
        Set lst = lstNewChannels
    Else
        Set lst = lstAllChannels
    End If
    For i = 1 To lst.ListItems.Count
        If (lst.ListItems.Item(i).Selected) Then SelectionCount = SelectionCount + 1
    Next i
    Set lst = Nothing
End Function

Private Sub TabStrip1_Click()
    If TabStrip1.SelectedItem.Index = nCurFrame _
        Then Exit Sub ' No need to change frame.
    ' Otherwise, hide old frame, show new.
    frmListViews(TabStrip1.SelectedItem.Index).Visible = True
    frmListViews(nCurFrame).Visible = False
    ' Set nCurFrame to new value.
    nCurFrame = TabStrip1.SelectedItem.Index
    
    If (nCurFrame = 1) Then
        cmdRefresh.Caption = "Refresh selected channels"
    Else
        cmdRefresh.Caption = "Refresh whole list"
    End If
    
    cmdAccept.Enabled = (SelectionCount(nCurFrame) <> 0) And (nCurFrame = 1)
    cmdKill.Enabled = (SelectionCount(nCurFrame) <> 0) And (MsChatPr1.ConnectionState = csLogged)
    cmdRefresh.Enabled = cmdKill.Enabled Or (nCurFrame = 2 And MsChatPr1.ConnectionState = csLogged)
    chkRefreshOnJoinOnPart.Enabled = cmdRefresh.Enabled
End Sub

Function StrChannelModesFromIRCChars(strIRCChars As String) As String
    StrChannelModesFromIRCChars = ""
    If (InStr(strIRCChars, "a")) Then
        If (StrChannelModesFromIRCChars = "") Then
            StrChannelModesFromIRCChars = "Auth Only"
        Else
            StrChannelModesFromIRCChars = StrChannelModesFromIRCChars + " + Auth Only"
        End If
    End If
    If (InStr(strIRCChars, "d")) Then
        If (StrChannelModesFromIRCChars = "") Then
            StrChannelModesFromIRCChars = "Cloneable"
        Else
            StrChannelModesFromIRCChars = StrChannelModesFromIRCChars + " + Cloneable"
        End If
    End If
    If (InStr(strIRCChars, "e")) Then
        If (StrChannelModesFromIRCChars = "") Then
            StrChannelModesFromIRCChars = "Clone"
        Else
            StrChannelModesFromIRCChars = StrChannelModesFromIRCChars + " + Clone"
        End If
    End If
    If (InStr(strIRCChars, "f")) Then
        If (StrChannelModesFromIRCChars = "") Then
            StrChannelModesFromIRCChars = "No Format"
        Else
            StrChannelModesFromIRCChars = StrChannelModesFromIRCChars + " + No Format"
        End If
    End If
    If (InStr(strIRCChars, "h")) Then
        If (StrChannelModesFromIRCChars = "") Then
            StrChannelModesFromIRCChars = "Hidden"
        Else
            StrChannelModesFromIRCChars = StrChannelModesFromIRCChars + " + Hidden"
        End If
    End If
    If (InStr(strIRCChars, "i")) Then
        If (StrChannelModesFromIRCChars = "") Then
            StrChannelModesFromIRCChars = "Invite"
        Else
            StrChannelModesFromIRCChars = StrChannelModesFromIRCChars + " + Invite"
        End If
    End If
    If (InStr(strIRCChars, "u")) Then
        If (StrChannelModesFromIRCChars = "") Then
            StrChannelModesFromIRCChars = "Knock"
        Else
            StrChannelModesFromIRCChars = StrChannelModesFromIRCChars + " + Knock"
        End If
    End If
    If (InStr(strIRCChars, "m")) Then
        If (StrChannelModesFromIRCChars = "") Then
            StrChannelModesFromIRCChars = "Moderated"
        Else
            StrChannelModesFromIRCChars = StrChannelModesFromIRCChars + " + Moderated"
        End If
    End If
    If (InStr(strIRCChars, "n")) Then
        If (StrChannelModesFromIRCChars = "") Then
            StrChannelModesFromIRCChars = "No Extern"
        Else
            StrChannelModesFromIRCChars = StrChannelModesFromIRCChars + " + No Extern"
        End If
    End If
    If (InStr(strIRCChars, "p")) Then
        If (StrChannelModesFromIRCChars = "") Then
            StrChannelModesFromIRCChars = "Private"
        Else
            StrChannelModesFromIRCChars = StrChannelModesFromIRCChars + " + Private"
        End If
    End If
    If (InStr(strIRCChars, "r")) Then
        If (StrChannelModesFromIRCChars = "") Then
            StrChannelModesFromIRCChars = "Registered"
        Else
            StrChannelModesFromIRCChars = StrChannelModesFromIRCChars + " + Registered"
        End If
    End If
    If (InStr(strIRCChars, "s")) Then
        If (StrChannelModesFromIRCChars = "") Then
            StrChannelModesFromIRCChars = "Secret"
        Else
            StrChannelModesFromIRCChars = StrChannelModesFromIRCChars + " + Secret"
        End If
    End If
    If (InStr(strIRCChars, "t")) Then
        If (StrChannelModesFromIRCChars = "") Then
            StrChannelModesFromIRCChars = "Topicop"
        Else
            StrChannelModesFromIRCChars = StrChannelModesFromIRCChars + " + Topicop"
        End If
    End If
    If (InStr(strIRCChars, "w")) Then
        If (StrChannelModesFromIRCChars = "") Then
            StrChannelModesFromIRCChars = "No Whisper"
        Else
            StrChannelModesFromIRCChars = StrChannelModesFromIRCChars + " + No Whisper"
        End If
    End If
    If (InStr(strIRCChars, "x")) Then
        If (StrChannelModesFromIRCChars = "") Then
            StrChannelModesFromIRCChars = "Auditorium"
        Else
            StrChannelModesFromIRCChars = StrChannelModesFromIRCChars + " + Auditorium"
        End If
    End If
    If (InStr(strIRCChars, "z")) Then
        If (StrChannelModesFromIRCChars = "") Then
            StrChannelModesFromIRCChars = "Service"
        Else
            StrChannelModesFromIRCChars = StrChannelModesFromIRCChars + " + Service"
        End If
    End If
End Function
Private Sub lstAllChannels_Click()
    cmdAccept.Enabled = False
    cmdKill.Enabled = (SelectionCount(2) <> 0) And (MsChatPr1.ConnectionState = csLogged)
    cmdRefresh.Enabled = MsChatPr1.ConnectionState = csLogged
    chkRefreshOnJoinOnPart.Enabled = cmdRefresh.Enabled
End Sub

Private Sub lstAllChannels_ColumnClick(ByVal ColumnHeader As ComctlLib.ColumnHeader)
    cmdAccept.Enabled = False
    cmdKill.Enabled = (SelectionCount(2) <> 0) And (MsChatPr1.ConnectionState = csLogged)
    cmdRefresh.Enabled = MsChatPr1.ConnectionState = csLogged
    chkRefreshOnJoinOnPart.Enabled = cmdRefresh.Enabled

    If (lstAllChannels.SortOrder = lvwDescending) Then
        lstAllChannels.SortOrder = lvwAscending
    Else
        lstAllChannels.SortOrder = lvwDescending
    End If
    
    lstAllChannels.SortKey = ColumnHeader.Index - 1
    lstAllChannels.Sorted = True
End Sub

Private Sub lstAllChannels_ItemClick(ByVal Item As ComctlLib.ListItem)
    cmdAccept.Enabled = False
    cmdKill.Enabled = (SelectionCount(2) <> 0) And (MsChatPr1.ConnectionState = csLogged)
    cmdRefresh.Enabled = MsChatPr1.ConnectionState = csLogged
    chkRefreshOnJoinOnPart.Enabled = cmdRefresh.Enabled
End Sub

'Private Sub tmrRefreshNewChannels_Timer()
'    If (MsChatPr1.ConnectionState = csLogged) Then
'        Dim nCount As Integer
'        Dim nRefreshCount As Integer
'        Dim itmX As ListItem
'        Dim nTmp As Integer
'
'        nCount = lstNewChannels.ListItems.Count
'        If (nCount <> 0) Then
'            If (nCount / 10 > MIN_NEWREFRESHPERCYCLE) Then
'                nRefreshCount = nCount / 10
'            Else
'                If (nCount < MIN_NEWREFRESHPERCYCLE) Then
'                    nRefreshCount = nCount
'                Else
'                    nRefreshCount = MIN_NEWREFRESHPERCYCLE
'                End If
'            End If
'            For nTmp = 1 To nRefreshCount
'                nLastNewChannelIndexRefreshed = nLastNewChannelIndexRefreshed + 1
'                If (nLastNewChannelIndexRefreshed > nCount) Then
'                    nLastNewChannelIndexRefreshed = 1
'                End If
'                Set itmX = lstNewChannels.ListItems.Item(nLastNewChannelIndexRefreshed)
'                ' commented code is necessary but crashes ocx!
'                'MsChatPr1.QueryChannelProperty cpnTopic, itmX.Text
'                'MsChatPr1.QueryChannelProperty cpnOnJoin, itmX.Text
'                'MsChatPr1.QueryChannelProperty cpnOnPart, itmX.Text
'            Next nTmp
'            Set itmX = Nothing
'        End If
'    End If
'End Sub


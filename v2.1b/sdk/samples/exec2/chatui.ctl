VERSION 5.00
Object = "{6B7E6392-850A-101B-AFC0-4210102A8DA7}#1.2#0"; "COMCTL32.OCX"
Object = "{3B7C8863-D78F-101B-B9B5-04021C009402}#1.1#0"; "richtx32.OCX"
Begin VB.UserControl ChatUI 
   Alignable       =   -1  'True
   AutoRedraw      =   -1  'True
   BackColor       =   &H00C0C0C0&
   ClientHeight    =   7950
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   11085
   ControlContainer=   -1  'True
   PaletteMode     =   0  'Halftone
   ScaleHeight     =   7950
   ScaleWidth      =   11085
   Begin VB.CommandButton cmdTextCopy 
      Caption         =   "Copy"
      Height          =   375
      Left            =   5640
      TabIndex        =   8
      Top             =   5160
      Width           =   1215
   End
   Begin VB.CommandButton cmdSendText 
      Caption         =   "&Send"
      Height          =   375
      Left            =   4440
      TabIndex        =   7
      Top             =   5160
      Width           =   1215
   End
   Begin VB.ComboBox cmbText 
      Height          =   315
      ItemData        =   "chatui.ctx":0000
      Left            =   0
      List            =   "chatui.ctx":0002
      Style           =   2  'Dropdown List
      TabIndex        =   6
      Top             =   5160
      Width           =   4455
   End
   Begin ComctlLib.ListView lvwMembers 
      Height          =   4935
      Left            =   7560
      TabIndex        =   1
      Top             =   0
      Width           =   3135
      _ExtentX        =   5530
      _ExtentY        =   8705
      View            =   3
      Sorted          =   -1  'True
      MultiSelect     =   -1  'True
      LabelWrap       =   -1  'True
      HideSelection   =   -1  'True
      _Version        =   327682
      Icons           =   "imgMembers"
      SmallIcons      =   "imgMembers"
      ForeColor       =   -2147483640
      BackColor       =   -2147483643
      BorderStyle     =   1
      Appearance      =   1
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      NumItems        =   1
      BeginProperty ColumnHeader(1) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
         Key             =   ""
         Object.Tag             =   ""
         Text            =   "Nick Name"
         Object.Width           =   2540
      EndProperty
   End
   Begin RichTextLib.RichTextBox rtbH 
      Height          =   3015
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   7575
      _ExtentX        =   13361
      _ExtentY        =   5318
      _Version        =   327681
      Enabled         =   -1  'True
      ReadOnly        =   -1  'True
      ScrollBars      =   2
      TextRTF         =   $"chatui.ctx":0004
   End
   Begin VB.CommandButton btnWhispToMembers 
      Caption         =   "&Whisper"
      Enabled         =   0   'False
      Height          =   975
      Left            =   5640
      TabIndex        =   4
      Top             =   3960
      Width           =   1935
   End
   Begin VB.CommandButton btnSendToRoom 
      Caption         =   "&Send To All"
      Enabled         =   0   'False
      Height          =   975
      Left            =   5640
      TabIndex        =   3
      Top             =   3000
      Width           =   1935
   End
   Begin RichTextLib.RichTextBox rtbToSomebody 
      Height          =   975
      Left            =   0
      TabIndex        =   2
      Top             =   3000
      Width           =   5655
      _ExtentX        =   9975
      _ExtentY        =   1720
      _Version        =   327681
      ScrollBars      =   2
      RightMargin     =   260
      AutoVerbMenu    =   -1  'True
      TextRTF         =   $"chatui.ctx":0133
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "Times New Roman"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
   End
   Begin RichTextLib.RichTextBox rtbWhisper 
      Height          =   975
      Left            =   0
      TabIndex        =   5
      Top             =   3960
      Width           =   5655
      _ExtentX        =   9975
      _ExtentY        =   1720
      _Version        =   327681
      ScrollBars      =   2
      RightMargin     =   260
      AutoVerbMenu    =   -1  'True
      TextRTF         =   $"chatui.ctx":0264
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "Times New Roman"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
   End
   Begin ComctlLib.ImageList imgMembers 
      Left            =   9720
      Top             =   7200
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   16711680
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   16711680
      _Version        =   327682
      BeginProperty Images {0713E8C2-850A-101B-AFC0-4210102A8DA7} 
         NumListImages   =   6
         BeginProperty ListImage1 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "chatui.ctx":0395
            Key             =   ""
         EndProperty
         BeginProperty ListImage2 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "chatui.ctx":04A7
            Key             =   ""
         EndProperty
         BeginProperty ListImage3 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "chatui.ctx":05B9
            Key             =   ""
         EndProperty
         BeginProperty ListImage4 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "chatui.ctx":06CB
            Key             =   ""
         EndProperty
         BeginProperty ListImage5 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "chatui.ctx":07DD
            Key             =   ""
         EndProperty
         BeginProperty ListImage6 {0713E8C3-850A-101B-AFC0-4210102A8DA7} 
            Picture         =   "chatui.ctx":08EF
            Key             =   ""
         EndProperty
      EndProperty
   End
   Begin VB.Menu mnuMember 
      Caption         =   "Member Menu"
      Begin VB.Menu mnuMemberProperties 
         Caption         =   "Properties"
      End
      Begin VB.Menu mnuMemberSeperator1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuMemberAway 
         Caption         =   "Set Away"
         Visible         =   0   'False
      End
      Begin VB.Menu mnuMemberChangeNick 
         Caption         =   "Change Nick Name"
         Visible         =   0   'False
      End
      Begin VB.Menu mnuMemberSeperator2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuMemberNetmeeting 
         Caption         =   "Netmeeting"
      End
      Begin VB.Menu mnuMemberIgnore 
         Caption         =   "Ignore"
      End
      Begin VB.Menu mnuMemberInvite 
         Caption         =   "&Invite Other"
      End
      Begin VB.Menu mnuMemberSeperator3 
         Caption         =   "-"
      End
      Begin VB.Menu mnuMemberKick 
         Caption         =   "Kick"
      End
      Begin VB.Menu mnuMemberBan 
         Caption         =   "Ban/Unban"
      End
      Begin VB.Menu mnuMemberSeperator4 
         Caption         =   "-"
      End
      Begin VB.Menu mnuMemberOwner 
         Caption         =   "Owner"
      End
      Begin VB.Menu mnuMemberHost 
         Caption         =   "Host"
      End
      Begin VB.Menu mnuMemberSpeaker 
         Caption         =   "Speaker"
      End
      Begin VB.Menu mnuMemberSpectator 
         Caption         =   "Spectator"
      End
      Begin VB.Menu mnuMemberSeperator5 
         Caption         =   "-"
      End
      Begin VB.Menu mnuMemberExecHost 
         Caption         =   "&Host"
      End
      Begin VB.Menu mnuMemberExecModerator 
         Caption         =   "&Moderator"
      End
      Begin VB.Menu mnuMemberExecGuest 
         Caption         =   "Special &Guest"
      End
   End
   Begin VB.Menu mnuText 
      Caption         =   "Text"
      Begin VB.Menu mnuTextUndo 
         Caption         =   "&Undo"
         Enabled         =   0   'False
         Shortcut        =   ^Z
      End
      Begin VB.Menu mnuTextSeperator1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuTextCut 
         Caption         =   "Cu&t"
         Enabled         =   0   'False
         Shortcut        =   ^X
      End
      Begin VB.Menu mnuTextCopy 
         Caption         =   "&Copy"
         Enabled         =   0   'False
         Shortcut        =   ^C
      End
      Begin VB.Menu mnuTextPaste 
         Caption         =   "&Paste"
         Enabled         =   0   'False
         Shortcut        =   ^V
      End
      Begin VB.Menu mnuTextDelete 
         Caption         =   "&Delete"
         Enabled         =   0   'False
         Shortcut        =   {DEL}
      End
      Begin VB.Menu mnuTextSeperator2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuTextToModerator 
         Caption         =   "Forward To Moderator"
      End
      Begin VB.Menu mnuTextToGuest 
         Caption         =   "Forward To Special Guest"
      End
      Begin VB.Menu mnuTextAnswer 
         Caption         =   "Answer"
      End
   End
End
Attribute VB_Name = "ChatUI"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit


Public RoomName As String
Public UserType As Integer

Dim lHistoryLen As Long
Dim nMemberCount As Integer

Public bEnumerationDone As Boolean

Dim MsChatPr1 As MsChatPr

Const iconHost = 2
Const iconModerator = 4
Const iconGuest = 3
Const iconAudience = 1

Private clpbdText As Clipboard


Public Channel1 As Channel     ' each ChatUI has its own Channel object
Public fTranscript As frmTranscript
Public fBanUsers As frmBanUsers
Public fRoomRules As frmRoomRule
Public fWarnings As frmWarnings
Public fPreparedText As frmPreparedText


Public Sub OnAddModerator(ByRef strName As String)
    If strName = "" Then
        Exit Sub
    End If
    
    Dim item As ListItem
    Set item = lvwMembers.FindItem(strName)
    If item Is Nothing Then
        Exit Sub
    End If
    
    item.SmallIcon = iconModerator
    If strName = MsChatPr1.NickName Then
        SetUserType utModerator
    End If
    
End Sub


Public Sub OnRemoveModerator(ByRef strName As String)
    If strName = "" Then
        Exit Sub
    End If
    
    Dim item As ListItem
    Set item = lvwMembers.FindItem(strName)
    If item Is Nothing Then
        Exit Sub
    End If

    item.SmallIcon = iconHost
    If strName = MsChatPr1.NickName Then
        SetUserType utHost
    End If
End Sub


Public Sub OnAddGuest(ByRef strGuestNick As String)
    If strGuestNick = "" Then
        Exit Sub
    End If
    
    Dim item As ListItem
    Set item = lvwMembers.FindItem(strGuestNick)
    If item Is Nothing Then
        Exit Sub
    End If
    
    item.SmallIcon = iconGuest
    If strGuestNick = MsChatPr1.NickName Then
        SetUserType utGuest
    End If
End Sub


Public Sub OnRemoveGuest(ByRef strGuestNick As String)
    If strGuestNick = "" Then
        Exit Sub
    End If

    Dim item As ListItem
    Set item = lvwMembers.FindItem(strGuestNick)
    If item Is Nothing Then
        Exit Sub
    End If
    
    item.SmallIcon = iconGuest
    If strGuestNick = MsChatPr1.NickName Then
        SetUserType utHost
    End If

End Sub


Public Sub AddModerator(ByRef strName As String)
    If Not strName = "" Then
        SendExecMessagesToAllHosts "EXECCHAT +MODERATOR " & strName, False
    End If
End Sub

Public Sub RemoveModerator(ByRef strName As String)
    If Not strName = "" Then
        SendExecMessagesToAllHosts "EXECCHAT -MODERATOR " & strName, False
    End If
End Sub

Public Sub AddGuest(ByRef strName As String)
    If Not strName = "" Then
        SendExecMessagesToAllHosts "EXECCHAT +GUEST " & strName, False
    End If
End Sub

Public Sub RemoveGuest(ByRef strName As String)
    If Not strName = "" Then
        SendExecMessagesToAllHosts "EXECCHAT -GUEST " & strName, False
    End If
End Sub



Public Sub SendToAll(ByRef strMsg As String, Optional ByVal fAppend As Boolean = True)
    On Error Resume Next
    Channel1.SendMessage msgtNormal, strMsg
    
    If Not Err = 0 Then
        MsgBox Err.Description
        Err.Clear
        Exit Sub
    End If
    
    If (fAppend) Then
        AppendHistory MsChatPr1.NickName & " says:", 0, 2, colorSay
        AppendHistory strMsg, 1, 1, colorMessage
    End If
End Sub


Public Sub Whisper(ByRef strMsg As String, ByRef strName, Optional ByVal fAppend As Boolean = True)
    On Error Resume Next
    Channel1.SendMessage msgtNormal, strMsg, strName

    If Not Err = 0 Then
        MsgBox Err.Description
        Err.Clear
        Exit Sub
    End If
    
    If fAppend Then
        AppendHistory MsChatPr1.NickName & " whispers to " & strName & ":", 0, 2, colorSay
        AppendHistory strMsg, 1, 1, colorWhisper
    End If
End Sub


Public Sub SendExecMessage(ByRef strMsg As String, ByRef strNick As String, Optional ByVal fAppend As Boolean = False)
    MsChatPr1.SendPrivateMessage pmtNotice + pmtCTCP, strMsg, strNick
End Sub


Public Sub SendExecMessagesToAllHosts(ByRef strMsg As String, Optional ByVal fAppend As Boolean = True)
    If strMsg = "" Then
        Exit Sub
    End If
    
    Dim item As ListItem
    Dim strMembers As String
    
    On Error Resume Next
    For Each item In lvwMembers.ListItems
        If (item.Tag And mmHost) Or (item.Tag And mmOwner) Then
            If Not strMembers = "" Then
                strMembers = strMembers + ", "
            End If
            
            strMembers = strMembers + item.Text
            SendExecMessage strMsg, item.Text, False
        End If
    Next
    
    If Not Err = 0 Then
        MsgBox Err.Description
        Err.Clear
        Exit Sub
    End If
    
    If fAppend Then
        AppendHistory MsChatPr1.NickName & " says to all hosts:", 0, 2, colorWhisper
        AppendHistory strMsg, 1, 1, colorMessage
    End If
    
End Sub



Public Sub GetCurrentSelectedItem(ByRef item As ListItem)
    Set item = lvwMembers.SelectedItem
End Sub


Public Sub ShowRules()
    fRoomRules.Show vbModal, Me
End Sub


Public Sub StartTranscript()
    If fTranscript.bTrascribing Then
        MsgBox "Transcript has already been started."
    Else
        fTranscript.Show vbModal, Me
    End If
End Sub



Public Sub StopTranscript()
    If Not fTranscript.bTrascribing Then
        MsgBox "Transcript is not started."
    Else
        fTranscript.StopTranscript
    End If
End Sub


Public Sub ChangeTranscriptOption()
    If Not fTranscript.bTrascribing Then
        MsgBox "Transcript is not started."
    Else
        fTranscript.Show vbModal, Me
    End If
End Sub



Public Sub Initialize(ByVal m As Integer, ByRef objMsChatPr As MsChatPr)
    lHistoryLen = 0
    UserType = m
    UserControl_Resize
    
    Set MsChatPr1 = objMsChatPr
    Set Channel1 = MsChatPr1.Channels.Add
    
    Set fTranscript = New frmTranscript
    fTranscript.Accociate rtbH, RoomName
    
    Set fBanUsers = New frmBanUsers
    
    Set fRoomRules = New frmRoomRule
    
    Set fWarnings = New frmWarnings
    
    Set fPreparedText = New frmPreparedText
End Sub


Public Sub Destroy()
    Set MsChatPr1 = Nothing
    Set Channel1 = Nothing
    
    Unload fTranscript
    Set fTranscript = Nothing
    
    Unload fBanUsers
    Set fBanUsers = Nothing
    
    Unload fRoomRules
    Set fRoomRules = Nothing
    
    Unload fWarnings
    Set fWarnings = Nothing
    
    Unload fPreparedText
    Set fPreparedText = Nothing
    
End Sub


Public Sub EnterRoom(ByRef strRoomName As String)
    RoomName = strRoomName
    ClearHistory
    lvwMembers.ListItems.Clear
    
    If Channel1.ChannelName = RoomName And _
        (Channel1.ChannelState = chsOpen Or Channel1.ChannelState = chsOpening) Then
        Exit Sub
    End If
    
    bEnumerationDone = False
    
    On Error Resume Next
    Channel1.JoinChannel RoomName
    
    If Not Err = 0 Then
        MsgBox Err.Description
        Exit Sub
    End If
End Sub


Public Sub LeaveRoom()
    Channel1.LeaveChannel
End Sub


Public Sub OnQuestionForwardedToModerator(ByRef Question As String)
    If Not UserType = utHost Then
        Exit Sub
    End If
    
    If Question = "" Then
        Exit Sub
    End If
    
    Dim Pos As Long
    Pos = InStr(rtbH.Text, Question)
    
    If Not Pos = 0 Then
        rtbH.SelStart = Pos - 1
        rtbH.SelLength = Len(Question)
        rtbH.SelColor = colorForwardedToModerator
        
        rtbH.SelLength = 0
    End If
End Sub


Public Sub OnChannelStateChange(ByVal NewState As enumChannelState)
    Dim bOpen As Boolean
    
    bOpen = (NewState = chsOpen)
    btnSendToRoom.Enabled = bOpen
    btnWhispToMembers.Enabled = bOpen

End Sub


Public Sub SetUserType(ByVal m As Integer)
    UserType = m
    UserControl_Resize
    
    Dim item As ListItem
    Set item = lvwMembers.FindItem(MsChatPr1.NickName)
    If Not item Is Nothing Then
        SetIcon item
        Set item = Nothing
    End If
End Sub



Public Sub BeginEnumMembers()
    Dim MemberQueryItems As New ChatItems
    bEnumerationDone = False
    
    lvwMembers.ListItems.Clear
    nMemberCount = 0
    
    MemberQueryItems.AssociatedType = "Query"
    MemberQueryItems.item(pnChannels) = RoomName
    Channel1.ListMembers MemberQueryItems
    
    Set MemberQueryItems = Nothing
    
End Sub


Public Sub OnChannelModeChange()
    UpdateIcons
End Sub


Public Sub UpdateIcons()
    Dim item As ListItem
    
    For Each item In lvwMembers.ListItems
        SetIcon item
        Set item = Nothing
    Next
End Sub


Private Sub SetIcon(ByRef item As ListItem)
    ' only use this to set an audience's icon
    If item.SmallIcon = iconModerator Or item.SmallIcon = iconGuest Then
        'don't change it
        Exit Sub
    End If
    
    If CBool(item.Tag And mmHost) Or CBool(item.Tag And mmOwner) Then
        item.SmallIcon = iconHost
    Else
        item.SmallIcon = iconAudience
    End If
    Exit Sub
End Sub


Public Sub AddMember(ByVal NickName As String, ByRef MemberItem As ChatItems)
    Dim itemNew As ListItem
    
    Set itemNew = lvwMembers.ListItems.Add
    
    itemNew.Text = VPropertyFromChatItems(MemberItem, pnNickName)
    itemNew.Tag = VPropertyFromChatItems(MemberItem, pnModes)
    SetIcon itemNew
    
    ' add a line to the history box if not in initial enumeration
    If bEnumerationDone = True Then
        If UserType = utHost Then
            AppendHistory NickName & " has joined the conversation.", 0, 2, colorJoin
        End If
        
        If CBool(mmOwner And Channel1.MemberModes) Then
            On Error Resume Next
            
            Dim item As ListItem
            For Each item In lvwMembers.ListItems
                Select Case item.SmallIcon
                    Case iconGuest:
                        SendExecMessage "EXECCHAT +GUEST " & item.Text, itemNew.Text
                    Case iconModerator:
                        SendExecMessage "EXECCHAT +MODERATOR " & item.Text, itemNew.Text
                End Select
                
                Set item = Nothing
            Next
            
            If Not Err = 0 Then
                MsgBox Err.Description
                Err.Clear
                Exit Sub
            End If
    
        End If
    End If
    
    nMemberCount = nMemberCount + 1
End Sub



Public Sub EndEnumMembers()
    bEnumerationDone = True
End Sub


Public Sub OnMemberModeChange(ByRef ModefierName As String, ByVal ModifiedNickname As String, _
                              ByVal OldMemberMode As Long, ByVal NewMemberMode As Long)
    Dim item As ListItem
    Set item = lvwMembers.FindItem(ModifiedNickname, , , False)
    
    If Not item Is Nothing Then
        item.Tag = NewMemberMode
        SetIcon item
    End If
    
    If (ModifiedNickname = MsChatPr1.NickName) Then
        Dim ChannelMode
        ChannelMode = VPropertyFromChatItems(Channel1.ChannelProperty(cpnModes), cpnModes)
        If Not ((ChannelMode And cmAuditorium) = 0) Then
            'BeginEnumMembers
        End If
    End If
    
    Set item = Nothing
End Sub


Public Sub OnNicknameChange(ByRef OldNickname As String, ByRef NewNickname As String)
    Dim item As ListItem
    Set item = lvwMembers.FindItem(OldNickname, , , False)
    
    If Not item Is Nothing Then
        item.Text = NewNickname
    End If
    
    Set item = Nothing
End Sub




Public Sub DelMember(ByRef strMemberName As String)
    Dim item As ListItem
    Set item = lvwMembers.FindItem(strMemberName, , , False)
    
    If Not item Is Nothing Then
        If UserType = utHost Then
            AppendHistory strMemberName & " has left the conversation.", 0, 2, colorLeave
        End If
        lvwMembers.ListItems.Remove item.index
    End If
    
    Set item = Nothing
End Sub


Public Sub ViewPreparedText()
    fPreparedText.fReloaded = False
    fPreparedText.Show vbModal
    
    If (fPreparedText.fReloaded) Then
        cmbText.Clear
        Dim strText As String
        Dim index As Integer
        
        index = 0
        strText = fPreparedText.strGetText(index)
        Do While (Not (strText = ""))
            cmbText.AddItem strText
            
            index = index + 1
            strText = fPreparedText.strGetText(index)
        Loop
        
        If index > 0 Then
            cmbText.ListIndex = 0
        End If
    End If
End Sub





Public Sub AppendHistory(ByVal strText As String, ByVal nIndent As Integer, ByVal nCR As Integer, _
                        Optional ByRef color As Variant = colorMessage)
    
    Dim i
    
    If (nCR > 0 And rtbH.Text <> "") Then
        For i = 1 To nCR
            rtbH.SelStart = lHistoryLen
            rtbH.SelLength = 0
            rtbH.SelText = vbCrLf
            lHistoryLen = lHistoryLen + 2
        Next
    End If
    
    With rtbH
        .SelStart = lHistoryLen
        .SelLength = 0
        .SelIndent = nIndent * 200
        .SelRightIndent = nIndent * 200
        .SelColor = color
        .SelBold = True
        
        .SelText = strText
    End With
    
    lHistoryLen = lHistoryLen + Len(strText)
    
End Sub



Public Sub ClearHistory()
    rtbH.TextRTF = ""
End Sub



Private Sub ResetRectangleControl(obj As Object, nX0 As Long, nY0 As Long, nX1 As Long, nY1 As Long)
    obj.Left = nX0
    obj.Top = nY0
    obj.width = Max(nX1 - nX0, 0)
    obj.height = Max(nY1 - nY0, 0)
    
    If (TypeOf obj Is RichTextBox) Then
        If (nX1 - nX0 - 500) > 20 Then
            obj.RightMargin = nX1 - nX0 - 500
        Else
            obj.RightMargin = 20
        End If
    End If
    
End Sub


Private Sub btnSendToRoom_Click()
    If rtbToSomebody.Text = "" Then
        MsgBox "Please input some thing to send."
        Exit Sub
    End If
    
    On Error Resume Next
    SendToAll rtbToSomebody.Text
    
    rtbToSomebody.Text = ""
End Sub


Private Sub btnWhispToMembers_Click()
    Dim item
    Dim strMembers As String
    
    If rtbWhisper.Text = "" Then
        MsgBox "Please input some thing to whisper."
        Exit Sub
    End If
    
    For Each item In lvwMembers.ListItems
        If item.Selected Then
            If Not strMembers = "" Then
                strMembers = strMembers + ", "
            End If
            
            strMembers = strMembers + item.Text
            Whisper rtbWhisper.Text, item.Text, False
        End If
    Next
    
    AppendHistory MsChatPr1.NickName & " whispers to " & strMembers & " :", 0, 2, colorWhisper
    AppendHistory rtbWhisper.Text, 1, 1, colorMessage
    rtbWhisper.Text = ""
End Sub


Private Sub cmdSendText_Click()
    If Not cmbText.Text = "" Then
        SendToAll (cmbText.Text)
    End If
End Sub

Private Sub cmdTextCopy_Click()
    If Not cmbText.Text = "" Then
        Clipboard.Clear
        Clipboard.SetText cmbText.Text
    End If
End Sub

Private Sub lvwMembers_DblClick()
    ' show member properties if a member is selected
    If lvwMembers.SelectedItem Is Nothing Then
        Exit Sub
    End If
    
    mnuMemberProperties_Click
End Sub


Private Sub lvwMembers_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    If Button = vbRightButton And Channel1.ChannelState = chsOpen Then
        Dim item As ListItem
        Set item = lvwMembers.HitTest(X, Y)
        
        If item Is Nothing Then
            Exit Sub
        End If
        
        Dim bModerated As Boolean
        Dim bMyself As Boolean
        Dim bHost As Boolean
        Dim bOwner As Boolean
        
        bModerated = VPropertyFromChatItems(Channel1.ChannelProperty(cpnModes), cpnModes) And cmModerated
        bMyself = (item = MsChatPr1.NickName)
        bHost = UserType = utHost And ((mmHost And Channel1.MemberModes) Or (mmOwner And Channel1.MemberModes))
        bOwner = mmOwner And Channel1.MemberModes
        'bHost = UserType = utHost
        
        mnuMemberSeperator2.Visible = bMyself
        mnuMemberChangeNick.Visible = bMyself
        mnuMemberNetmeeting.Enabled = Not bMyself
        mnuMemberAway.Visible = bMyself
        mnuMemberAway.Checked = item.Tag And mmAway
        
        mnuMemberSeperator3.Visible = bHost
        mnuMemberBan.Visible = bHost
        mnuMemberKick.Visible = bHost
        mnuMemberIgnore.Visible = bHost
        
        mnuMemberSeperator4.Visible = bHost
        mnuMemberOwner.Visible = bHost
        mnuMemberHost.Visible = bHost
        mnuMemberSpeaker.Visible = bHost
        mnuMemberSpectator.Visible = bHost
        
        'mnuMemberSeperator5.Visible = bHost
        'mnuMemberExecHost.Visible = bHost
        'mnuMemberExecModerator.Visible = bHost
        'mnuMemberExecGuest.Visible = bHost
        
        mnuMemberExecHost.Enabled = bOwner And Not bMyself
        mnuMemberExecModerator.Enabled = bOwner And Not bMyself
        mnuMemberExecGuest.Enabled = bOwner And Not bMyself
        
        mnuMemberExecModerator.Checked = item.SmallIcon = iconModerator
        mnuMemberExecGuest.Checked = item.SmallIcon = iconGuest
        mnuMemberExecHost.Checked = (bOwner Or bHost) And (Not mnuMemberExecModerator.Checked) And (Not mnuMemberExecGuest.Checked)
        
        mnuMemberBan.Enabled = (Not bMyself) And bHost
        mnuMemberKick.Enabled = (Not bMyself) And bHost
        
        mnuMemberIgnore.Enabled = Not bMyself
        mnuMemberIgnore.Checked = item.Tag And mmClientIgnored
        
        mnuMemberOwner.Checked = item.Tag And mmOwner
        mnuMemberOwner.Enabled = CBool(mmOwner And Channel1.MemberModes) And (Not bMyself)
        
        mnuMemberHost.Checked = item.Tag And mmHost
        mnuMemberHost.Enabled = CBool(mmOwner And Channel1.MemberModes) And (Not bMyself)
        
        mnuMemberSpeaker.Checked = (CBool(item.Tag And mmVoice) And bModerated) Or _
                                    (CBool(item.Tag And mmNoVoice) And Not bModerated)
        mnuMemberSpeaker.Enabled = bHost And (Not bMyself) And bModerated
        
        mnuMemberSpectator.Checked = CBool(item.Tag And mmNoVoice) And bModerated
        mnuMemberSpectator.Enabled = bHost And bModerated And (Not bMyself) And bModerated
        
        PopupMenu mnuMember
        
        Set item = Nothing
    End If
End Sub


Private Sub mnuMemberExecHost_Click()
    ' make this nick a channel host if it's not a channel host or owner yet
    Dim Mode
    Mode = Channel1.MemberModes(lvwMembers.SelectedItem.Text)
    If Not CBool((mmHost And Mode) Or (mmOwner And Mode)) Then
        Channel1.ChangeMemberProperty pnModes, mmHost, lvwMembers.SelectedItem.Text
        Exit Sub
    End If
    
    ' reset moderator and special guest
    Dim item As ListItem
    Set item = lvwMembers.SelectedItem
    
    If item.SmallIcon = iconGuest Then
        RemoveGuest item.Text
    ElseIf item.SmallIcon = iconModerator Then
        RemoveModerator item.Text
    End If
    
    item.SmallIcon = iconHost
    Set item = Nothing
End Sub


Private Sub mnuMemberExecModerator_Click()
    Dim Mode
    Mode = Channel1.MemberModes(lvwMembers.SelectedItem.Text)
    If Not CBool((mmHost And Mode) Or (mmOwner And Mode)) Then
        Channel1.ChangeMemberProperty pnModes, mmHost, lvwMembers.SelectedItem.Text
    End If
    
    Dim item As ListItem
    Set item = lvwMembers.SelectedItem
    
    If item.SmallIcon = iconModerator Then
        ' already a moderator
        ' do nothing
    Else
        ' not a moderator
        AddModerator item.Text
    End If
    
    item.SmallIcon = iconModerator
    Set item = Nothing
End Sub


Private Sub mnuMemberExecGuest_Click()
    Dim Mode
    Mode = Channel1.MemberModes(lvwMembers.SelectedItem.Text)
    If Not CBool((mmHost And Mode) Or (mmOwner And Mode)) Then
        Channel1.ChangeMemberProperty pnModes, mmHost, lvwMembers.SelectedItem.Text
    End If

    Dim item As ListItem
    Set item = lvwMembers.SelectedItem
    
    If item.SmallIcon = iconGuest Then
        ' already a special guest
        ' do nothing
    Else
        ' not a special guest
        AddGuest item.Text
    End If
    
    item.SmallIcon = iconGuest
    Set item = Nothing
End Sub


Private Sub mnuMemberInvite_Click()
    Dim fInvite As New frmInvite
    fInvite.Show vbModal, Me
    
    If fInvite.OK Then
        Channel1.SendInvitation fInvite.txtNick
    End If
    
    Unload fInvite
    Set fInvite = Nothing
End Sub


Private Sub mnuMemberNetmeeting_Click()
    fMainForm.StartNetMeetingWith lvwMembers.SelectedItem.Text
End Sub


Private Sub mnuMemberProperties_Click()
    Dim fMemberProperty As New frmUserProp
    Set fMemberProperty.Channel1 = Channel1
    Set fMemberProperty.MsChatPr1 = MsChatPr1
    fMemberProperty.txtNickName = lvwMembers.SelectedItem.Text
    
    If fMemberProperty.GetProperties = True Then
        fMemberProperty.Show vbModal, Me
    End If
    
    Unload fMemberProperty
End Sub


Private Sub mnuMemberAway_Click()
    Dim fAwayMsg As New frmAwayMessage
    
    If Channel1.MemberModes And mmAway Then
        MsChatPr1.SetAway False
    Else
        fAwayMsg.Show vbModal, Me
        
        If fAwayMsg.OK Then
            MsChatPr1.SetAway True, CStr(fAwayMsg.txtAwayMessage)
        End If
    End If
    
    Unload fAwayMsg
    Set fAwayMsg = Nothing
End Sub


Private Sub mnuMemberChangeNick_Click()
    Dim fLogin As New frmLogin
    
    If Not lvwMembers.SelectedItem = MsChatPr1.NickName Then
        MsgBox "Can't change nick name of other's."
        Exit Sub
    End If
    
    fLogin.Show vbModal, Me
    
    If fLogin.OK Then
        On Error GoTo Cleanup
        MsChatPr1.ChangeNickname CStr(fLogin.txtNick)
        lvwMembers.SelectedItem.Text = fLogin.txtNick
    End If
    
Cleanup:
    Unload fLogin
    Set fLogin = Nothing
End Sub


Private Sub mnuMemberIgnore_Click()
    Dim lModes As Long
    lModes = Channel1.MemberModes(lvwMembers.SelectedItem.Text)
    
    On Error Resume Next
    
    Err.Clear
    ' check or uncheck ignore menu item
    If lModes And mmClientIgnored Then
        ' reset ignored flag
        Channel1.ChangeMemberProperty pnModes, mmNotClientIgnored, lvwMembers.SelectedItem.Text
    Else
        ' set ignored flag
        Channel1.ChangeMemberProperty pnModes, mmClientIgnored, lvwMembers.SelectedItem.Text
    End If
    
    If Not Err = 0 Then
        MsgBox "Error: " & Err.Description
        Err.Clear
    End If
End Sub


Private Sub mnuMemberKick_Click()
    Dim fKickReason As New frmWhyKick
    
    fKickReason.Show vbModal, Me
    
    If fKickReason.OK Then
        Channel1.KickMember lvwMembers.SelectedItem.Text, CStr(fKickReason.txtReason)
    End If
    
    Unload fKickReason
    Set fKickReason = Nothing
End Sub


Private Sub mnuMemberBan_Click()
    Dim varNick
    varNick = lvwMembers.SelectedItem.Text
    fBanUsers.txtNick = lvwMembers.SelectedItem.Text
    fBanUsers.txtUserName = VPropertyFromChatItems(Channel1.MemberProperty(pnUserName, varNick), pnUserName)
    fBanUsers.txtIPAddress = VPropertyFromChatItems(Channel1.MemberProperty(pnIPAddress, varNick), pnIPAddress)
    fBanUsers.Show vbModal, Me
    
    Rem Dim BanItem As New ChatItems
    Rem BanItem.AssociatedType = "Query"
    Rem Item.item(pnNickName) = lvwMembers.SelectedItem.Text
    Rem Channel1.BanMembers BanItem, True
    Rem Set BanItem = Nothing
End Sub


Private Sub mnuMemberOwner_Click()
    Channel1.ChangeMemberProperty pnModes, mmOwner, lvwMembers.SelectedItem.Text
End Sub


Private Sub mnuMemberHost_Click()
    Channel1.ChangeMemberProperty pnModes, mmHost, lvwMembers.SelectedItem.Text
End Sub


Private Sub mnuMemberSpeaker_Click()
    Channel1.ChangeMemberProperty pnModes, mmVoice, lvwMembers.SelectedItem.Text
End Sub


Private Sub mnuMemberSpectator_Click()
    Channel1.ChangeMemberProperty pnModes, mmNoVoice, lvwMembers.SelectedItem.Text
End Sub

Private Sub rtbH_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    If Button = vbRightButton Then
        Dim bSelected As Boolean
        bSelected = Not rtbH.SelText = ""
        
        mnuTextToModerator.Visible = UserType = utHost
        mnuTextToGuest.Visible = UserType = utModerator
        mnuTextAnswer.Visible = UserType = utGuest
        
        mnuTextToModerator.Enabled = bSelected
        mnuTextToGuest.Enabled = bSelected
        mnuTextAnswer.Enabled = bSelected
        
        PopupMenu mnuText
    End If
End Sub


Private Sub mnuTextToModerator_Click()
    If fMainForm.NickModerator = "" Then
        MsgBox "There's no moderator."
        Exit Sub
    End If
    
    MsChatPr1.SendPrivateMessage pmtNormal, rtbH.SelText, fMainForm.NickModerator
    
    Dim item As ListItem
    On Error Resume Next
    For Each item In lvwMembers.ListItems
        If (item.Tag And mmHost) Or (item.Tag And mmOwner) Then
            ' notify other hosts
            MsChatPr1.SendPrivateMessage pmtNotice + pmtCTCP, "EXECCHAT TOMODERATOR " & rtbH.SelText, item.Text
        End If
        Set item = Nothing
    Next
    
    rtbH.SelColor = colorForwardedToModerator
End Sub


Private Sub mnuTextToGuest_Click()
    If fMainForm.NickSpecialGuest = "" Then
        MsgBox "There's no special guest."
        Exit Sub
    End If
    
    MsChatPr1.SendPrivateMessage pmtNormal, rtbH.SelText, fMainForm.NickSpecialGuest
    rtbH.SelColor = colorForwardedToGuest
End Sub


Private Sub mnuTextAnswer_Click()
    Dim fAnswer As New frmAnswer
    fAnswer.rtbQuestion.Text = rtbH.SelText
    fAnswer.Show vbModal
    
    If fAnswer.bSent Then
        rtbH.SelColor = colorAnswered
    End If
    
    Unload fAnswer
    Set fAnswer = Nothing
End Sub


Private Sub UserControl_Resize()
    ResetUI
End Sub

Private Sub ResetUI()
    cmbText.Left = ScaleLeft
    cmbText.Top = ScaleTop + ScaleHeight - cmbText.height - 100
    'cmbText.width = ScaleWidth - cmdSendText.width - cmdTextCopy.width - 400
    
    cmdSendText.Left = cmbText.Left + cmbText.width
    cmdSendText.Top = cmbText.Top
    
    cmdTextCopy.Left = cmbText.Left + cmbText.width + cmdSendText.width
    cmdTextCopy.Top = cmbText.Top
    
    Dim DX As Long
    Dim DY As Long
    Dim x0 As Long, x1 As Long, x2 As Long, x3 As Long
    Dim y0 As Long, y1 As Long, y2 As Long, y3 As Long
    
    Dim h As Long
    h = ScaleHeight - cmbText.height - 200
    
    DX = Max(500, Min(ScaleWidth / 11, 1000))
    DY = Max(400, Min(h / 5, 1000))
    
    x0 = ScaleLeft
    x1 = ScaleWidth - 5 * DX
    x2 = ScaleWidth - 3 * DX
    x3 = ScaleWidth
    
    y0 = ScaleTop
    y1 = h - 2 * DY
    y2 = h - DY
    y3 = h
    
    ResetRectangleControl rtbH, x0, y0, x2, y1
    ResetRectangleControl lvwMembers, x2, y0, x3, y3
    ResetRectangleControl rtbToSomebody, x0, y1, x1, y2
    
    ResetRectangleControl rtbWhisper, x0, y2, x1, y3
    ResetRectangleControl btnSendToRoom, x1, y1, x2, y2
    ResetRectangleControl btnWhispToMembers, x1, y2, x2, y3
End Sub



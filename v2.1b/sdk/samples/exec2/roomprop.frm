VERSION 5.00
Begin VB.Form frmRoomProp 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Room Properties"
   ClientHeight    =   7710
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   5505
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   7710
   ScaleWidth      =   5505
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox txtOwnerKey 
      Height          =   285
      Left            =   2280
      TabIndex        =   26
      Top             =   5640
      Width           =   2415
   End
   Begin VB.TextBox txtHostKey 
      Height          =   285
      Left            =   2280
      TabIndex        =   24
      Top             =   6000
      Width           =   2415
   End
   Begin VB.TextBox txtLanguage 
      Height          =   285
      Left            =   2280
      TabIndex        =   23
      Top             =   5280
      Width           =   2415
   End
   Begin VB.CheckBox chkCreateOnly 
      Caption         =   "Fail me if the room name has already been used"
      Height          =   255
      Left            =   360
      TabIndex        =   20
      Top             =   6480
      Width           =   3735
   End
   Begin VB.CheckBox chkAuditorium 
      Caption         =   "&Auditorium"
      Height          =   255
      Left            =   360
      TabIndex        =   19
      Top             =   3360
      Value           =   1  'Checked
      Width           =   1695
   End
   Begin VB.CommandButton btnCancel 
      Caption         =   "Cancel"
      Default         =   -1  'True
      Height          =   375
      Left            =   3240
      TabIndex        =   18
      Top             =   6960
      Width           =   1215
   End
   Begin VB.CommandButton btnOK 
      Caption         =   "OK"
      Height          =   375
      Left            =   1440
      TabIndex        =   17
      Top             =   6960
      Width           =   1215
   End
   Begin VB.TextBox txtRating 
      Height          =   285
      Left            =   2280
      TabIndex        =   16
      Top             =   4920
      Width           =   2415
   End
   Begin VB.OptionButton optSecret 
      Caption         =   "S&ecret"
      Height          =   255
      Left            =   2280
      TabIndex        =   14
      Top             =   3360
      Width           =   975
   End
   Begin VB.OptionButton optHidden 
      Caption         =   "&Hidden"
      Height          =   255
      Left            =   2280
      TabIndex        =   13
      Top             =   3000
      Width           =   975
   End
   Begin VB.OptionButton optPrivate 
      Caption         =   "Pri&vate"
      Height          =   255
      Left            =   2280
      TabIndex        =   12
      Top             =   2640
      Width           =   975
   End
   Begin VB.OptionButton optPublic 
      Caption         =   "&Public"
      Height          =   255
      Left            =   2280
      TabIndex        =   11
      Top             =   2280
      Value           =   -1  'True
      Width           =   975
   End
   Begin VB.TextBox txtPassword 
      Enabled         =   0   'False
      Height          =   285
      IMEMode         =   3  'DISABLE
      Left            =   2280
      TabIndex        =   10
      Top             =   4320
      Width           =   2415
   End
   Begin VB.TextBox txtMaxUser 
      Enabled         =   0   'False
      Height          =   285
      Left            =   2280
      TabIndex        =   9
      Text            =   "0"
      Top             =   3960
      Width           =   1215
   End
   Begin VB.CheckBox chkPassword 
      Caption         =   "&Optional password:"
      Height          =   255
      Left            =   360
      TabIndex        =   8
      Top             =   4320
      Width           =   1695
   End
   Begin VB.CheckBox chkSetMaxUser 
      Caption         =   "Set Maximum &User:"
      Height          =   255
      Left            =   360
      TabIndex        =   7
      Top             =   3960
      Width           =   1695
   End
   Begin VB.CheckBox chkSetTopic 
      Caption         =   "&SetTopic host only"
      Height          =   255
      Left            =   360
      TabIndex        =   6
      Top             =   2640
      Value           =   1  'Checked
      Width           =   1695
   End
   Begin VB.CheckBox chkInviteOnly 
      Caption         =   "&Invite only"
      Height          =   255
      Left            =   360
      TabIndex        =   5
      Top             =   3000
      Width           =   1575
   End
   Begin VB.CheckBox chkModerated 
      Caption         =   "&Moderated"
      Height          =   255
      Left            =   360
      TabIndex        =   4
      Top             =   2280
      Width           =   1575
   End
   Begin VB.TextBox txtTopic 
      Height          =   735
      Left            =   360
      TabIndex        =   3
      Top             =   1200
      Width           =   4695
   End
   Begin VB.TextBox txtRoomName 
      Height          =   285
      Left            =   1800
      TabIndex        =   2
      Top             =   360
      Width           =   3015
   End
   Begin VB.Label Label6 
      Caption         =   "Owner password:"
      Height          =   255
      Left            =   360
      TabIndex        =   25
      Top             =   5640
      Width           =   1455
   End
   Begin VB.Label Label5 
      Caption         =   "Host password:"
      Height          =   255
      Left            =   360
      TabIndex        =   22
      Top             =   6000
      Width           =   1455
   End
   Begin VB.Label Label4 
      Caption         =   "Preferred Language:"
      Height          =   255
      Left            =   360
      TabIndex        =   21
      Top             =   5280
      Width           =   1575
   End
   Begin VB.Label Label3 
      Caption         =   "&Rating:"
      Height          =   255
      Left            =   360
      TabIndex        =   15
      Top             =   4920
      Width           =   855
   End
   Begin VB.Label Label2 
      Caption         =   "&Topic:"
      Height          =   255
      Left            =   360
      TabIndex        =   1
      Top             =   840
      Width           =   615
   End
   Begin VB.Label Label1 
      Caption         =   "Chat &room name:"
      Height          =   255
      Left            =   360
      TabIndex        =   0
      Top             =   360
      Width           =   1335
   End
End
Attribute VB_Name = "frmRoomProp"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public bCreate As Boolean
Public bHost As Boolean
Public bCreated As Boolean



Private Sub SetControlState()
    ' make sure only appropriate controls are shown or enabled
    Dim bIRCX As Boolean
    bIRCX = (fMainForm.MsChatPr1.ConnectionType = ctIRCX)
    
    ' Room name can only be changed at creation time
    txtRoomName.Enabled = bCreate
    
    ' Most properties can only be changed by host
    chkAuditorium.Enabled = bHost
    chkInviteOnly.Enabled = bHost
    chkModerated.Enabled = bHost
    chkPassword.Enabled = bHost
    chkSetMaxUser.Enabled = bHost
    
    chkSetTopic.Enabled = bHost
    txtTopic.Enabled = bHost Or (chkSetTopic = 0)
    
    optPublic.Enabled = bCreate
    optPrivate.Enabled = bCreate
    optHidden.Enabled = bCreate
    optSecret.Enabled = bCreate
    chkCreateOnly.Enabled = bCreate
    
    txtMaxUser.Enabled = bHost And chkSetMaxUser.Value
    txtPassword.Enabled = bHost And chkPassword.Value
    
    ' IRCX only properties
    txtRating.Enabled = bHost And bIRCX
    txtHostKey.Enabled = bHost And bIRCX
    txtOwnerKey.Enabled = bHost And bIRCX
    txtLanguage.Enabled = bHost And bIRCX
    
    btnOK.Enabled = bHost Or bCreate
End Sub


Private Sub btnCancel_Click()
    Me.Hide
End Sub


Private Sub GetChannelMode(ByRef Mode As Long)
    Mode = 0
    SetFlagIf Mode, cmPublic, optPublic
    SetFlagIf Mode, cmPrivate, optPrivate
    SetFlagIf Mode, cmHidden, optHidden
    SetFlagIf Mode, cmSecret, optSecret
    
    SetFlagIf Mode, cmInvite, chkInviteOnly
    SetFlagIf Mode, cmModerated, chkModerated
    SetFlagIf Mode, cmTopicop, chkSetTopic
End Sub


Private Sub GetCreationMode(ByRef Mode As Long)
    Mode = 0
    
    SetFlagIf Mode, cmAuditorium, chkAuditorium    ' only at creation time
    SetFlagIf Mode, cmCreateOnly, chkCreateOnly
End Sub


Private Sub CreateChannel()
    If Not fMainForm.ChatUI1.Channel1.ChannelState = chsClosed Then
        MsgBox "There's already a channel open!"
        Exit Sub
    End If
    
    Dim Mode As Long
    Mode = 0
    
    GetCreationMode Mode
    fMainForm.ChatUI1.Channel1.CreateChannel CStr(txtRoomName), , Mode
    
    bCreated = True
    
    'Do While True
    '   If fMainForm.ChatUI1.Channel1.ChannelState = chsOpen Then
    '       Exit Do
    '   End If
    'Loop
    
    'ChangeChannelProperty
End Sub


Public Sub ChangeChannelProperty()
    If Not fMainForm.ChatUI1.Channel1.ChannelName = txtRoomName Then
        ' MsgBox "You can't change this room's property since you are not in it."
        Exit Sub
    End If

    Dim ChannelMode As Long
    GetChannelMode ChannelMode
    
    Dim Channel As Channel
    Set Channel = fMainForm.ChatUI1.Channel1
    
    Channel.ChangeChannelProperty cpnModes, ChannelMode
    Channel.ChangeChannelProperty cpnTopic, CStr(txtTopic)
    
    If chkPassword Then
        Channel.ChangeChannelProperty cpnKeyword, CStr(txtPassword)
    Else
        'bug 4418
        'Channel.ChangeChannelProperty cpnKeyword, Empty
    End If
    
    If chkSetMaxUser Then
        Channel.ChangeChannelProperty cpnMaxMemberCount, CLng(txtMaxUser)
    Else
        Channel.ChangeChannelProperty cpnMaxMemberCount, CLng(0)
    End If
    
    If Not txtRating = "" Then
        Channel.ChangeChannelProperty cpnRating, CStr(txtRating)
    End If
    
    Channel.ChangeChannelProperty cpnLanguage, CStr(txtLanguage)
    Channel.ChangeChannelProperty cpnHostKey, CStr(txtHostKey)
    Channel.ChangeChannelProperty cpnOwnerKey, CStr(txtOwnerKey)
    
    Set Channel = Nothing
End Sub


Private Sub btnOK_Click()
    If bCreate Then
        CreateChannel
    Else
        ChangeChannelProperty
    End If
    
    Me.Hide
End Sub


Public Function GetProperties() As Boolean
    Dim MsChatPr1 As MsChatPr
    Dim bIRCX As Boolean
    bIRCX = (fMainForm.MsChatPr1.ConnectionType = ctIRCX)
    
    Set MsChatPr1 = fMainForm.MsChatPr1
    
    If Not ((MsChatPr1.ConnectionState = csConnected) Or (MsChatPr1.ConnectionState = csLogged)) Then
        MsgBox "You are not connected to any chat server."
        GetProperties = False
        GoTo ExitHere
    End If
    
    Dim varRoomName
    varRoomName = txtRoomName
    txtTopic = VPropertyFromChatItems(MsChatPr1.ChannelProperty(cpnTopic, varRoomName), cpnTopic)
    txtMaxUser = VPropertyFromChatItems(MsChatPr1.ChannelProperty(cpnMaxMemberCount, varRoomName), cpnMaxMemberCount)
    txtPassword = VPropertyFromChatItems(MsChatPr1.ChannelProperty(cpnKeyword, varRoomName), cpnKeyword)
    
    If bIRCX = True Then
        txtRating = VPropertyFromChatItems(MsChatPr1.ChannelProperty(cpnRating, varRoomName), cpnRating)
        txtLanguage = VPropertyFromChatItems(MsChatPr1.ChannelProperty(cpnLanguage, varRoomName), cpnLanguage)
        
        On Error Resume Next    ' we may not have access to the following info, so ignore the error
        txtHostKey = VPropertyFromChatItems(MsChatPr1.ChannelProperty(cpnHostKey, varRoomName), cpnHostKey)
        txtOwnerKey = VPropertyFromChatItems(MsChatPr1.ChannelProperty(cpnOwnerKey, varRoomName), cpnOwnerKey)
        Err.Clear
    End If
    
    On Error GoTo 0
    
    Dim nChannelMode As Long
    nChannelMode = VPropertyFromChatItems(MsChatPr1.ChannelProperty(cpnModes, varRoomName), cpnModes)
    
    optPublic.Value = nChannelMode And cmPublic
    optPrivate.Value = nChannelMode And cmPrivate
    optHidden.Value = nChannelMode And cmHidden
    optSecret.Value = nChannelMode And cmSecret
    
    chkAuditorium.Value = nBitwiseAnd(nChannelMode, cmAuditorium)
    chkCreateOnly.Value = nBitwiseAnd(nChannelMode, cmCreateOnly)
    chkInviteOnly.Value = nBitwiseAnd(nChannelMode, cmInvite)
    chkModerated.Value = nBitwiseAnd(nChannelMode, cmModerated)
    chkSetTopic.Value = nBitwiseAnd(nChannelMode, cmTopicop)
    
    If txtPassword = "" Then
        chkPassword.Value = 0
    Else
        chkPassword.Value = 1
    End If
    
    If CInt(txtMaxUser) = 0 Then
        chkSetMaxUser.Value = 0
    Else
        chkSetMaxUser.Value = 1
    End If
    
    SetControlState
    GetProperties = True
    
ExitHere:
    If Not Err = 0 Then
        MsgBox "Error in RoomProp::GetProperties ---- " & Err.Description   ' this is only for debug
        Err.Clear
    End If
    Set MsChatPr1 = Nothing
End Function

Private Sub chkPassword_Click()
    SetControlState
End Sub

Private Sub chkSetMaxUser_Click()
    SetControlState
End Sub

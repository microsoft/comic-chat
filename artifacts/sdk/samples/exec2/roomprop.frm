VERSION 5.00
Begin VB.Form frmRoomProp 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Room Properties"
   ClientHeight    =   6315
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   5235
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   6315
   ScaleWidth      =   5235
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox txtOwnerKey 
      Height          =   285
      Left            =   2040
      TabIndex        =   19
      Top             =   4440
      Width           =   2415
   End
   Begin VB.TextBox txtHostKey 
      Height          =   285
      Left            =   2040
      TabIndex        =   21
      Top             =   4800
      Width           =   2415
   End
   Begin VB.CheckBox chkCreateOnly 
      Caption         =   "Fail me if the room name has already been used"
      Height          =   255
      Left            =   120
      TabIndex        =   22
      Top             =   5280
      Value           =   1  'Checked
      Width           =   3735
   End
   Begin VB.CheckBox chkAuditorium 
      Caption         =   "&Auditorium"
      Height          =   255
      Left            =   120
      TabIndex        =   7
      Top             =   2760
      Value           =   1  'Checked
      Width           =   1695
   End
   Begin VB.CommandButton btnCancel 
      Caption         =   "Cancel"
      Default         =   -1  'True
      Height          =   375
      Left            =   2760
      TabIndex        =   24
      Top             =   5760
      Width           =   1215
   End
   Begin VB.CommandButton btnOK 
      Caption         =   "OK"
      Height          =   375
      Left            =   1200
      TabIndex        =   23
      Top             =   5760
      Width           =   1215
   End
   Begin VB.TextBox txtRating 
      Enabled         =   0   'False
      Height          =   285
      Left            =   2040
      TabIndex        =   17
      Top             =   4080
      Width           =   2415
   End
   Begin VB.OptionButton optSecret 
      Caption         =   "S&ecret"
      Height          =   255
      Left            =   2040
      TabIndex        =   11
      Top             =   2760
      Width           =   975
   End
   Begin VB.OptionButton optHidden 
      Caption         =   "&Hidden"
      Height          =   255
      Left            =   2040
      TabIndex        =   10
      Top             =   2400
      Width           =   975
   End
   Begin VB.OptionButton optPrivate 
      Caption         =   "Pri&vate"
      Height          =   255
      Left            =   2040
      TabIndex        =   9
      Top             =   2040
      Width           =   975
   End
   Begin VB.OptionButton optPublic 
      Caption         =   "&Public"
      Height          =   255
      Left            =   2040
      TabIndex        =   8
      Top             =   1680
      Value           =   -1  'True
      Width           =   975
   End
   Begin VB.TextBox txtPassword 
      Enabled         =   0   'False
      Height          =   285
      IMEMode         =   3  'DISABLE
      Left            =   2040
      TabIndex        =   15
      Top             =   3600
      Width           =   2415
   End
   Begin VB.TextBox txtMaxUser 
      Enabled         =   0   'False
      Height          =   285
      Left            =   2040
      TabIndex        =   13
      Text            =   "0"
      Top             =   3240
      Width           =   1215
   End
   Begin VB.CheckBox chkPassword 
      Caption         =   "&Optional password:"
      Height          =   255
      Left            =   120
      TabIndex        =   14
      Top             =   3600
      Width           =   1695
   End
   Begin VB.CheckBox chkSetMaxUser 
      Caption         =   "Set Maximum &User:"
      Height          =   255
      Left            =   120
      TabIndex        =   12
      Top             =   3240
      Width           =   1695
   End
   Begin VB.CheckBox chkSetTopic 
      Caption         =   "&SetTopic host only"
      Height          =   255
      Left            =   120
      TabIndex        =   5
      Top             =   2040
      Value           =   1  'Checked
      Width           =   1695
   End
   Begin VB.CheckBox chkInviteOnly 
      Caption         =   "&Invite only"
      Height          =   255
      Left            =   120
      TabIndex        =   6
      Top             =   2400
      Width           =   1575
   End
   Begin VB.CheckBox chkModerated 
      Caption         =   "&Moderated"
      Height          =   255
      Left            =   120
      TabIndex        =   4
      Top             =   1680
      Width           =   1575
   End
   Begin VB.TextBox txtTopic 
      Height          =   735
      Left            =   120
      TabIndex        =   3
      Top             =   720
      Width           =   4815
   End
   Begin VB.TextBox txtRoomName 
      Height          =   285
      Left            =   1560
      TabIndex        =   1
      Top             =   120
      Width           =   3375
   End
   Begin VB.Label Label6 
      Caption         =   "Owner password:"
      Height          =   255
      Left            =   120
      TabIndex        =   18
      Top             =   4440
      Width           =   1455
   End
   Begin VB.Label Label5 
      Caption         =   "Host password:"
      Height          =   255
      Left            =   120
      TabIndex        =   20
      Top             =   4800
      Width           =   1455
   End
   Begin VB.Label Label3 
      Caption         =   "&Rating:"
      Height          =   255
      Left            =   120
      TabIndex        =   16
      Top             =   4080
      Width           =   855
   End
   Begin VB.Label Label2 
      Caption         =   "&Topic:"
      Height          =   255
      Left            =   120
      TabIndex        =   2
      Top             =   480
      Width           =   615
   End
   Begin VB.Label Label1 
      Caption         =   "Chat &room name:"
      Height          =   255
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   1335
   End
End
Attribute VB_Name = "frmRoomProp"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'****************************************************************************
'
'  Module:     Exec2.exe
'  File:       RoomProp.frm
'
'  Copyright (c) Microsoft Corporation 1996-1998
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************

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
    
    chkAuditorium.Enabled = bCreate
    chkCreateOnly.Enabled = bCreate
    
    txtMaxUser.Enabled = bHost And chkSetMaxUser.Value
    txtPassword.Enabled = bHost And chkPassword.Value
    
    ' IRCX only properties
    txtHostKey.Enabled = bHost And bIRCX And bCreate
    txtOwnerKey.Enabled = bHost And bIRCX And bCreate
    
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
    
    SetFlagIf Mode, cmAuditorium, chkAuditorium
    SetFlagIf Mode, cmCreateOnly, chkCreateOnly
End Sub


Private Sub CreateChannel()
    If Not fMainForm.ChatUI1.Channel1.ChannelState = chsClosed Then
        MsgBox "There's already a channel open!"
        Exit Sub
    End If
    
    Dim Mode As Long
    Mode = 0
    
    GetChannelMode Mode
    fMainForm.ChatUI1.Channel1.CreateChannel CStr(txtRoomName), , Mode
    
    bCreated = True
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
    
    If txtHostKey.Enabled Then
        Channel.ChangeChannelProperty cpnHostKey, CStr(txtHostKey)
    End If
    
    If txtOwnerKey.Enabled Then
        Channel.ChangeChannelProperty cpnOwnerKey, CStr(txtOwnerKey)
    End If
    
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

VERSION 5.00
Begin VB.Form frmUserProp 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "User Properties"
   ClientHeight    =   4275
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4485
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4275
   ScaleWidth      =   4485
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.CheckBox chkSpeaker 
      Caption         =   "Speaker"
      Enabled         =   0   'False
      Height          =   255
      Left            =   2400
      TabIndex        =   19
      Top             =   2880
      Width           =   1695
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   360
      Left            =   2400
      TabIndex        =   18
      Tag             =   "Cancel"
      Top             =   3720
      Width           =   1140
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   360
      Left            =   960
      TabIndex        =   17
      Tag             =   "OK"
      Top             =   3720
      Width           =   1140
   End
   Begin VB.CheckBox chkNoVoice 
      Caption         =   "Sp&eechless"
      Enabled         =   0   'False
      Height          =   255
      Left            =   2400
      TabIndex        =   16
      Top             =   3240
      Width           =   1695
   End
   Begin VB.CheckBox chkHost 
      Caption         =   "&Host"
      Enabled         =   0   'False
      Height          =   255
      Left            =   2400
      TabIndex        =   15
      Top             =   2520
      Width           =   1695
   End
   Begin VB.CheckBox chkOwner 
      Caption         =   "&Channel Owner"
      Enabled         =   0   'False
      Height          =   255
      Left            =   2400
      TabIndex        =   14
      Top             =   2160
      Width           =   1695
   End
   Begin VB.CheckBox chkSysOp 
      Caption         =   "&System operator"
      Enabled         =   0   'False
      Height          =   255
      Left            =   120
      TabIndex        =   13
      Top             =   3240
      Width           =   1455
   End
   Begin VB.CheckBox chkAway 
      Caption         =   "A&way"
      Enabled         =   0   'False
      Height          =   255
      Left            =   120
      TabIndex        =   12
      Top             =   2880
      Width           =   1335
   End
   Begin VB.CheckBox chkAuthenticated 
      Caption         =   "&Authenticated"
      Enabled         =   0   'False
      Height          =   255
      Left            =   120
      TabIndex        =   11
      Top             =   2520
      Width           =   1335
   End
   Begin VB.CheckBox chkIgmored 
      Caption         =   "Ign&ored"
      Enabled         =   0   'False
      Height          =   255
      Left            =   120
      TabIndex        =   10
      Top             =   2160
      Width           =   975
   End
   Begin VB.TextBox txtIPAddress 
      Height          =   285
      Left            =   1200
      Locked          =   -1  'True
      TabIndex        =   9
      Top             =   1680
      Width           =   3135
   End
   Begin VB.TextBox txtUserName 
      Height          =   285
      Left            =   1200
      Locked          =   -1  'True
      TabIndex        =   7
      Top             =   600
      Width           =   3135
   End
   Begin VB.TextBox txtRealName 
      Height          =   285
      Left            =   1200
      Locked          =   -1  'True
      TabIndex        =   5
      Top             =   960
      Width           =   3135
   End
   Begin VB.TextBox txtIdentity 
      Height          =   285
      Left            =   1200
      Locked          =   -1  'True
      TabIndex        =   3
      Top             =   1320
      Width           =   3135
   End
   Begin VB.TextBox txtNickName 
      Height          =   285
      Left            =   1200
      Locked          =   -1  'True
      TabIndex        =   1
      Top             =   240
      Width           =   3135
   End
   Begin VB.Label Label5 
      Caption         =   "&IP address:"
      Height          =   255
      Left            =   120
      TabIndex        =   8
      Top             =   1680
      Width           =   855
   End
   Begin VB.Label Label4 
      Caption         =   "&User Name:"
      Height          =   255
      Left            =   120
      TabIndex        =   6
      Top             =   600
      Width           =   975
   End
   Begin VB.Label Label3 
      Caption         =   "&Real Name:"
      Height          =   255
      Left            =   120
      TabIndex        =   4
      Top             =   960
      Width           =   975
   End
   Begin VB.Label Label2 
      Caption         =   "I&dentity:"
      Height          =   255
      Left            =   120
      TabIndex        =   2
      Top             =   1320
      Width           =   735
   End
   Begin VB.Label Label1 
      Caption         =   "&Nick Name:"
      Height          =   255
      Left            =   120
      TabIndex        =   0
      Top             =   240
      Width           =   975
   End
End
Attribute VB_Name = "frmUserProp"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'****************************************************************************
'
'  Module:     Exec2.exe
'  File:       UserProp.frm
'
'  Copyright (c) Microsoft Corporation 1996-1998
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************

Option Explicit

Public MsChatPr1 As MsChatPr
Public Channel1 As Channel



Private Sub cmdCancel_Click()
    Me.Hide
End Sub


Private Sub cmdOK_Click()
    Me.Hide
End Sub


Private Sub Form_Unload(Cancel As Integer)
    Set MsChatPr1 = Nothing
    Set Channel1 = Nothing
End Sub


Public Function GetProperties() As Boolean
    If txtNickName = "" Then
        MsgBox "FormUserProperties::GetProperties ---- Nick name is empty!!"
        GetProperties = False
        Exit Function
    End If
    
    If Not Channel1.ChannelState = chsOpen Then
        MsgBox "You are not in the room and thus can't get the properties of its member."
        GetProperties = False
        Exit Function
    End If
    
    Dim lMode As Long
    Dim varNick
    varNick = CStr(txtNickName)
    
    txtIdentity = Channel1.MemberIdentity(varNick)
    
    txtUserName = VPropertyFromChatItems(Channel1.MemberProperty(pnUserName, varNick), pnUserName)
    txtIPAddress = VPropertyFromChatItems(Channel1.MemberProperty(pnIPAddress, varNick), pnIPAddress)
    txtRealName = VPropertyFromChatItems(MsChatPr1.UserProperty(pnRealName, varNick), pnRealName)
    
    lMode = VPropertyFromChatItems(Channel1.MemberProperty(pnModes, varNick), pnModes)
    chkAuthenticated = nBitwiseAnd(mmAuthMember, lMode)
    chkAway = nBitwiseAnd(mmAway, lMode)
    chkIgmored = nBitwiseAnd(mmClientIgnored, lMode)
    
    chkSysOp = nBitwiseAnd(mmSysop, lMode)
    chkOwner = nBitwiseAnd(mmOwner, lMode)
    chkHost = nBitwiseAnd(mmHost, lMode)
    
    
    Dim bModerated As Boolean
    bModerated = VPropertyFromChatItems(Channel1.ChannelProperty(cpnModes), cpnModes) And cmModerated
    
    If (CBool(lMode And mmVoice) And bModerated) Or _
        (CBool(lMode And mmNoVoice) And Not bModerated) Then
        chkSpeaker = 1
    Else
        chkSpeaker = 0
    End If
    
    If CBool(lMode And mmNoVoice) And bModerated Then
        chkNoVoice = 1
    Else
        chkNoVoice = 0
    End If
    
    GetProperties = True
End Function


Function nBitwiseAnd(ByVal a As Integer, ByVal b As Integer) As Integer
    If (a And b) = 0 Then
        nBitwiseAnd = 0
    Else
        nBitwiseAnd = 1
    End If
End Function


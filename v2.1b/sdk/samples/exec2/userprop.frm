VERSION 5.00
Begin VB.Form frmUserProp 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "User Properties"
   ClientHeight    =   5880
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   5520
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5880
   ScaleWidth      =   5520
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.CheckBox chkSpeaker 
      Caption         =   "Speaker"
      Enabled         =   0   'False
      Height          =   255
      Left            =   2640
      TabIndex        =   21
      Top             =   3960
      Width           =   1695
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   360
      Left            =   3360
      TabIndex        =   20
      Tag             =   "Cancel"
      Top             =   5280
      Width           =   1140
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   360
      Left            =   1920
      TabIndex        =   19
      Tag             =   "OK"
      Top             =   5280
      Width           =   1140
   End
   Begin VB.CheckBox chkNoVoice 
      Caption         =   "Sp&eechless"
      Enabled         =   0   'False
      Height          =   255
      Left            =   2640
      TabIndex        =   18
      Top             =   4320
      Width           =   1695
   End
   Begin VB.CheckBox chkHost 
      Caption         =   "&Host"
      Enabled         =   0   'False
      Height          =   255
      Left            =   2640
      TabIndex        =   17
      Top             =   3600
      Width           =   1695
   End
   Begin VB.CheckBox chkOwner 
      Caption         =   "&Channel Owner"
      Enabled         =   0   'False
      Height          =   255
      Left            =   2640
      TabIndex        =   16
      Top             =   3240
      Width           =   1695
   End
   Begin VB.CheckBox chkSysOp 
      Caption         =   "&System operator"
      Enabled         =   0   'False
      Height          =   255
      Left            =   360
      TabIndex        =   15
      Top             =   4320
      Width           =   1455
   End
   Begin VB.CheckBox chkAway 
      Caption         =   "A&way"
      Enabled         =   0   'False
      Height          =   255
      Left            =   360
      TabIndex        =   14
      Top             =   3960
      Width           =   1335
   End
   Begin VB.CheckBox chkAuthenticated 
      Caption         =   "&Authenticated"
      Enabled         =   0   'False
      Height          =   255
      Left            =   360
      TabIndex        =   13
      Top             =   3600
      Width           =   1335
   End
   Begin VB.CheckBox chkIgmored 
      Caption         =   "Ign&ored"
      Enabled         =   0   'False
      Height          =   255
      Left            =   360
      TabIndex        =   12
      Top             =   3240
      Width           =   975
   End
   Begin VB.TextBox txtMemberMode 
      Height          =   285
      Left            =   1800
      Locked          =   -1  'True
      TabIndex        =   11
      Top             =   2760
      Width           =   3255
   End
   Begin VB.TextBox txtIPAddress 
      Height          =   285
      Left            =   1800
      Locked          =   -1  'True
      TabIndex        =   9
      Top             =   2280
      Width           =   3255
   End
   Begin VB.TextBox txtUserName 
      Height          =   285
      Left            =   1800
      Locked          =   -1  'True
      TabIndex        =   7
      Top             =   840
      Width           =   3255
   End
   Begin VB.TextBox txtRealName 
      Height          =   285
      Left            =   1800
      Locked          =   -1  'True
      TabIndex        =   5
      Top             =   1320
      Width           =   3255
   End
   Begin VB.TextBox txtIdentity 
      Height          =   285
      Left            =   1800
      Locked          =   -1  'True
      TabIndex        =   3
      Top             =   1800
      Width           =   3255
   End
   Begin VB.TextBox txtNickName 
      Height          =   285
      Left            =   1800
      Locked          =   -1  'True
      TabIndex        =   1
      Top             =   360
      Width           =   3255
   End
   Begin VB.Label Label6 
      Caption         =   "&Member Mode:"
      Height          =   255
      Left            =   360
      TabIndex        =   10
      Top             =   2760
      Width           =   1215
   End
   Begin VB.Label Label5 
      Caption         =   "&IP address:"
      Height          =   255
      Left            =   360
      TabIndex        =   8
      Top             =   2280
      Width           =   1215
   End
   Begin VB.Label Label4 
      Caption         =   "&User Name:"
      Height          =   255
      Left            =   360
      TabIndex        =   6
      Top             =   840
      Width           =   1215
   End
   Begin VB.Label Label3 
      Caption         =   "&Real Name:"
      Height          =   255
      Left            =   360
      TabIndex        =   4
      Top             =   1320
      Width           =   1215
   End
   Begin VB.Label Label2 
      Caption         =   "I&dentity:"
      Height          =   255
      Left            =   360
      TabIndex        =   2
      Top             =   1800
      Width           =   1215
   End
   Begin VB.Label Label1 
      Caption         =   "&Nick Name:"
      Height          =   255
      Left            =   360
      TabIndex        =   0
      Top             =   360
      Width           =   1215
   End
End
Attribute VB_Name = "frmUserProp"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
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
    
    Dim varNick
    varNick = CStr(txtNickName)
    
    txtIdentity = Channel1.MemberIdentity(varNick)
    
    txtUserName = VPropertyFromChatItems(Channel1.MemberProperty(pnUserName, varNick), pnUserName)
    txtIPAddress = VPropertyFromChatItems(Channel1.MemberProperty(pnIPAddress, varNick), pnIPAddress)
    txtMemberMode = VPropertyFromChatItems(Channel1.MemberProperty(pnModes, varNick), pnModes)
    txtRealName = VPropertyFromChatItems(MsChatPr1.UserProperty(pnRealName, varNick), pnRealName)
    
    chkAuthenticated = nBitwiseAnd(mmAuthMember, txtMemberMode)
    chkAway = nBitwiseAnd(mmAway, txtMemberMode)
    chkIgmored = nBitwiseAnd(mmClientIgnored, txtMemberMode)
    
    chkSysOp = nBitwiseAnd(mmSysop, txtMemberMode)
    chkOwner = nBitwiseAnd(mmOwner, txtMemberMode)
    chkHost = nBitwiseAnd(mmHost, txtMemberMode)
    
    
    Dim bModerated As Boolean
    bModerated = VPropertyFromChatItems(Channel1.ChannelProperty(cpnModes), cpnModes) And cmModerated
    
    If (CBool(txtMemberMode And mmVoice) And bModerated) Or _
        (CBool(txtMemberMode And mmNoVoice) And Not bModerated) Then
        chkSpeaker = 1
    Else
        chkSpeaker = 0
    End If
    
    If CBool(txtMemberMode And mmNoVoice) And bModerated Then
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


VERSION 5.00
Begin VB.Form frmLogin 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Login"
   ClientHeight    =   3510
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4080
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3510
   ScaleWidth      =   4080
   ShowInTaskbar   =   0   'False
   StartUpPosition =   2  'CenterScreen
   Tag             =   "Login"
   Begin VB.OptionButton optGuest 
      Caption         =   "Special &Guest"
      Height          =   255
      Left            =   120
      TabIndex        =   9
      Top             =   4200
      Visible         =   0   'False
      Width           =   1455
   End
   Begin VB.OptionButton optModerator 
      Caption         =   "&Moderator"
      Height          =   255
      Left            =   120
      TabIndex        =   8
      Top             =   3840
      Visible         =   0   'False
      Width           =   1095
   End
   Begin VB.OptionButton optHost 
      Caption         =   "&Host"
      Height          =   255
      Left            =   120
      TabIndex        =   7
      Top             =   3480
      Value           =   -1  'True
      Visible         =   0   'False
      Width           =   975
   End
   Begin VB.TextBox txtRoom 
      Height          =   285
      IMEMode         =   3  'DISABLE
      Left            =   1320
      TabIndex        =   6
      Text            =   "#ExecChat2"
      Top             =   3240
      Visible         =   0   'False
      Width           =   2325
   End
   Begin VB.TextBox txtNick 
      Height          =   285
      Left            =   1320
      TabIndex        =   1
      Text            =   "MyNick"
      Top             =   360
      Width           =   2325
   End
   Begin VB.TextBox txtUser 
      Height          =   285
      Left            =   1305
      TabIndex        =   2
      Text            =   "ExecChat2"
      Top             =   720
      Width           =   2325
   End
   Begin VB.TextBox txtRealName 
      Height          =   285
      Left            =   1320
      TabIndex        =   3
      Text            =   "Real Name"
      Top             =   1080
      Width           =   2325
   End
   Begin VB.TextBox txtPassword 
      Height          =   285
      IMEMode         =   3  'DISABLE
      Left            =   1305
      PasswordChar    =   "*"
      TabIndex        =   4
      Top             =   1605
      Width           =   2325
   End
   Begin VB.TextBox txtServer 
      Height          =   285
      IMEMode         =   3  'DISABLE
      Left            =   1320
      TabIndex        =   5
      Text            =   "chloe1"
      Top             =   2040
      Width           =   2325
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   360
      Left            =   840
      TabIndex        =   10
      Tag             =   "OK"
      Top             =   2880
      Width           =   1140
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   360
      Left            =   2280
      TabIndex        =   11
      Tag             =   "Cancel"
      Top             =   2880
      Width           =   1140
   End
   Begin VB.Label lblLabels 
      Caption         =   "&Room Name:"
      Height          =   255
      Index           =   3
      Left            =   120
      TabIndex        =   14
      Top             =   3240
      Visible         =   0   'False
      Width           =   1080
   End
   Begin VB.Label lblLabels 
      Caption         =   "&Server Name:"
      Height          =   255
      Index           =   2
      Left            =   120
      TabIndex        =   13
      Top             =   2040
      Width           =   1080
   End
   Begin VB.Label lblLabels 
      Caption         =   "&Password:"
      Height          =   255
      Index           =   1
      Left            =   105
      TabIndex        =   0
      Tag             =   "&Password:"
      Top             =   1620
      Width           =   1080
   End
   Begin VB.Label lblLabels 
      Caption         =   "&Real Name:"
      Height          =   255
      Index           =   5
      Left            =   120
      TabIndex        =   16
      Tag             =   "&User Name:"
      Top             =   1080
      Width           =   1080
   End
   Begin VB.Label lblLabels 
      Caption         =   "&User Name:"
      Height          =   255
      Index           =   0
      Left            =   105
      TabIndex        =   12
      Tag             =   "&User Name:"
      Top             =   750
      Width           =   1080
   End
   Begin VB.Label lblLabels 
      Caption         =   "&Nick Name:"
      Height          =   255
      Index           =   4
      Left            =   120
      TabIndex        =   15
      Tag             =   "&User Name:"
      Top             =   375
      Width           =   1080
   End
End
Attribute VB_Name = "frmLogin"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public UserType As Integer
Public OK As Boolean
Public bTryLogin As Boolean



Private Sub Form_Load()
    Dim sBuffer As String
    Dim lSize As Long
    
    UserType = utHost
    
    sBuffer = Space$(255)
    lSize = Len(sBuffer)
    Call GetUserName(sBuffer, lSize)
    If lSize > 0 Then
        txtUser.Text = Left$(sBuffer, lSize)
    Else
        txtUser.Text = vbNullString
    End If
    
    txtNick.SelStart = 0
    txtNick.SelLength = Len(txtNick)
End Sub





Private Sub cmdCancel_Click()
    OK = False
    Me.Hide
End Sub


Private Sub cmdOK_Click()
    If txtServer = "" Then
        MsgBox "Invalid Server, try again!", , "Login"
        txtServer.SetFocus
        txtServer.SelStart = 0
        txtServer.SelLength = Len(txtServer.Text)
        Exit Sub
    End If
    
    If txtNick = "" Then
        MsgBox "Invalid Nick name, try again!", , "Login"
        txtNick.SetFocus
        txtNick.SelStart = 0
        txtNick.SelLength = Len(txtNick.Text)
        Exit Sub
    End If
    
    If optHost Then
        UserType = utHost
    ElseIf optGuest Then
        UserType = utGuest
    ElseIf optModerator Then
        UserType = utModerator
    End If
    
    If bTryLogin = False Then
        OK = True
    Else
        OK = Login
    End If
    
    If OK = True Then
        fMainForm.lblNickName = txtNick
        Me.Hide
    End If
End Sub


Private Function Login() As Boolean
    Dim varSecPackages
    On Error Resume Next
    
    varSecPackages = "NTLM;ANON"    ' try NTLM authentication first
    
    fMainForm.MsChatPr1.Login txtServer, txtNick, _
        CStr(txtUser), CStr(txtRealName), CStr(txtPassword), _
        varSecPackages
    
    If Err = 0 Then
        Login = True
    Else
        Login = False
        MsgBox "Error during login: " & Err.Description, , "Login"
        Err.Clear
    End If
End Function


VERSION 5.00
'****************************************************************************
'
'  Module:     PlayBd.ocx
'  File:       PlayBd.frm
'
'  Copyright (c) Microsoft Corporation 1996-1997
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************
Object = "{A2BBC341-F930-11D0-99CB-00C04FD64497}#3.0#0"; "PlayBd.ocx"
Begin VB.Form Form1 
   BackColor       =   &H00FFFF00&
   Caption         =   "Form1"
   ClientHeight    =   5100
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   9240
   LinkTopic       =   "Form1"
   ScaleHeight     =   5100
   ScaleWidth      =   9240
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   23
      Left            =   8880
      TabIndex        =   43
      Top             =   4680
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   22
      Left            =   8880
      TabIndex        =   42
      Top             =   4320
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   21
      Left            =   8520
      TabIndex        =   41
      Top             =   4680
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   20
      Left            =   8520
      TabIndex        =   40
      Top             =   4320
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   19
      Left            =   8160
      TabIndex        =   39
      Top             =   4680
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   18
      Left            =   8160
      TabIndex        =   38
      Top             =   4320
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   17
      Left            =   7800
      TabIndex        =   37
      Top             =   4680
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   16
      Left            =   7800
      TabIndex        =   36
      Top             =   4320
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   15
      Left            =   7440
      TabIndex        =   35
      Top             =   4680
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   14
      Left            =   7440
      TabIndex        =   34
      Top             =   4320
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   13
      Left            =   7080
      TabIndex        =   33
      Top             =   4680
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   12
      Left            =   7080
      TabIndex        =   32
      Top             =   4320
      Width           =   255
   End
   Begin VB.CheckBox chkWhite 
      BackColor       =   &H00FFFF80&
      Caption         =   "White?"
      Height          =   255
      Left            =   6480
      TabIndex        =   31
      Top             =   3960
      Width           =   975
   End
   Begin VB.CommandButton cmdSetCheckerPositions 
      Caption         =   "Set"
      Height          =   255
      Left            =   8400
      TabIndex        =   30
      Top             =   3960
      Width           =   735
   End
   Begin VB.CommandButton cmdGetCheckerPositions 
      Caption         =   "Get"
      Height          =   255
      Left            =   7560
      TabIndex        =   29
      Top             =   3960
      Width           =   735
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   11
      Left            =   6720
      TabIndex        =   27
      Top             =   4680
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   10
      Left            =   6720
      TabIndex        =   26
      Top             =   4320
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   9
      Left            =   6360
      TabIndex        =   25
      Top             =   4680
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   8
      Left            =   6360
      TabIndex        =   24
      Top             =   4320
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   7
      Left            =   6000
      TabIndex        =   23
      Top             =   4680
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   6
      Left            =   6000
      TabIndex        =   22
      Top             =   4320
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   5
      Left            =   5640
      TabIndex        =   21
      Top             =   4680
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   4
      Left            =   5640
      TabIndex        =   20
      Top             =   4320
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   3
      Left            =   5280
      TabIndex        =   19
      Top             =   4680
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   2
      Left            =   5280
      TabIndex        =   18
      Top             =   4320
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   1
      Left            =   4920
      TabIndex        =   17
      Top             =   4680
      Width           =   255
   End
   Begin VB.TextBox txtCP 
      Height          =   285
      Index           =   0
      Left            =   4920
      TabIndex        =   16
      Top             =   4320
      Width           =   255
   End
   Begin VB.TextBox txtEYD 
      Height          =   285
      Left            =   8760
      TabIndex        =   14
      Top             =   2160
      Width           =   255
   End
   Begin VB.TextBox txtEXD 
      Height          =   285
      Left            =   8400
      TabIndex        =   13
      Top             =   2160
      Width           =   255
   End
   Begin VB.TextBox txtEYO 
      Height          =   285
      Left            =   7080
      TabIndex        =   12
      Top             =   2160
      Width           =   255
   End
   Begin VB.TextBox txtEXO 
      Height          =   285
      Left            =   6720
      TabIndex        =   11
      Top             =   2160
      Width           =   255
   End
   Begin VB.CommandButton cmdResetCheckers 
      Caption         =   "Reset Checkers"
      Height          =   255
      Left            =   6720
      TabIndex        =   9
      Top             =   600
      Width           =   1335
   End
   Begin VB.CommandButton cmdMoveTo 
      Caption         =   "MoveTo"
      Height          =   255
      Left            =   7440
      TabIndex        =   8
      Top             =   1080
      Width           =   855
   End
   Begin VB.TextBox txtYD 
      Height          =   285
      Left            =   8760
      TabIndex        =   7
      Text            =   "2"
      Top             =   1080
      Width           =   255
   End
   Begin VB.TextBox txtXD 
      Height          =   285
      Left            =   8400
      TabIndex        =   6
      Text            =   "1"
      Top             =   1080
      Width           =   255
   End
   Begin VB.TextBox txtYO 
      Height          =   285
      Left            =   7080
      TabIndex        =   5
      Text            =   "1"
      Top             =   1080
      Width           =   255
   End
   Begin VB.TextBox txtXO 
      Height          =   285
      Left            =   6720
      TabIndex        =   4
      Text            =   "2"
      Top             =   1080
      Width           =   255
   End
   Begin VB.CommandButton cmdSetSize 
      Caption         =   "Set"
      Height          =   255
      Left            =   8160
      TabIndex        =   2
      Top             =   120
      Width           =   735
   End
   Begin VB.TextBox txtBoardSize 
      Height          =   285
      Left            =   7680
      TabIndex        =   1
      Text            =   "8"
      Top             =   120
      Width           =   375
   End
   Begin Project1.PlayBoard PlayBoard1 
      Height          =   3135
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   4815
      _ExtentX        =   8493
      _ExtentY        =   5530
   End
   Begin VB.Label Label6 
      BackStyle       =   0  'Transparent
      Caption         =   "Y"
      Height          =   255
      Left            =   4560
      TabIndex        =   45
      Top             =   4680
      Width           =   255
   End
   Begin VB.Label Label5 
      BackStyle       =   0  'Transparent
      Caption         =   "X"
      Height          =   255
      Left            =   4560
      TabIndex        =   44
      Top             =   4320
      Width           =   255
   End
   Begin VB.Label Label4 
      Alignment       =   2  'Center
      BackStyle       =   0  'Transparent
      Caption         =   "Chercher Positions"
      Height          =   255
      Left            =   6840
      TabIndex        =   28
      Top             =   3600
      Width           =   2055
   End
   Begin VB.Label Label3 
      Alignment       =   2  'Center
      BackStyle       =   0  'Transparent
      Caption         =   "moved to"
      Height          =   255
      Left            =   7440
      TabIndex        =   15
      Top             =   2160
      Width           =   855
   End
   Begin VB.Label Label2 
      BackStyle       =   0  'Transparent
      Caption         =   "Event feedback:"
      Height          =   255
      Left            =   6720
      TabIndex        =   10
      Top             =   1800
      Width           =   1335
   End
   Begin VB.Label Label1 
      BackStyle       =   0  'Transparent
      Caption         =   "BoardSize:"
      Height          =   255
      Left            =   6720
      TabIndex        =   3
      Top             =   120
      Width           =   855
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub cmdGetCheckerPositions_Click()
    Dim vrgnPos As Variant
    Dim i As Integer
    
    vrgnPos = PlayBoard1.CheckerPositions(chkWhite.Value = 1)
    For i = 1 To (PlayBoard1.Size - 2) * PlayBoard1.Size / 2
        txtCP(i - 1).Text = vrgnPos(i)
    Next i
End Sub

Private Sub cmdMoveTo_Click()
    PlayBoard1.MoveChecker CInt(txtXO.Text), CInt(txtYO.Text), CInt(txtXD.Text), CInt(txtYD.Text)
End Sub

Private Sub cmdResetCheckers_Click()
    PlayBoard1.ResetCheckers
End Sub

Private Sub cmdSetCheckerPositions_Click()
    Dim vrgnPos() As Variant
    Dim i As Integer
    
    ReDim vrgnPos(1 To (PlayBoard1.Size - 2) * PlayBoard1.Size / 2)
    
    For i = 1 To (PlayBoard1.Size - 2) * PlayBoard1.Size / 2
        vrgnPos(i) = CInt(txtCP(i - 1).Text)
    Next i
    PlayBoard1.CheckerPositions(chkWhite.Value = 1) = vrgnPos
End Sub

Private Sub cmdSetSize_Click()
    PlayBoard1.Size = CInt(txtBoardSize.Text)
End Sub

Private Sub Form_Load()
    PlayBoard1.Left = 0
    PlayBoard1.Top = 0
    PlayBoard1.Height = 4000
    PlayBoard1.Width = 5500
End Sub

Private Sub PlayBoard1_OnCheckerMoved(FromMoveCheckerCall As Boolean, XO As Integer, YO As Integer, XD As Integer, YD As Integer)
    txtEXO.Text = CStr(XO)
    txtEYO.Text = CStr(YO)
    txtEXD.Text = CStr(XD)
    txtEYD.Text = CStr(YD)
    Beep
End Sub

VERSION 4.00
'****************************************************************************
'
'  Module:     CHECKER.EXE
'  File:       Checker.frm
'
'  Copyright (c) Microsoft Corporation 1996-1997
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************
Begin VB.Form Main 
   BackColor       =   &H00FFFFC0&
   Caption         =   "MS Chat Monitoring Tool"
   ClientHeight    =   8760
   ClientLeft      =   135
   ClientTop       =   1740
   ClientWidth     =   11850
   Height          =   9165
   Left            =   75
   LinkTopic       =   "Main"
   ScaleHeight     =   8760
   ScaleWidth      =   11850
   Top             =   1395
   Width           =   11970
   Begin VB.Frame Frame5 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Enter/Exit Room"
      Height          =   735
      Left            =   6840
      TabIndex        =   30
      Top             =   4560
      Width           =   4935
      Begin VB.CommandButton cmdExitRoom 
         Caption         =   "Exit Room"
         Height          =   255
         Left            =   3360
         TabIndex        =   2
         Top             =   360
         Width           =   1455
      End
      Begin VB.CommandButton cmdCancelEntering 
         Caption         =   "Cancel Entering"
         Height          =   255
         Left            =   1680
         TabIndex        =   1
         Top             =   360
         Width           =   1575
      End
      Begin VB.CommandButton cmdEnterRoom 
         Caption         =   "Enter Room"
         Default         =   -1  'True
         Height          =   255
         Left            =   120
         TabIndex        =   0
         Top             =   360
         Width           =   1455
      End
   End
   Begin VB.Frame Frame4 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Ignore Participant"
      Height          =   975
      Left            =   6840
      TabIndex        =   28
      Top             =   3480
      Width           =   4935
      Begin VB.CommandButton cmdDoNotIgnore 
         Caption         =   "Don't Ignore"
         Height          =   255
         Left            =   2520
         TabIndex        =   12
         Top             =   600
         Width           =   2175
      End
      Begin VB.CommandButton cmdIgnore 
         Caption         =   "Ignore"
         Height          =   255
         Left            =   240
         TabIndex        =   11
         Top             =   600
         Width           =   2175
      End
      Begin VB.Label Label5 
         BackColor       =   &H00FFFFC0&
         Caption         =   "Ignore or not the selected member."
         Height          =   255
         Left            =   1320
         TabIndex        =   29
         Top             =   240
         Width           =   2535
      End
   End
   Begin VB.Frame Frame1 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Participant Real Name"
      Height          =   2295
      Left            =   6840
      TabIndex        =   22
      Top             =   1080
      Width           =   4935
      Begin VB.CommandButton cmdGetRealName 
         Caption         =   "Get Real Name"
         Height          =   975
         Left            =   4080
         TabIndex        =   10
         Top             =   480
         Width           =   735
      End
      Begin VB.Frame Frame2 
         BackColor       =   &H00FFFFC0&
         Caption         =   "Get Real Name from"
         Height          =   1095
         Left            =   120
         TabIndex        =   23
         Top             =   360
         Width           =   3735
         Begin VB.TextBox txtAlias 
            Enabled         =   0   'False
            Height          =   285
            Left            =   1320
            TabIndex        =   9
            Top             =   720
            Width           =   2295
         End
         Begin VB.OptionButton optAlias 
            BackColor       =   &H00FFFFC0&
            Caption         =   "This Alias"
            Height          =   255
            Left            =   120
            TabIndex        =   8
            Top             =   720
            Width           =   1095
         End
         Begin VB.OptionButton optSelection 
            BackColor       =   &H00FFFFC0&
            Caption         =   "Selected member"
            Height          =   255
            Left            =   120
            TabIndex        =   7
            Top             =   360
            Value           =   -1  'True
            Width           =   1815
         End
      End
      Begin VB.Label lblAsyncRealName 
         Height          =   255
         Left            =   1680
         TabIndex        =   27
         Top             =   1920
         Width           =   3135
      End
      Begin VB.Label lblAsyncAlias 
         Height          =   255
         Left            =   120
         TabIndex        =   26
         Top             =   1920
         Width           =   1335
      End
      Begin VB.Label Label2 
         BackColor       =   &H00FFFFC0&
         Caption         =   "Real Name:"
         Height          =   255
         Left            =   1680
         TabIndex        =   25
         Top             =   1680
         Width           =   1215
      End
      Begin VB.Label Label1 
         BackColor       =   &H00FFFFC0&
         Caption         =   "Alias:"
         Height          =   255
         Left            =   120
         TabIndex        =   24
         Top             =   1680
         Width           =   735
      End
   End
   Begin VB.CommandButton cmdWhisperNoWhisper 
      Caption         =   "No/Whisper"
      Height          =   255
      Left            =   6840
      TabIndex        =   3
      Top             =   5400
      Width           =   1455
   End
   Begin VB.CommandButton cmdSetTopic 
      Caption         =   "Set Topic"
      Height          =   255
      Left            =   4800
      TabIndex        =   6
      Top             =   960
      Width           =   1455
   End
   Begin VB.TextBox txtTopic 
      Height          =   285
      Left            =   1680
      TabIndex        =   5
      Top             =   960
      Width           =   3015
   End
   Begin VB.CommandButton cmdClearHistory 
      Caption         =   "Clear History"
      Height          =   255
      Left            =   8640
      TabIndex        =   4
      Top             =   5400
      Width           =   1335
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   255
      Left            =   10320
      TabIndex        =   13
      Top             =   5400
      Width           =   1455
   End
   Begin MSChatObjectsCtl.MSChat MSChat1 
      Height          =   2775
      Left            =   120
      TabIndex        =   70
      Top             =   5880
      Width           =   11655
      _ExtentX        =   20558
      _ExtentY        =   4895
      Appearance      =   0
      BorderStyle     =   0
      UIOption        =   3839
      BackColor       =   192
   End
   Begin VB.Label lblState 
      Alignment       =   2  'Center
      BackColor       =   &H000000C0&
      Caption         =   "Disconnected"
      Height          =   255
      Left            =   10680
      TabIndex        =   69
      Top             =   360
      Width           =   1095
   End
   Begin VB.Label lblYourTurn 
      Alignment       =   2  'Center
      BackColor       =   &H00C0FFFF&
      Caption         =   "Up to you!"
      Height          =   255
      Left            =   9000
      TabIndex        =   68
      Top             =   360
      Width           =   1335
   End
   Begin VB.Label Label12 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Your turn to play?"
      Height          =   255
      Left            =   9000
      TabIndex        =   67
      Top             =   120
      Width           =   1335
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   31
      Left            =   3370
      TabIndex        =   66
      Top             =   5240
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   30
      Left            =   2290
      TabIndex        =   65
      Top             =   5240
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   29
      Left            =   1210
      TabIndex        =   64
      Top             =   5240
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   28
      Left            =   140
      TabIndex        =   63
      Top             =   5240
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   27
      Left            =   3910
      TabIndex        =   62
      Top             =   4690
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   26
      Left            =   2840
      TabIndex        =   61
      Top             =   4690
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   25
      Left            =   1760
      TabIndex        =   60
      Top             =   4690
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   24
      Left            =   680
      TabIndex        =   59
      Top             =   4690
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   23
      Left            =   3370
      TabIndex        =   58
      Top             =   4160
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   22
      Left            =   2290
      TabIndex        =   57
      Top             =   4160
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   21
      Left            =   1210
      TabIndex        =   56
      Top             =   4160
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   20
      Left            =   140
      TabIndex        =   55
      Top             =   4160
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   19
      Left            =   3910
      TabIndex        =   54
      Top             =   3610
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   18
      Left            =   2840
      TabIndex        =   53
      Top             =   3610
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   17
      Left            =   1760
      TabIndex        =   52
      Top             =   3610
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   16
      Left            =   675
      TabIndex        =   51
      Top             =   3610
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   15
      Left            =   3370
      TabIndex        =   50
      Top             =   3070
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   14
      Left            =   2290
      TabIndex        =   49
      Top             =   3070
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   13
      Left            =   1210
      TabIndex        =   48
      Top             =   3070
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   12
      Left            =   140
      TabIndex        =   47
      Top             =   3070
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   11
      Left            =   3910
      TabIndex        =   46
      Top             =   2540
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   10
      Left            =   2840
      TabIndex        =   45
      Top             =   2540
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   9
      Left            =   1760
      TabIndex        =   44
      Top             =   2540
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   8
      Left            =   675
      TabIndex        =   43
      Top             =   2540
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   7
      Left            =   3370
      TabIndex        =   42
      Top             =   2000
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   6
      Left            =   2290
      TabIndex        =   41
      Top             =   2000
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   5
      Left            =   1210
      TabIndex        =   40
      Top             =   2000
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   4
      Left            =   140
      TabIndex        =   39
      Top             =   2000
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   3
      Left            =   3910
      TabIndex        =   38
      Top             =   1460
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   2
      Left            =   2840
      TabIndex        =   37
      Top             =   1460
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   525
      Index           =   1
      Left            =   1760
      TabIndex        =   36
      Top             =   1460
      Width           =   525
   End
   Begin VB.Label Label11 
      BackStyle       =   0  'Transparent
      Height          =   530
      Index           =   0
      Left            =   680
      TabIndex        =   35
      Top             =   1460
      Width           =   530
   End
   Begin VB.Label Label9 
      BackStyle       =   0  'Transparent
      Height          =   615
      Left            =   4440
      TabIndex        =   34
      Top             =   3600
      Width           =   2175
   End
   Begin VB.Label Label6 
      BackStyle       =   0  'Transparent
      Height          =   615
      Left            =   4440
      TabIndex        =   33
      Top             =   1440
      Width           =   2175
   End
   Begin VB.Label Label4 
      Alignment       =   2  'Center
      BackColor       =   &H00FFFFFF&
      Caption         =   "Dead White Checkers"
      Height          =   255
      Left            =   4560
      TabIndex        =   32
      Top             =   3840
      Width           =   1935
   End
   Begin VB.Label Label3 
      Alignment       =   2  'Center
      BackColor       =   &H00000000&
      Caption         =   "Dead Black Checkers"
      ForeColor       =   &H00FFFFFF&
      Height          =   255
      Left            =   4560
      TabIndex        =   31
      Top             =   1680
      Width           =   1935
   End
   Begin VB.Image imgWhite 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   11
      Left            =   4080
      Picture         =   "checker.frx":0000
      Tag             =   "1"
      Top             =   4200
      Width           =   330
   End
   Begin VB.Image imgWhite 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   10
      Left            =   3000
      Picture         =   "checker.frx":061A
      Tag             =   "1"
      Top             =   4200
      Width           =   330
   End
   Begin VB.Image imgWhite 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   9
      Left            =   1920
      Picture         =   "checker.frx":0C34
      Tag             =   "1"
      Top             =   4200
      Width           =   330
   End
   Begin VB.Image imgWhite 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   8
      Left            =   840
      Picture         =   "checker.frx":124E
      Tag             =   "1"
      Top             =   4200
      Width           =   330
   End
   Begin VB.Image imgWhite 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   7
      Left            =   3480
      Picture         =   "checker.frx":1868
      Tag             =   "1"
      Top             =   4800
      Width           =   330
   End
   Begin VB.Image imgWhite 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   6
      Left            =   2400
      Picture         =   "checker.frx":1E82
      Tag             =   "1"
      Top             =   4800
      Width           =   330
   End
   Begin VB.Image imgWhite 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   5
      Left            =   1320
      Picture         =   "checker.frx":249C
      Tag             =   "1"
      Top             =   4800
      Width           =   330
   End
   Begin VB.Image imgWhite 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   4
      Left            =   240
      Picture         =   "checker.frx":2AB6
      Tag             =   "1"
      Top             =   4800
      Width           =   330
   End
   Begin VB.Image imgWhite 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   3
      Left            =   4080
      Picture         =   "checker.frx":30D0
      Tag             =   "1"
      Top             =   5280
      Width           =   330
   End
   Begin VB.Image imgWhite 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   2
      Left            =   3000
      Picture         =   "checker.frx":36EA
      Tag             =   "1"
      Top             =   5280
      Width           =   330
   End
   Begin VB.Image imgWhite 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   1
      Left            =   1920
      Picture         =   "checker.frx":3D04
      Tag             =   "1"
      Top             =   5280
      Width           =   330
   End
   Begin VB.Image imgBlack 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   11
      Left            =   3480
      Picture         =   "checker.frx":431E
      Tag             =   "0"
      Top             =   2640
      Width           =   330
   End
   Begin VB.Image imgBlack 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   10
      Left            =   2400
      Picture         =   "checker.frx":4938
      Tag             =   "0"
      Top             =   2640
      Width           =   330
   End
   Begin VB.Image imgBlack 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   9
      Left            =   1320
      Picture         =   "checker.frx":4F52
      Tag             =   "0"
      Top             =   2640
      Width           =   330
   End
   Begin VB.Image imgBlack 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   8
      Left            =   240
      Picture         =   "checker.frx":556C
      Tag             =   "0"
      Top             =   2640
      Width           =   330
   End
   Begin VB.Image imgBlack 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   7
      Left            =   3960
      Picture         =   "checker.frx":5B86
      Tag             =   "0"
      Top             =   2160
      Width           =   330
   End
   Begin VB.Image imgBlack 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   6
      Left            =   2880
      Picture         =   "checker.frx":61A0
      Tag             =   "0"
      Top             =   2160
      Width           =   330
   End
   Begin VB.Image imgBlack 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   5
      Left            =   1920
      Picture         =   "checker.frx":67BA
      Tag             =   "0"
      Top             =   2160
      Width           =   330
   End
   Begin VB.Image imgBlack 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   4
      Left            =   720
      Picture         =   "checker.frx":6DD4
      Tag             =   "0"
      Top             =   2040
      Width           =   330
   End
   Begin VB.Image imgBlack 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   3
      Left            =   3480
      Picture         =   "checker.frx":73EE
      Tag             =   "0"
      Top             =   1560
      Width           =   330
   End
   Begin VB.Image imgBlack 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   2
      Left            =   2400
      Picture         =   "checker.frx":7A08
      Tag             =   "0"
      Top             =   1560
      Width           =   330
   End
   Begin VB.Image imgBlack 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   1
      Left            =   1320
      Picture         =   "checker.frx":8022
      Tag             =   "0"
      Top             =   1560
      Width           =   330
   End
   Begin VB.Image imgWhite 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   0
      Left            =   840
      Picture         =   "checker.frx":863C
      Tag             =   "1"
      Top             =   5280
      Width           =   330
   End
   Begin VB.Image imgBlack 
      DragMode        =   1  'Automatic
      Height          =   330
      Index           =   0
      Left            =   240
      Picture         =   "checker.frx":8C56
      Tag             =   "0"
      Top             =   1560
      Width           =   330
   End
   Begin VB.Image imgBackGround 
      Height          =   4335
      Left            =   120
      Picture         =   "checker.frx":9270
      Top             =   1440
      Width           =   6465
   End
   Begin VB.Label lblRoomPath 
      Height          =   255
      Left            =   4080
      TabIndex        =   21
      Top             =   360
      Width           =   4575
   End
   Begin VB.Label lblUserName 
      Height          =   255
      Left            =   2160
      TabIndex        =   20
      Top             =   360
      Width           =   1695
   End
   Begin VB.Label lblAlias 
      Height          =   255
      Left            =   120
      TabIndex        =   19
      Top             =   360
      Width           =   1815
   End
   Begin VB.Label Label18 
      BackColor       =   &H00FFFFC0&
      Caption         =   "State"
      Height          =   255
      Left            =   10680
      TabIndex        =   18
      Top             =   120
      Width           =   615
   End
   Begin VB.Label Label13 
      Alignment       =   1  'Right Justify
      BackColor       =   &H00FFFFC0&
      Caption         =   "Chat Room Topic"
      Height          =   255
      Left            =   120
      TabIndex        =   17
      Top             =   960
      Width           =   1335
   End
   Begin VB.Label Label10 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Name"
      Height          =   255
      Left            =   2160
      TabIndex        =   16
      Top             =   120
      Width           =   1815
   End
   Begin VB.Label Label8 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Room Path"
      Height          =   255
      Left            =   4080
      TabIndex        =   15
      Top             =   120
      Width           =   4575
   End
   Begin VB.Label Label7 
      BackColor       =   &H00FFFFC0&
      Caption         =   "Alias "
      Height          =   255
      Left            =   120
      TabIndex        =   14
      Top             =   120
      Width           =   1815
   End
End
Attribute VB_Name = "Main"
Attribute VB_Creatable = False
Attribute VB_Exposed = False

Option Explicit

Const MASK_NOWHISPER = 2
Const MASK_IGNORED = 4

Const STAT_NOWHISPER = 8
Const STAT_IGNORED = 16

Const CAP_NOWHISPER = "Deny Whispers"
Const CAP_WHISPER = "Accept Whispers"

Const CAP_YOURTURN = "Yes"
Const CAP_NOTYOURTURN = "No"

Const COLUMNS = 8
Const ROWS = 8
Const CHK_PER_ROW = COLUMNS / 2
Const ROW_PER_TEAM = 3

Const TYPE_MOVE = 0
Const TYPE_ASKFORPOS = 1
Const TYPE_GIVEPOS = 2

Dim nDeadBlack As Integer
Dim nDeadWhite As Integer
Dim fGotPositions As Boolean
Dim lgParticipantID1 As Long
Dim lgParticipantID2 As Long

Sub BroadcastMove(Tag As Integer, Index As Integer, Left As Integer, Top As Integer)
    On Error Resume Next
    If (MSChat1.State = 3) Then
        Dim rgInt(0 To 4) As Integer
        Dim vMess As Variant
        
        rgInt(0) = TYPE_MOVE
        rgInt(1) = Tag
        rgInt(2) = Index
        rgInt(3) = Left
        rgInt(4) = Top
        
        MSChat1.SendMessage -1, 0, rgInt
        
        lblYourTurn.Caption = CAP_NOTYOURTURN
        lblYourTurn.BackColor = &HC0&
    End If
End Sub
Sub InitializeChkPositions()
    Dim X, Y As Integer
    
    For Y = 0 To ROW_PER_TEAM - 1
        For X = 0 To CHK_PER_ROW - 1
            imgBlack(Y * CHK_PER_ROW + X).Left = imgBackGround.Left + 120 + X * 1080 + (Y Mod 2) * 540
            imgBlack(Y * CHK_PER_ROW + X).Top = imgBackGround.Top + 120 + Y * 540
        Next X
    Next Y
    
    For Y = 0 To ROW_PER_TEAM - 1
        For X = 0 To CHK_PER_ROW - 1
            imgWhite(Y * CHK_PER_ROW + X).Left = imgBackGround.Left + 120 + (X + 1) * 1080 - ((Y Mod 2) + 1) * 540
            imgWhite(Y * CHK_PER_ROW + X).Top = imgBackGround.Top + imgBackGround.Height - 120 - Y * 540 - imgWhite(0).Height
        Next X
    Next Y
End Sub


Sub SendPositions(ParticipantID As Long)
    On Error Resume Next
    If (MSChat1.State = 3) Then
        Dim rgInt(0 To CHK_PER_ROW * ROW_PER_TEAM * 4) As Integer
        Dim X As Integer
        
        rgInt(0) = TYPE_GIVEPOS
        
        For X = 0 To CHK_PER_ROW * ROW_PER_TEAM - 1
            rgInt(2 * X + 1) = imgBlack(X).Left
            rgInt(2 * (X + 1)) = imgBlack(X).Top
        Next X
        
        For X = 0 To CHK_PER_ROW * ROW_PER_TEAM - 1
            rgInt(2 * X + 1 + CHK_PER_ROW * ROW_PER_TEAM * 2) = imgWhite(X).Left
            rgInt(2 * (X + 1) + CHK_PER_ROW * ROW_PER_TEAM * 2) = imgWhite(X).Top
        Next X
        
        MSChat1.SendMessage ParticipantID, 1, rgInt
    End If
End Sub

Sub SetState(NewState As Integer)
    Select Case NewState
    Case 1:
        cmdEnterRoom.Enabled = True
        cmdCancelEntering.Enabled = False
        cmdExitRoom.Enabled = False
        lblState.Caption = "Disconnected"
        lblState.BackColor = &HC0&
        MSChat1.BackColor = &HC0&
    Case 2:
        cmdEnterRoom.Enabled = False
        cmdCancelEntering.Enabled = True
        cmdExitRoom.Enabled = False
        lblState.Caption = "Connecting..."
        lblState.BackColor = &H80FF&
        MSChat1.BackColor = &H80FF&
    Case 3:
        cmdEnterRoom.Enabled = False
        cmdCancelEntering.Enabled = False
        cmdExitRoom.Enabled = True
        lblState.Caption = "Connected"
        lblState.BackColor = &HC000&
        MSChat1.BackColor = &HC000&
    End Select
End Sub






Private Sub cmdGetRealName_Click()
    On Error Resume Next
    
    Dim txtReal As String
    
    lblAsyncAlias.Caption = ""
    lblAsyncRealName.Caption = ""
    
    If (optSelection.Value) Then
        MSChat1.GetParticipantRealName -1, "", False, txtReal
    Else
        MSChat1.GetParticipantRealName 0, txtAlias.Text, False, txtReal
    End If
End Sub










Private Sub Form_Unload(Cancel As Integer)
    Unload EnterOptions
End Sub





Private Sub imgBackGround_DragDrop(Source As Control, X As Single, Y As Single)
    Source.Left = (Int(X / 540) + 0.5) * 540 + imgBackGround.Left - Source.Width / 2
    Source.Top = (Int(Y / 540) + 0.5) * 540 + imgBackGround.Top - Source.Height / 2
    
    BroadcastMove Source.Tag, Source.Index, Source.Left, Source.Top
End Sub


Private Sub MSChat1_OnBeginEnumeration()
    lgParticipantID1 = 0
    lgParticipantID2 = 0
    fGotPositions = False
End Sub

Private Sub MSChat1_OnEnterParticipant(ByVal ParticipantID As Long, ByVal Alias As String, ByVal Status As Integer)
    If (Not fGotPositions) Then
        lgParticipantID1 = lgParticipantID2
        lgParticipantID2 = ParticipantID
    End If
End Sub



Private Sub MSChat1_OnParticipantRealName(ByVal Alias As String, ByVal RealName As String)
    On Error Resume Next
    lblAsyncAlias.Caption = Alias
    lblAsyncRealName.Caption = RealName
End Sub

Private Sub MSChat1_OnRoomTopicChanged(ByVal NewRoomTopic As String)
    On Error Resume Next
    txtTopic.Text = NewRoomTopic
End Sub


Private Sub MSChat1_OnStateChanged(ByVal NewState As Integer)
    SetState NewState
End Sub














Private Sub cmdCancelEntering_Click()
    On Error Resume Next
    MSChat1.CancelEntering
End Sub

Private Sub cmdClearHistory_Click()
    On Error Resume Next
    MSChat1.ClearHistory
End Sub


Private Sub cmdDoNotIgnore_Click()
    On Error Resume Next
    MSChat1.SetParticipantStatus -1, MASK_IGNORED, 0
End Sub

Private Sub cmdEnterRoom_Click()
    EnterOptions.Show
End Sub



Private Sub cmdExit_Click()
    End
End Sub


Private Sub cmdExitRoom_Click()
    On Error Resume Next
    MSChat1.ExitRoom
End Sub













Private Sub cmdIgnore_Click()
    On Error Resume Next
    MSChat1.SetParticipantStatus -1, MASK_IGNORED, STAT_IGNORED
End Sub


























Private Sub cmdSetTopic_Click()
    On Error Resume Next
    MSChat1.RoomTopic = txtTopic.Text
End Sub






Private Sub cmdWhisperNoWhisper_Click()
    On Error Resume Next
    If (cmdWhisperNoWhisper.Caption = CAP_NOWHISPER) Then
        MSChat1.SetParticipantStatus MSChat1.ThisParticipantID, MASK_NOWHISPER, STAT_NOWHISPER
        cmdWhisperNoWhisper.Caption = CAP_WHISPER
    Else
        MSChat1.SetParticipantStatus MSChat1.ThisParticipantID, MASK_NOWHISPER, 0
        cmdWhisperNoWhisper.Caption = CAP_NOWHISPER
    End If
End Sub
Private Sub Form_Load()
    
    cmdWhisperNoWhisper.Caption = CAP_NOWHISPER
    InitializeChkPositions
    nDeadBlack = 0
    nDeadWhite = 0
    
End Sub




Private Sub MSChat1_OnEndEnumeration()
    On Error Resume Next
    txtTopic.Text = MSChat1.RoomTopic
    lblUserName.Caption = MSChat1.ThisParticipantName
    lblRoomPath.Caption = MSChat1.RoomPath
    
    fGotPositions = True
    If (MSChat1.MemberCount > 1) Then
        If (lgParticipantID1 <> MSChat1.ThisParticipantID) Then
            MSChat1.SendMessage lgParticipantID1, 1, TYPE_ASKFORPOS
        Else
            MSChat1.SendMessage lgParticipantID2, 1, TYPE_ASKFORPOS
        End If
    End If
End Sub
Private Sub MSChat1_OnMessage(ByVal SenderID As Long, ByVal Message As Variant, ByVal MessageType As Integer)
    On Error Resume Next
    
    If (VarType(Message) = vbInteger + vbArray) Then
        Select Case Message(0)
            Case TYPE_MOVE:
                If (Message(1) = 0) Then  'black checker was moved
                    imgBlack(Message(2)).Left = Message(3)
                    imgBlack(Message(2)).Top = Message(4)
                Else                      'white checker was moved
                    imgWhite(Message(2)).Left = Message(3)
                    imgWhite(Message(2)).Top = Message(4)
                End If
                lblYourTurn.Caption = CAP_YOURTURN
                lblYourTurn.BackColor = &HC000&

            Case TYPE_GIVEPOS:
                Dim X As Integer
                
                For X = 0 To CHK_PER_ROW * ROW_PER_TEAM - 1
                    imgBlack(X).Left = Message(X * 2 + 1)
                    imgBlack(X).Top = Message((X + 1) * 2)
                Next X
                For X = 0 To CHK_PER_ROW * ROW_PER_TEAM - 1
                    imgWhite(X).Left = Message(X * 2 + 1 + CHK_PER_ROW * ROW_PER_TEAM * 2)
                    imgWhite(X).Top = Message((X + 1) * 2 + CHK_PER_ROW * ROW_PER_TEAM * 2)
                Next X
        End Select
    End If
    
    If (VarType(Message) = vbInteger) Then
        Select Case Message
            Case TYPE_ASKFORPOS:
                SendPositions SenderID
            End Select
    End If
End Sub







Private Sub optAlias_Click()
    txtAlias.Enabled = True
End Sub

Private Sub optSelection_Click()
    txtAlias.Enabled = False
End Sub






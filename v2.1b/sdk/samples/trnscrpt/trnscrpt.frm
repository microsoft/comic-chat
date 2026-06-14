VERSION 4.00
'****************************************************************************
'
'  Module:     Trnscrpt.EXE
'  File:       Trnscrpt.frm
'
'  Copyright (c) Microsoft Corporation 1996-1997
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************
Begin VB.Form Form1 
   Caption         =   "Chat Transcripter"
   ClientHeight    =   6696
   ClientLeft      =   1356
   ClientTop       =   1488
   ClientWidth     =   7380
   FillColor       =   &H00FF0000&
   BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
      Name            =   "MS Sans Serif"
      Size            =   9.6
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   ForeColor       =   &H000000FF&
   Height          =   7020
   Left            =   1308
   LinkTopic       =   "Form1"
   ScaleHeight     =   6696
   ScaleWidth      =   7380
   Top             =   1212
   Width           =   7476
   Begin VB.Frame Frame3 
      Caption         =   "Chat Transcripter Session"
      Height          =   3252
      Left            =   120
      TabIndex        =   14
      Top             =   3240
      Width           =   7092
      Begin VB.Timer Timer2 
         Enabled         =   0   'False
         Interval        =   1000
         Left            =   4440
         Top             =   240
      End
      Begin VB.Timer Timer1 
         Interval        =   1000
         Left            =   5880
         Top             =   240
      End
      Begin MSChatObjectsCtl.MSChat MSChat1 
         Height          =   2892
         Left            =   120
         TabIndex        =   15
         Top             =   240
         Width           =   6852
         _ExtentX        =   12086
         _ExtentY        =   5101
         UIOption        =   3980
         BackColor       =   255
      End
   End
   Begin VB.Frame Frame2 
      Caption         =   "Chat Transcripter Commands"
      Height          =   732
      Left            =   120
      TabIndex        =   11
      Top             =   2400
      Width           =   7092
      Begin VB.CommandButton CurTranscript 
         Caption         =   "View Current Transcript"
         Height          =   372
         Left            =   3960
         TabIndex        =   13
         Top             =   240
         Width           =   3012
      End
      Begin VB.CommandButton TranscriptCmd 
         Caption         =   "Start Transcribing"
         Height          =   372
         Left            =   120
         TabIndex        =   12
         Top             =   240
         Width           =   3732
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Chat Transcripter Options"
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "Times New Roman"
         Size            =   10.2
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   2292
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   7092
      Begin VB.TextBox Peak 
         BackColor       =   &H00FF80FF&
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Arial"
            Size            =   7.8
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00000000&
         Height          =   288
         Left            =   3480
         TabIndex        =   28
         Top             =   1838
         Visible         =   0   'False
         Width           =   852
      End
      Begin VB.TextBox Total 
         BackColor       =   &H00FF80FF&
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Arial"
            Size            =   7.8
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00FF0000&
         Height          =   288
         Left            =   3480
         TabIndex        =   26
         Top             =   1560
         Visible         =   0   'False
         Width           =   852
      End
      Begin VB.TextBox Current 
         BackColor       =   &H00FF80FF&
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Arial"
            Size            =   7.8
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H0000FF00&
         Height          =   288
         Left            =   3480
         TabIndex        =   24
         Top             =   1320
         Visible         =   0   'False
         Width           =   852
      End
      Begin VB.TextBox HostKey 
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Times New Roman"
            Size            =   10.2
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   336
         Left            =   5640
         TabIndex        =   22
         Top             =   840
         Width           =   1212
      End
      Begin VB.CheckBox MembStatus 
         Caption         =   "Allow Participation"
         Height          =   240
         Left            =   4680
         TabIndex        =   19
         Top             =   1320
         Width           =   2052
      End
      Begin VB.TextBox Interval 
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Times New Roman"
            Size            =   10.2
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   336
         Left            =   2040
         TabIndex        =   18
         Text            =   "1"
         Top             =   1320
         Width           =   372
      End
      Begin VB.CheckBox TextSpacing 
         Caption         =   "Text Spacing"
         Height          =   252
         Left            =   4680
         TabIndex        =   16
         Top             =   1580
         Value           =   1  'Checked
         Width           =   1692
      End
      Begin VB.CheckBox Reconnect 
         Caption         =   "AutoReconnect"
         Height          =   252
         Left            =   4680
         TabIndex        =   10
         Top             =   1860
         Width           =   1692
      End
      Begin VB.TextBox ChatRoom 
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Times New Roman"
            Size            =   10.2
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   336
         Left            =   1560
         TabIndex        =   9
         Text            =   "//ChatSrv1/#ChatRoom"
         Top             =   360
         Width           =   2292
      End
      Begin VB.CommandButton Browse 
         Caption         =   "..."
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Times New Roman"
            Size            =   10.8
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   372
         Left            =   3960
         TabIndex        =   8
         Top             =   840
         Width           =   372
      End
      Begin VB.TextBox TranscriptFile 
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Times New Roman"
            Size            =   10.2
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   336
         Left            =   1560
         TabIndex        =   7
         Top             =   840
         Width           =   2292
      End
      Begin VB.TextBox Alias 
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Times New Roman"
            Size            =   10.2
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   336
         Left            =   5640
         TabIndex        =   6
         Text            =   "Transcripter"
         Top             =   360
         Width           =   1212
      End
      Begin VB.TextBox Retries 
         Enabled         =   0   'False
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Times New Roman"
            Size            =   10.2
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   336
         Left            =   2040
         TabIndex        =   5
         Text            =   "2"
         Top             =   1800
         Width           =   372
      End
      Begin VB.Label Label10 
         BackColor       =   &H00FF80FF&
         BorderStyle     =   1  'Fixed Single
         Caption         =   "Peak"
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Arial"
            Size            =   12
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00000000&
         Height          =   288
         Left            =   2640
         TabIndex        =   27
         Top             =   1838
         Visible         =   0   'False
         Width           =   852
      End
      Begin VB.Label Label9 
         BackColor       =   &H00FF80FF&
         BorderStyle     =   1  'Fixed Single
         Caption         =   "Total"
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Arial"
            Size            =   12
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00FF0000&
         Height          =   288
         Left            =   2640
         TabIndex        =   25
         Top             =   1560
         Visible         =   0   'False
         Width           =   852
      End
      Begin VB.Label Label8 
         BackColor       =   &H00FF80FF&
         BorderStyle     =   1  'Fixed Single
         Caption         =   "Current"
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Arial"
            Size            =   12
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H0000FF00&
         Height          =   288
         Left            =   2640
         TabIndex        =   23
         Top             =   1320
         Visible         =   0   'False
         Width           =   852
      End
      Begin VB.Label Label7 
         Caption         =   "Host Key"
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Times New Roman"
            Size            =   10.8
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   252
         Left            =   4680
         TabIndex        =   21
         Top             =   860
         Width           =   1452
      End
      Begin VB.Label Label6 
         Alignment       =   2  'Center
         Caption         =   "(Minutes)"
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Times New Roman"
            Size            =   7.8
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   204
         Left            =   480
         TabIndex        =   20
         Top             =   1560
         Width           =   732
      End
      Begin VB.Label Label5 
         Caption         =   "Log Interval"
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Times New Roman"
            Size            =   10.8
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   252
         Left            =   240
         TabIndex        =   17
         Top             =   1360
         Width           =   1212
      End
      Begin MSComDlg.CommonDialog CommonDialog1 
         Left            =   3960
         Top             =   360
         _Version        =   65536
         _ExtentX        =   677
         _ExtentY        =   677
         _StockProps     =   0
         DefaultExt      =   ".txt"
         DialogTitle     =   "ChatTranscript File"
         Filter          =   "Text (*.txt)|*.txt|All (*.*)|*.*"
         FilterIndex     =   1
         InitDir         =   "."
      End
      Begin VB.Label Label4 
         Caption         =   "Number of Retries"
         Enabled         =   0   'False
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Times New Roman"
            Size            =   10.8
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   252
         Left            =   240
         TabIndex        =   4
         Top             =   1800
         Width           =   1812
      End
      Begin VB.Label Label3 
         Caption         =   "Alias"
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Times New Roman"
            Size            =   10.8
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   252
         Left            =   4680
         TabIndex        =   3
         Top             =   400
         Width           =   492
      End
      Begin VB.Label Label2 
         Caption         =   "Log File"
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Times New Roman"
            Size            =   10.8
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   252
         Left            =   240
         TabIndex        =   2
         Top             =   880
         Width           =   1452
      End
      Begin VB.Label Label1 
         Caption         =   "Chat Room"
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "Times New Roman"
            Size            =   10.8
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   252
         Left            =   240
         TabIndex        =   1
         Top             =   400
         Width           =   1212
      End
   End
End
Attribute VB_Name = "Form1"
Attribute VB_Creatable = False
Attribute VB_Exposed = False

Const INSERT_BLANK = &H100&
Const ALLOW_RESIZING = &H400&
Const SENSITIVE_URLS = &H800&

Const SEND_BUTTON = &H1&
Const WHISPER_BUTTON = &H2&
Const SEND_TEXTBOX = &H10&

Const INFORM_JOIN = &H20&
Const INFORM_LEAVE = &H40&

Const MASK_PRIVILEGES = &H1&
Const STAT_PARTICIPANT = &H2&
Const STAT_SPECTATOR = &H4&

Public TimerTicks As Integer
Public CurTicks As Integer
Public CancelFlag As Boolean

Public CurTry As Integer
Public TotalCount As Integer
Public MaxCount As Integer
Sub Enable_ChatOptions(ByVal flag As Boolean)
ChatRoom.Enabled = flag
TranscriptFile.Enabled = flag
Alias.Enabled = flag
Browse.Enabled = flag
HostKey.Enabled = flag
Label1.Enabled = flag
Label2.Enabled = flag
Label3.Enabled = flag
Label7.Enabled = flag
Timer1.Enabled = Not flag

If (HostKey.Text = "") Then
    current.Visible = False
    total.Visible = False
    Peak.Visible = False
    Label8.Visible = False
    Label9.Visible = False
    Label10.Visible = False
Else
    current.Visible = Not flag
    total.Visible = Not flag
    Peak.Visible = Not flag
    Label8.Visible = Not flag
    Label9.Visible = Not flag
    Label10.Visible = Not flag
End If

End Sub

Sub Set_FileName()
strFileName = ChatRoom.Text
Do
    nPos = InStr(strFileName, "/")
    If (nPos > 0) Then
        strTemp = Mid(strFileName, nPos + 1)
        strFileName = strTemp
    End If
Loop Until nPos = 0
TranscriptFile.Text = strFileName + ".txt"
If (HostKey.Text = "") Then
Else
TranscriptFile.Text = "Host_" + TranscriptFile.Text
End If
End Sub

Sub Update_File(ByVal Stats As Boolean)
'MsgBox "Update File Called", , "testing"
On Error GoTo ErrorHandler
CurHistory = MSChat1.History
If ((CurHistory <> "") Or (Stats)) Then
    CurTranscript.Enabled = False
    Open TranscriptFile.Text For Append Access Write As #1
    
    Print #1, CurHistory
    MSChat1.ClearHistory
     
    If (Stats And (HostKey.Text <> "")) Then
        TempStr = ":::::::::::::::::::::::::::::::::::::::::"
        Print #1, TempStr, TempStr, TempStr, TempStr
        Print #1, "    Chat Room:", ChatRoom.Text;
        Print #1, "      ClosingTime :", Time$
        Print #1, " "
        Print #1, "Total number of participants : ";
        Print #1, TotalCount
        Print #1, "Peak participation during session: ";
        Print #1, MaxCount
        Print #1, " "
        Print #1, TempStr, TempStr, TempStr, TempStr
'        Print #1, "Current number of participants : "
'        Print #1, current.Text
    End If
       
    Close #1
    CurTranscript.Enabled = True
End If

Exit Sub        ' Exit to avoid handler.
ErrorHandler:   ' Error-handling routine.
    Select Case Err.Number  ' Evaluate error number.
        Case 75
            Msg = "Error Saving Transcript File! Make Sure you have access privelages to the file"
            MsgBox Msg, , TranscriptFile.Text
            Err.Clear   ' Clear Err object fields
        Case Else
            ' Tell user what happened. Then clear the Err object.
            Msg = "There was an error attempting to open the OLE object!"
            MsgBox Msg, , Err.Number
            Err.Clear   ' Clear Err object fields
    
            ' Handle other situations here...
    End Select
    CurTranscript.Enabled = True

End Sub

Private Sub Browse_Click()
Rem CommonDialog1.DefaultExt = ".txt"
Rem CommonDialog1.Filter = "Text (*.txt)|*.txt|All (*.*)|*.*"
Rem CommonDialog1.FilterIndex = 1
Rem CommonDialog1.DialogTitle = "ChatTranscript File"
Rem CommonDialog1.DefaultExt = ".txt"
Rem CommonDialog1.DefaultExt = ".txt"
CommonDialog1.filename = TranscriptFile.Text
CommonDialog1.Flags = &H800& Or &H8000& Or &H2& Or &H200000 Or &H4&
CommonDialog1.InitDir = "."
'CommonDialog1.Action = 2
CommonDialog1.ShowSave
TranscriptFile.Text = CommonDialog1.filename
End Sub


Private Sub ChatRoom_Change()
'Call Set_FileName
End Sub



Private Sub ChatRoom_LostFocus()
Call Set_FileName
End Sub


Private Sub CurTranscript_Click()
    On Error GoTo ErrorHandler   ' Defer error trapping.
    Open TranscriptFile.Text For Input Access Read Shared As 2
    FileStr = Input(LOF(2), 2)
    Close #2
    Load Form2
    Form2.RichTextBox1.TextRTF = FileStr
    Form2.Visible = True
'    Else
'        Msg = "Unable to Access Transcript File"
'       MsgBox Msg, , TranscriptFile.Text
'  End If
    'Form2.RichTextBox1.LoadFile TranscriptFile.Text, rtfText
    Exit Sub        ' Exit to avoid handler.
ErrorHandler:   ' Error-handling routine.
    Select Case Err.Number  ' Evaluate error number.
        Case 53
            Msg = "Transcript File Doesn't exist"
            MsgBox Msg, , TranscriptFile.Text
            Err.Clear   ' Clear Err object fields
           
        Case 75
            Msg = "Error Reading Transcript File! Make Sure the file exists"
            MsgBox Msg, , TranscriptFile.Text
            Err.Clear   ' Clear Err object fields
        Case Else
            ' Tell user what happened. Then clear the Err object.
            Msg = "There was an error attempting to open the OLE object!"
            MsgBox Msg, , Err.Number
            Err.Clear   ' Clear Err object fields
    
            ' Handle other situations here...
    End Select
    
End Sub

Private Sub Form_Load()

Call Set_FileName

TimerTicks = Interval.Text
CurTicks = 0

CurTry = 0

End Sub



Private Sub HostKey_LostFocus()
    If (Alias.Text = "Transcripter") Then
        Alias.Text = "THost"
    End If
    
    If (InStr(TranscriptFile.Text, "Host_") = 0) Then
            TranscriptFile.Text = "Host_" + TranscriptFile.Text
    End If

End Sub


Private Sub Interval_Change()
    iVal = 1
    On Error Resume Next
    iVal = CInt(Interval.Text)
    Interval.Text = iVal
    TimerTicks = iVal
'    Timer1.Interval = 1000 * 60 * iVal ' Set interval.
End Sub

Private Sub MembStatus_Click()
On Error Resume Next
If (MembStatus.Value = 0) Then
'    If (MSChat1.State = 3) Then
'        MSChat1.SetParticipantStatus MSChat1.ThisParticipantID, MASK_PRIVILEGES, STAT_SPECTATOR
'    End If
    MSChat1.UIOption = MSChat1.UIOption And Not (SEND_TEXTBOX Or SEND_BUTTON Or WHISPER_BUTTON)
Else
'    If (MSChat1.State = 3) Then
'        MSChat1.SetParticipantStatus MSChat1.ThisParticipantID, MASK_PRIVILEGES, STAT_PARTICIPANT
'    End If
    MSChat1.UIOption = MSChat1.UIOption Or (SEND_TEXTBOX Or SEND_BUTTON Or WHISPER_BUTTON)
End If
 

End Sub

Private Sub MSChat1_OnBeginEnumeration()
    TotalCount = 0
    MaxCount = TotalCount
End Sub

Private Sub MSChat1_OnEnterParticipant(ByVal ParticipantID As Long, ByVal Alias As String, ByVal Status As Integer)
    current.Text = MSChat1.MemberCount
    TotalCount = TotalCount + 1
    total.Text = TotalCount
    If (MaxCount < MSChat1.MemberCount) Then
        MaxCount = MSChat1.MemberCount
        Peak.Text = MaxCount
    End If
End Sub

Private Sub MSChat1_OnExitParticipant(ByVal ParticipantID As Long)
    current.Text = MSChat1.MemberCount
End Sub


Private Sub MSChat1_OnHistoryFull(ByVal Percent As Integer)
Call Update_File(False)
End Sub

Private Sub MSChat1_OnStateChanged(ByVal NewState As Integer)
    If (NewState = 1) Then
        Update_File (True)
        
        If ((CancelFlag = True) Or (Reconnect.Value = 0) Or ((Reconnect.Value = 1) And (CurTry >= Retries.Text))) Then
            TranscriptCmd.Caption = "Start Transcribing"
            TranscriptCmd.Enabled = True
            MSChat1.BackColor = 255
            CurTry = 0
            Call Enable_ChatOptions(True)
        Else
            TranscriptCmd.Caption = "ReStarting Transcribing"
            TranscriptCmd.Enabled = False
            CurTry = CurTry + 1
            MSChat1.BackColor = &HFF0000
            Timer2.Interval = 30000
            Timer2.Enabled = True
        End If
    Else
        If (NewState = 2) Then
            TranscriptCmd.Caption = "Cancel Entering"
            MSChat1.BackColor = 33023
        Else
            On Error Resume Next
            If (NewState = 3) Then
                TranscriptCmd.Caption = "Stop Transcribing"
                MSChat1.BackColor = 49152
                CurTry = 0
                TranscriptCmd.Enabled = True
            End If
        End If
    End If
End Sub





Private Sub Reconnect_Click()
If Reconnect.Value = 0 Then
    Retries.Enabled = False
    Label4.Enabled = False
Else
    Retries.Enabled = True
    Label4.Enabled = True
End If
End Sub

Private Sub TextSpacing_Click()
If TextSpacing.Value = 0 Then
    MSChat1.UIOption = MSChat1.UIOption And Not INSERT_BLANK
Else
    MSChat1.UIOption = MSChat1.UIOption Or INSERT_BLANK
End If
End Sub

Private Sub Timer1_Timer()
CurTicks = CurTicks + 1
If (CurTicks >= TimerTicks) Then
    CurTicks = 0
    Call Update_File(False)
End If
End Sub

Private Sub Timer2_Timer()
If (MSChat1.State = 1) Then
    Call TranscriptCmd_Click
End If
End Sub

Private Sub TranscriptCmd_Click()

If (MSChat1.State = 1) Then
    Rem make sure the file is writable.
    On Error GoTo ErrorHandler
    Open TranscriptFile.Text For Append Access Write As #1
    Close #1
    Timer1.Interval = 60000  ' Set interval to be 1 minute.
    Timer2.Enabled = False
    
    CancelFlag = False
    MSChat1.EnterRoom ChatRoom.Text, HostKey.Text, Alias.Text, "ANON;ALL", 1, 1
    Call Enable_ChatOptions(False)
Else
    CancelFlag = True
    If (MSChat1.State = 2) Then
        TranscriptCmd.Enabled = False
        MSChat1.CancelEntering
        MSChat1.ClearHistory
    Else
        If (MSChat1.State = 3) Then
            TranscriptCmd.Enabled = False
            MSChat1.ExitRoom
            MSChat1.ClearHistory
        End If
    End If
    
    Call Enable_ChatOptions(True)
End If


Exit Sub        ' Exit to avoid handler.
ErrorHandler:   ' Error-handling routine.
    Select Case Err.Number  ' Evaluate error number.
        Case 75
            Msg = "Error Saving Transcript File! Make Sure you have access privelages to the file"
            MsgBox Msg, , TranscriptFile.Text
            Err.Clear   ' Clear Err object fields
        Case Else
            ' Tell user what happened. Then clear the Err object.
            Msg = "There was an error attempting to open the OLE object!"
            MsgBox Msg, , Err.Number
            Err.Clear   ' Clear Err object fields
    
            ' Handle other situations here...
    End Select

End Sub


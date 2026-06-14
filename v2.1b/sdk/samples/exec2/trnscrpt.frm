VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.1#0"; "COMDLG32.OCX"
Begin VB.Form frmTranscript 
   Caption         =   "Transcript Options"
   ClientHeight    =   3195
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   6315
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3195
   ScaleWidth      =   6315
   StartUpPosition =   3  'Windows Default
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   240
      Top             =   2640
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   327680
   End
   Begin VB.CommandButton btnCancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   3720
      TabIndex        =   8
      Top             =   2520
      Width           =   1095
   End
   Begin VB.CommandButton btnOK 
      Caption         =   "OK"
      Height          =   375
      Left            =   1920
      TabIndex        =   7
      Top             =   2520
      Width           =   1095
   End
   Begin VB.CheckBox chkResetHistory 
      Caption         =   "&Delete history after writing to transcript file."
      Height          =   255
      Left            =   240
      TabIndex        =   6
      Top             =   1920
      Width           =   3495
   End
   Begin VB.TextBox txtInterval 
      Height          =   285
      Left            =   1560
      TabIndex        =   4
      Text            =   "60"
      Top             =   1440
      Width           =   1335
   End
   Begin VB.CommandButton btnBrowse 
      Caption         =   "&Browse"
      Height          =   375
      Left            =   5040
      TabIndex        =   2
      Top             =   960
      Width           =   975
   End
   Begin VB.TextBox txtFileName 
      Height          =   285
      Left            =   1560
      TabIndex        =   1
      Top             =   960
      Width           =   3255
   End
   Begin VB.Timer TimerTranscript 
      Enabled         =   0   'False
      Interval        =   30000
      Left            =   840
      Top             =   2640
   End
   Begin VB.Label lblDescription 
      Caption         =   "The chat room's history will be appended to the file you choose below."
      Height          =   255
      Left            =   120
      TabIndex        =   9
      Top             =   360
      Width           =   5055
   End
   Begin VB.Label lblSeconds 
      Caption         =   "seconds"
      Height          =   255
      Left            =   3000
      TabIndex        =   5
      Top             =   1440
      Width           =   735
   End
   Begin VB.Label Labels 
      Caption         =   "Update &Interval:"
      Height          =   255
      Left            =   240
      TabIndex        =   3
      Top             =   1440
      Width           =   1215
   End
   Begin VB.Label lblTranscriptFile 
      Caption         =   "&File Name:"
      Height          =   255
      Left            =   240
      TabIndex        =   0
      Top             =   960
      Width           =   855
   End
End
Attribute VB_Name = "frmTranscript"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit


Const strFilter = "All File(*.*)|*.*"
Const strDefaultFile = "ExecChat.log"       ' in current dir

Public bTrascribing As Boolean

Dim OpenDir As String                       ' init dir for browse dialog
Dim nFile As Integer
Dim nNextStart As Integer

Dim rtbHistory As RichTextBox
Dim strRoomName As String



Rem public subroutines

Public Sub Accociate(ByRef rtbHistory1 As RichTextBox, ByRef strRoomName1 As String)
    If bTrascribing = True Then
        MsgBox "FormTranscript::Accociate --- Transcript is already started."
        Exit Sub
    End If
    
    strRoomName = strRoomName1
    Set rtbHistory = Nothing
    Set rtbHistory = rtbHistory1
End Sub


Public Function StopTranscript() As Boolean
    If bTrascribing = False Then
        MsgBox "FormTranscript::StopTranscript --- Transcript is not started."
        StopTranscript = False
        Exit Function
    End If

    bTrascribing = False
    TimerTranscript.Enabled = False
    
    UpdateTranscript        ' get everything in the history box
    WriteToTranscriptFile "\------------- Transcript for " & strRoomName & _
                        " ends at: " & Now & "   ------------/" & vbCrLf & vbCrLf
    Close #nFile
    
    nFile = -1
    
    ' these two are disabled in StartTranscript
    txtFileName.Enabled = True
    btnBrowse.Enabled = True
    
    StopTranscript = True
End Function


Public Sub ChangeOptions()
    If bTrascribing = False Then
        MsgBox "FormTranscript::ChangeOptions --- Transcript is not started."
        Exit Sub
    End If
    
    Me.Show
End Sub














Rem Private subroutines

Private Sub btnBrowse_Click()
    CommonDialog1.InitDir = OpenDir
    CommonDialog1.Filter = strFilter
    CommonDialog1.filename = txtFileName
    
    CommonDialog1.CancelError = True
    
    On Error GoTo UserCanceled
    CommonDialog1.ShowOpen
    
    txtFileName = CommonDialog1.filename
    OpenDir = Left(txtFileName, InStr(1, txtFileName, "\"))
    Exit Sub
    
UserCanceled:
    Err.Clear
End Sub


Private Sub btnCancel_Click()
    Me.Hide
End Sub


Private Sub btnOK_Click()
    If txtFileName = "" Then
        MsgBox "Invalid file name."
        Exit Sub
    End If
    
    If bTrascribing = True Then                         ' just want to change options
        TimerTranscript.Enabled = False
        TimerTranscript.Interval = txtInterval * 1000   ' in milliseconds
        TimerTranscript.Enabled = True
        Me.Hide
        Exit Sub
    End If
    
    If StartTranscript = False Then
        MsgBox "Can't start transcript."
        Exit Sub
    End If
    
    Me.Hide
End Sub



Private Sub Form_Load()
    nFile = -1      ' an invalid file number
    nNextStart = 0
    bTrascribing = False
    
    OpenDir = CurDir
    txtFileName = OpenDir & "\" & strDefaultFile
    
End Sub


Private Sub Form_Unload(Cancel As Integer)
    If bTrascribing = True Then
        StopTranscript
    End If
End Sub


Private Function StartTranscript() As Boolean
    nFile = FreeFile
    
    On Error GoTo ErrOpenFile
    Open txtFileName For Append Access Write As #nFile
    WriteToTranscriptFile "/------------- Transcript for " & strRoomName & _
                        " begins at: " & Now & " ------------\"
    
    nNextStart = 0
    bTrascribing = True
    
    TimerTranscript.Interval = txtInterval * 1000   ' in milliseconds
    TimerTranscript.Enabled = True
        
    ' don't allow changing file name once started
    txtFileName.Enabled = False
    btnBrowse.Enabled = False
    
    StartTranscript = True
    Exit Function
    
ErrOpenFile:
    StartTranscript = False
End Function


Private Sub TimerTranscript_Timer()
    If bTrascribing = False Then
        Exit Sub
    End If
    
    UpdateTranscript
End Sub




Private Sub WriteToTranscriptFile(ByRef strText As String)
    Print #nFile, strText
End Sub



Private Sub UpdateTranscript()
    Dim lLength As Long
    lLength = Len(rtbHistory.Text) - nNextStart
    
    If lLength = 0 Then     ' nothing to log
        Exit Sub
    End If
    
    rtbHistory.SelStart = nNextStart
    rtbHistory.SelLength = lLength
    WriteToTranscriptFile rtbHistory.SelText
    
    If chkResetHistory.Value = 1 Then
        rtbHistory.Text = ""
        nNextStart = 0
    Else
        nNextStart = nNextStart + lLength
    End If
End Sub


VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Transcript"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Dim nFile As Integer
Dim nNextStart As Integer
Dim bTrascribing As Boolean

Dim nInterval As Integer
Dim bResetHistory As Boolean

Dim ChatUI1 As ChatUI
Dim rtbHistory As RichTextBox
Dim Timer1 As New Timer


Public Sub StartTranscript(ByRef ChatUI As ChatUI, _
                           ByRef rtbHistory1 As RichTextBox, _
                           ByRef strFileName As String, _
                           Optional ByVal nInterval1 As Integer = 60, _
                           Optional ByVal bResetHistory1 As Boolean = False)

    nFile = FreeFile
    
    On Error GoTo ErrOpenFile
    Open TranscriptFile.Text For Append Access Write As #nFile
    Write #nLogFile, "/----------------- Transcript for " & ChatUI1.RoomName & " begins -----------------\"
    
    nNextStart = 0
    bTrascribing = True
    nInterval = nInterval1
    bResetHistory = bResetHistory1
    
    Set ChatUI1 = ChatUI1
    Set rtbHistory = rtbHistory1
    Timer1.Enabled = True
    Timer1.Interval = nInterval1
    
ErrOpenFile:

End Sub


Public Sub StopTranscript()
    Write #nLogFile, "\----------------- Transcript for " & ChatUI1.RoomName & " ends -----------------/"
    Close #nLogFile
    
    Set ChatUI1 = Nothing
    Set rtbHistory = Nothing
    Set Timer1 = Nothing
End Sub


Public Sub WriteToTranscriptFile(ByRef strText As String)
    If bTrascribing = False Then
        MsgBox "Transcript is not started."
    Exit Sub
    
    Write #nLogFile, strText
End Sub


Private Sub Timer1_Timer()
    CurTicks = CurTicks + 1
    If (CurTicks >= TimerTicks) Then
        CurTicks = 0
        Call Update_File(False)
    End If
End Sub

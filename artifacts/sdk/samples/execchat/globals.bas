'****************************************************************************
'
'  Module:     ExecChat.EXE
'  File:       Globals.bas
'
'  Copyright (c) Microsoft Corporation 1996-1997
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************
Attribute VB_Name = "Globals"
Option Explicit

Global Const MASK_PRIVILEGES = 1
Global Const MASK_IGNORED = 4

Global Const STAT_HOST = 1
Global Const STAT_PARTICIPANT = 2
Global Const STAT_SPECTATOR = 4
Global Const STAT_IGNORED = 16

Global Const RN_MODERATOR = 1
Global Const RN_GUEST = 2
Global Const RN_PARTICIPANT = 3

Global Const APP_TITLE = "Executive Chat Monitoring Tool"

Global Const UIOPT_HOST = 3596
Global Const UIOPT_MODERATOR = 3596
Global Const UIOPT_GUEST = 3865

Global Const MSCHAT_TOP = 2160
Global Const FUNCTIONS_TOP = 1440

Global nLogFile As Integer
Global nRealNameType As Integer
Global lModeratorID As Long
Global lGuestID As Long

Declare Function GetPrivateProfileStringA Lib "Kernel32" (ByVal lpApplicationName As String, lpKeyName As Any, ByVal lpDefault As String, ByVal lpReturnedString As String, ByVal nSize As Integer, ByVal lpFileName As String) As Integer

Sub CloseLogFile()
    On Error Resume Next
    WriteLogFile "End of Log"
    Close #nLogFile   ' Close file.
End Sub

Sub DisplayError(strProcedure As String)
    If (Err.Number < 1000 Or Err.Number > 1090) Then
        MsgBox Err.Description, vbOKOnly, "Error " + CStr(Err.Number) + " in " + strProcedure
    End If
    WriteLogFile Err.Description + ", Error " + CStr(Err.Number) + " in " + strProcedure
End Sub

Sub OpenLogFile()

    On Error Resume Next
    
    Dim nSuffix As Integer
    
    nLogFile = FreeFile   ' Get unused file number.
    nSuffix = 0
    
    Do
        Err.Clear
        Open "ExecChat" & CStr(nSuffix) & ".Log" For Output Lock Write As #nLogFile ' Create filename.
        nSuffix = nSuffix + 1
    Loop Until (Err.Number = 0 Or nSuffix > 32)
    
    If (Err.Number = 0) Then
        Write #nLogFile, "ExecChat Log File"  ' Output text.
    End If
End Sub

Function VBGetPrivateProfileString(sAppName$, sKeyName$, sDefault$, sFileName$) As String
Static sProfileBuf As String * 127
Dim nret%

  On Error GoTo errVBGetPrivateProfileString

 'nret% = GetPrivateProfileString(sAppName$, ByVal sKeyName$, sDefault$, sProfileBuf$, Len(sProfileBuf$), sIniDir$ + sFileName$)
  nret% = GetPrivateProfileStringA(sAppName$, ByVal sKeyName$, sDefault$, sProfileBuf$, Len(sProfileBuf$), sFileName$)
  If nret% = 0 Then
    VBGetPrivateProfileString = ""
  Else
    VBGetPrivateProfileString = Left$(sProfileBuf$, nret%)
  End If
  Exit Function
  
errVBGetPrivateProfileString:
    DisplayError "VBGetPrivateProfileString"
    VBGetPrivateProfileString = ""
End Function

Sub WriteLogFile(strLog As String)
    On Error Resume Next
    Write #nLogFile, strLog  ' Output text.
End Sub



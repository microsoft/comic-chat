Attribute VB_Name = "MainModule"
'****************************************************************************
'
'  Module:     Exec2.exe
'  File:       Module1.bas
'
'  Copyright (c) Microsoft Corporation 1996-1998
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************

Option Explicit

Public fMainForm As frmMain
Public fEvents As New frmEvents


Sub Main()
    Dim fLogin As New frmLogin
    Set fMainForm = New frmMain
    fMainForm.Show
    
    fLogin.bTryLogin = True
    fLogin.Show vbModal
    If Not fLogin.OK Then
        'User canceled
        Unload fMainForm
        Exit Sub
        End
    End If
    
    'fEvents.Show vbModeless
    
    fMainForm.UserType = fLogin.UserType

ExitMain:
    If Not Err = 0 Then
        MsgBox "Error: " & Err.Description
    End If
End Sub


Attribute VB_Name = "MainModule"
Option Explicit

Public fMainForm As frmMain
Public fEvents As New frmEvents


Sub Main()
    Dim fLogin As New frmLogin
    Set fMainForm = New frmMain
    
    fLogin.bTryLogin = True
    fLogin.Show vbModal
    If Not fLogin.OK Then
        'User canceled
        GoTo ExitMain
        End
    End If
    
    'fEvents.Show vbModeless
    
    fMainForm.UserType = fLogin.UserType
    fMainForm.Show vbModal

ExitMain:
    If Not Err = 0 Then
        MsgBox "Error: " & Err.Description
    End If
    Unload fLogin
    Unload fMainForm
    Unload fEvents
End Sub


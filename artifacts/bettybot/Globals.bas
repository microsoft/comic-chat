Attribute VB_Name = "Globals"
Option Explicit

'global constants
Global Const NUMUSERS = 16
Global Const NUMCHANNELS = 20
Global Const NUMMEMCHANNELS = 4

'global variables
Global rgstrNicknames(NUMUSERS) As String
Global rgstrChannels(NUMCHANNELS) As String
Global rgiMemberChannels(NUMUSERS, NUMMEMCHANNELS) As Integer
Global bDisconnecting As Boolean

Sub InitGlobals()

    On Error GoTo errInitGlobals
    
    bDisconnecting = False
    
    rgstrNicknames(0) = "Hank"
    rgstrNicknames(1) = "Sue"
    rgstrNicknames(2) = "Mark"
    rgstrNicknames(3) = "Mike"
    rgstrNicknames(4) = "Jay"
    rgstrNicknames(5) = "HarleyD"
    rgstrNicknames(6) = "JohnnyBGood"
    rgstrNicknames(7) = "JamesB"
    rgstrNicknames(8) = "SophieM"
    rgstrNicknames(9) = "EmmanuelleII"
    rgstrNicknames(10) = "ClaudiaS"
    rgstrNicknames(11) = "BrigitteB"
    rgstrNicknames(12) = "MilaJ"
    rgstrNicknames(13) = "KimB"
    rgstrNicknames(14) = "MicheleP"
    rgstrNicknames(15) = "Zorg"
    
    rgstrChannels(0) = "#ESPN;General"
    rgstrChannels(1) = "#ESPN;WebTours"
    rgstrChannels(2) = "#ESPN;NBA"
    rgstrChannels(3) = "#ESPN;NFL_Expansion"
    rgstrChannels(4) = "#ESPN;NHL"
    rgstrChannels(5) = "#ESPN;Canada"
    rgstrChannels(6) = "#ESPN;Violence"
    rgstrChannels(7) = "#ESPNMLB;Baseball"
    rgstrChannels(8) = "#ESPNMLB;Strike"
    rgstrChannels(9) = "#ESPN;Soccer"
    rgstrChannels(10) = "#ESPN;World_Cup"
    rgstrChannels(11) = "#ESPN;Brazil"
    rgstrChannels(12) = "#ESPN;Golf"
    rgstrChannels(13) = "#ESPN;Womens_Golf"
    rgstrChannels(14) = "#ESPN;Football"
    rgstrChannels(15) = "#ESPN;Superbowl"
    rgstrChannels(16) = "#ESPN;Tennis"
    rgstrChannels(17) = "#ESPN;Andre"
    rgstrChannels(18) = "#ESPN;Player_Behavior"
    rgstrChannels(19) = "#ESPNMLB;Cricket"
    
    rgiMemberChannels(0, 0) = 0
    rgiMemberChannels(0, 1) = 7
    rgiMemberChannels(0, 2) = 11
    rgiMemberChannels(0, 3) = 14
    rgiMemberChannels(1, 0) = 0
    rgiMemberChannels(1, 1) = 8
    rgiMemberChannels(1, 2) = 12
    rgiMemberChannels(1, 3) = 15
    rgiMemberChannels(2, 0) = 7
    rgiMemberChannels(2, 1) = 18
    rgiMemberChannels(2, 2) = 11
    rgiMemberChannels(2, 3) = 16
    rgiMemberChannels(3, 0) = 11
    rgiMemberChannels(3, 1) = 18
    rgiMemberChannels(3, 2) = 1
    rgiMemberChannels(3, 3) = 15
    rgiMemberChannels(4, 0) = 0
    rgiMemberChannels(4, 1) = 5
    rgiMemberChannels(4, 2) = 14
    rgiMemberChannels(4, 3) = 12
    rgiMemberChannels(5, 0) = 2
    rgiMemberChannels(5, 1) = 11
    rgiMemberChannels(5, 2) = 4
    rgiMemberChannels(5, 3) = 7
    rgiMemberChannels(6, 0) = 8
    rgiMemberChannels(6, 1) = 4
    rgiMemberChannels(6, 2) = 18
    rgiMemberChannels(6, 3) = 0
    rgiMemberChannels(7, 0) = 2
    rgiMemberChannels(7, 1) = 14
    rgiMemberChannels(7, 2) = 13
    rgiMemberChannels(7, 3) = 4
    rgiMemberChannels(8, 0) = 12
    rgiMemberChannels(8, 1) = 5
    rgiMemberChannels(8, 2) = 18
    rgiMemberChannels(8, 3) = 15
    rgiMemberChannels(9, 0) = 17
    rgiMemberChannels(9, 1) = 4
    rgiMemberChannels(9, 2) = 9
    rgiMemberChannels(9, 3) = 6
    rgiMemberChannels(10, 0) = 2
    rgiMemberChannels(10, 1) = 8
    rgiMemberChannels(10, 2) = 5
    rgiMemberChannels(10, 3) = 0
    rgiMemberChannels(11, 0) = 3
    rgiMemberChannels(11, 1) = 13
    rgiMemberChannels(11, 2) = 12
    rgiMemberChannels(11, 3) = 6
    rgiMemberChannels(12, 0) = 1
    rgiMemberChannels(12, 1) = 17
    rgiMemberChannels(12, 2) = 9
    rgiMemberChannels(12, 3) = 10
    rgiMemberChannels(13, 0) = 2
    rgiMemberChannels(13, 1) = 12
    rgiMemberChannels(13, 2) = 11
    rgiMemberChannels(13, 3) = 3
    rgiMemberChannels(14, 0) = 1
    rgiMemberChannels(14, 1) = 7
    rgiMemberChannels(14, 2) = 2
    rgiMemberChannels(14, 3) = 11
    rgiMemberChannels(15, 0) = 13
    rgiMemberChannels(15, 1) = 10
    rgiMemberChannels(15, 2) = 6
    rgiMemberChannels(15, 3) = 3
    
    Exit Sub

errInitGlobals:
    DisplayError "InitGlobals"
End Sub

Sub DisplayError(strCallerName As String)
    Dim strError As String
    
    strError = "Betty Bot Error " + CStr(Err.Number) + " in " + strCallerName + ": " + Err.Description
    Debug.Print strError
    
    Betty.lstEvents.AddItem strError, 0
    Err.Clear
End Sub

Function StrConnectionNameFromState(ConnectionState As MsChatPrCtl.enumConnectionState) As String
    Select Case ConnectionState
        Case csDisconnected:
            StrConnectionNameFromState = "Disconnected"
        Case csConnecting:
            StrConnectionNameFromState = "Connecting..."
        Case csConnected:
            StrConnectionNameFromState = "Connected"
        Case csLogging:
            StrConnectionNameFromState = "Logging..."
        Case csLogged:
            StrConnectionNameFromState = "Logged"
        Case csDisconnecting:
            StrConnectionNameFromState = "Disconnecting..."
    End Select
End Function

Function StrChannelNameFromState(ChannelState As MsChatPrCtl.enumChannelState) As String
    Select Case ChannelState
        Case chsClosed:
            StrChannelNameFromState = "Closed"
        Case chsOpening:
            StrChannelNameFromState = "Opening..."
        Case chsOpen:
            StrChannelNameFromState = "Open"
    End Select
End Function

Sub ReleaseChannelObjects()
    On Error GoTo errReleaseChannelObjects
    Dim i, j, jmax As Integer
    
    For i = 0 To NUMUSERS - 1
        jmax = Betty.MsChatPr(i).Channels.Count
        For j = 1 To jmax
            Betty.MsChatPr(i).Channels.Remove 1
        Next j
    Next i
    Exit Sub
    
errReleaseChannelObjects:
    DisplayError "ReleaseChannelObjects"
Exit Sub

End Sub

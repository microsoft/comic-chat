VERSION 5.00
Object = "{9FE8A900-96AC-11CF-9086-00AA006C3FBC}#2.0#0"; "MSCHATPR.OCX"
Begin VB.Form Betty 
   Caption         =   "Betty Bot"
   ClientHeight    =   5085
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   7845
   Icon            =   "Betty.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   5085
   ScaleWidth      =   7845
   StartUpPosition =   3  'Windows Default
   Begin VB.Timer Timer 
      Enabled         =   0   'False
      Index           =   12
      Interval        =   13000
      Left            =   6000
      Top             =   5640
   End
   Begin VB.Timer Timer 
      Enabled         =   0   'False
      Index           =   11
      Interval        =   13000
      Left            =   5520
      Top             =   5640
   End
   Begin VB.Timer Timer 
      Enabled         =   0   'False
      Index           =   10
      Interval        =   2000
      Left            =   5040
      Top             =   5640
   End
   Begin VB.Timer Timer 
      Enabled         =   0   'False
      Index           =   9
      Interval        =   2000
      Left            =   4560
      Top             =   5640
   End
   Begin VB.Timer Timer 
      Index           =   8
      Interval        =   6000
      Left            =   4080
      Top             =   5640
   End
   Begin VB.Timer Timer 
      Enabled         =   0   'False
      Index           =   7
      Interval        =   11000
      Left            =   3600
      Top             =   9840
   End
   Begin VB.Timer Timer 
      Enabled         =   0   'False
      Index           =   6
      Interval        =   3500
      Left            =   3120
      Top             =   5640
   End
   Begin VB.Timer Timer 
      Enabled         =   0   'False
      Index           =   5
      Interval        =   1000
      Left            =   2640
      Top             =   5640
   End
   Begin VB.Timer Timer 
      Enabled         =   0   'False
      Index           =   4
      Interval        =   1500
      Left            =   2160
      Top             =   5640
   End
   Begin VB.Timer Timer 
      Enabled         =   0   'False
      Index           =   3
      Interval        =   1000
      Left            =   1680
      Top             =   5640
   End
   Begin VB.Timer Timer 
      Enabled         =   0   'False
      Index           =   2
      Interval        =   8500
      Left            =   1200
      Top             =   5640
   End
   Begin VB.Timer Timer 
      Enabled         =   0   'False
      Index           =   1
      Interval        =   6000
      Left            =   720
      Top             =   5640
   End
   Begin VB.Timer Timer 
      Enabled         =   0   'False
      Index           =   0
      Interval        =   500
      Left            =   240
      Top             =   5640
   End
   Begin VB.ListBox lstEvents 
      Height          =   3765
      Left            =   120
      TabIndex        =   6
      Top             =   1200
      Width           =   7575
   End
   Begin VB.CommandButton cmdStopBot 
      Caption         =   "Sto&p Bot"
      Height          =   375
      Left            =   1560
      TabIndex        =   5
      Top             =   120
      Width           =   1335
   End
   Begin VB.TextBox txtDemoerNick 
      Height          =   285
      Left            =   5040
      TabIndex        =   4
      Text            =   "djk"
      Top             =   600
      Width           =   1695
   End
   Begin VB.TextBox txtServerName 
      Height          =   285
      Left            =   5040
      TabIndex        =   1
      Text            =   "chloe1"
      Top             =   120
      Width           =   2655
   End
   Begin VB.CommandButton cmdStartBot 
      Caption         =   "&Start Bot"
      Height          =   375
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   1335
   End
   Begin VB.Label lblEventLog 
      Caption         =   "Events Log:"
      Height          =   195
      Left            =   120
      TabIndex        =   7
      Top             =   900
      Width           =   2055
   End
   Begin VB.Label lblDemoerNick 
      Caption         =   "Demoer Nickname:"
      Height          =   255
      Left            =   3360
      TabIndex        =   3
      Top             =   600
      Width           =   1455
   End
   Begin VB.Label lblServerName 
      Caption         =   "Server Name:"
      Height          =   255
      Left            =   3360
      TabIndex        =   2
      Top             =   120
      Width           =   1095
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr 
      Index           =   15
      Left            =   7320
      OleObjectBlob   =   "Betty.frx":0442
      Top             =   5160
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr 
      Index           =   14
      Left            =   6840
      OleObjectBlob   =   "Betty.frx":0466
      Top             =   5160
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr 
      Index           =   13
      Left            =   6360
      OleObjectBlob   =   "Betty.frx":048A
      Top             =   5160
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr 
      Index           =   12
      Left            =   5880
      OleObjectBlob   =   "Betty.frx":04AE
      Top             =   5160
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr 
      Index           =   11
      Left            =   5400
      OleObjectBlob   =   "Betty.frx":04D2
      Top             =   5160
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr 
      Index           =   10
      Left            =   4920
      OleObjectBlob   =   "Betty.frx":04F6
      Top             =   5160
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr 
      Index           =   9
      Left            =   4440
      OleObjectBlob   =   "Betty.frx":051A
      Top             =   5160
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr 
      Index           =   8
      Left            =   3960
      OleObjectBlob   =   "Betty.frx":053E
      Top             =   5160
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr 
      Index           =   7
      Left            =   3480
      OleObjectBlob   =   "Betty.frx":0562
      Top             =   5160
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr 
      Index           =   6
      Left            =   3000
      OleObjectBlob   =   "Betty.frx":0586
      Top             =   5160
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr 
      Index           =   5
      Left            =   2520
      OleObjectBlob   =   "Betty.frx":05AA
      Top             =   5160
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr 
      Index           =   4
      Left            =   2040
      OleObjectBlob   =   "Betty.frx":05CE
      Top             =   5160
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr 
      Index           =   3
      Left            =   1560
      OleObjectBlob   =   "Betty.frx":05F2
      Top             =   5160
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr 
      Index           =   2
      Left            =   1080
      OleObjectBlob   =   "Betty.frx":0616
      Top             =   5160
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr 
      Index           =   1
      Left            =   600
      OleObjectBlob   =   "Betty.frx":063A
      Top             =   5160
   End
   Begin MsChatPrCtl.MsChatPr MsChatPr 
      Index           =   0
      Left            =   120
      OleObjectBlob   =   "Betty.frx":065E
      Top             =   5160
   End
End
Attribute VB_Name = "Betty"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Option Compare Text

Private Sub cmdStartBot_Click()
    Err.Clear
    bDisconnecting = False
    ReleaseChannelObjects
    ConnectControl 0
End Sub

Private Sub cmdStopBot_Click()
    bDisconnecting = True
    DisconnectControl 0
End Sub

Private Sub Form_Load()
    InitGlobals
End Sub

Sub ConnectControl(Index As Integer)

    On Error GoTo errConnectControl
    
    Dim i As Integer
    
    i = Index
    
    Do While (i < NUMUSERS)
        If (MsChatPr(i).ConnectionState = csDisconnected) Then
            MsChatPr(i).Login txtServerName.Text, rgstrNicknames(Index)
            Exit Do
        Else
            lstEvents.AddItem "Control #" + CStr(i) + " is not disconnected."
        End If
        i = i + 1
    Loop
    Exit Sub
    
errConnectControl:
    DisplayError "ConnectControl"
End Sub

Sub DisconnectControl(Index As Integer)
    On Error GoTo errDisconnectControl
    
    Dim i As Integer
    
    i = Index
    
    Do While (i < NUMUSERS)
        Select Case MsChatPr(i).ConnectionState
            Case csDisconnected, csDisconnecting:   'do nothing
            Case csConnecting, csLogging:
                MsChatPr(i).CancelConnectLogin
                Exit Do
            Case csConnected, csLogged:
                MsChatPr(i).Disconnect
                Exit Do
        End Select
        i = i + 1
    Loop
    Exit Sub
    
errDisconnectControl:
    DisplayError "DisconnectControl"
End Sub

Private Sub MsChatPr_OnAddMember(Index As Integer, ByVal Channel As MsChatPrCtl.Channel, ByVal Nickname As String, ByVal MemberItems As MsChatPrCtl.ChatItems)
    On Error GoTo errOnAddMember
    
    If (Index = 0 And Channel.ChannelName = rgstrChannels(0) And Nickname = txtDemoerNick.Text) Then
        'demoer joined #ESPN-GENERAL
        Timer(0).Enabled = True
        Timer(1).Enabled = True
        Timer(2).Enabled = True
        Debug.Print Nickname + " joined " + Channel.ChannelName + " --> Enabled timer #0 + #1 + #2"
    End If
    
    If (Index = 3 And Channel.ChannelName = rgstrChannels(1) And Nickname = txtDemoerNick.Text) Then
        'demoer joined #ESPN-WEBTOUR
        Timer(9).Enabled = True
        Debug.Print Nickname + " joined " + Channel.ChannelName + " --> Enabled timer #9"
    End If
    Exit Sub
    
errOnAddMember:
    DisplayError "OnAddMember"
End Sub

Private Sub MsChatPr_OnChannelError(Index As Integer, ByVal Channel As MsChatPrCtl.Channel, ByVal ErrorCode As Long, ByVal Description As String)
    On Error GoTo errOnChannelError
    lstEvents.AddItem "Control #" + CStr(Index) + " channel " + Channel.ChannelName + " error (" + CStr(ErrorCode) + ")=" + Description, 0
    Exit Sub
    
errOnChannelError:
    DisplayError "OnConnectionState"
End Sub

Private Sub MsChatPr_OnChannelState(Index As Integer, ByVal Channel As MsChatPrCtl.Channel, ByVal NewChannelState As MsChatPrCtl.enumChannelState)
    
    On Error GoTo errOnChannelState
    lstEvents.AddItem "Control #" + CStr(Index) + " channel " + Channel.ChannelName + " state=" + StrChannelNameFromState(NewChannelState), 0
    
    If (Index = 1 And NewChannelState = chsOpen And Channel.ChannelName = rgstrChannels(19)) Then
        'Sue: Who do you pick for the cricket national champoinships?
        Channel.SendMessage msgtNormal, "Who do you pick for the cricket national champoinships?"
        Timer(8).Enabled = False
    End If
    Exit Sub
    
errOnChannelState:
    DisplayError "OnChannelState"
End Sub

Private Sub MsChatPr_OnConnectionError(Index As Integer, ByVal ErrorCode As Long, ByVal Description As String, ByVal ChannelName As Variant)

    On Error GoTo errOnConnectionError
    lstEvents.AddItem "Control #" + CStr(Index) + " connection error (" + CStr(ErrorCode) + ")=" + Description, 0
    Exit Sub
    
errOnConnectionError:
    DisplayError "OnConnectionState"
End Sub

Private Sub MsChatPr_OnConnectionState(Index As Integer, ByVal NewConnectionState As MsChatPrCtl.enumConnectionState)

    On Error GoTo errOnConnectionState
    lstEvents.AddItem "Control #" + CStr(Index) + " connected state=" + StrConnectionNameFromState(NewConnectionState), 0
    
    Dim i As Integer
    Dim chan As Channel
    
    If (NewConnectionState = csLogged) Then
        For i = 0 To NUMMEMCHANNELS - 1
            Set chan = MsChatPr(Index).Channels.Add
            chan.JoinChannel rgstrChannels(rgiMemberChannels(Index, i))
            Set chan = Nothing
        Next i
    End If
    
    If (Index < NUMUSERS - 1) Then
        If (Not bDisconnecting And NewConnectionState = csLogged) Then
            ConnectControl Index + 1
        Else
            If (bDisconnecting And NewConnectionState = csDisconnected) Then
                DisconnectControl Index + 1
            End If
        End If
    End If
    Exit Sub
    
errOnConnectionState:
    DisplayError "OnConnectionState"
End Sub

Private Sub MsChatPr_OnDelMember(Index As Integer, ByVal Channel As MsChatPrCtl.Channel, ByVal Nickname As String)
    On Error GoTo errOnDelMember
    If (Index = 1 And Channel.ChannelName = "#ESPNMLB;CRICKET" And Nickname = txtDemoerNick.Text) Then
        Channel.LeaveChannel
        Timer(8).Enabled = True
    End If
    Exit Sub
    
errOnDelMember:
    DisplayError "OnDelMember"
End Sub

Private Sub MsChatPr_OnMessage(Index As Integer, ByVal Channel As MsChatPrCtl.Channel, ByVal SenderNickname As String, ByVal MessageType As Long, ByVal Message As Variant, ByVal RecipientNicknames As Variant, ByVal DataMessageTag As Variant)
    On Error GoTo errOnMessage
    
    If (Index = 1 And MessageType = msgtNormal) Then
        If (SenderNickname = txtDemoerNick.Text) Then
            If (Channel.ChannelName = rgstrChannels(0)) Then
                If (0 <> InStr(1, CStr(Message), "Hi ", vbTextCompare)) Then
                    'Sue got a message from demoer, saying Hi everyone
                    Timer(3).Enabled = True
                    Debug.Print SenderNickname + " sent " + CStr(Message) + " --> Enabled timer #3"
                    Exit Sub
                End If
            End If
        End If
    End If
    
    If (Index = 0 And MessageType = msgtNormal) Then
        If (SenderNickname = txtDemoerNick.Text) Then
            If (Channel.ChannelName = rgstrChannels(0)) Then
                If (0 <> InStr(1, CStr(Message), "How can I ", vbTextCompare)) Then
                    'Hank got a message from demoer, saying How can I find more about Basketball?
                    Timer(4).Enabled = True
                    Debug.Print SenderNickname + " sent " + CStr(Message) + " --> Enabled timer #4"
                    Exit Sub
                End If
            End If
        End If
    End If
    
    If (Index = 2 And MessageType = msgtNormal) Then
        If (SenderNickname = txtDemoerNick.Text) Then
            If (Channel.ChannelName = rgstrChannels(18)) Then
                If (0 <> InStr(1, CStr(Message), "What do you think ", vbTextCompare)) Then
                    'Mark got a message from demoer, saying What do you think about Sprewell's Behavior?
                    Timer(5).Enabled = True
                    Timer(6).Enabled = True
                    Timer(7).Enabled = True
                    Debug.Print SenderNickname + " sent " + CStr(Message) + " --> Enabled timer #5 + #6 + #7"
                    Exit Sub
                End If
            End If
        End If
    End If
    
    If (Index = 3 And MessageType = msgtNormal) Then
        If (SenderNickname = txtDemoerNick.Text) Then
            If (Channel.ChannelName = rgstrChannels(1)) Then
                If (0 <> InStr(1, CStr(Message), "Sure", vbTextCompare)) Then
                    'Mike got a message from demoer, saying Sure
                    Debug.Print SenderNickname + " sent " + CStr(Message) + " --> Start WEB Tour"
                    Channel.SendMessage msgtCTCP, "TOUR START", SenderNickname
                    Exit Sub
                End If
            End If
        End If
    End If
    
    Exit Sub
    
errOnMessage:
    DisplayError "OnMessage"
End Sub

Private Sub MsChatPr_OnPrivateMessage(Index As Integer, ByVal SenderNickname As String, ByVal PrivateMessageType As Long, ByVal Message As Variant, ByVal DataMessageTag As Variant)
    On Error GoTo errOnPrivateMessage
    
    If (Index = 3 And PrivateMessageType = pmtNotice + pmtCTCP) Then
        If (SenderNickname = txtDemoerNick.Text) Then
            If (0 <> InStr(1, CStr(Message), "TOUR", vbTextCompare)) Then
                'Mike got tour acceptance notice
                Timer(10).Enabled = True
                Debug.Print SenderNickname + " sent " + CStr(Message) + " --> Enabled timer #10"
            End If
        End If
    End If
    Exit Sub
    
errOnPrivateMessage:
    DisplayError "OnPrivateMessage"
End Sub

Private Sub Timer_Timer(Index As Integer)
    On Error GoTo errTimer
    
    If (Index <> 8) Then
        Timer(Index).Enabled = False
        Debug.Print "Disabled timer #" + CStr(Index)
    End If
    
    Select Case Index
    Case 0:
        'Hank: ESPN is the best. It's got Sports Center, NBA Today, and Up Close.
        MsChatPr(0).Channels(1).SendMessage msgtNormal, "ESPN is the best. It's got Sports Center, NBA Today, and Up Close."
    Case 1:
        'Jay: Sure, but ESPN2 has Fitness Beach.
        MsChatPr(4).Channels(1).SendMessage msgtNormal, "Sure, but ESPN2 has Fitness Beach."
    Case 2:
        'Hank: True."
        MsChatPr(0).Channels(1).SendMessage msgtNormal, "True."
    Case 3:
        'Sue: Hi + demoer nickname
        MsChatPr(1).Channels(1).SendMessage msgtNormal, "Hi " + txtDemoerNick.Text
    Case 4:
        'Hank: Go to the NBA pages. Just click here: http://espn.sprotszone.com/nba/index.html.
        MsChatPr(0).Channels(1).SendMessage msgtNormal, "Go to the NBA pages. Just click here: http://espn.sportszone.com/nba/index.html."
    Case 5:
        'Mark: He should be cained.
        MsChatPr(2).Channels(2).SendMessage msgtNormal, "He should be cained."
    Case 6:
        'Mike: Didn't he settle recently? Hardly a victory for the NBA and sportsmanship in general.
        MsChatPr(3).Channels(2).SendMessage msgtNormal, "Didn't he settle recently? Hardly a victory for the NBA and sportsmanship in general."
    Case 7:
        'Mark: demoer nickname - I'm going to add you to my buddy list.
        MsChatPr(2).Channels(2).SendMessage msgtNormal, txtDemoerNick.Text + " - I'm going to add you to my buddy list."
        MsChatPr(2).SendPrivateMessage pmtCTCP, "BuddyList", txtDemoerNick.Text
    Case 8:
        'was room #ESPN-MLB-CRICKET created already?
        If (MsChatPr(4).ConnectionState = csLogged) Then
            Dim ci As ChatItems
            Dim chan As Channel
            Set ci = MsChatPr(4).ChannelProperty("Name", rgstrChannels(19))
            Set ci = Nothing
            Set chan = MsChatPr(1).Channels.Add
            chan.JoinChannel rgstrChannels(19)
            Set chan = Nothing
        End If
    Case 9:
        'Mike: demoer nickname - do you want to join our web tour of all the Sportszone highlights?
        MsChatPr(3).Channels(3).SendMessage msgtNormal, txtDemoerNick.Text + " - do you want to join our web tour of all the Sportszone highlights?"
    Case 10:
        'Mike: Sportszone has the best Soccer coverage on the web, as you can see here.
        MsChatPr(3).Channels(3).SendMessage msgtNormal, "Sportszone has the best Soccer coverage on the web, as you can see here."
        MsChatPr(3).Channels(3).SendMessage msgtCTCP, "GUIDE HTTP://ESPN.SPORTSZONE.COM/soccer/index.html"
        Timer(11).Enabled = True
    Case 11:
        'Mike: Up to the minute soccer scores are available on this page.
        MsChatPr(3).Channels(3).SendMessage msgtNormal, "Up to the minute soccer scores are available on this page."
        MsChatPr(3).Channels(3).SendMessage msgtCTCP, "GUIDE HTTP://ESPN.SPORTSZONE.COM/soccer/mls/today/sco.html"
        Timer(12).Enabled = True
    Case 12:
        'Mike: Our Tennis coverage wins numerous awards.
        MsChatPr(3).Channels(3).SendMessage msgtNormal, "Our Tennis coverage wins numerous awards."
        MsChatPr(3).Channels(3).SendMessage msgtCTCP, "GUIDE HTTP://ESPN.SPORTSZONE.COM/ten/index.html"
    End Select
    Exit Sub
    
errTimer:
    If (Err.Number <> 3076) Then
        DisplayError "Timer"
    Else
        Err.Clear
    End If
End Sub

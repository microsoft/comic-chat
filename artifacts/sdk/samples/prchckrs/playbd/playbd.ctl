VERSION 5.00
Begin VB.UserControl PlayBoard 
   BackColor       =   &H00C0E0FF&
   ClientHeight    =   1620
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   6975
   ScaleHeight     =   1620
   ScaleWidth      =   6975
   Begin VB.Shape WhiteCheck 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H00FFFFFF&
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   11
      Left            =   4920
      Shape           =   2  'Oval
      Top             =   480
      Width           =   255
   End
   Begin VB.Shape WhiteCheck 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H00FFFFFF&
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   10
      Left            =   4560
      Shape           =   2  'Oval
      Top             =   480
      Width           =   255
   End
   Begin VB.Shape WhiteCheck 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H00FFFFFF&
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   9
      Left            =   4200
      Shape           =   2  'Oval
      Top             =   480
      Width           =   255
   End
   Begin VB.Shape WhiteCheck 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H00FFFFFF&
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   8
      Left            =   3840
      Shape           =   2  'Oval
      Top             =   480
      Width           =   255
   End
   Begin VB.Shape WhiteCheck 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H00FFFFFF&
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   7
      Left            =   6480
      Shape           =   2  'Oval
      Top             =   120
      Width           =   255
   End
   Begin VB.Shape WhiteCheck 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H00FFFFFF&
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   6
      Left            =   6120
      Shape           =   2  'Oval
      Top             =   120
      Width           =   255
   End
   Begin VB.Shape WhiteCheck 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H00FFFFFF&
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   5
      Left            =   5760
      Shape           =   2  'Oval
      Top             =   120
      Width           =   255
   End
   Begin VB.Shape WhiteCheck 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H00FFFFFF&
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   4
      Left            =   5400
      Shape           =   2  'Oval
      Top             =   120
      Width           =   255
   End
   Begin VB.Shape WhiteCheck 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H00FFFFFF&
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   3
      Left            =   4920
      Shape           =   2  'Oval
      Top             =   120
      Width           =   255
   End
   Begin VB.Shape WhiteCheck 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H00FFFFFF&
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   2
      Left            =   4560
      Shape           =   2  'Oval
      Top             =   120
      Width           =   255
   End
   Begin VB.Shape WhiteCheck 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H00FFFFFF&
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   1
      Left            =   4200
      Shape           =   2  'Oval
      Top             =   120
      Width           =   255
   End
   Begin VB.Shape WhiteCheck 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H00FFFFFF&
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   0
      Left            =   3840
      Shape           =   2  'Oval
      Top             =   120
      Width           =   255
   End
   Begin VB.Shape BlackCheck 
      BorderStyle     =   0  'Transparent
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   11
      Left            =   4920
      Shape           =   2  'Oval
      Top             =   1200
      Width           =   255
   End
   Begin VB.Shape BlackCheck 
      BorderStyle     =   0  'Transparent
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   10
      Left            =   4560
      Shape           =   2  'Oval
      Top             =   1200
      Width           =   255
   End
   Begin VB.Shape BlackCheck 
      BorderStyle     =   0  'Transparent
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   9
      Left            =   4200
      Shape           =   2  'Oval
      Top             =   1200
      Width           =   255
   End
   Begin VB.Shape BlackCheck 
      BorderStyle     =   0  'Transparent
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   8
      Left            =   3840
      Shape           =   2  'Oval
      Top             =   1200
      Width           =   255
   End
   Begin VB.Shape BlackCheck 
      BorderStyle     =   0  'Transparent
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   7
      Left            =   6480
      Shape           =   2  'Oval
      Top             =   840
      Width           =   255
   End
   Begin VB.Shape BlackCheck 
      BorderStyle     =   0  'Transparent
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   6
      Left            =   6120
      Shape           =   2  'Oval
      Top             =   840
      Width           =   255
   End
   Begin VB.Shape BlackCheck 
      BorderStyle     =   0  'Transparent
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   5
      Left            =   5760
      Shape           =   2  'Oval
      Top             =   840
      Width           =   255
   End
   Begin VB.Shape BlackCheck 
      BorderStyle     =   0  'Transparent
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   4
      Left            =   5400
      Shape           =   2  'Oval
      Top             =   840
      Width           =   255
   End
   Begin VB.Shape BlackCheck 
      BorderStyle     =   0  'Transparent
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   3
      Left            =   4920
      Shape           =   2  'Oval
      Top             =   840
      Width           =   255
   End
   Begin VB.Shape BlackCheck 
      BorderStyle     =   0  'Transparent
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   2
      Left            =   4560
      Shape           =   2  'Oval
      Top             =   840
      Width           =   255
   End
   Begin VB.Shape BlackCheck 
      BorderStyle     =   0  'Transparent
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   1
      Left            =   4200
      Shape           =   2  'Oval
      Top             =   840
      Width           =   255
   End
   Begin VB.Shape BlackCheck 
      BorderStyle     =   0  'Transparent
      FillStyle       =   0  'Solid
      Height          =   255
      Index           =   0
      Left            =   3840
      Shape           =   2  'Oval
      Top             =   840
      Width           =   255
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   31
      Left            =   120
      Top             =   1200
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   30
      Left            =   600
      Top             =   1200
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   29
      Left            =   1080
      Top             =   1200
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   28
      Left            =   1560
      Top             =   1200
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   27
      Left            =   2040
      Top             =   1200
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   26
      Left            =   2520
      Top             =   1200
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   25
      Left            =   3000
      Top             =   1200
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   24
      Left            =   3480
      Top             =   1200
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   23
      Left            =   120
      Top             =   840
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   22
      Left            =   600
      Top             =   840
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   21
      Left            =   1080
      Top             =   840
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   20
      Left            =   1560
      Top             =   840
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   19
      Left            =   2040
      Top             =   840
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   18
      Left            =   2520
      Top             =   840
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   17
      Left            =   3000
      Top             =   840
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   16
      Left            =   3480
      Top             =   840
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   15
      Left            =   120
      Top             =   480
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   14
      Left            =   600
      Top             =   480
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   13
      Left            =   1080
      Top             =   480
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   12
      Left            =   1560
      Top             =   480
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   11
      Left            =   2040
      Top             =   480
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   10
      Left            =   2520
      Top             =   480
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   9
      Left            =   3000
      Top             =   480
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   8
      Left            =   3480
      Top             =   480
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   7
      Left            =   3480
      Top             =   120
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   6
      Left            =   3000
      Top             =   120
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   5
      Left            =   2520
      Top             =   120
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   4
      Left            =   2040
      Top             =   120
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   3
      Left            =   1560
      Top             =   120
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   2
      Left            =   1080
      Top             =   120
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   1
      Left            =   600
      Top             =   120
      Width           =   195
   End
   Begin VB.Shape BlackSquare 
      BorderStyle     =   0  'Transparent
      FillColor       =   &H000000C0&
      FillStyle       =   0  'Solid
      Height          =   195
      Index           =   0
      Left            =   120
      Top             =   120
      Width           =   195
   End
End
Attribute VB_Name = "PlayBoard"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit

Dim g_nSize As Integer
Dim g_nCheckerCount As Integer
Dim g_nSquareCount As Integer
Dim g_nCheckerRadiusX As Integer
Dim g_nCheckerRadiusY As Integer
Dim g_iWCLocked As Integer
Dim g_iBCLocked As Integer
Dim g_nDeltaX As Integer
Dim g_nDeltaY As Integer
Dim g_nXO As Integer
Dim g_nYO As Integer
Public Event OnCheckerMoved(FromMoveCheckerCall As Boolean, XO As Integer, YO As Integer, XD As Integer, YD As Integer)


Private Sub UserControl_Initialize()
    Debug.Print "UserControl_Initialize"
    g_iWCLocked = -1
    g_iBCLocked = -1
    g_nSize = 4
    ConstructBoard
    ResetCheckerPositions
End Sub

Public Property Get Size() As Integer
    Size = g_nSize
End Property

Public Property Let Size(ByVal nNewSize As Integer)
    If (nNewSize >= 4 And nNewSize <= 8 And nNewSize <> g_nSize And (nNewSize Mod 2) = 0) Then
        ' Update the prop value
        g_nSize = nNewSize
        PropertyChanged "Size"
        ' Redraw the whole board
        ConstructBoard
        'Reposition the checkers
        ResetCheckerPositions
End If
End Property

Private Sub ConstructBoard()
    Dim i As Integer
    Debug.Print "ConstructBoard"
    g_nCheckerCount = (g_nSize - 2) * g_nSize / 4
    g_nSquareCount = (g_nSize * g_nSize / 2)
    'Visibility and sizing
    For i = 0 To g_nSquareCount - 1
        BlackSquare(i).Visible = True
        BlackSquare(i).Width = Width \ (g_nSize * 3 / 2)
        BlackSquare(i).Height = Height \ g_nSize
    Next i
    For i = g_nSquareCount To 31
        BlackSquare(i).Visible = False
    Next i
    For i = 0 To g_nCheckerCount - 1
        WhiteCheck(i).Visible = True
        BlackCheck(i).Visible = True
        WhiteCheck(i).Width = BlackSquare(0).Width / 2
        BlackCheck(i).Width = BlackSquare(0).Width / 2
        WhiteCheck(i).Height = BlackSquare(0).Height / 2
        BlackCheck(i).Height = BlackSquare(0).Height / 2
    Next i
    For i = g_nCheckerCount To 11
        WhiteCheck(i).Visible = False
        BlackCheck(i).Visible = False
    Next i
    
    'Positioning
    For i = 0 To g_nSquareCount - 1
        BlackSquare(i).Left = (2 * (i Mod (g_nSize / 2)) + (((2 * i) \ g_nSize + 1) Mod 2)) * BlackSquare(0).Width
        BlackSquare(i).Top = (2 * i \ g_nSize) * BlackSquare(0).Height
        'Debug.Print "BlackSquare(" + CStr(i) + ")=(" + CStr(BlackSquare(i).Left) + "," + CStr(BlackSquare(i).Top) + ")"
    Next i
    g_nCheckerRadiusX = WhiteCheck(0).Width / 2
    g_nCheckerRadiusY = WhiteCheck(0).Height / 2
End Sub

Private Sub UserControl_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    ' try to lock a checker
    If (LockWhiteChecker(CInt(X), CInt(Y)) = -1) Then
        LockBlackChecker CInt(X), CInt(Y)
    End If
End Sub

Private Sub UserControl_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
    If g_iWCLocked <> -1 Then
        WhiteCheck(g_iWCLocked).Left = X + g_nDeltaX
        WhiteCheck(g_iWCLocked).Top = Y + g_nDeltaY
    Else
        If g_iBCLocked <> -1 Then
            BlackCheck(g_iBCLocked).Left = X + g_nDeltaX
            BlackCheck(g_iBCLocked).Top = Y + g_nDeltaY
        End If
    End If
End Sub

Private Sub UserControl_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    If g_iWCLocked <> -1 Then
        WhiteCheck(g_iWCLocked).Left = ClosestX(CInt(WhiteCheck(g_iWCLocked).Left))
        WhiteCheck(g_iWCLocked).Top = ClosestY(CInt(WhiteCheck(g_iWCLocked).Top))
        If BigX2SmallX(WhiteCheck(g_iWCLocked).Left + 10) <> g_nXO Or BigY2SmallY(WhiteCheck(g_iWCLocked).Top + 10) <> g_nYO Then
            RaiseEvent OnCheckerMoved(False, g_nXO, g_nYO, BigX2SmallX(WhiteCheck(g_iWCLocked).Left + 10), BigY2SmallY(WhiteCheck(g_iWCLocked).Top + 10))
        End If
        g_iWCLocked = -1
    Else
        If g_iBCLocked <> -1 Then
            BlackCheck(g_iBCLocked).Left = ClosestX(CInt(BlackCheck(g_iBCLocked).Left))
            BlackCheck(g_iBCLocked).Top = ClosestY(CInt(BlackCheck(g_iBCLocked).Top))
            If BigX2SmallX(BlackCheck(g_iBCLocked).Left + 10) <> g_nXO Or BigY2SmallY(BlackCheck(g_iBCLocked).Top + 10) <> g_nYO Then
                RaiseEvent OnCheckerMoved(False, g_nXO, g_nYO, BigX2SmallX(BlackCheck(g_iBCLocked).Left + 10), BigY2SmallY(BlackCheck(g_iBCLocked).Top + 10))
            End If
            g_iBCLocked = -1
        End If
    End If
End Sub

Private Sub UserControl_Resize()
    ConstructBoard
    ResetCheckerPositions
End Sub

Private Sub ResetCheckerPositions()
    Dim i As Integer
    Debug.Print "ResetCheckerPositions"
    For i = 0 To g_nCheckerCount - 1
        WhiteCheck(i).Left = BlackSquare(i).Left + (BlackSquare(0).Width - WhiteCheck(0).Width) / 2
        WhiteCheck(i).Top = BlackSquare(i).Top + (BlackSquare(0).Height - WhiteCheck(0).Height) / 2
        BlackCheck(i).Left = BlackSquare(g_nSquareCount - 1 - i).Left + (BlackSquare(0).Width - BlackCheck(0).Width) / 2
        BlackCheck(i).Top = BlackSquare(g_nSquareCount - 1 - i).Top + (BlackSquare(0).Height - BlackCheck(0).Height) / 2
    Next i
End Sub

Public Sub MoveChecker(XO As Integer, YO As Integer, XD As Integer, YD As Integer)
    
    Dim i, iWCO, iBCO As Integer
    
    iWCO = -1
    iBCO = -1
    
    'check if initial point is a white checker
    i = 0
    Do
        If BigX2SmallX(WhiteCheck(i).Left + 10) = XO And BigY2SmallY(WhiteCheck(i).Top + 10) = YO Then
            iWCO = i
            Exit Do
        End If
        i = i + 1
    Loop While (i < g_nCheckerCount)
    
    If iWCO = -1 Then
        'check if the initial point is a black checker
        i = 0
        Do
            If BigX2SmallX(BlackCheck(i).Left + 10) = XO And BigY2SmallY(BlackCheck(i).Top + 10) = YO Then
                iBCO = i
                Exit Do
            End If
            i = i + 1
        Loop While (i < g_nCheckerCount)
    End If
    
    If (-1 = iWCO And -1 = iBCO) Then
        'initial point is not a checker
        Exit Sub
    End If
    
    'check if final point is a white checker
    'i = 0
    'Do
    '    If BigX2SmallX(WhiteCheck(i).Left + 10) = XD And BigY2SmallY(WhiteCheck(i).Top + 10) = YD Then
    '        Exit Sub
    '    End If
    '    i = i + 1
    'Loop While (i < g_nCheckerCount)
        
    'check if the final point is a black checker
    'i = 0
    'Do
    '    If BigX2SmallX(BlackCheck(i).Left + 10) = XD And BigY2SmallY(BlackCheck(i).Top + 10) = YD Then
    '        Exit Sub
    '    End If
    '    i = i + 1
    'Loop While (i < g_nCheckerCount)
    
    ' can make the move
    If (-1 <> iWCO) Then
        WhiteCheck(iWCO).Left = SmallX2BigX(XD)
        WhiteCheck(iWCO).Top = SmallY2BigY(YD)
    Else
        BlackCheck(iBCO).Left = SmallX2BigX(XD)
        BlackCheck(iBCO).Top = SmallY2BigY(YD)
    End If
    
    RaiseEvent OnCheckerMoved(True, XO, YO, XD, YD)
    
    Debug.Print "Moving checker from (" + CStr(XO) + "," + CStr(YO) + ") to (" + CStr(XD) + "," + CStr(YD) + ")"
End Sub

Private Function BigX2SmallX(X As Integer) As Integer
    BigX2SmallX = X \ BlackSquare(0).Width + 1
End Function

Private Function BigY2SmallY(Y As Integer) As Integer
    BigY2SmallY = Y \ BlackSquare(0).Height + 1
End Function

Private Function SmallX2BigX(X As Integer) As Integer
    SmallX2BigX = (X - 1) * BlackSquare(0).Width + (BlackSquare(0).Width - BlackCheck(0).Width) / 2
End Function

Private Function SmallY2BigY(Y As Integer) As Integer
    SmallY2BigY = (Y - 1) * BlackSquare(0).Height + (BlackSquare(0).Height - BlackCheck(0).Height) / 2
End Function
Private Function ClosestX(X As Integer) As Integer
    ClosestX = (X \ BlackSquare(0).Width) * BlackSquare(0).Width + (BlackSquare(0).Width - BlackCheck(0).Width) / 2
End Function
Private Function ClosestY(Y As Integer) As Integer
    ClosestY = (Y \ BlackSquare(0).Height) * BlackSquare(0).Height + (BlackSquare(0).Height - BlackCheck(0).Height) / 2
End Function

Private Function LockWhiteChecker(X As Integer, Y As Integer) As Integer
    Dim i As Integer
    i = 0
    g_iWCLocked = -1
    Do
        If Abs(WhiteCheck(i).Left + g_nCheckerRadiusX - X) < g_nCheckerRadiusX And Abs(WhiteCheck(i).Top + g_nCheckerRadiusY - Y) < g_nCheckerRadiusY Then
            g_iWCLocked = i
            g_nDeltaX = WhiteCheck(i).Left - X
            g_nDeltaY = WhiteCheck(i).Top - Y
            g_nXO = BigX2SmallX(WhiteCheck(i).Left + 10)
            g_nYO = BigY2SmallY(WhiteCheck(i).Top + 10)
            Exit Do
        End If
        i = i + 1
    Loop While (i < g_nCheckerCount)
    LockWhiteChecker = g_iWCLocked
End Function

Private Function LockBlackChecker(X As Integer, Y As Integer) As Integer
    Dim i As Integer
    i = 0
    g_iBCLocked = -1
    Do
        If Abs(BlackCheck(i).Left + g_nCheckerRadiusX - X) < g_nCheckerRadiusX And Abs(BlackCheck(i).Top + g_nCheckerRadiusY - Y) < g_nCheckerRadiusY Then
            g_iBCLocked = i
            g_nDeltaX = BlackCheck(i).Left - X
            g_nDeltaY = BlackCheck(i).Top - Y
            g_nXO = BigX2SmallX(BlackCheck(i).Left + 10)
            g_nYO = BigY2SmallY(BlackCheck(i).Top + 10)
            Exit Do
        End If
        i = i + 1
    Loop While (i < g_nCheckerCount)
    LockBlackChecker = g_iBCLocked
End Function

Public Sub ResetCheckers()
    ResetCheckerPositions
    g_iWCLocked = -1
    g_iBCLocked = -1
End Sub

Public Property Get CheckerPositions(White As Boolean) As Variant
    Dim vrgnPos() As Variant
    Dim i, j As Integer
    
    ReDim vrgnPos(1 To g_nCheckerCount * 2)
    j = 1
    If White Then
        For i = 0 To g_nCheckerCount - 1
            vrgnPos(j) = BigX2SmallX(WhiteCheck(i).Left + 10)
            vrgnPos(j + 1) = BigY2SmallY(WhiteCheck(i).Top + 10)
            j = j + 2
        Next i
    Else
        For i = 0 To g_nCheckerCount - 1
            vrgnPos(j) = BigX2SmallX(BlackCheck(i).Left + 10)
            vrgnPos(j + 1) = BigX2SmallX(BlackCheck(i).Top + 10)
            j = j + 2
        Next i
    End If
    CheckerPositions = vrgnPos
End Property

Public Property Let CheckerPositions(White As Boolean, ByVal NewPositions As Variant)
    Dim i As Integer
    Select Case VarType(NewPositions)
        Case vbArray + vbVariant, vbArray + vbInteger:
            If UBound(NewPositions) <> g_nCheckerCount * 2 Then
                Exit Property
            End If
            For i = 1 To g_nCheckerCount * 2
                If VarType(NewPositions(i)) <> vbInteger Then
                    Exit Property
                End If
            Next i
            For i = 1 To g_nCheckerCount
                If White Then
                    WhiteCheck(i - 1).Left = SmallX2BigX(CInt(NewPositions(2 * (i - 1) + 1)))
                    WhiteCheck(i - 1).Top = SmallY2BigY(CInt(NewPositions(2 * i)))
                Else
                    BlackCheck(i - 1).Left = SmallX2BigX(CInt(NewPositions(2 * (i - 1) + 1)))
                    BlackCheck(i - 1).Top = SmallY2BigY(CInt(NewPositions(2 * i)))
                End If
            Next i
    End Select
End Property

VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.1#0"; "COMDLG32.OCX"
Object = "{3B7C8863-D78F-101B-B9B5-04021C009402}#1.1#0"; "richtx32.OCX"
Begin VB.Form frmPreparedText 
   Caption         =   "Prepared Text"
   ClientHeight    =   5070
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   6075
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5070
   ScaleWidth      =   6075
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdCancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   3000
      TabIndex        =   7
      Top             =   4440
      Width           =   1215
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   1560
      TabIndex        =   6
      Top             =   4440
      Width           =   1215
   End
   Begin VB.CommandButton btnRead 
      Caption         =   "Load File"
      Height          =   375
      Left            =   120
      TabIndex        =   5
      Top             =   720
      Width           =   1215
   End
   Begin RichTextLib.RichTextBox rtbText 
      Height          =   2295
      Left            =   120
      TabIndex        =   4
      Top             =   1800
      Width           =   5775
      _ExtentX        =   10186
      _ExtentY        =   4048
      _Version        =   327681
      Enabled         =   -1  'True
      ReadOnly        =   -1  'True
      ScrollBars      =   3
      AutoVerbMenu    =   -1  'True
      TextRTF         =   $"PreText.frx":0000
   End
   Begin VB.ComboBox comboIndex 
      Height          =   315
      ItemData        =   "PreText.frx":012F
      Left            =   120
      List            =   "PreText.frx":0131
      Style           =   2  'Dropdown List
      TabIndex        =   3
      Top             =   1440
      Width           =   2655
   End
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   5640
      Top             =   720
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   327680
   End
   Begin VB.CommandButton btnBrowse 
      Caption         =   "Browse"
      Height          =   375
      Left            =   4560
      TabIndex        =   2
      Top             =   240
      Width           =   1335
   End
   Begin VB.TextBox txtFileName 
      Height          =   285
      Left            =   1080
      TabIndex        =   1
      Top             =   240
      Width           =   3255
   End
   Begin VB.Label Label1 
      Caption         =   "File Name:"
      Height          =   255
      Left            =   120
      TabIndex        =   0
      Top             =   240
      Width           =   855
   End
End
Attribute VB_Name = "frmPreparedText"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Option Base 0

Const strFilter = "All File(*.*)|*.*"
Const strDefaultFile = "ExecChat.txt"       ' in current dir

Public OK As Boolean

Public fReloaded As Boolean

Private strLoadedFile As String
Private arrayText(0 To 511) As String
Private cbParagraphs As Integer


Private Sub cmdCancel_Click()
    txtFileName = strLoadedFile     ' set to the current loaded file
    OK = False
    Me.Hide
End Sub


Private Sub cmdOK_Click()
    If Not (strLoadedFile = txtFileName) Then
        If (Not ReadFile(txtFileName)) Then
            OK = False
            Exit Sub
        End If
    End If
    
    strLoadedFile = txtFileName
    
    OK = True
    Me.Hide
End Sub


Private Sub Form_Load()
    rtbText.RightMargin = rtbText.width
    txtFileName = strDefaultFile
    strLoadedFile = ""
    cbParagraphs = 0
    OK = False
    fReloaded = False
End Sub


Private Sub btnBrowse_Click()

    CommonDialog1.InitDir = CurDir
    CommonDialog1.Filter = strFilter
    CommonDialog1.filename = txtFileName
    
    CommonDialog1.CancelError = True
    
    On Error GoTo UserCanceled
    CommonDialog1.ShowOpen
    
    txtFileName = CommonDialog1.filename
    Exit Sub
    
UserCanceled:
    Err.Clear
End Sub


Private Sub btnRead_Click()
    fReloaded = ReadFile(txtFileName)
End Sub


Private Sub comboIndex_Click()
    If comboIndex.ListIndex >= 0 Then
        rtbText.Text = ""
        rtbText.SelStart = 0
        rtbText.SelLength = 0
        rtbText.SelText = arrayText(comboIndex.ListIndex)
    End If
End Sub


Private Function ReadFile(strFileName As String) As Boolean
    ReadFile = False
    
    If strFileName = "" Then
        MsgBox "Empty file name."
        Exit Function
    End If
    
    If strLoadedFile = txtFileName Then
        ReadFile = True
        Exit Function
    End If
    
    Dim nTextFile As Integer
    nTextFile = FreeFile
    
    On Error Resume Next
    Open strFileName For Input Access Read As #nTextFile
    
    If Not Err = 0 Then
        MsgBox "Error OpenFile: " & Err.Description
        Err.Clear
        Exit Function
    End If
    
    On Error GoTo 0
    Dim strParagraph 'As String
    Dim index As Long
    
    index = -1
    comboIndex.Clear
    cbParagraphs = 0
    
    Do While Not EOF(nTextFile)
        Line Input #nTextFile, strParagraph
        
        If Not strParagraph = "" Then
            index = index + 1
            comboIndex.AddItem "Paragraph " & index
            arrayText(index) = strParagraph
        End If
    Loop
    
    If index >= 0 Then
        comboIndex.ListIndex = 0
    End If
    
    Close #nTextFile
    
    cbParagraphs = index + 1
    strLoadedFile = strFileName
    fReloaded = True
    ReadFile = True
End Function


Function strGetText(ByVal index As Integer) As String
    If (cbParagraphs > index) Then
        strGetText = arrayText(index)
    Else
        strGetText = ""
    End If
    
End Function

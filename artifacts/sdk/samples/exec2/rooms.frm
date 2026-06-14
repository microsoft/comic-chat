VERSION 5.00
Object = "{6B7E6392-850A-101B-AFC0-4210102A8DA7}#1.1#0"; "COMCTL32.OCX"
Begin VB.Form frmRooms 
   Caption         =   "Enter a Room"
   ClientHeight    =   5235
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   5985
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5235
   ScaleWidth      =   5985
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox txtRoomName 
      Height          =   285
      Left            =   1560
      TabIndex        =   6
      Text            =   "#ExecChat2"
      Top             =   4080
      Width           =   3975
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "&Enter"
      Default         =   -1  'True
      Height          =   360
      Left            =   1680
      TabIndex        =   4
      Tag             =   "OK"
      Top             =   4560
      Width           =   1140
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   360
      Left            =   3120
      TabIndex        =   3
      Tag             =   "Cancel"
      Top             =   4560
      Width           =   1140
   End
   Begin ComctlLib.ListView lvwRooms 
      Height          =   2775
      Left            =   240
      TabIndex        =   1
      Top             =   1080
      Width           =   5535
      _ExtentX        =   9763
      _ExtentY        =   4895
      View            =   3
      Sorted          =   -1  'True
      LabelWrap       =   -1  'True
      HideSelection   =   -1  'True
      _Version        =   327682
      ForeColor       =   -2147483640
      BackColor       =   -2147483643
      BorderStyle     =   1
      Appearance      =   1
      NumItems        =   3
      BeginProperty ColumnHeader(1) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
         Key             =   ""
         Object.Tag             =   ""
         Text            =   "Room Name"
         Object.Width           =   2540
      EndProperty
      BeginProperty ColumnHeader(2) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
         SubItemIndex    =   1
         Key             =   ""
         Object.Tag             =   ""
         Text            =   "Topic"
         Object.Width           =   2540
      EndProperty
      BeginProperty ColumnHeader(3) {0713E8C7-850A-101B-AFC0-4210102A8DA7} 
         SubItemIndex    =   2
         Key             =   ""
         Object.Tag             =   ""
         Text            =   "Member Count"
         Object.Width           =   2540
      EndProperty
   End
   Begin VB.CommandButton btnUpdateRooms 
      Caption         =   "&Update Room List"
      Height          =   375
      Left            =   240
      TabIndex        =   0
      Top             =   240
      Width           =   1695
   End
   Begin VB.Label lblRoomToEnter 
      Caption         =   "&Room To Enter:"
      Height          =   255
      Left            =   240
      TabIndex        =   5
      Top             =   4080
      Width           =   1215
   End
   Begin VB.Label lblTotalRooms 
      Caption         =   "Rooms not enumed."
      Height          =   255
      Left            =   240
      TabIndex        =   2
      Top             =   840
      Width           =   1695
   End
End
Attribute VB_Name = "frmRooms"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'****************************************************************************
'
'  Module:     Exec2.exe
'  File:       Rooms.frm
'
'  Copyright (c) Microsoft Corporation 1996-1998
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
' THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'****************************************************************************

Option Explicit

Public bEnter As Boolean

Dim ChannelCount As Integer


Private Sub cmdCancel_Click()
    bEnter = False
    Me.Hide
End Sub

Private Sub cmdOK_Click()
    If txtRoomName.Text = "" Then
        MsgBox "Please input the room name to enter!", , "Enter a Room"
        txtRoomName.SetFocus
    Else
        bEnter = True
        Me.Hide
    End If

End Sub

Private Sub Form_Load()
    lblTotalRooms = "Not enumerated."
    txtRoomName = "#ExecChat2"
    lvwRooms.ListItems.Clear
    ChannelCount = 0
End Sub


Private Sub Form_Unload(Cancel As Integer)
    lvwRooms.ListItems.Clear
    ChannelCount = 0
    bEnter = False
End Sub


Private Sub btnUpdateRooms_Click()
    BeginEnumChannels
End Sub


Private Sub lvwRooms_DblClick()
    If lvwRooms.SelectedItem Is Nothing Then
        Exit Sub
    End If
    
    Dim fRoomProp As New frmRoomProp
    fRoomProp.txtRoomName = lvwRooms.SelectedItem.Text
    
    If fRoomProp.GetProperties = True Then
        fRoomProp.Show vbModal, Me
    End If
    
    Unload fRoomProp
    Set fRoomProp = Nothing
End Sub

Private Sub lvwRooms_ItemClick(ByVal item As ListItem)
    txtRoomName = item.Text
End Sub


Private Sub lvwRooms_ColumnClick(ByVal ColumnHeader As ColumnHeader)
    lvwRooms.SortKey = ColumnHeader.index - 1
End Sub



Public Sub BeginEnumChannels()
    Dim ChannelQueryItems As New ChatItems
    
    lvwRooms.ListItems.Clear
    ChannelCount = 0
    
    ChannelQueryItems.AssociatedType = "Query"
    fMainForm.MsChatPr1.ListChannels ChannelQueryItems
    
    Set ChannelQueryItems = Nothing

End Sub


Public Sub AddChannel(ByRef ChannelItem As ChatItems)
    Dim item As ListItem
    
    Set item = lvwRooms.ListItems.Add

    item.Text = VPropertyFromChatItems(ChannelItem, cpnName)
    item.SubItems(1) = VPropertyFromChatItems(ChannelItem, cpnTopic)
    item.SubItems(2) = VPropertyFromChatItems(ChannelItem, cpnMemberCount)
    
    Set item = Nothing
    
    ChannelCount = ChannelCount + 1
End Sub


Public Sub EndEnumChannels()
    lblTotalRooms = "Total " & ChannelCount & " Rooms"
End Sub

//****************************************************************************
//  Module:     Chat2000.EXE
//  File:       Globals.h
//              
//
//  Copyright (c) Microsoft Corporation 1995-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#ifndef _GLOBALS_
#define _GLOBALS_

#define IDC_OCX_CHAT					1000

//UI options
#define UI_OPT_SEND_BUTTON				0x0001
#define	UI_OPT_WHISPER_BUTTON			0x0002
#define	UI_OPT_PARTICIPANT_LISTBOX		0x0004
#define	UI_OPT_RECEIVE_TEXTBOX			0x0008
#define	UI_OPT_SEND_TEXTBOX				0x0010
#define	UI_OPT_INFORM_JOIN				0x0020
#define	UI_OPT_INFORM_LEAVE				0x0040
#define	UI_OPT_INFORM_MEMCHG			0x0080
#define	UI_OPT_INSERT_BLANK				0x0100
#define	UI_OPT_DISPLAY_ERRORMESSAGES	0x0200
#define	UI_OPT_ALLOW_RESIZING			0x0400
#define	UI_OPT_SENSITIVE_URLS			0x0800
#define UI_OPT_ALL						0x0FFF

//Flags for EnterRoom
#define ENTER_FLAG_JOIN_CREATE				0x0001
#define	ENTER_FLAG_DO_NOT_EXPOSE			0x0002
#define	ENTER_FLAG_READ_ONLY				0x0004
#define	ENTER_FLAG_MIC_ONLY					0x0008
#define	ENTER_FLAG_AUTHENTICATED_ONLY		0x0010
#define	ENTER_FLAG_AUTHENTICATED_CAN_TALK	0x0020

//Type for EnterRoom
#define ENTER_TYPE_ANY_USER					0x0001
#define	ENTER_TYPE_HOST_CAN_CHANGE_TOPIC	0x0002
#define	ENTER_TYPE_LOGGED_USERS_ONLY		0x0004
#define	ENTER_TYPE_DISABLE_WHISPERING		0x0008
#define	ENTER_TYPE_STRONG_HOST				0x0010
#define	ENTER_TYPE_SPECTATOR_DEFAULT		0x0020

//States
#define STATE_DISCONNECTED					0x0001
#define	STATE_CONNECTING					0x0002
#define	STATE_CONNECTED						0x0003

//Participant states
#define PARTICIPANT_STATE_HOST				0x0001
#define	PARTICIPANT_STATE_PARTICIPANT		0x0002
#define	PARTICIPANT_STATE_SPECTATOR			0x0004
#define	PARTICIPANT_STATE_NO_WISPER			0x0008
#define	PARTICIPANT_STATE_IGNORED			0x0010


//metrix convertions macros
#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   ( (HIMETRIC_PER_INCH*(x) + ((ppli)>>1)) / (ppli) )
#define MAP_LOGHIM_TO_PIX(x,ppli)   ( ((ppli)*(x) + HIMETRIC_PER_INCH/2) / HIMETRIC_PER_INCH )

#endif _GLOBALS_
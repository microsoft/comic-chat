//=--------------------------------------------------------------------------=
// PrRes.H
//=--------------------------------------------------------------------------=
// Copyright 1995-1996 Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=
//
// resource IDs.
//
#ifndef __PRRES_H__

#include "XRes.H"

#define RESID_TOOLBOX_PRBITMAP				2


//=--------------------------------------------------------------------------=
// Strings
//
#define IDS_PR_OCXNAME						7010
#define IDS_PR_ABOUTMESS					7011

#define IDS_COMICSTART						7040
#define IDS_VERSION							7040
#define IDS_SOUND							7041
#define IDS_LAGTIME							7042
#define IDS_LOCALTIME						7043
#define IDS_APPEARSAS						7044
#define IDS_GETINFO							7045
#define IDS_BACKGROUND						7046
#define IDS_CCPREFIX						7047
#define IDS_CCSUFFIX1						7048
#define IDS_CCSUFFIX2						7049
#define IDS_ACTION							7050
#define IDS_M3								7051
#define IDS_M5								7052
#define IDS_COMICEND						7052

#define IDS_UNKNOWN							7060

//=--------------------------------------------------------------------------=
// Error Strings
//
#define IDS_CPR_E_FAILURE					7200
#define IDS_CPR_E_INVALIDARG				7201
//
// Connection errors
//
#define IDS_CPR_E_ALREADYCONNECTING			7220
#define IDS_CPR_E_ALREADYCONNECTED			7221
#define IDS_CPR_E_ALREADYDISCONNECTING		7222
#define IDS_CPR_E_NOOPINPROGRESS			7223
#define IDS_CPR_E_DISCONNECTFIRST			7224
#define IDS_CPR_E_NOTCONNECTED				7225
#define IDS_CPR_E_HOSTNOTFOUND				7226
#define IDS_CPR_E_SOCKETCREATE				7227
#define IDS_CPR_E_CANTCONNECT				7228
#define IDS_CPR_E_SOCKETERROR				7229
#define IDS_CPR_E_LOSTCONNECTION			7230
#define IDS_CPR_E_SOCKETCLOSED				7231
#define IDS_CPR_E_NETWORKDOWN				7232
#define IDS_CPR_E_HOSTDROPPEDCONNECTION		7233
//
// Login errors
//
#define IDS_CPR_E_ALREADYLOGGING			7240
#define IDS_CPR_E_ALREADYLOGGED				7241
#define IDS_CPR_E_NOTLOGGED					7242
#define IDS_CPR_E_ALREADYREGISTERED			7243
#define IDS_CPR_E_NOTREGISTERED				7244
#define IDS_CPR_E_BADSECUPACK				7245
#define IDS_CPR_E_LOGINFAILED				7246
#define IDS_CPR_E_AUTHFAILED				7247
#define IDS_CPR_E_AUTHNOTAVAIL				7248
#define IDS_CPR_E_AUTHENTICATEDONLY			7249
//
// Channel Errors
//
#define IDS_CPR_E_ALREADYOPEN				7260
#define IDS_CPR_E_ALREADYOPENING			7261
#define IDS_CPR_E_NOTOPEN					7262
#define IDS_CPR_E_NOTCLOSED					7263
#define IDS_CPR_E_OPENNOTUNIQUE				7264
#define IDS_CPR_E_CHANNELBADNAME			7265
#define IDS_CPR_E_CHANNELBADTOPIC			7266
#define IDS_CPR_E_CHANNELBADKEY				7267
#define IDS_CPR_E_CHANNELBADHOSTKEY			7268
#define IDS_CPR_E_CHANNELBADOWNERKEY		7269
#define IDS_CPR_E_CHANNELBADMODES			7270
#define IDS_CPR_E_BADMAXMEMBERCOUNT			7271
#define IDS_CPR_E_CHANNELFULL				7272
#define IDS_CPR_E_TOOMANYCHANNELS			7273
#define IDS_CPR_E_NOTONCHANNEL				7274
#define IDS_CPR_E_ALREADYONCHANNEL			7275
#define IDS_CPR_E_NOSUCHCHANNEL				7276
#define IDS_CPR_E_INVITEONLYCHANNEL			7277
#define IDS_CPR_E_CHANNELKEYALREADYSET		7278

//
// Property Errors
// 
#define IDS_CPR_E_BADPROPERTY				7280
#define IDS_CPR_E_BADVALUE					7281
#define IDS_CPR_E_PROPNOTREADABLE			7282
#define IDS_CPR_E_PROPREADONLY				7283
#define IDS_CPR_E_PROPNOTWRITEABLE			7284

//
// Member Errors
// 
#define IDS_CPR_E_NOSUCHMEMBER				7290
#define IDS_CPR_E_MEMBERBADMODES			7291

//
// User Errors
//
#define IDS_CPR_E_BADNICKNAME				7300
#define IDS_CPR_E_BADUSERNAME				7301
#define IDS_CPR_E_BADREALNAME				7302
#define IDS_CPR_E_BADPASSWORD				7303
#define IDS_CPR_E_NICKNAMEINUSE				7304
#define IDS_CPR_E_NICKCOLLISION				7305
#define IDS_CPR_E_NOSUCHNICK				7306
#define IDS_CPR_E_USERBADMODES				7307
#define IDS_CPR_E_NICKTOOFAST				7308
#define IDS_CPR_E_NICKNOCHANGE				7309

//
// Sending
//
#define IDS_CPR_E_TOOMUCHDATA				7310
#define IDS_CPR_E_CANTSEND					7311
#define IDS_CPR_E_INVALIDRECIPIENTLIST		7312
#define IDS_CPR_E_BADREASON					7313
#define IDS_CPR_E_INVALIDAWAYMESSAGE		7314
#define IDS_CPR_E_NULLMSG					7315
#define IDS_CPR_E_BADMSGTYPE				7316
#define IDS_CPR_E_NOWHISPER					7317
#define IDS_CPR_E_BADTAG					7318

//
// Protocol Errors
//
#define IDS_CPR_E_SERVER					7330
#define IDS_CPR_E_NOTIRCX					7331
#define IDS_CPR_E_PROPMODE					7332
#define IDS_CPR_E_NOMOTD					7333
#define IDS_CPR_E_UNKNOWNCOMMAND			7334

//
// Access Rights errors
//
#define IDS_CPR_E_NOTSYSOP					7340
#define IDS_CPR_E_NOTOWNER					7341
#define IDS_CPR_E_NOTHOST					7342
#define IDS_CPR_E_RATINGBLOCK				7343
#define IDS_CPR_E_BANNEDFROMCHANNEL			7344
#define IDS_CPR_E_BANNEDFROMSERVER			7345
#define IDS_CPR_E_WILLBEBANNED				7346
#define IDS_CPR_E_CANTIGNORE				7347
#define IDS_CPR_E_NOJOINMICONLY				7348
#define IDS_CPR_E_NOJOINREMOTE				7349
#define IDS_CPR_E_NOJOINDYNAMIC				7350
#define IDS_CPR_E_NODYNAMICCHANNELS			7351
#define IDS_CPR_E_AUTHONLY					7352
#define IDS_CPR_E_OVERFLOWABORT				7353
#define IDS_CPR_E_CANTCHANGEUSERMODE		7354

//
// ChatItems related errors
//
#define IDS_CPR_E_ITEMNOTAVAILABLE			7360
#define IDS_CPR_E_ITEMNAMENA				7361
#define IDS_CPR_E_INVALIDASSOCIATEDTYPE		7362
#define IDS_CPR_E_INVALIDITEMNAME			7363
#define IDS_CPR_E_INVALIDITEMVALUE			7364
#define IDS_CPR_E_NOTOPERATOR				7365
#define IDS_CPR_E_TIMEOUT					7366

//
// Channel Collection Errors
//
#define IDS_CPR_E_CHANOBJ_BADINDEX			7370
#define IDS_CPR_E_CHANOBJ_INDEXOOB			7371
#define IDS_CPR_E_CHANOBJ_DELETED			7372
#define IDS_CPR_E_CHANOBJ_BADKEY			7373
#define IDS_CPR_E_CHANOBJ_KEYNOTUNIQUE		7374


#define __PRRES_H__
#endif // __PRRES_H__

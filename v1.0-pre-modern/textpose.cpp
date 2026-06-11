// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "stdafx.h"

#include "chat.h"
#include "bbox.h"
#include "vector2d.h"
#include "pe.h"
#include "dib.h"
#include "avatar.h"
#include "bodycam.h"
#include "ui.h"
#include "resource.h"

extern CChatApp theApp;

static UINT ruleIDs[] = {ID_RULE_SHOUT, ID_RULE_LAUGH, ID_RULE_HAPPY, ID_RULE_SAD,
						 ID_RULE_POINTOTHER, ID_RULE_POINTSELF, ID_RULE_WAVE,
						 ID_RULE_COY, ID_RULE_ANGRY, ID_RULE_SCARED, ID_RULE_BORED};
static float ruleEMs[] = {EM_SHOUT, EM_LAUGH, EM_HAPPY, EM_SAD,
						  EM_POINTOTHER, EM_POINTSELF, EM_WAVE,
						  EM_COY, EM_ANGRY, EM_SCARED, EM_BORED};

BOOL CheckForUppers(const char *buff) {
	int nUppers = 0;
	while (*buff != '\0') {
		if (islower(*buff)) return FALSE;
		if (isupper(*buff++)) nUppers++;
	}
	if (nUppers > 1)			// only bother if there's more than one upper char
		return TRUE;
	else return FALSE;
}

int CheckWord(const char *buff, const char *substr) {
	const char *loc = buff;
	while (loc = strstr(loc, substr)) {													// it is a substring
		if ((loc == buff) || isspace(*(loc-1))) {				// that starts a word
			int len = strlen(substr);
			char after = loc[len];
			if (!after || isspace(after) || ispunct(after))		// that is a word
				return TRUE;
		}
		loc++;
	}
	return FALSE;
}

#if 0
void CheckForLaughs(const char *buff, CEmotionOpts &emOpts) {
	if (CheckWord(buff, "ROTFL"))
		emOpts.Add(EM_LAUGH, 1.0, 9);
	if (CheckWord(buff, "LOL"))
		emOpts.Add(EM_LAUGH, .6, 11);
}

// Others "Points" are checked for in CheckStarts
void CheckForPoints(const char *buff, CEmotionOpts &emOpts) {
	if (CheckWord(buff, "are you") || CheckWord(buff, "will you") || CheckWord(buff, "did you")
		|| CheckWord(buff, "aren't you") || CheckWord(buff, "don't you"))
		emOpts.Add(EM_POINTOTHER, .8, 8);
	if (CheckWord(buff, "i'm") || CheckWord(buff, "i will") || CheckWord(buff, "i'll") || CheckWord(buff, "i am"))
		emOpts.Add(EM_POINTSELF, .8, 8);
}

int StartCompare(const char *sent, char *substring, int len) {
	return (strnicmp(sent, substring, len) == 0 && !isalnum(sent[len]));
}

void CheckStarts(const char *sent, CEmotionOpts &emOpts) {
	if (StartCompare(sent, "HI", 2) || StartCompare(sent, "Bye", 3) ||
		StartCompare(sent, "Hello", 5) || StartCompare(sent, "Welcome", 7) ||
		StartCompare(sent, "Howdy", 5))
			emOpts.Add(EM_WAVE, 1.0, 11);
	if (StartCompare(sent, "I", 1))
		emOpts.Add(EM_POINTSELF, 1.0, 3);
	if (StartCompare(sent, "You", 3))
		emOpts.Add(EM_POINTOTHER, 1.0, 3);
}

#endif

static char *sentenceTerminator = ".!?";

#if 0
void ForSentenceStarts(const char *buff, CEmotionOpts &emOpts, void func(const char *, CEmotionOpts &)) {
	while (TRUE) {
		while (ispunct(*buff) || isspace(*buff)) buff++;
		if (!(*buff)) return;
		(*func)(buff, emOpts);
		buff = strpbrk(buff, sentenceTerminator);
		if (!buff) return;
	}
}
#endif

const char *GetNextSentenceStart(const char *buff) {
	buff = strpbrk(buff, sentenceTerminator); // else, return first word after sentence terminator
	if (!buff) return NULL;
	while (ispunct(*buff) || isspace(*buff)) buff++;
	return buff;
}

char *ToLower(const char *buff) {
	char *newStr = strdup(buff);
	char *sptr = newStr;
	while (*sptr) {
		if (isupper(*sptr)) *sptr = tolower(*sptr);
		sptr++;
	}
	return newStr;
}


CEmotionOpts emo;

void ChatPreSendText(CString &str, int avID) {
	void GetEmotionsFromString(CString &str, CEmotionOpts &emOpts);

	if (!theApp.m_bComicView) return;

	CAvatarX *av = avID ? GetAvatar(avID) : MyAvatar();
	if (!av || av->m_freeze != AF_UNFROZEN) return;
	GetEmotionsFromString(str, emo);
	CBody *newBody = av->GetBodyFromEmotion(emo);
	av->UpdateBody(newBody);
}

void InitializeEmotionRules() {
	CString rule;
	void LoadCompositeRule(float, CString &);

	int nRules = sizeof(ruleIDs) / sizeof(UINT);
	for (int i = 0; i < nRules; i++) {
		rule.LoadString(ruleIDs[i]);
		LoadCompositeRule(ruleEMs[i], rule);
	}
}

void LoadCompositeRule(float emotion, CString &rule) {
	// rules consist of \n separated entries of the form "Function(args);Strength".
	// load each of these in sequence
	BOOL LoadSingleRule(float, const char *, const char **);

	// just single byte right now...
	const char *rptr = rule;
	while (TRUE) {
		if (!LoadSingleRule(emotion, rptr, &rptr)) break;
	}
}

// eventually make this smart about quote escapes, but for now they aren't necessary.
const char *ReadString(const char *string, char *buff) {
	const char *firstQuote = strchr(string, '"');
	if (!firstQuote) {
		*buff = '\0';
		return string;
	}
	const char *secondQuote = strchr(firstQuote+1, '"');
	if (!secondQuote) {
		*buff = '\0';
		return string;
	}
	int len = secondQuote - firstQuote - 1;
	strncpy(buff, firstQuote+1, len);
	buff[len] = '\0';
	return (secondQuote+1);
}


BOOL LoadSingleRule(float emotion, const char *start, const char **end) {
	char function[20], arg[200], *aptr = arg, *fptr = function, strengthStr[100];
	const char *sptr = start;
	void RegisterRule(float, const char *, const char *, int);

	while(!isprint(*sptr) && *sptr) sptr++;   // proceed to start...

	// parse keyword
	if (!*sptr) return FALSE;
	while(*sptr != '(' && *sptr)
		*fptr++ = *sptr++;
	*fptr = '\0';
	if (!*sptr) return FALSE;

	// parse arg
	sptr++;			// increment past (
	sptr = ReadString(sptr, arg);
	while(*sptr != ';' && *sptr) sptr++;
	if (!*sptr) return FALSE;

	// parse strength
	sptr++;			// increment past ;
	char *strPtr = strengthStr;
	while (*sptr != '\n' && *sptr)
		if (isdigit(*sptr)) *strPtr++ = *sptr++;
	*strPtr = '\0';
	int strength = atoi(strengthStr);
	while(*sptr == '\n') sptr++;
	*end = sptr;

	RegisterRule(emotion, function, arg, strength);

	return (*sptr != '\0');
}

static CPtrList generalRules;
static CPtrList wordRules;
static CPtrList sentenceRules;
static int capsStrength = 0;   // rule not active by default (also used as test)
static float capsEmotion;

typedef struct {
	char *arg;
	int length;
	int strength;
	float emotion;
	BOOL caseSensitive;
} STRINGUNIT;

inline void AddToGeneral(STRINGUNIT *rule) {
	generalRules.AddTail(rule);
}

inline void AddToWord(STRINGUNIT *rule) {
	wordRules.AddTail(rule);
}


inline void AddToSentence(STRINGUNIT *rule) {
	sentenceRules.AddTail(rule);
}

STRINGUNIT *StringUnit(float emotion, const char* arg, int strength, BOOL caseSensitive) {
	STRINGUNIT *unit = (STRINGUNIT *) malloc (sizeof(STRINGUNIT));
	unit->emotion = emotion;
	unit->arg = caseSensitive ? strdup(arg) : ToLower(arg);
	unit->length = strlen(arg);
	unit->strength = strength;
	unit->caseSensitive = caseSensitive;
	return unit;
}

void RegisterRule(float emotion, const char *function, const char *arg, int strength) {
	if (!stricmp(function, "AllCaps")) {
		capsStrength = strength;
		capsEmotion = emotion;
	}
	else if (!stricmp(function, "FindString"))
		AddToGeneral(StringUnit(emotion, arg, strength, TRUE));
	else if (!stricmp(function, "FindString*"))
		AddToGeneral(StringUnit(emotion, arg, strength, FALSE));
	else if (!stricmp(function, "CheckWord"))
		AddToWord(StringUnit(emotion, arg, strength, TRUE));
	else if (!stricmp(function, "CheckWord*"))
		AddToWord(StringUnit(emotion, arg, strength, FALSE));
	else if (!stricmp(function, "CheckStart"))
		AddToSentence(StringUnit(emotion, arg, strength, TRUE));
	else if (!stricmp(function, "CheckStart*"))
		AddToSentence(StringUnit(emotion, arg, strength, FALSE));
}

int StartCompare2(const char *sent, const char *substring, int len) {
	return (strncmp(sent, substring, len) == 0 && !isalnum(sent[len]));
}

void GetEmotionsFromString(CString &str, CEmotionOpts &emOpts) {
	const char *buff = (LPCTSTR) str;
	char *lower = ToLower(buff);
	emOpts.m_nOpts = 0;

	// check for uppers
	if (capsStrength && CheckForUppers(buff)) {
		emOpts.Add(capsEmotion, 1.0, capsStrength);
	}

	// check generals
	POSITION pos = generalRules.GetHeadPosition();
	while (pos) {
		STRINGUNIT *unit = (STRINGUNIT *) generalRules.GetNext(pos);
		if (unit->caseSensitive) {
			if (strstr(buff, unit->arg)) emOpts.Add(unit->emotion, 1.0, unit->strength);
		} else if (strstr(lower, unit->arg)) emOpts.Add(unit->emotion, 1.0, unit->strength);
	}

	// check words
	pos = wordRules.GetHeadPosition();
	while (pos) {
		STRINGUNIT *unit = (STRINGUNIT *) wordRules.GetNext(pos);
		if (unit->caseSensitive) {
			if (CheckWord(buff, unit->arg)) emOpts.Add(unit->emotion, 1.0, unit->strength);
		} else if (CheckWord(lower, unit->arg)) emOpts.Add(unit->emotion, 1.0, unit->strength);
	}

	// check sentences
	const char *bptr = buff;
	while (isspace(*bptr)) bptr++;					  // prune off leading white space
	while (bptr && *bptr) {
		char *lptr = lower + (bptr - buff);
		pos = sentenceRules.GetHeadPosition();
		while (pos) {
			STRINGUNIT *unit = (STRINGUNIT *) sentenceRules.GetNext(pos);
			if (unit->caseSensitive) {
				if (StartCompare2(buff, unit->arg, unit->length))
					emOpts.Add(unit->emotion, 1.0, unit->strength);
			} else if (StartCompare2(lower, unit->arg, unit->length))
				emOpts.Add(unit->emotion, 1.0, unit->strength);
		}
		bptr = GetNextSentenceStart(bptr);
	}

	free(lower);
}

void DestroyEmotionList(CPtrList &list) {
	POSITION pos = list.GetHeadPosition();
	while (pos) {
		STRINGUNIT *unit = (STRINGUNIT *) list.GetNext(pos);
		free(unit->arg);
		free(unit);
	}
}

void DestroyEmotionRules() {
	DestroyEmotionList(generalRules);
	DestroyEmotionList(wordRules);
	DestroyEmotionList(sentenceRules);
}



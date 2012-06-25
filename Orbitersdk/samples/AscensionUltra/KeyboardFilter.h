// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// KeyboardFilter.h
// Class definition of keyboard filter.
// ==============================================================
#pragma once

class KeyboardFilter
{
public:
	KeyboardFilter(void *obj, int (*consumeDirectKey)(void*, char*), void (*prefilter)(void*, WPARAM &wparam, LPARAM &lparam)=NULL);
	~KeyboardFilter(void);
	HHOOK GetHook();	
	int Process(short key, bool down);
	void Preprocess(WPARAM &wparam, LPARAM &lparam);
	static KeyboardFilter *Current;
private:
	static const short OAPI2VK[256], VK2OAPI[256];	
	int (*ConsumeDirectKey)(void*, char*);
	void (*Prefilter)(void*, WPARAM &wparam, LPARAM &lparam);
	HHOOK hhook;
	void *object;
	char *kstate;
};

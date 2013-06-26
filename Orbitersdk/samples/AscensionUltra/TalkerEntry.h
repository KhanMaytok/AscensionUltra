// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// TalkerEntry.h
// Definition of entries for text-to-speech facility.
// ==============================================================

#pragma once
#define TALKERFLUSH		0x01
#define TALKERWAVE		0x02

struct TalkerEntry {
	LPCWSTR message;
	char *display;
	double size;
	VECTOR3 color;
	int flags;
	bool valid;
};
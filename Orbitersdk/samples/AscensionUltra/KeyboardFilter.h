// Copyright (C) 2010-2014 Friedrich Kastner-Masilko
//
// This file is part of the Ascension Ultra Project
//
// Ascension Ultra is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of
// the License, or (at your option) any later version.
//
// Ascension Ultra is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Ascension Ultra. If not, see
// <http://www.gnu.org/licenses/>.

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

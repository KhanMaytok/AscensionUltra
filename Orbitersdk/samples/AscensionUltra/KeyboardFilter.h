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

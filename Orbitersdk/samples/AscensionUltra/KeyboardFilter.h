#pragma once

class KeyboardFilter
{
public:
	KeyboardFilter(void *obj, int (*consumeDirectKey)(void*, char*));
	~KeyboardFilter(void);
	HHOOK GetHook();	
	int Process(short key, bool down);
	static KeyboardFilter *Current;
private:
	static const short OAPI2VK[256], VK2OAPI[256];	
	int (*ConsumeDirectKey)(void*, char*);
	HHOOK hhook;
	void *object;
	char *kstate;
};

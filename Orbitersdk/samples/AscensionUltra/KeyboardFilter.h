#pragma once

class KeyboardFilter
{
public:
	KeyboardFilter(void);
	~KeyboardFilter(void);
	void GetCheckString(char *msg);
private:
	short OAPI2VK[256], VK2OAPI[256];
};

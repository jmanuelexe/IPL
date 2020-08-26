#include "mem.h"
#include <memory>
/*
TStringList::~TStringList()
{
	for (word i = 0; i < str.size(); i++)
		free(i);
}

word TStringList::add(const char* string)
{
	int stlen = (int) strlen(string) + 1;
	char* newStr = (char*)malloc(stlen);

	if (!newStr) return 0;
	strcpy(newStr, string);

	for (word i = 0; i < str.size(); i++)
	{
		if (!str[i]) {
			str[i] = newStr;
			return i;
		}
	}
	str.push_back(newStr);

	return str.size() - 1;
}

void TStringList::free(const word index)
{
	if (str[index]) {
		::free(str[index]);
		str[index] = 0;
	}
}
*/
//====================================================================================
//
//=====================================================================================
TSimpleAlloc::TSimpleAlloc() {
	start = 0; size = 0;
}

char* TSimpleAlloc::get() { return start; }

TSimpleAlloc::TSimpleAlloc(const word size)
{
	start = 0;
	resize(size);
}

unsigned TSimpleAlloc::getSize()
{
	return size;
}

#define allocate(previous, size) realloc(previous, size)
#define GROWVECTOR(previous, type, size) (type)allocate(previous, size);
#define MIN_ELEMENT 1
bool TSimpleAlloc::resize(const word count)
{
	if (count > getSize()) {
		int newsize = (count < MIN_ELEMENT) ? MIN_ELEMENT : (int)((float)count * 1.5f);
		char* temp = (char*)allocate(start, newsize);// GROWVECTOR(start, char*, newsize);

		if (temp) {
			start = temp;
			this->size = newsize;
			return true;
		}
	}
	return false;
}

TSimpleAlloc::~TSimpleAlloc()
{
	if (start) {
		free(start);
		start = 0;
	}
}
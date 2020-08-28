#pragma once
#include "types.h"

class Array
{
public:
	TVariant* vars;
	word count;
	byte ref;
public:
	Array() { ref = 0; count = 0; vars = 0; }
	bool resize(const int CountElements);
	void dispose();
};


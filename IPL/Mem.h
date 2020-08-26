#pragma once
#include "types.h"
#include <Vector>

class TSimpleAlloc {
protected:
	char* start;
	unsigned size;
public:
	unsigned getSize();
	TSimpleAlloc();
	char* get();
	TSimpleAlloc(const word size);
	bool resize(const word size);
	virtual ~TSimpleAlloc();
};

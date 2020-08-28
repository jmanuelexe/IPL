#include "Array.h"
#include <stdlib.h>


/**********************************************************************************************
			Array Class definition
**********************************************************************************************/
//increase /decrease the count of object for array has(not the bytes)
bool Array::resize(const int CountElements)
{
	TVariant* temp;
	word size = CountElements * int(sizeof(TVariant));
	temp = (TVariant*)realloc(vars, size);

	if (temp) {
		vars = temp;
		this->count = CountElements;
		return true;
	}
	return false;
}

void Array::dispose()
{
	if (vars)
	{
		free(vars);
		vars = 0;
	}
}


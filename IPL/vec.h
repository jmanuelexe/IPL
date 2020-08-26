#ifndef vector_H
#define vector_H

#include <stdlib.h>
#include <string.h>

#define declare_vecvar(T) struct vec_##T

#define declare_vec(T) typedef struct vec_##T{														\
	T* m_last, *m_first, *m_end;											\
	unsigned count;															\
}vec_##T;																	\
void v_init_##T(vec_##T *gc);												\
const unsigned v_size_##T(vec_##T*gc);										\
const unsigned v_capacity_##T(vec_##T*gc);									\
const bool v_isEmpty_##T(vec_##T*gc);										\
const T *vec_getint(vec_##T*gc);											\
T* v_endint(vec_##T*gc);													\
T * v_back_##T(vec_##T*gc);													\
T *v_begin_##T(vec_##T*gc);													\
void v_move_##T(T*dest, T *source, unsigned count);							\
bool v_reserve_##T(vec_##T*gc, const unsigned count);						\
int v_resize_##T(vec_##T*gc, const unsigned newsize);						 \
const unsigned int v_free_space(vec_##T*gc);								\
inline T* v_at(vec_##T*gc, const unsigned Pos);								\
T* v_push_back_##T(vec_##T* gc, T data);									\
typedef bool(*SortFunc)(T* one, T* two);									\
	const unsigned int v_free_space(vec_##T*gc);							\
inline T* v_at(vec_##T*gc, const unsigned Pos);								\
T* v_push_back_##T(vec_##T* gc, T data);									\
void v_swap_##T(vec_##T*gc, T* left, T* right);								\
void v_dispose(vec_##T*gc);													\
void v_quickSort_##T(vec_##T* vc, T* left, T* right, SortFunc compare);		\
void v_sort_##T(vec_##T*vc, SortFunc compare);								



#define define_vec(T)												\
void v_init_##T(vec_##T *gc){												\
	gc->m_last = gc->m_first = gc->m_end = 0;								\
	gc->count = 0;															\
}																			\
const unsigned v_size_##T(vec_##T*gc){	return (unsigned)(gc->m_end - gc->m_first);}\
const unsigned v_capacity_##T(vec_##T*gc){return (unsigned)(gc->m_last - gc->m_first);}\
const bool v_isEmpty_##T(vec_##T*gc){	return !v_size_##T(gc);}\
const T *vec_getint(vec_##T*gc){return gc->m_first;}\
T* v_endint(vec_##T*gc){ return gc->m_end;}\
T * v_back_##T(vec_##T*gc){return (gc->m_end - 1);}\
T *v_begin_##T(vec_##T*gc){return gc->m_first;}\
void v_move_##T(T*dest, T *source, unsigned count){ memcpy(dest, source, count * sizeof(char)); }\
bool v_reserve_##T(vec_##T*gc, const unsigned count){\
	if (v_capacity_##T(gc) < count){\
		const float rate = 1.5f;\
		unsigned newsize = v_capacity_##T(gc) + count;\
		unsigned grow = (unsigned)((float)v_capacity_##T(gc) * rate);\
		if (newsize < grow) newsize = grow;\
		T* ptr = (T*)malloc(sizeof(T) * newsize);\
		int len = v_size_##T(gc);\
		if (ptr != 0)\
		{\
			if (gc->m_first)\
			{\
				v_move_##T(ptr, gc->m_first, (unsigned)(gc->m_end - gc->m_first));\
				free(gc->m_first);\
						}\
		}\
		else{ return false; }\
		gc->m_first = ptr;\
		gc->m_last = ptr + newsize;\
		gc->m_end = gc->m_first + len;\
	}\
	return true;\
}\
int v_resize_##T(vec_##T*gc, const unsigned newsize)\
{\
	if (v_size_##T(gc) < newsize)\
	{\
		v_reserve_##T(gc, newsize);\
		gc->m_end += (newsize - v_size_##T(gc));\
		gc->count = v_size_##T(gc);\
	}\
	gc->count = newsize;\
	gc->m_end = gc->m_first + newsize;\
	return gc->count;\
}\
const unsigned int v_free_space(vec_##T*gc){return gc->m_last - gc->m_end;}\
inline T* v_at(vec_##T*gc, const unsigned Pos){return (gc->m_first + Pos);}	\
T* v_push_back_##T(vec_##T* gc, T data)										\
{																			\
	v_resize_##T(gc, v_size_##T(gc) + 1);									\
	*(gc->m_end-1) = data;													\
	gc->count = v_size_##T(gc);												\
	return (gc->m_end - 1);													\
}																			\
void v_swap_##T(vec_##T*gc, T* left, T* right)								\
{																			\
	char *tmp = (char*)malloc(sizeof(char));								\
	memcpy(tmp, left, sizeof(char));										\
	memcpy(left, right, sizeof(char));										\
	memcpy(right, tmp, sizeof(char));										\
	free(tmp);																\
}																			\
void v_dispose(vec_##T*gc)													\
{																			\
	if (gc->m_first){														\
		free(gc->m_first);													\
		v_init_##T(gc);														\
	}																		\
}																			\
void v_quickSort_##T(vec_##T* vc, T* left, T* right, SortFunc compare)		\
{																			\
	T *i = left, *j = right;										\
	T *pivot = left + ((right - left) / 2);							\
	while (i <= j)													\
	{																\
		while (compare(pivot, i))									\
			i++;													\
		while (compare(j, pivot))j--;								\
		if (i <= j)													\
		{															\
			if (i != j) v_swap_##T(vc, i, j);						\
			i++;													\
			j--;													\
		}															\
	};																\
	if (left < j) v_quickSort_##T(vc, left, j, compare);			\
	if (i < right) v_quickSort_##T(vc, i, right, compare);			\
}																	\
void v_sort_##T(vec_##T*vc, SortFunc compare)						\
{																	\
	if (vc->m_first)												\
		v_quickSort_##T(vc, vc->m_first, vc->m_end - 1, compare);	\
}

/*\
typedef bool(*FinCond)(T* one, T* two);\
int v_find_##T(vec_##T* gc, FinCond finfunc)\
{\
	T* n;\
	for (n = gc->m_first; n < gc->m_end; n++)\
		if (finfunc(n)) return n;\
	return -1;\
}
*/
//
//void insert(int  Pos, T &data)
//{
//	//increase space since we are inserting one element
//	reserve(size() + 1);
//	T* to = m_first + Pos;
//
//	for (T* n = m_end; n > to; n--)
//		swap(n - 1, n);
//
//	m_first[Pos] = data;
//	m_end++;		//increase the end since one space
//}
//
//
//void erase(T *data)
//{
//	if (std::is_class<T>::value)	data->~T();
//
//	for (iterator n = data; n < m_end - 1; n++)
//		memcpy(n, n + 1, sizeof(T));
//
//	count--;
//
//	if (!count) clear();
//	else m_end--;
//}

#endif
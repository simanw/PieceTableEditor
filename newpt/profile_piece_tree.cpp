#include <windows.h>
#include <limits.h>
#include "pt_PieceTable.hpp"

static unsigned int
random_number(unsigned int max)
{
	int r = rand();
	return (unsigned int) ((r * 1.0 / RAND_MAX) * max);
}

void
profile_insertion()
{
	pt_PieceTable pt;
	__int64 t1, t2, freq, mean = 0, last = _I64_MAX / 10;
	unsigned int r;
	FILE* f = fopen("insertion2.txt", "w");

	QueryPerformanceFrequency((LARGE_INTEGER*) &freq);
	
	for (unsigned int i = 0; i < 15000; ++i)
	{
		r = random_number(i);

		QueryPerformanceCounter((LARGE_INTEGER*) &t1);
		pt.insertCharacter(r, 'a');
		QueryPerformanceCounter((LARGE_INTEGER*) &t2);

		if (t2 - t1 > 10 * last)
			mean += last;
		else
		{
			mean += t2 - t1;
			last = t2 - t1;
		}

		printf("i = %d\n", i);

		if (i % 1000 == 0)
		{
			mean *= 1000000.0;
			fprintf(f, "%u\t%I64d\n", pt.sizeInternalTree(), mean / freq);
			fflush(f);
			mean = 0;
			last = t2 - t1;
		}
	}		

	fclose(f);
}

void
profile_delete()
{
	pt_PieceTable pt;
	__int64 t1, t2, freq, mean = 0, last = _I64_MAX / 10;
	unsigned int r, i;
	FILE* f = fopen("delete.txt", "w");
	if (f == NULL)
	{
		printf("Impossible to open file delete.txt to log the results.\n");
		return;
	}

	QueryPerformanceFrequency((LARGE_INTEGER*) &freq);
	
	for (i = 0; i < 1000000; ++i)
		pt.insertCharacter(i, 'a' + i % ('z' - 'a'));

	printf("nb of pieces: %d.\n", pt.sizeInternalTree());

	for (i = 1; i < 1000001; ++i)
	{
		r = random_number(1000000 - i);

		QueryPerformanceCounter((LARGE_INTEGER*) &t1);
		pt.deleteCharacter(r);
		QueryPerformanceCounter((LARGE_INTEGER*) &t2);

		if (t2 - t1 > 10 * last)
			mean += last;
		else
		{
			mean += t2 - t1;
			last = t2 - t1;
		}

		if (i % 1000 == 0)
		{
			mean *= 1000000.0;
			fprintf(f, "%u\t%I64d\n", pt.sizeInternalTree(), mean / freq);
			mean = 0;
			last = t2 - t1;
		}
	}		

	fclose(f);
}

#if 0
int
main()
{
	profile_delete();
	return 0;
}
#endif

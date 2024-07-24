#include <stdio.h>

__attribute__((constructor))
void ctor(void)
{
	printf("CTOR");
}

__attribute__((destructor))
void dtor(void)
{
	printf("DTOR");
}

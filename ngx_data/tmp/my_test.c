#include <stdio.h>

int func(int n)
{
	int sum = 0;
	int i;
	for (i = 0; i < n; ++i)
	{
		sum += i;
	}
	
	return sum;
}

int main()
{
	int i;
	long result = 0;
	for (i = 1; i < 100; ++i)
	{
		result += i;
	}
	
	printf("1--100 sum = %ld/n", result);
	printf("1--250 sum = %d/n", func(250));

	return 0;
}

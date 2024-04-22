#include <stdio.h>
#include "uniform.h"
double uniform(double a, double b,long int * seed)
{
	double t;
	*seed = 2045 * (*seed) + 1;
	*seed = *seed - (*seed / 1048576) * 1048576;
	t = (*seed) / 1048576.0;
	t = a + (b - a) * t;
	return t;
}
int main()
{
    double a, b, x;
    int i, j;
    long int s;
    // a = 0.0;
    // b = 1.0;
    // s = 13579;
    // for (i = 0; i < 10; i++)
    // {
    //     for (j = 0; j < 5; j++)
    //     {
    //         x = uniform(a, b, &s);
    //         printf("%13.7f", x);
    //     }
    //     printf("\n");
    // }
    // getchar();
    return 0;
}
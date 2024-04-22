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
void my_func(int a)
{
	(void)a;
}

bool my_func(double a)
{
	return a == 0.0;
}

extern "C" int my_func(bool z)
{
	return z ? 1 : 0;
}

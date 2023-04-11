#include <iostream>
#include <omp.h>
#include <math.h>

using namespace std;

int main()
{
	const double PI = 3.141592653589793238463;
	int num_threads = 0;
	int test_times = 100;
	int num_steps = 100000;
	double step, x, pi, sum = 0;
	double stime, etime, time, sum_time = 0;
	cout << "输入线程数：";
	cin >> num_threads;
	cout << endl;
	omp_set_num_threads(num_threads);
	step = 1.0 / (double)num_steps;

	for (int j = 0; j < test_times; j++)
	{
		stime = omp_get_wtime();
#pragma omp parallel for private(x) reduction(+:sum)
		for (int i = 0; i < num_steps; i++)
		{
			x = (i + 0.5) * step;
			sum = sum + 4.0 / (1.0 + x * x);
		}
		etime = omp_get_wtime();
		time = etime - stime;
		sum_time = sum_time + time;
		pi = step * sum;
		//printf("time[%3d]=%5.2fms", j, time * 1000);
		//printf("\terror=%5.3e", abs(pi - PI));
		//printf("\n");
		sum = 0;
	}
	printf("num_steps=%d\nthreads=%d\n", num_steps, num_threads);
	printf("ave_time=%6.1fus\nerror=%5.2e\n", sum_time / test_times * 1000000, abs(pi - PI));
	return 0;
}
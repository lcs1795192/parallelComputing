#include<mpi.h>
#include<stdio.h>

int main(int argc, char** argv)
{
	const int masterid = 0, cols = 10, rows = 10;
	int myid, cores, sendid, receid;
	double a[rows][cols], b[cols], c[cols], buffer[cols], ans;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &cores);
	if (myid == masterid)//主进程
	{
		//赋值
		for (int i = 0; i < cols; i++)
		{
			printf("a[%d]=",i);
			for (int j = 0; j < rows; j++)
			{
				a[i][j] = (double)(i+j)/2;
				printf("%3.1f ", a[i][j]);
			}
			b[i] = 1;
			printf("\tb[%d]=%3.1f\n",i, b[i]);
		}

		//发送数据(tag=10)
		MPI_Bcast(b, cols, MPI_DOUBLE, masterid, MPI_COMM_WORLD);
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				buffer[j] = a[i][j];
			}
			sendid = i % (cores - 1) + 1;
			MPI_Send(buffer, cols, MPI_DOUBLE, sendid, 10, MPI_COMM_WORLD);
		}
		//发送终止信号(tag=0)
		for (int i = 1; i < cores; i++)
		{
			MPI_Send(buffer, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
		}

		//回收数据(tag=99)
		for (int i = 0; i < rows; i++)
		{
			receid = i % (cores - 1) + 1;
			MPI_Recv(&ans, 1, MPI_DOUBLE, receid, 99, MPI_COMM_WORLD, &status);
			c[i] = ans;
			printf("\nc[%d]=%3.1f", i, ans);
		}
	}
	else//从进程
	{
		MPI_Bcast(&b, cols, MPI_DOUBLE, masterid, MPI_COMM_WORLD);
		MPI_Recv(&buffer, cols, MPI_DOUBLE, masterid, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		do
		{
			ans = 0;
			for (int i = 0; i < cols; i++)
			{
				ans = ans + buffer[i] * b[i];
			}
			MPI_Send(&ans, 1, MPI_DOUBLE, masterid, 99, MPI_COMM_WORLD);
			MPI_Recv(&buffer, cols, MPI_DOUBLE, masterid, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		} while (status.MPI_TAG != 0);
	}
	MPI_Finalize();

	return 0;
}
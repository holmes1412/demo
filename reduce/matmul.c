//  gcc -o mm_with_blas mm_with_blas.c -lopenblas

#include <stdio.h>
#include <stdlib.h>
#include <openblas/cblas.h>

void print_matrix(double *m, int row, int col)
{
	printf("Matrix(%d,%d):\n[\n", row, col);
	for (int i = 0; i < row; i++) {
		printf("  [ ");
		for (int j = 0; j < col; j++) {
			printf("%f ", m[i * col + j]);
		}
		printf("]\n");
	}
	printf("]\n");
}

void matmul(double *a, double *b, double *c, int m, int k, int n)
{
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			c[i * n + j] = 0;
			for (int l = 0; l < k; l++)
				c[i * n + j] += a[i * k + l] * b[l * n + j];
		}
	}
}

void matmul_partial(double *a, double *b, double *c, int column_of_a, int column_of_b,
					int c_row_begin, int c_row_end, int c_col_begin, int c_col_end)
{
	int k = column_of_a;
	int n = column_of_b;

	for (int i = c_row_begin; i <= c_row_end; i++)
	{
		for (int j = c_col_begin; j <= c_col_end; j++)
		{
			c[i * n + j] = 0;
			for (int l = 0; l < k; l++)
				c[i * n + j] += a[i * k + l] * b[l * n + j];
		}
	}
}

int main()
{
	int m = 2; // row of a
	int k = 3; // column of a / row of b
	int n = 4; // column of b

	double *a = (double *)malloc(m * k * sizeof(double));
	double *b = (double *)malloc(k * n * sizeof(double));

	double *c = (double *)malloc(m * n * sizeof(double));
	double *d = (double *)malloc(m * n * sizeof(double));

	for (int i = 0; i < m * k; i++)
		a[i] = i + 1;

	for (int i = 0; i < k * n; i++)
		b[i] = i + 1;

	print_matrix(a, m, k);
	print_matrix(b, k, n);

	// test1
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, 1.0, a, k, b, n, 0.0, c, n);

	// test2
//	matmul(a, b, d, m, k, n);

	// test3
	matmul_partial(a, b, d, k, n, 0, 0, 0, 1);
	matmul_partial(a, b, d, k, n, 0, 0, 2, 3);
	matmul_partial(a, b, d, k, n, 1, 1, 0, 1);
	matmul_partial(a, b, d, k, n, 1, 1, 2, 3);
	
	print_matrix(c, m, n);
	print_matrix(d, m, n);


	free(a);
	free(b);
	free(c);
	free(d);

	return 0;
}

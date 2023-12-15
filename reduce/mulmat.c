//  gcc -o matmul matmul.c -lopenblas

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

/*
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
*/

void split(double *a, double *b, double *c, int column_of_a, int column_of_b,
		   int c_row_begin, int c_row_end, int c_col_begin, int c_col_end, int depth)
{
	int row = c_row_end - c_row_begin;
	int col = c_col_end - c_col_begin;
	int m;

	if (depth < 7 && row > 1 && col > 1 && (row >= 1 || col >= 2))
	{
		if (row > col)
		{
			m = row / 2 + c_row_begin;
			split(a, b, c, column_of_a, column_of_b, c_row_begin, m, c_col_begin, c_col_end, ++depth);
			split(a, b, c, column_of_a, column_of_b, m, c_row_end, c_col_begin, c_col_end, ++depth);
		}
		else
		{
			m = col / 2 + c_col_begin;
			split(a, b, c, column_of_a, column_of_b, c_row_begin, c_row_end, c_col_begin, m, ++depth);
			split(a, b, c, column_of_a, column_of_b, c_row_begin, c_row_end, m, c_col_end, ++depth);
		}
		//TODO: merge
	}
	else
		printf("r=%d col=%d depth=%d compute: [%d,%d), [%d,%d)\n",
				row, col, depth, c_row_begin, c_row_end, c_col_begin, c_col_end);

	matmul_partial(a, b, c, column_of_a, column_of_b ,
				   c_row_begin, c_row_end, c_col_begin, c_col_end);	
}

void split_nocopy(double *a, double *b, double *c, int column_of_a, int column_of_b,
				  int c_row_begin, int c_row_end, int c_col_begin, int c_col_end, int depth)
{
	int row = c_row_end - c_row_begin;
	int col = c_col_end - c_col_begin;
	int m;

	if (depth < 7 && row > 1 && col > 1 && (row >= 1 || col >= 2))
	{
		if (row > col)
		{
			m = row / 2 + c_row_begin;
			split_nocopy(a, b, c, column_of_a, column_of_b, c_row_begin, m, c_col_begin, c_col_end, ++depth);
			split_nocopy(a, b, c, column_of_a, column_of_b, m, c_row_end, c_col_begin, c_col_end, ++depth);
		}
		else
		{
			m = col / 2 + c_col_begin;
			split_nocopy(a, b, c, column_of_a, column_of_b, c_row_begin, c_row_end, c_col_begin, m, ++depth);
			split_nocopy(a, b, c, column_of_a, column_of_b, c_row_begin, c_row_end, m, c_col_end, ++depth);
		}
	}
	else
		printf("r=%d col=%d depth=%d compute: [%d,%d), [%d,%d)\n",
				row, col, depth, c_row_begin, c_row_end, c_col_begin, c_col_end);

	matmul_partial(a, b, c, column_of_a, column_of_b ,
				   c_row_begin, c_row_end, c_col_begin, c_col_end);	
}

void matmul_partial(double *a, double *b, double *c, int column_of_a, int column_of_b,
					int c_row_begin, int c_row_end, int c_col_begin, int c_col_end)
{
	int k = column_of_a;
	int n = column_of_b;

	for (int i = c_row_begin; i < c_row_end; i++)
	{
		for (int j = c_col_begin; j < c_col_end; j++)
		{
			c[i * n + j] = 0;
			for (int l = 0; l < k; l++)
				c[i * n + j] += a[i * k + l] * b[l * n + j];
		}
	}
}

void calculate_with_cblas(double *a, double *b, double *c, int m, int k, int n,
						  int c_row_begin, int c_row_end, int c_col_begin, int c_col_end)
{
	int row = c_row_end - c_row_begin;
	int col = c_col_end - c_col_begin;

	// 1. split
	int size_a = row * k * sizeof(double);
	int size_b = k * col * sizeof(double);
	int size_c = row * col * sizeof(double);

	double *d_a = (double *)malloc(size_a);
	double *d_b = (double *)malloc(size_b);
	double *d_c = (double *)malloc(size_c);

/*
	// k is a->get_column();
	for (int i = c_row_begin; i < c_row_end; i++)
	for (int j = 0; j < k; j ++)
		d_a[(i - c_row_begin) * k + j] = a[i * k + j];
*/

//	double *ptr_a = &a[c_row_begin * k];

	double *ptr_a = a;
	ptr_a += c_row_begin * k;
	memcpy(d_a, ptr_a, size_a);

	// k is b->get_row()
	// n is b->get_column()
	for (int i = 0; i < k; i++)
	for (int j = c_col_begin; j < c_col_end; j++)
		d_b[i * col + j - c_col_begin] = b[i * n + j];

	// 2. calculate
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, row, col, k, 1.0, d_a, k, d_b, col, 0.0, d_c, col);

	// 3. merge
	for (int i = 0; i < row; i++)
	for (int j = 0; j < col; j++)
		c[(i + c_row_begin) * col + j + c_col_begin] = d_c[i * col + j];

	free(d_a);
	free(d_b);
	free(d_c);
}

void parallel_with_cblas(double *a, double *b, double *c, int m, int k, int n,
						 int c_row_begin, int c_row_end, int c_col_begin, int c_col_end)
{
	int row = c_row_end - c_row_begin;
	int col = c_col_end - c_col_begin;
	int mid;

	if (/*depth < 7&& */row > 1 && col > 1 && (row >= 1 || col >= 2))
	{
		if (row > col)
		{
			mid = row / 2 + c_row_begin;
			parallel_with_cblas(a, b, c, m, k, n, c_row_begin, mid, c_col_begin, c_col_end);
			parallel_with_cblas(a, b, c, m, k, n, mid, c_row_end, c_col_begin, c_col_end);
		}
		else
		{
			mid = col / 2 + c_col_begin;
			parallel_with_cblas(a, b, c, m, k, n, c_row_begin, c_row_end, c_col_begin, mid);
			parallel_with_cblas(a, b, c, m, k, n, c_row_begin, c_row_end, mid, c_col_end);
		}
	}
	else
		printf("r=%d col=%d parallel_with_cblas() compute: [%d,%d), [%d,%d)\n",
			   row, col, c_row_begin, c_row_end, c_col_begin, c_col_end);

	calculate_with_cblas(a, b, c, m, k, n, c_row_begin, c_row_end, c_col_begin, c_col_end);
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
/*
    // old version
	matmul_partial(a, b, d, k, n, 0, 0, 0, 1);
	matmul_partial(a, b, d, k, n, 0, 0, 2, 3);
	matmul_partial(a, b, d, k, n, 1, 1, 0, 1);
	matmul_partial(a, b, d, k, n, 1, 1, 2, 3);
*/

/*
	matmul_partial(a, b, d, k, n, 0, 1, 0, 2);
	matmul_partial(a, b, d, k, n, 0, 1, 2, 4);
	matmul_partial(a, b, d, k, n, 1, 2, 0, 2);
	matmul_partial(a, b, d, k, n, 1, 2, 2, 4);
*/

/*
	// test with no reduce	
	matmul_partial(a, b, d, k, n, 0, 1, 0, 2);
	matmul_partial(a, b, d, k, n, 0, 1, 2, 4);
	matmul_partial(a, b, d, k, n, 1, 2, 0, 2);
	matmul_partial(a, b, d, k, n, 1, 2, 2, 4);
*/
/*
	// test with reduece
	int split_row_count = 2;
	int split_column_count = 2;

	for (int i = 0; i < split_row_count; i++)
	for (int j = 0; j < split_column_count; j++)	
	{
		int row_begin = (m + 1) / 2;
				
	}
*/

//	split_nocopy(a, b, d, k, n, 0, 2, 0, 4, 0);

//	split(a, b, d, k, n, 0, 2, 0, 4, 0);

	// test with parallel using cblas
	parallel_with_cblas(a, b, d, m, k, n, 0, m, 0, n);

	printf("\n\nfinish calculation:\n\n");
	print_matrix(c, m, n);
	print_matrix(d, m, n);

	free(a);
	free(b);
	free(c);
	free(d);

	return 0;
}

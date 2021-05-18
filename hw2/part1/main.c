#include <stdlib.h>
#include <stdio.h>

/* Parameters M,N,R,P can change from one test to another */
#define M 3
#define N 4
#define R 2
#define P 5


/* Auxilarry function to print the result. Use it to test yourselves. */
void print_result_matrix(int matrix[M][R]) {
  for(int i=0; i<M; i++) {
    for(int j=0; j<R; j++) {
      if(j<R-1) printf("%d, ",matrix[i][j]);
      else printf("%d",matrix[i][j]);
    }
    printf("\n");
  }
}


/* This function takes the pointer "matrix" to a matrix(2D array) and changing the matrix element in
 * the [row][col] location to the "value" parameter. For its implementation, the function needs also
 * the number of columns "num_of_columns" in the matrix.
 * 
 * NOTICE 1: It assumes you sent valid "row" and "col" values. If you don't do that, the behaviour
 * will be unexpected.
 * 
 * NOTICE 2: This function's specific implementation can change from one test to another (although it
 * will always do what it should, of course).
 */
void set_elemnt_in_matrix(int* matrix, int num_of_columns, int row, int col, int value) {
   *((matrix + row*num_of_columns) + col) = value;
}


/* The next 2 functions are YOUR responsibility. Do not let us down :) */ 
int get_elemnt_from_matrix(int* matrix, int num_of_columns, int row, int col);
void multiplyMatrices(int* first, int* second, int* result, int m, int n, int r, unsigned int field_size);

/* The main function is also test zone. Go wild while testing */
int main() {
  int matrix1[M][N] = { {1,2,3,8},
		       {4,7,1,1},
		       {2,3,4,1} };
  int matrix2[N][R] = { {1,1},
		       {0,1},
		       {0,0},
		       {1,1} };
  int result[M][R] = { {0,0},
		       {0,0},
		       {0,0} };
  printf("Element (1,1) in matrix1 is: %d\n", get_elemnt_from_matrix((int*)matrix1,N,1,1));
  multiplyMatrices((int*)matrix1, (int*)matrix2, (int*)result, M, N, R, P);
  print_result_matrix(result);
  return 0;
}
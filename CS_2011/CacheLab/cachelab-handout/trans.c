/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * Solution written by Adam Camilli (aocamilli) 
 * 4/27/2017
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include <stdlib.h>

#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void transpose_square_32(int M, int N, int A[N][M], int B[M][N]);
void transpose_square_64(int M, int N, int A[N][M], int B[M][N]);
void transpose_other(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 * 
 * Transposes a matrix while minimizing cache misses
 * Deals with 3 cases:
 *     32x32 matrices
 *     64x64 matrices
 *     other sizes, for example 61x67
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
  if (M == 32 && M == N) 
    transpose_square_32(M,N,A,B);
  else if (M == 64 && M == N) 
    transpose_square_64(M,N,A,B);
  else 
    transpose_other(M,N,A,B);
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

char transpose_square_32_desc[] = "Transpose 32x32 matrix";
/* transpose_square_32 - Is called by transpose_submit for 32x32 matrices 
 * 
 * To avoid misses, employs blocking as well as use of a temp variable when
 * iterating through diagonals
 */
void transpose_square_32(int M, int N, int A[N][M], int B[M][N]) {

  /* Sizeof(int) == 4, so each line can hold 8 ints. 
   * Blocking should therefore be best with block size 8 
   */
  int block_size;
  // Regular indices
  int r, c;
  // Block indices
  int cblock, rblock; 
  int diag_elt; 
  int diag_index; // Obviously row == col on diagonal, so only one index needed
  block_size = 8; // Theoretical optimal size that proved to be so in practice

  // Quadruple for-loops, here we go...
  for (c = 0; c < M; c += block_size) {
    for (r = 0; r < N; r += block_size) {
      for (rblock = r; rblock < r + block_size; rblock++) {
	for (cblock = c; cblock < c + block_size; cblock++) {
	  if (rblock != cblock) 
	    B[cblock][rblock] =	A[rblock][cblock];
	  else {
	    diag_elt = A[rblock][cblock];
	    diag_index = rblock;
	  }
	}
	// While on diagonal, use stored elt to avoid misses
	if (c == r) 
	  B[diag_index][diag_index] = diag_elt;     
      }
    }
  }
}

char transpose_square_64_desc[] = "Transpose 64x64 matrix";
/* transpose_square_64 - Is called by transpose_submit for 64x64 matrices  
 *
 * To avoid misses, uses blocking in conjunction with several local variables which 
 * act as "registers" that save misses as we iterate. 
 */ 
void transpose_square_64(int M, int N, int A[N][M], int B[M][N]) {
  // Unfortunately # of local vars is limited to 12, so we can only use 8 "registers"
  int reg1, reg2, reg3, reg4, reg5, reg6, reg7, reg8;
  // Row, column, and block indices
  int r, c, i;
  
  for (r = 0; r < N; r += 8) {
    /* Now operating with blocks */
    for (c = 0; c < M; c += 8) {
      // Work going up to catch every row
      for (i = 0; i < 8; i++){
	// "registers" to avoid misses by storing the 4 numbers in the current row in A
	reg1 = A[c + i][r];
	reg2 = A[c + i][r + 1];
	reg3 = A[c + i][r + 2];
	reg4 = A[c + i][r + 3];
	if (i == 0) {
	  // On first iteration,so store the start of each row
	  reg5 = A[c + i][r + 4];
	  reg6 = A[c + i][r + 5];
	  reg7 = A[c + i][r + 6];
	  reg8 = A[c + i][r + 7];
	}
	// Swap using registers and begin to save misses
	// 64 is the size of each row
	B[r][c + i] = reg1;
	B[r][c + i + 64] = reg2;
	B[r][c + i + 128] = reg3;
	B[r][c + i + 192] = reg4;
      }
      // Now go down, only 7 iterations since there is a
      // corner block shared by rows and columns
      for (i = 7; i > 0; i--) {
	reg1 = A[c + i][r + 4];
	reg2 = A[c + i][r + 5];
	reg3 = A[c + i][r + 6];
	reg4 = A[c + i][r + 7];
	// More misses dodged thanks to registers
	B[r + 4][c + i] = reg1;
	B[r + 4][c + i + 64] = reg2;
	B[r + 4][c + i + 128] = reg3;
	B[r + 4][c + i + 192] = reg4;
      }
      // Finish using row starts we saved at beginning of iteration, more misses averted
      B[r + 4][c] = reg5;
      B[r + 4][c + 64] = reg6;
      B[r + 4][c + 128] = reg7;
      B[r + 4][c + 192] = reg8;
    }
  }
}

/* Transposes other matrices not 32x32 or 64x64 */
char transpose_other_desc[] = "Transpose other matrices";
void transpose_other(int M, int N, int A[N][M], int B[M][N]) {
  int r, c, rblock, cblock; 
  int temp;

  /* After extensive fiddling, (16,4) and (8,1) seem to be the most stable block sizes, 
   * with (8,1) edging out when M and N are close in size. However, (8,1) does terrble 
   * with high # of rows. We therefore optimize by chosing which one is appropriate.
   *
   * This is allowed since our algorithms are permitted to be cache-aware.
   */
  int rblock_size, cblock_size;
  // Experimenting showed this threshold keeps misses < 2000 for all MxN that approximately equal 4096
  // except for those two darn edge cases (63,65) and (65,63), both of which are slightly above 2000
  //int threshold = 4; 
  if (M > 80) { // M >> N
    rblock_size = 16;
    cblock_size = 4;
  } else if (abs(M - N) > 2) { // M and N very close
    rblock_size = 8;
    cblock_size = 1;
  } else { // The sight of many attempts to solve 63,65 and vice versa, this was the best I could find
    rblock_size = 4;
    cblock_size = 1;
  }
  
  for(r = 0; r < N; r += rblock_size) {
    for(c = 0; c < M; c += cblock_size) {
      // Make sure we don't go out of bounds
      for(rblock = r; (rblock < r + rblock_size) && (rblock < N); rblock++) {
	for(cblock = c; (cblock < c + cblock_size) && (cblock < M); cblock++) {
	  // Store diagonals like we did for 32x32
	  if (rblock - r == cblock - c) 
	    temp = A[rblock][cblock];	  
	  else 
	    B[cblock][rblock] =	A[rblock][cblock];	 
	}
	for (cblock = c; (cblock < c + cblock_size) && (cblock < M); cblock++) {
	  if (rblock - r == cblock - c) 
	    B[cblock][rblock] = temp;	  
	}
      }
    }
  }
}

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions() {
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}


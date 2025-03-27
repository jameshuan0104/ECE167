/* 
 * File:   MatrixMath.c
 *
 * Created on April 18, 2021, 2:49 AM
 */

// Standard libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// User libraries:
#include "MatrixMath.h"

#define FALSE 0
#define TRUE 1

/**
 * MatrixPrint displays a 3x3 array to standard output with clean, readable, 
 * consistent formatting.
 */
void MatrixPrint(float mat[3][3]) {
    printf(" ----------------------------------- \n");
    int y, x;
    for (y = 0; y < DIM; y++) {
        printf("|");
        for (x = 0; x < DIM; x++) {
            printf(" %9.4f |", mat[y][x]);
        }
        printf("\n ----------------------------------- \n");
    }
    printf("\n");
    return;
}

/**
 * MatrixEquals checks if the two matrix arguments are equal (to within FP_DELTA).
 */
int MatrixEquals(float mat1[3][3], float mat2[3][3]) {
    int y, x;
    for (y = 0; y < DIM; y++) {
        for (x = 0; x < DIM; x++) {
            if (abs(mat1[y][x] - mat2[y][x]) > FP_DELTA) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

/**
 * MatrixAdd performs an element-wise matrix addition operation on two 3x3 matrices and 
 * "returns" the result by modifying the third argument.
 */
void MatrixAdd(float mat1[3][3], float mat2[3][3], float result[3][3]) {
    int y, x;
    for (y = 0; y < DIM; y++) {
        for (x = 0; x < DIM; x++) {
            result[y][x] = mat1[y][x] + mat2[y][x];
        }
    }
}

/**
 * MatrixMultiply performs a matrix-matrix multiplication operation on two 3x3
 * matrices and "returns" the result by modifying the third argument.
 */
void MatrixMultiply(float mat1[3][3], float mat2[3][3], float result[3][3]) {
    int y, x, i;
    for (y = 0; y < DIM; y++) {
        for (x = 0; x < DIM; x++) {
            result[y][x] = 0;
            for (i = 0; i < DIM; i++) {
                result[y][x] += (mat1[y][i] * mat2[i][x]);
            }
        }
    }
}

/**
 * MatrixVectorMultiply performs a matrix-vector multiplication operation on a 3x3 and a 3x1
 * matrices and "returns" the result by modifying the third argument.
 */
void MatrixVectorMultiply(float mat[3][3], float vec[3], float result[3]) {
    int y, x;
    for (y = 0; y < DIM; y++) {
        result[y] = 0;
        for (x = 0; x < DIM; x++) {
            result[y] += mat[y][x] * vec[x];
        }
    }

}

/**
 * MatrixScalarAdd performs the addition of a matrix and a scalar.  Each element 
 * of the matrix is increased by x. The result is "returned"by modifying the third argument.
 */
void MatrixScalarAdd(float x, float mat[3][3], float result[3][3]) {
    int y, xx;
    for (y = 0; y < DIM; y++) {
        for (xx = 0; xx < DIM; xx++) {
            result[y][xx] = mat[y][xx] + x;
        }
    }
}

/**
 * MatrixScalarMultiply performs the multiplication of a matrix and a scalar.
 * Each element of the matrix is multiplied x.
 */
void MatrixScalarMultiply(float x, float mat[3][3], float result[3][3]) {
    int y, xx;
    for (y = 0; y < DIM; y++) {
        for (xx = 0; xx < DIM; xx++) {
            result[y][xx] = x * mat[y][xx];
        }
    }
}

/**
 * MatrixTrace calculates the trace of a 3x3 matrix.
 */
float MatrixTrace(float mat[3][3]) {
    int i;
    float ans = 0;
    for (i = 0; i < DIM; i++) {
        ans += mat[i][i];
    }
    return ans;
}

/**
 * MatrixTranspose calculates the transpose of a matrix and "returns" the
 * result through the second argument.
 */
void MatrixTranspose(float mat[3][3], float result[3][3]) {
    int y, x;
    for (y = 0; y < DIM; y++) {
        for (x = 0; x < DIM; x++) {
            result[x][y] = mat[y][x];
        }
    }
}

/**
 * MatrixSubmatrix finds a submatrix of a 3x3 matrix that is 
 * formed by removing the i-th row and the j-th column.  The 
 * submatrix is "returned" by modifying the final argument.
 */
void MatrixSubmatrix(int i, int j, float mat[3][3], float result[2][2]) {
    int y, x, yy, xx;
    for (y = 0, yy = 0; yy < 2; y++, yy++) {
        for (x = 0, xx = 0; xx < 2; x++, xx++) {
            if (y == i) {
                y++;
            }
            if (x == j) {
                x++;
            }
            result[yy][xx] = mat[y][x];
        }
    }
}

/**
 * MatrixDeterminant calculates the determinant of a 3x3 matrix 
 * and returns the value as a float.
 */
float MatrixDeterminant(float mat[3][3]) {
    float ans = 0;
    float temp[2][2];
    MatrixSubmatrix(0, 0, mat, temp);
    ans += mat[0][0] * (temp[0][0] * temp[1][1] - temp[1][0] * temp[0][1]);
    MatrixSubmatrix(0, 1, mat, temp);
    ans -= mat[0][1] * (temp[0][0] * temp[1][1] - temp[1][0] * temp[0][1]);
    MatrixSubmatrix(0, 2, mat, temp);
    ans += mat[0][2] * (temp[0][0] * temp[1][1] - temp[1][0] * temp[0][1]);
    return ans;
}

/* MatrixInverse calculates the inverse of a matrix and
 * "returns" the result by modifying the second argument.
 */
void MatrixInverse(float mat[3][3], float result[3][3]) {
    int y, x;
    float temp[2][2];
    float temp2[3][3] = {
        {},
        {},
        {}
    };
    float temp3[3][3];
    for (y = 0; y < DIM; y++) {
        for (x = 0; x < DIM; x++) {
            MatrixSubmatrix(y, x, mat, temp);
            if ((y + x) % 2 == 0) {
                temp2[y][x] = temp[0][0] * temp[1][1] - temp[1][0] * temp[0][1];
            } else {
                temp2[y][x] = -1 * (temp[0][0] * temp[1][1] - temp[1][0] * temp[0][1]);
            }
        }
    }
    MatrixTranspose(temp2, temp3);
    MatrixScalarMultiply(1.0 / MatrixDeterminant(mat), temp3, result);
}

/* VectorScalarMultuply multiplies a 3D vector by a scalar and 
 * "returns" the result by modifying the third argument.
 */
void VectorScalarMultiply(float x, float vec[3], float result[3]) {
    int i;
    for (i = 0; i < DIM; i++) {
        result[i] = x * vec[i];
    }
}

// #define MML_TEST
#ifdef MML_TEST

#include <Board.h>

// Module-level variables:
float zero_matrix[3][3] = {
    {},
    {},
    {}
};


int main() {
    
    BOARD_Init();

    int test1, test2;
    int passes = 0;
    float functionResult1[3][3];
    float functionResult2[2][2], functionResult3[2][2];

    // Matrices tested
    float testMatrix1[3][3] = {
        {-1.3120, -5.4243, 3.2310},
        {6.6540, 4.5700, 7.8790},
        {3.6700, -8.5400, 2.2130}
    };
    float testMatrix2[3][3] = {
        {1.3210, 3.4500, 3.4500},
        {6.4500, 4.5000, 4.6120},
        {3.9090, 4.4000, 3.2000}
    };
    float testMatrix3[3][3] = {
        {14, 2, 5},
        {9, 25, 6},
        {1, 19, 21}
    };
    float testMatrix4[3][3] = {
        {2, 14, 32},
        {3, 18, 10},
        {1, 12, 30}
    };

    // Print greeting
    printf("Beginning mml test harness, compiled on %s %s\n", __DATE__, __TIME__);

    // MatrixEquals Test
    test1 = MatrixEquals(zero_matrix, zero_matrix);
    test2 = MatrixEquals(testMatrix1, testMatrix2);
    if (test1 != 0 && test2 == 0) {
        printf("PASSED (2/2): MatrixEquals()\n");
        passes++;
    } else if (test1 != 0 || test2 == 0) {
        printf("PASSED (1/2): MatrixEquals()\n");
    } else {
        printf("PASSED (0/2): MatrixEquals()\n");
    }

    // MatrixAdd Test
    float answerAdd1[3][3] = {
        {0.0090, -1.9743, 6.6810},
        {13.1040, 9.0700, 12.4910},
        {7.5790, -4.1400, 5.4130}
    };
    float answerAdd2[3][3] = {
        {16, 16, 37},
        {12, 43, 16},
        {2, 31, 51}
    };
    MatrixAdd(testMatrix1, testMatrix2, functionResult1);
    test1 = MatrixEquals(functionResult1, answerAdd1);
    MatrixAdd(testMatrix3, testMatrix4, functionResult1);
    test2 = MatrixEquals(functionResult1, answerAdd2);
    if (test1 != 0 && test2 != 0) {
        printf("PASSED (2/2): MatrixAdd()\n");
        passes++;
    } else if (test1 != 0 || test2 != 0) {
        printf("PASSED (1/2): MatrixAdd()\n");
    } else {
        printf("PASSED (0/2): MatrixAdd()\n");
    }

    // MatrixMultiply Test
    float answerMultiply1[3][3] = {
        {-24.0899, -14.7193, -19.2041},
        {69.0654, 78.1889, 69.2459},
        {-41.5843, -16.0313, -19.6434}
    };
    float answerMultiply2[3][3] = {
        {39, 292, 618},
        {99, 648, 718},
        {80, 608, 852}
    };
    MatrixMultiply(testMatrix1, testMatrix2, functionResult1);
    test1 = MatrixEquals(functionResult1, answerMultiply1);
    MatrixMultiply(testMatrix3, testMatrix4, functionResult1);
    test2 = MatrixEquals(functionResult1, answerMultiply2);
    if (test1 != 0 && test2 != 0) {
        printf("PASSED (2/2): MatrixMultiply()\n");
        passes++;
    } else if (test1 != 0 || test2 != 0) {
        printf("PASSED (1/2): MatrixMultiply()\n");
    } else {
        printf("PASSED (0/2): MatrixMultiply()\n");
    }

    // MatrixScalarAdd Test
    float answerScalarAdd1[3][3] = {
        {-0.3120, -4.4243, 4.2310},
        {7.6540, 5.5700, 8.8790},
        {4.6700, -7.5400, 3.2130}
    };
    float answerScalarAdd2[3][3] = {
        {8.6800, -3.3200, -0.3200},
        {3.6800, 19.6800, 0.6800},
        {-4.3200, 13.6800, 15.6800}
    };
    MatrixScalarAdd(1, testMatrix1, functionResult1);
    test1 = MatrixEquals(functionResult1, answerScalarAdd1);
    MatrixScalarAdd(-5.32, testMatrix3, functionResult1);
    test2 = MatrixEquals(functionResult1, answerScalarAdd2);
    if (test1 != 0 && test2 != 0) {
        printf("PASSED (2/2): MatrixScalarAdd()\n");
        passes++;
    } else if (test1 != 0 || test2 != 0) {
        printf("PASSED (1/2): MatrixScalarAdd()\n");
    } else {
        printf("PASSED (0/2): MatrixScalarAdd()\n");
    }

    // MatrixScalarMultiply Test
    float answerScalarMultiply1[3][3] = {
        {},
        {},
        {}
    };
    float answerScalarMultiply2[3][3] = {
        {20.3280, 2.9040, 7.2600},
        {13.0680, 36.3000, 8.7120},
        {1.4520, 27.5880, 30.4920}
    };
    MatrixScalarMultiply(0, testMatrix1, functionResult1);
    test1 = MatrixEquals(functionResult1, answerScalarMultiply1);
    MatrixScalarMultiply(1.452, testMatrix3, functionResult1);
    test2 = MatrixEquals(functionResult1, answerScalarMultiply2);
    if (test1 != 0 && test2 != 0) {
        printf("PASSED (2/2): MatrixScalarMultiply()\n");
        passes++;
    } else if (test1 != 0 || test2 != 0) {
        printf("PASSED (1/2): MatrixScalarMultiply()\n");
    } else {
        printf("PASSED (0/2): MatrixScalarMultiply()\n");
    }

    // MatrixTrace Test
    test1 = (abs(MatrixTrace(testMatrix1) - 5.4710) < FP_DELTA);
    test2 = (abs(MatrixTrace(testMatrix3) - 60) < FP_DELTA);
    if (test1 != 0 && test2 != 0) {
        printf("PASSED (2/2): MatrixTrace()\n");
        passes++;
    } else if (test1 != 0 || test2 != 0) {
        printf("PASSED (1/2): MatrixTrace()\n");
    } else {
        printf("PASSED (0/2): MatrixTrace()\n");
    }

    // MatrixTranspose Test
    float answerTranspose1[3][3] = {
        {-1.3120, 6.6540, 3.6700},
        {-5.4243, 4.5700, -8.5400},
        {3.2310, 7.8790, 2.2130}
    };
    float answerTranspose2[3][3] = {
        {14, 9, 1},
        {2, 25, 19},
        {5, 6, 21}
    };
    MatrixTranspose(testMatrix1, functionResult1);
    test1 = MatrixEquals(functionResult1, answerTranspose1);
    MatrixTranspose(testMatrix3, functionResult1);
    test2 = MatrixEquals(functionResult1, answerTranspose2);
    if (test1 != 0 && test2 != 0) {
        printf("PASSED (2/2): MatrixTranspose()\n");
        passes++;
    } else if (test1 != 0 || test2 != 0) {
        printf("PASSED (1/2): MatrixTranspose()\n");
    } else {
        printf("PASSED (0/2): MatrixTranspose()\n");
    }

    // MatrixSubmatrix Test
    int i, j;
    test1 = 1;
    test2 = 1;
    float answerSubmatrix1[2][2] = {
        {-5.4243, 3.2310},
        {-8.5400, 2.2130}
    };
    float answerSubmatrix2[2][2] = {
        {9, 25},
        {1, 19}
    };
    MatrixSubmatrix(1, 0, testMatrix1, functionResult2);
    MatrixSubmatrix(0, 2, testMatrix3, functionResult3);
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            if (abs(functionResult2[i][j] - answerSubmatrix1[i][j]) > FP_DELTA) {
                test1 = 0;
            }
            if (abs(functionResult3[i][j] - answerSubmatrix2[i][j]) > FP_DELTA) {
                test2 = 0;
            }
        }
    }
    if (test1 != 0 && test2 != 0) {
        printf("PASSED (2/2): MatrixSubmatrix()\n");
        passes++;
    } else if (test1 != 0 || test2 != 0) {
        printf("PASSED (1/2): MatrixSubmatrix()\n");
    } else {
        printf("PASSED (0/2): MatrixSubmatrix()\n");
    }

    //   MatrixDeterminant Test
    test1 = (abs(MatrixDeterminant(testMatrix1) - -416.3152) < FP_DELTA);
    test2 = (abs(MatrixDeterminant(testMatrix3) - 6118) < FP_DELTA);
    if (test1 != 0 && test2 != 0) {
        printf("PASSED (2/2): MatrixDeterminant()\n");
        passes++;
    } else if (test1 != 0 || test2 != 0) {
        printf("PASSED (1/2): MatrixDeterminant()\n");
    } else {
        printf("PASSED (0/2): MatrixDeterminant()\n");
    }

    // MatrixInverse Test
    float answerInverse1[3][3] = {
        {-0.2885, 0.2027, 0.0189},
        {-0.1278, -0.4532, 0.7910},
        {0.5281, 0.3756, -0.7983}
    };
    float answerInverse2[3][3] = {
        {1.4189, -0.1216, -1.4730},
        {-0.2703, 0.0946, 0.2568},
        {0.0608, -0.0338, -0.0203}
    };
    MatrixInverse(testMatrix2, functionResult1);
    test1 = MatrixEquals(functionResult1, answerInverse1);
    MatrixInverse(testMatrix4, functionResult1);
    test2 = MatrixEquals(functionResult1, answerInverse2);
    if (test1 != 0 && test2 != 0) {
        printf("PASSED (2/2): MatrixInverse()\n");
        passes++;
    } else if (test1 != 0 || test2 != 0) {
        printf("PASSED (1/2): MatrixInverse()\n");
    } else {
        printf("PASSED (0/2): MatrixInverse()\n");
    }

    // Print function pass-rate
    printf("----------------------------------------\n");
    printf("%d out of 10 functions passed (%.1f%%)", passes, 100.0 * (float) passes / 10.0);
    
    
    // MatrixPrint Test
    printf("\n\nOutput of MatrixPrint():\n");
    MatrixPrint(testMatrix1);
    printf("Expected output of MatrixPrint(): \n");
    printf(" ----------------------------------- \n");
    printf("|   -1.3120 |   -5.4243 |    3.2310 |\n");
    printf(" ----------------------------------- \n");
    printf("|    6.6540 |    4.5700 |    7.8790 |\n");
    printf(" ----------------------------------- \n");
    printf("|    3.6700 |   -8.5400 |    2.2130 |\n");
    printf(" ----------------------------------- \n");

    while (1);
}

#endif
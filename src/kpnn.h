/*
    Drofa - UCI compatable chess engine
        Copyright (C) 2017 - 2019  Rhys Rustad-Elliott
                      2020 - 2023  Litov Alexander
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef KPNN_H
#define KPNN_H

#include <math.h>


// use net KPnet => 264x8x2 => opS /egS outputs
#define N_INPUTS   (64 * 2 * 2)
#define N_HIDDEN   (16)

int OUTPUT_BIAS1 = -15;
int OUTPUT_BIAS2 = 3;
int HIDDEN_BIAS[N_HIDDEN] = {23, 10, 22, 17, 13, 3, -23, 3, -17, 10, -3, 13, 20, -6, 16, 19, };
int OUTPUT_WEIGHTS1[N_HIDDEN] = {13, -47, -7, 35, -15, 32, 16, 14, -14, -9, -7, 33, -14, 22, 16, -35, };
int OUTPUT_WEIGHTS2[N_HIDDEN] = {8, 17, 24, 2, 1, 2, -9, -7, -5, -14, -12, -22, -7, -6, -1, 1, };


int HIDDEN_WEIGHTS[N_INPUTS * N_HIDDEN] = {
          3, 1, -3, 1, -4, 2, 4, -1, -3, 3, -4, -1, -1, 2, -13, -31, 1, 2, -3, -0, 0, 5, -3, -29, 3, 1, -2, -0, -2, -1, -5, 4, -3,
          2, -1, 85, -7, 3, -4, 6, -1, -1, 3, -4, -6, -3, 3, 0, 12, 2, 0, 18, -3, 70, -11, -4, -4, 0, 4, -2, -2, -4, -3, 0, 1,
          -1, 2, 0, -1, -4, -3, -4, -30, -39, -36, -1, 13, 22, 9, 23, -1, -2, -11, -5, 26, 57, 12, -6, -3, 1, 5, -0, 19, 4, 2, 5, -3,
          -2, -3, 37, -1, 1, -0, 4, -3, 0, 43, 2, -2, -3, 2, 4, -2, 6, 2, -6, -2, -3, 2, 3, 0, 4, 0, -1, 3, 5, 0, 5, 5,
          19, 15, 45, 9, 22, 3, 2, 61, 61, 52, 54, 28, 27, -12, -5, 12, 46, 60, 68, 36, 42, -2, 13, 8, 12, 50, -47, 38, 18, 36, 6, 37,
          23, 8, 29, -51, 34, 59, 20, -5, 9, 7, -48, -58, -39, 42, -45, -15, 9, 31, -3, -45, 4, 33, -52, -9, -18, 11, -6, -18, 21, -4, -26, -4,
          -31, -3, -29, -3, -10, 2, 19, 11, -18, -22, 31, 26, 60, 7, 3, -33, 3, 3, 17, 21, 6, 9, 16, 1, 2, -4, -12, 26, 13, -2, 6, 5,
          -25, 19, -1, 5, 13, 5, 12, 6, 14, 7, 7, 8, -2, 10, 8, 6, 15, 22, 22, 10, 7, 7, 7, 44, 10, 15, -15, 12, 6, 7, 6, 2,
          4, -2, -2, 0, 0, -3, -4, 7, -1, -6, 1, -2, 3, 3, -1, 11, 9, 1, -0, 2, 2, 0, -0, -1, -2, -2, -0, 1, 3, 2, 1, -3,
          -7, -27, -11, 0, 6, 3, 4, -2, -24, -19, -11, -5, 7, 20, 16, -62, -45, -40, -11, -9, -1, -30, -46, 4, 0, -4, 5, -2, 2, 2, 4, 1,
          -3, 4, 5, -4, 4, 2, 1, 6, 12, -3, 0, -1, -0, 6, 40, 5, 15, 11, 0, 6, -9, -4, -5, 10, 8, 7, 6, 0, -4, -4, -4, 8,
          14, 6, 3, 0, -3, -4, -3, 5, 9, 6, 2, -2, -3, -3, -2, 7, 8, 3, 2, -2, -5, -3, -3, 3, -4, -2, -3, -1, 0, 2, 4, -2,
          -5, -4, -9, 13, 10, 1, 4, -7, -12, -12, -8, 7, 8, 9, 3, -16, -7, -20, -9, 5, 2, 25, 5, -8, -19, -20, 17, 12, 11, 31, 2, -6,
          -13, -14, -12, 22, -68, 14, -11, 23, -7, -19, -43, -33, -22, -56, 1, 7, -10, -31, -45, -2, -10, 18, 23, -53, -53, -28, -30, -7, -51, 16, -58, 45,
          1, 50, 2, -10, 14, 23, -3, 31, -23, 6, -9, -6, 6, 0, 59, 30, 13, 40, 75, -2, 0, 24, 20, -29, -1, 32, -3, 1, 2, 54, 52, -11,
          -8, -8, -8, -2, 28, 29, 50, -14, -15, -9, -3, 2, 37, 39, 23, -19, -15, -23, -18, -3, 18, 30, 33, -13, -17, -17, -8, 10, 13, 28, 32, 3,
          1, 1, -3, 5, -1, -3, 3, 4, 3, 6, 6, 2, 9, 7, 11, 5, 5, 6, 4, 5, 7, 9, 12, 4, 7, 7, 11, 9, 9, 9, 15, 7,
          7, 10, 11, 18, 8, 12, 17, 9, 8, 16, 8, 21, 10, -95, 15, -14, 10, -2, -3, 2, -72, -50, -73, 2, 4, 3, 5, -3, 5, 5, -4, 0,
          5, 5, -1, -4, 0, 5, 0, 48, 6, 13, -7, -2, -2, -4, -16, -11, -8, -15, -11, -16, -11, -11, -9, -8, -11, -12, -14, -14, -14, -11, -12, -11,
          -14, -11, -12, -11, -10, -14, -15, -11, -10, -12, -10, -12, -9, -15, -16, -11, -11, -15, -9, -11, -9, -14, -15, -4, 5, -1, 2, -3, 3, 3, -2, -42,
          2, 6, -1, -5, 5, 5, -135, 5, 1, 4, 1, 4, 4, 5, 5, 5, -0, 2, 0, -0, 3, 2, 7, -8, -1, -2, -3, -2, 1, 2, 1, -7,
          -0, -3, -8, -1, -3, 1, -4, -11, -7, -2, -6, -5, -6, -3, -8, -13, -3, -8, -3, -10, -8, -12, -13, 0, -5, -14, -3, -12, -9, -13, -25, 61,
          -35, 73, -20, 0, -12, -13, 2, -3, -15, -13, 78, -10, -13, -15, -5, -11, -14, -17, -9, -8, -13, -10, -6, -7, -11, -13, -11, -7, -10, -9, -5, -2,
          -7, -5, -7, -7, -6, -4, 48, -34, -0, -3, -5, -3, -0, 35, 33, 6, 4, 0, -6, -5, 2, 9, 53, 18, -1, 0, 33, 11, 21, 3, 29, 4,
          0, 4, -3, 1, 0, -4, 4, -5, -8, -4, 2, -8, 1, 9, 20, -6, -7, -6, -1, 0, 4, 9, 11, -7, -10, -7, -4, 3, 9, 11, 11, -13,
          -11, -10, -6, 5, 18, 11, 9, -13, -17, -11, -6, 0, 8, 12, -7, -12, -14, -10, 11, 28, 27, 41, 1, 1, -4, 3, 1, 1, 0, 2, 0, -3,
          3, 2, 5, 5, 2, 4, -3, -15, -25, -26, -15, -19, -12, 6, 28, 5, -3, -11, -17, -13, -10, -7, 16, -2, -1, 2, -3, -12, -10, -0, 3, -3,
          -1, -3, -1, -2, 2, 3, 1, -3, -2, -2, -0, 7, 7, 7, 9, -1, 2, -3, 6, 9, 5, -3, 4, 4, -3, -3, 0, 1, 5, 5, 0, 30,
          45, 34, 0, -10, -12, -9, -14, 36, 43, 13, -6, -9, -6, -9, -7, 2, 10, 15, 5, -4, 4, -1, -13, 16, 11, 9, 14, 11, 11, 1, -27, 4,
          36, 16, 11, 4, -2, -13, -15, 12, 6, 26, 13, 2, -18, -17, -13, 14, 45, -4, 6, 10, -15, -12, -29, -11, 11, -8, 2, -38, -7, -5, -4, -13,
          -22, 8, 6, 1, -17, 24, 30, 6, 9, -14, -17, -8, -4, -8, 25, -3, -30, -5, 6, -2, 5, 6, 19, -8, -39, -20, -8, -2, -2, 8, 8, -25,
          -26, -24, -6, -3, 1, 4, 6, -15, 3, -2, 4, -0, -2, -1, -1, -2, 11, 16, 4, 0, 1, -1, -8, -11, -4, 5, 6, 17, 15, 7, 1, 3,
          3, 1, 0, 3, -1, -4, -1, 4, -2, 10, -11, 2, -4, -6, -5, 4, 6, 10, 9, 0, -1, -3, 3, -3, -3, -3, -0, 6, -2, -8, -1, -9,
          -0, -4, -3, 8, 22, 3, 12, -8, -9, -3, 0, 13, 20, 2, 69, -17, -19, -35, -3, -5, -46, -12, -23, 0, 5, -3, 0, 4, -2, -2, -2, 3,
          -2, 1, 2, -1, -3, -2, 2, 6, 8, 13, -12, 5, 8, -14, 46, 2, 28, 13, -28, -1, -6, -12, -25, 14, 9, 16, 8, -6, -9, -11, -10, 14,
          11, 11, 5, -8, -9, -12, -9, 9, 6, 7, 2, 2, -12, -5, -22, 7, 4, 6, -6, 4, 14, -10, -16, 1, -2, 1, 0, 1, -4, -2, -4, 3,
          -3, 2, -3, 7, 12, 7, 13, -15, 61, 21, 25, 8, 7, 16, -2, 2, 15, -32, -39, -43, -0, 14, 14, -49, -29, -19, -38, 12, -40, -17, 42, 12,
          -57, -8, 1, -39, -12, 10, -13, -5, -31, -34, -15, 19, 54, -48, 16, 10, -11, -28, -12, 48, 69, 26, 7, 9, -15, -13, -42, -16, -40, 48, -9, -14,
          32, 7, -14, -19, -11, 4, -0, 8, 2, 10, -24, -34, -5, -42, -37, -23, 25, -6, -3, -25, 63, 44, -38, -26, 27, 74, 77, 74, 45, 68, -6, 18,
          -1, 16, 57, 38, 47, 25, 37, -8, 10, 65, 70, 54, 32, 16, 25, -3, -8, 32, 4, 11, 21, 7, 7, -14, -14, -19, 5, -11, 21, 0, 9, 4,
          4, 2, -3, -2, 0, 5, -3, 9, 17, 8, 4, -3, 2, 0, -2, 8, 15, 7, 2, -0, -2, -1, -3, 8, 25, 6, 4, -1, -2, -2, -4, 8,
          16, 8, 3, -5, -6, -5, -5, 11, 32, 39, 16, -1, -22, 0, -9, -22, -9, -17, 35, 37, 9, 54, 24, 3, -2, 5, -3, 1, 5, -1, 0, 2,
          -1, 2, 1, 1, 4, -3, 5, 68, -12, -4, -21, -6, -2, -10, -38, -35, -32, -27, -23, -4, 5, 1, 6, -11, -11, -8, -9, -1, 4, 3, 2, -2,
          -4, -4, -0, -1, 2, 2, 0, 19, 15, 4, 1, 3, 1, 1, 1, 18, -4, -11, 1, -0, 0, 2, 1, 0, -2, 5, 4, 0, 3, 2, 5, -17,
          -17, -20, -22, -20, 20, 19, 20, -10, -14, -22, 11, 25, 24, 23, 20, -15, -16, -18, 54, 24, 26, 26, 25, -18, -22, 18, 45, 41, 34, 28, 23, 0,
          -12, -16, -26, 47, 34, 34, 27, -20, 5, -11, -30, -23, 26, 31, 17, -43, -10, -21, -23, 19, -58, 27, -26, 9, -2, -3, 1, -19, 9, 16, -20, 31,
          -0, -20, -10, -18, -4, -15, 19, 58, -18, -13, -19, -14, -16, -10, -5, -20, -29, -20, -19, -15, -13, -17, -22, -13, -20, -20, -16, -19, -17, -21, -6, 29,
          -17, -20, -11, -17, -13, -15, 12, -6, -15, -26, -18, -10, -9, 17, 55, -11, -13, -11, -13, 6, 24, 16, 14, -8, -6, -2, 5, 29, 55, 7, 7, -1,
          -1, 3, -3, -4, -3, 3, -2, -1, 1, -1, -1, -2, -4, -3, -3, -0, 2, -1, -2, -2, -1, -5, -4, 1, 1, 1, -2, -2, -5, -3, -4, 0,
          1, 2, -2, -3, -5, -23, 0, -2, 3, 4, -7, -5, -10, -16, -5, 13, 18, 15, 4, 69, 69, -7, 1, -1, -2, -1, 0, -3, -4, 4, 3, -1,
          -3, -3, 1, -3, 3, 0, 5, -43, -14, -66, -46, -105, -15, -10, -77, 22, -2, 2, -1, 2, 3, -27, -1, -0, 0, 0, 0, 2, 9, -0, -84, -2,
          -2, 0, 0, 1, 7, -79, 15, -1, -1, 1, -1, 1, 3, 24, 21, 1, -1, 0, -0, -0, 25, 16, 18, 4, 3, 1, -4, 4, 5, -1, -3, -12,
          -16, -13, -1, 0, -6, -4, -6, -11, -19, -1, -15, 0, -4, -1, -1, -12, 5, 1, -3, -3, 4, -2, 70, -12, 8, 12, 1, -1, 0, 58, 85, -69,
          5, -31, 4, 5, 8, 82, 5, -30, -36, 2, 5, 85, -63, 2, 91, 20, -20, 2, 7, 8, 4, 10, 5, 22, 32, 18, -16, -11, 15, 8, -11, -53,
          -57, 11, 2, -58, 55, -37, 3, -28, 27, 5, -35, 14, -52, -0, -40, 9, 8, -10, -52, -14, 4, 0, 7, -0, 24, 20, 14, 12, 2, 5, -5, 24,
          21, 19, 20, 13, 18, 4, -8, 16, 19, 21, 19, 22, 14, -20, 20, 18, 12, 5, 22, -38, 46, 54, -16, 5, 8, 3, -29, -46, 74, -20, -2, -3,
          -1, -2, 3, 2, 1, 1, -1, 3, -35, -6, 0, 13, 0, -4, 3, 7, -2, -62, -2, 6, -3, -3, -1, -3, 21, 3, -83, 3, -3, -1, 1, 48,
          3, -1, 4, -13, -0, 7, 3, -9, 22, 6, 8, 13, 7, 6, 13, 36, 31, 35, 16, 28, 5, -5, -11, 4, -3, 1, 2, -2, -3, 2, 1, 4,
          -3, 0, 4, -2, 1, -3, 3, -19, -11, -63, -2, -29, -28, -13, 27, 2, -23, -27, 1, 13, -18, -17, -4, 6, -2, -4, -58, 2, -5, 10, -2, -4,
          -1, 2, -0, -4, 3, 4, -3, -3, -5, 1, 2, -2, -1, -3, -1, 1, -4, 1, 3, -1, -2, 3, -5, 5, -4, -4, -4, 5, 3, 5, 4, -14,
          2, -5, 26, -9, -15, -4, -14, -31, -12, -22, -25, 30, -1, 3, -0, -5, 29, 59, 53, 73, 55, 59, -10, -38, 5, 15, -4, 55, 51, 85, -8, 5,
          -8, 49, -1, 54, -6, 47, -1, 14, 18, 16, -6, 32, 6, -13, -2, 20, -26, 30, 21, 8, 80, 43, -17, 38, 32, 35, 45, -47, -9, -9, 17, -15,
          -44, -45, 19, -9, 22, -71, 14, 15, -22, 1, 13, 44, -11, -12, -11, -47, -28, -32, -27, 16, -17, -18, 18, -36, -18, -55, 10, -14, -22, -43, 24, 22,
          -24, -32, -14, -19, -38, -60, -7, -32, -27, -7, -43, -41, -40, 4, -51, 2, -0, 3, -19, -46, -18, 7, 5, 15, 2, 6, -1, 13, 16, 7, 7, 3,
          0, -2, 3, -4, -4, 0, -3, 3, 7, 3, 7, 8, -0, -1, -4, 2, 6, 3, 6, 0, 1, 1, -1, -2, 4, 5, 6, 2, -8, -3, -3, 7,
          8, 5, 1, -5, 7, 7, 27, 15, 10, 2, -5, -30, -2, 6, -18, 9, 11, 0, -8, -2, -42, -46, 25, 5, 4, 1, 4, -1, 1, -1, -4, 3,
          -1, 3, 5, -4, 0, 3, 3, 17, 50, 17, 9, 45, 36, -3, -5, -15, 5, 9, -2, 6, 7, -9, -20, 6, 9, 7, 5, 4, -0, -1, 1, 7,
          9, 7, -1, -3, -3, -12, 2, 5, 5, 1, 1, -1, 16, 6, 20, 4, 5, 3, -5, -1, -14, -49, 7, 4, -2, 1, -2, -4, -1, -3, -2, 4,
          7, 6, -3, 12, -7, -2, 6, 14, -6, -5, -13, 1, -6, -1, 1, 34, 1, -14, -6, -33, -7, -1, -5, 36, -35, -45, -41, -58, -30, -21, -1, -26,
          -43, -46, -32, -26, -67, -35, -19, 29, -41, -3, -10, -18, -49, -9, 12, 25, 6, -18, -3, 2, 10, -12, -2, 4, 5, -17, -12, -14, -22, 28, -7, -25,
          21, 32, -1, -3, 10, 12, -6, -31, 21, 14, 8, 11, -5, -8, 9, 18, 19, 21, -3, 11, 12, 21, 14, 17, 41, 23, 7, 9, 1, 11, -17, 33,
          15, 44, 13, 11, 12, 2, -23, 23, -33, 18, 11, 5, 9, -5, -3, 10, 16, 12, 4, 10, -57, -26, -31, 20, 20, 12, 40, -57, -7, -17, -14, 1,
          3, 1, 0, 2, -4, 4, 5, 3, 10, 12, 5, 3, -11, 9, 11, -0, 4, 7, 8, -0, -1, 7, 4, 1, 1, 5, 8, 7, 3, 4, 7, 4,
          -3, 1, 16, 7, 14, 7, 3, -15, -1, -3, -1, 13, 10, 8, 3, -27, -1, -5, -3, 9, -4, 12, 6, 0, -1, 1, -3, 1, 2, 0, -1, -4,
          -4, 2, 0, 0, 3, 5, -4, 7, -20, 2, -13, -23, -11, -19, -22, -13, -4, -59, -4, -8, 2, -8, -14, -6, -1, 6, 1, -1, -6, -9, -11, 1,
          5, 5, -1, -3, -6, -8, -13, -1, 7, 2, -4, -5, 12, -10, -11, 6, 5, 1, 11, 3, 2, -2, -17, -2, 2, 1, 5, 2, -4, 2, 1, 68,
          46, 54, 24, 42, 5, -9, -6, 21, 18, 5, 17, 8, -9, -12, -8, 13, -6, 1, -2, -9, -9, -4, -6, 47, 12, 4, -10, 0, -9, -4, -5, 27,
          3, 3, 12, -4, -4, -8, -8, 24, 72, 33, 24, 15, -3, 10, 11, 46, 73, 30, 28, 27, 39, 22, 6, 28, 11, -14, 29, 11, 27, 49, -17, -71,
          -39, -29, -1, 10, -22, -5, -17, -60, -68, -26, -53, -38, -40, -15, -16, -35, -19, -3, -58, -22, 5, 2, -1, 12, -32, 33, -7, 2, 1, 2, 4, -27,
          39, 74, 11, 0, 2, -1, 2, -18, 45, 40, 45, 37, 4, 4, 1, 15, 38, 29, 32, 22, 6, -1, 1, -30, -4, 2, -36, -12, -2, 3, 0, -2,
          5, 5, 3, -2, 3, 2, 0, -6, -2, -4, -15, -2, -8, -14, 3, -4, -4, -4, -5, -4, -10, -4, -4, -7, -7, -4, -4, -0, -4, -6, -3, -8,
          -4, -3, 0, -5, -4, -2, -3, 43, 5, 35, 1, 12, -6, -4, 4, 67, 59, 47, 23, 14, 5, 11, 3, -4, -1, -3, 2, -4, 5, 4, 2, 2,
          1, 0, -2, 3, 2, 0, 2, -15, 3, -7, 54, -8, -35, -54, -11, -1, -10, 59, 6, -2, 12, -87, 6, -1, -2, -0, -3, 6, -52, 18, 33, 3,
          -1, -5, 4, 1, 8, 11, 13, 1, -3, 1, 0, 0, 3, 4, 5, -2, -1, -5, -7, 0, 8, 2, -1, -2, 3, -1, 1, 2, 1, 2, 2, 12,
          -1, 4, 8, 13, 4, 2, -9, 8, -2, -1, -2, 0, -2, 0, -1, -15, -8, -6, -1, 1, -2, 5, 13, 1, -3, -8, -5, 0, 1, -0, 5, -1,
          45, -17, -2, -4, -2, 0, 1, -11, -3, -8, 29, 8, -2, -2, 6, 51, -18, -4, 30, -1, 4, 1, 14, 20, 46, 33, -2, 25, -6, -2, 0, -27,
          -23, -2, -22, -33, -9, -43, -42, 3, -5, -17, -40, -49, -53, 2, -18, -13, -28, -37, 5, -38, -44, 4, -37, -32, -30, -29, -31, 3, -45, -44, -55, -16,
          -24, -16, 11, 12, 13, 20, 14, -42, -24, 2, 7, 19, 11, 20, 15, -20, -29, -29, -10, -25, 15, 11, 15, -31, -41, -71, -25, -71, 11, 14, -117, -4,
          5, 0, 0, -2, 1, -1, 1, 4, -0, -3, 0, -8, -6, -10, -11, 3, 3, 2, -4, -7, -9, -11, -14, 11, 6, 3, 1, -4, -9, -8, -11, 17,
          6, -4, 0, -4, -6, -8, -7, 9, 54, 33, -3, -4, -16, -7, -8, 19, 26, 24, 2, 1, -5, -5, -2, 0, 2, 0, 3, 0, 3, 5, -4, 1,
          -1, -3, 3, -2, -1, 3, 1, -73, -39, -49, -33, -36, -34, -30, -33, -5, -17, -15, -37, -7, 19, 33, 40, -9, -3, -17, -13, -0, 8, 5, 9, -1,
          -2, -4, 3, 3, 6, 8, 4, 7, 6, 3, 2, 2, 2, 7, 6, 5, 0, 2, 2, -1, 6, 3, 5, -2, 5, 4, -1, 5, 2, -3, -3, -0,
          -16, -16, 13, 20, 7, 37, 39, -10, -11, -3, -9, 4, 6, 58, 45, -10, -13, -12, -2, -0, -1, 84, 91, -6, -8, -7, -4, -5, -5, -0, 18, 14,
          -15, -5, -7, -7, -8, -8, 2, 42, 9, 91, -10, -6, -8, -7, -3, 67, 59, 40, 51, -8, -4, -8, 2, 63, -1, 54, 3, -22, 9, 7, -5, -10,
          3, -25, 30, -6, 7, -7, 0, -14, -20, -7, -14, -1, -8, -8, -42, -5, -2, -13, -21, -103, -29, -6, -67, -3, -8, -10, -17, -22, 13, 15, 9, -8,
          -11, -12, -18, -17, 11, 22, 10, -10, -7, -17, -17, -28, 13, 25, 13, 0, 7, 1, -1, 11, 17, 11, 11, 2, 4, 6, 5, 11, 14, 7, 9, 1,
          -4, 4, 3, -3, 1, -4, -1, -8, -1, -11, 0, -9, 82, 13, -12, -5, -9, 1, -5, -7, -4, -9, -17, -8, -8, -14, -1, -2, -1, 5, -5, -9,
          -10, -5, -5, -3, -2, 1, 3, -5, -10, -15, -11, -9, -4, -5, -0, 5, -13, -21, -5, -4, -1, -6, -17, -1, -4, -4, 5, -3, 1, -3, 0, -4,
          -3, 2, -4, -4, -3, 3, 3, -20, -11, -15, 11, 13, 34, 18, -60, -16, -10, -2, 11, 10, -59, -10, 5, -5, -3, 8, 6, -3, -5, -2, -6, -6,
          -5, -1, 6, 1, 8, 0, 5, -1, -1, -2, -5, -1, 1, -0, -3, -4, -7, -2, 4, -7, 1, 1, 5, -1, -2, 5, -2, 5, 4, -3, 4, -12,
          -2, 4, 7, 48, 10, 21, 18, -11, -11, -11, 1, 18, 43, 32, 28, -20, -3, -13, -7, -14, -18, -51, 11, -5, -64, -10, -13, -14, -18, -13, -27, -15,
          -10, -11, -9, -15, -14, -14, 12, -10, -11, -11, -14, -13, -13, -9, -14, 4, -17, -18, -14, -14, -13, -19, -13, 50, -22, -16, -19, -12, -16, -8, -2, -11,
          -5, 43, -4, -29, -4, -1, -69, -20, 31, -8, -3, -19, -4, 5, 3, 11, 2, -6, -10, -0, 2, 3, -2, -6, 2, -8, -8, 1, 82, -1, 3, -19,
          -9, 6, 1, 15, 9, 5, -0, -12, 9, 7, 2, 8, 1, 1, 3, 11, 15, 1, 6, 1, 5, -1, 3, 4, 5, 7, 17, -3, 10, 4, -6, -3,
          2, -4, -2, -3, 0, -3, 3, -4, 6, -0, -3, 2, -3, -6, 3, -1, -0, 5, -3, -9, -1, -2, 6, 2, 6, 3, 9, -0, 7, -1, 6, 6,
          3, 0, 14, 15, 11, -1, -10, -1, 10, 29, 21, 16, 15, 1, -11, -13, 5, 18, 16, 27, -2, 5, 12, 5, 0, 1, 0, 3, 2, -4, 1, -4,
          2, 1, 3, -2, -1, 0, 2, -14, -21, -20, -6, -2, -28, 3, -57, 14, 3, -9, -13, -18, 12, 1, 15, -8, -4, 4, -4, -6, -2, 6, 19, -1,
          1, -3, -2, 5, -2, 0, 11, -0, -3, 0, -5, 13, -1, -14, -17, -3, -4, -8, -16, 15, -3, -36, -19, 5, 4, 4, 1, 0, 1, -3, 2, 2,
          -5, 2, 31, -22, -22, -1, -4, 11, 4, 8, -11, 21, -11, -8, 4, -3, 15, 8, 10, 38, 92, 2, -10, -13, 25, 2, -0, 29, 49, 31, 38, -3,
          57, 21, -3, 38, 28, 37, 4, -10, 9, 9, 57, 34, 52, 27, 9, -12, -12, 18, 32, 27, 29, 0, 52, -29, 62, 35, 21, -15, 64, 6, 55, 1,
          -15, -10, -20, -17, -29, -29, -7, 3, -21, -19, -29, -12, -33, -39, -7, 4, -14, -29, -29, -23, -47, -65, 14, -21, -12, -15, -9, -28, -28, -41, -21, 48,
          -51, -28, -20, -19, -36, -70, -52, -5, -4, 11, -5, 1, -64, -57, -30, -6, -14, 1, 11, 8, 5, -9, -3, 3, 15, 21, -11, 26, 16, 3, -3, -4,
          -1, 3, 0, -2, -2, -1, -1, 16, 7, 4, -3, 11, -4, -4, -12, 10, 8, 6, -1, 1, -2, -11, -12, 10, 1, 10, 9, 5, -4, 2, -10, -3,
          2, 14, 8, -2, -15, 4, -5, 5, 14, 22, 12, -9, 5, -0, -7, 11, 12, 33, 15, -2, 6, -20, 28, 3, -4, -1, 3, 0, 3, 1, 5, 5,
          4, 1, 0, 1, 1, -1, 3, -19, -24, -18, -21, -16, -33, -6, -11, -12, -8, -8, -10, -5, 4, -3, 8, -1, -9, -10, 5, -1, -3, -1, -6, -4,
          -7, -2, 3, -1, -8, -5, -13, -9, -2, 9, 4, -9, -11, -2, -7, -5, 9, 41, -8, 5, -11, 2, -6, 2, 5, 1, 3, -4, 3, 3, -4, -19,
          -11, -7, -19, -9, -22, 14, 5, -9, -22, -19, -3, 16, 0, 10, 8, -9, -13, 11, 9, 15, 14, 7, 10, -8, 29, 15, 24, 9, 46, 21, -17, 14,
          29, 16, 13, 11, 21, -26, -44, -17, 14, 18, -5, 6, 25, 13, -34, -1, -10, -23, -9, -9, 20, 3, 23, -21, 12, 19, -12, -9, 19, -9, -30, 39,
          -1, -13, -6, -1, -23, 8, 18, 5, -24, -11, 25, -15, -42, -5, 44, -10, -6, -25, -23, -3, 0, -33, 14, 16, -17, -5, 4, -0, -43, -6, -36, 13,
          -4, -11, -3, -31, -49, -95, -48, 10, 9, -23, -32, -37, -85, -18, 74, 6, 39, -58, 31, 2, 11, -2, 9, 6, 24, 20, 37, 28, 47, 8, 12, 5,
          0, -4, -4, -1, 4, 3, 1, 2, -2, 0, 10, 15, 3, 5, 1, -3, -2, -5, 0, 9, 5, 9, 8, -3, 2, -3, -3, -0, 2, 2, 4, 0,
          -2, 0, -3, -9, -7, 2, -3, 3, -1, -7, -15, -18, -12, -8, 15, -6, -18, -22, -15, -13, -14, 3, 32, 3, 1, 1, 2, 3, 5, -2, -2, -3,
          2, 5, -1, 4, -1, 2, -3, -12, -17, -14, -6, 7, 26, 38, 18, -9, -9, -4, -2, 2, 11, 8, 3, -11, -15, -13, -6, 2, 15, 14, 4, -4,
          -9, -6, -5, 1, 7, 8, 13, -6, -11, -3, -1, 1, 1, 8, 13, -8, -6, -3, 0, -7, 3, 3, 21, -4, 4, 1, 1, -2, 3, -3, -1, 3,
          -7, 5, 29, 9, 11, 6, -2, -13, 4, 8, 6, 3, 3, -3, -6, -4, -9, -2, -15, -3, -7, -3, -8, -14, -11, -26, -23, -11, -8, -1, 9, -21,
          -15, -30, -17, -10, -5, -3, -5, -4, -35, -35, -10, -8, 1, -5, 13, -15, -30, -28, -18, -18, -7, -27, 2, -21, -19, -29, -34, -24, -19, -26, 7, -11,
          -19, 19, -7, 12, -4, 4, -3, 4, 10, 3, -10, 11, 3, -20, -32, 35, -1, 12, 12, 0, -7, -22, -10, 26, 19, 15, 23, 9, -4, -1, -13, 22,
          20, 28, 44, 13, 12, 2, -25, -33, 14, 7, 16, 2, 0, -9, -11, 32, 32, 19, -5, -7, -7, -9, -9, 23, 33, 28, -6, -5, -8, -15, -17, };




int sigmoid(int x){
    return round(1 / (1 + exp(-1 * x)));
}

#endif

//////////////////////////
// Alexander Agruso     //
// MATH 3323 Homework 8 //
// Euler's Method       //
//////////////////////////

#include "stdio.h"
#include "stdlib.h"

// f(t,y) is a function f that satisfies the differential equation
//   dy/dt=f(t,y)
// in this instance, f(t,y)=3-2t-0.5y
float f(float t, float y) {
  return 3.0f - 2.0f * t - 0.5f * y;
}

// iterates euler's method on f(t,y)
float euler(int iterations, float step, float y0, float t0) {
  float yn = y0 + f(t0, y0) * (step);
  float tn = step;
  printf("%.4f | %.4f | %d\n", yn, tn, 1);

  for (int i = 1; i < iterations; i++) {
    yn = yn + f(tn, yn) * (step);
    tn += step;
    printf("%.4f | %.4f | %d\n", yn, tn, i + 1);
  }

  return yn;
}

int main() {
  printf("Euler approximation of the particular initial value problem\n");
  printf("  dy/dt=3-2t-0.5y; y(0)=1\n");
  printf("--------------------\n");
  printf(" y_n   | t_n    | i \n");
  printf("--------------------\n");
  euler(5, 0.2f, 1.0f, 0.0f);

  return EXIT_SUCCESS;
}
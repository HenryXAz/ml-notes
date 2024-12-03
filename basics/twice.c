#include <stdio.h>
#include <stdlib.h>
#include <time.h>

float train[][2] = {
  {0, 0},
  {1, 2},
  {2, 4},
  {3, 6},
  {4, 8},
};

#define train_count sizeof(train) / sizeof(train[0])

float rand_float(void) {
  return (float) rand() / (float) RAND_MAX;
}

float cost(float w, float b) {
  float result = 0.0f;

  for (size_t i=0;i < train_count; ++i) {
    float x = train[i][0];
    float y = x*w + b;
      
    float d = y - train[i][1];
    result += d*d;
    
  }

  result /= train_count;

  return result;
}

int main() { 
  srand(time(0)); 
  
  float w = rand_float() * 10.0f;
  float b = rand_float() * 5.0f;

  float eps = 1e-3; 
  float rate = 1e-1;

  printf("cost = %f\n", cost(w, b));

  for (size_t i=0; i< 100; ++i) {
    float c = cost(w, b); 

    float dw = (cost(w + eps, b) - c) / eps;
    float db = (cost(w, b + eps ) - c) / eps;
 
    w -=rate*dw;
    b-=rate*db;
  }
  
  printf("------------------------\n");
  printf("cost = %f\n", cost(w, b));
  printf("w =%f, b = %f\n", w, b);

  return 0;
}


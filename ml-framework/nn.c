#include <time.h>

#define NN_IMPLEMENTATION
#include "nn.h"

typedef struct {
  Mat a0, a1, a2;
  Mat w1, w2, b1, b2;
} Xor;

void forward_xor(Xor m) {

  mat_dot(m.a1, m.a0, m.w1);
  mat_sum(m.a1, m.b1);
  mat_sig(m.a1);

  mat_dot(m.a2, m.a1, m.w2);
  mat_sum(m.a2, m.b2);
  mat_sig(m.a2);

}

float cost(Xor m, Mat ti, Mat to) {
  assert(ti.rows == to.rows);
  assert(to.cols == m.a2.cols);
  size_t n = ti.rows; 

  float c = 0.f;

  for (size_t i=0; i<n; ++i) {
    Mat x = mat_row(ti, i);
    Mat y = mat_row(to, i);

    mat_copy(m.a0, x);
    forward_xor(m);

    size_t q = to.cols;
    
    for (size_t j=0; j<q; ++j) {
      float d = MAT_AT(m.a2, 0, j) - MAT_AT(y, 0, j);
      c += d*d;
    }

  } 

  return c/n;
}

void finite_diff(Xor m, Xor g, float eps, Mat ti, Mat to) {
  float saved;

  float c = cost(m, ti, to);

  for (size_t i=0; i<m.w1.rows; ++i) {
    for (size_t j=0; j<m.w1.cols; ++j) {
      saved = MAT_AT(m.w1, i, j);

      MAT_AT(m.w1, i, j) += eps;
      MAT_AT(g.w1, i, j) = (cost(m, ti, to) - c) / eps;    
      MAT_AT(m.w1, i, j) = saved;

    }
  }

  for (size_t i=0; i<m.b1.rows; ++i) {
    for (size_t j=0; j<m.b1.cols; ++j) {
      saved = MAT_AT(m.b1, i, j);
 
      MAT_AT(m.b1, i, j) += eps;
      MAT_AT(g.b1, i, j) = (cost(m, ti, to) - c) / eps;    
      MAT_AT(m.b1, i, j) = saved;

    }
  }


  for (size_t i=0; i<m.w2.rows; ++i) {
    for (size_t j=0; j<m.w2.cols; ++j) {
      saved = MAT_AT(m.w2, i, j);

      MAT_AT(m.w2, i, j) += eps;
      MAT_AT(g.w2, i, j) = (cost(m, ti, to) - c) / eps;
      MAT_AT(m.w2, i, j) = saved;

    }
  }
 
  for (size_t i=0; i<m.b2.rows; ++i) {
    for (size_t j=0; j<m.b2.cols; ++j) {
      saved = MAT_AT(m.b2, i, j);

      MAT_AT(m.b2, i, j) += eps;
      MAT_AT(g.b2, i, j) = (cost(m, ti, to) - c) / eps;
      MAT_AT(m.b2, i, j) = saved;

    }
  }

}


Xor xor_alloc() {
  Xor m;

  m.a0 = mat_alloc(1, 2);

  m.w1 = mat_alloc(2,2);
  m.b1 = mat_alloc(1, 2);
  m.a1 = mat_alloc(1, 2);
  
  m.w2 = mat_alloc(2, 1);
  m.b2 = mat_alloc(1, 1);
  m.a2 = mat_alloc(1, 1); 
  return m;
}

void xor_learn(Xor m, Xor g, float rate) {
  for (size_t i=0; i<m.w1.rows; ++i) {
    for (size_t j=0; j<m.w1.cols; ++j) {
      MAT_AT(m.w1, i, j) -= rate*MAT_AT(g.w1, i, j);
    }
  }

  for (size_t i=0; i<m.b1.rows; ++i) {
    for (size_t j=0; j<m.b1.cols; ++j) {
      MAT_AT(m.b1, i, j) -= rate*MAT_AT(g.b1, i, j);
    }
  }


  for (size_t i=0; i<m.w2.rows; ++i) {
    for (size_t j=0; j<m.w2.cols; ++j) {
      MAT_AT(m.w2, i, j) -= rate*MAT_AT(g.w2, i, j);
    }
  }
 
  for (size_t i=0; i<m.b2.rows; ++i) {
    for (size_t j=0; j<m.b2.cols; ++j) {
      MAT_AT(m.b2, i, j) -= rate*MAT_AT(g.b2, i, j);
    }
  }
}


float td[] = {
  0, 0, 0,
  1, 0, 1,
  0, 1, 1,
  1, 1, 0,
};


int main(void) { 
  srand(time(0));
   
  size_t stride = 3;
  size_t n = sizeof(td)/sizeof(td[0]) / stride;

  Mat ti = {
    .rows = n,
    .cols = 2,
    .stride = stride,
    .es = td,
  }; 


  Mat to = {
    .rows = n,
    .cols = 1,
    .stride = stride,
    .es = td+2,
  };

  size_t arch[] = {2, 2, 1};
  NN nn = nn_alloc(arch, ARRAY_LEN(arch));
  NN g = nn_alloc(arch, ARRAY_LEN(arch));
  nn_rand(nn, 0, 1);

  float eps = 1e-1;
  float rate = 1e-1;

  printf("___________________\n");

  printf("cost = %f\n", nn_cost(nn, ti, to));

  for (size_t i=0; i<10*1000; ++i) {  
    nn_finite_diff(nn, g, eps, ti, to);
    nn_learn(nn, g, rate);
  } 
  
 
  printf("___________________\n");
  
  printf("final cost = %f\n", nn_cost(nn, ti, to));

  printf("___________________\n");

  #if 1

  for (size_t i=0; i<2; ++i) {
    for (size_t j=0; j<2; ++j) {
      MAT_AT(NN_INPUT(nn), 0, 0 ) = i;
      MAT_AT(NN_INPUT(nn), 0, 1) = j;

      nn_forward(nn);

      printf("%zu ^ %zu = %f\n", i, j, MAT_AT(NN_OUTPUT(nn), 0, 0)) ;

    }
  }

  #endif

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------------
// File:     Matrix.cpp
// Purpose:  Describes a 2D matrix
// Authors:  James Pugh
//           This software was initially developped at the:
//             Distributed Intelligent Systems and Algorithms Laboratory (DISAL), EPFL, Switzerland
//           Please add a reference to the original publication if you publish any article derived from this work:
//             C. M. Cianci, T. Lochmatter, J. Pugh, and A. Martinoli.
//             Toward Multi-Level Modeling of Robotic Sensor Networks: A Case Study in Acoustic Event Monitoring.
//             In International Conference on Robot Communication and Coordination (ROBOCOMM), pages 1-8, Athens, Greece, 2007.
//----------------------------------------------------------------------------------------------------------------------------

#include "Matrix.h"
#include <iostream>
#include <assert.h>

using namespace std;

Matrix::Matrix(int size) {
  
  assert(size > 1 && size <= MAXSIZE);

  _size = size;

  /* Create identity matrix */
  for (int i = 0; i < _size; i++)
    for (int j = 0; j < _size; j++)
      if (i == j)
        mat[i][j] = 1.0;
      else
        mat[i][j] = 0.0;
}

Matrix::Matrix(double v00, double v01, double v10, double v11) {
  _size = 2;
  mat[0][0] = v00;
  mat[0][1] = v01;
  mat[1][0] = v10;
  mat[1][1] = v11;
}

Matrix::Matrix(double v00, double v01, double v02,
               double v10, double v11, double v12,
               double v20, double v21, double v22) {
  _size = 3;
  mat[0][0] = v00;
  mat[0][1] = v01;
  mat[0][2] = v02;
  mat[1][0] = v10;
  mat[1][1] = v11;
  mat[1][2] = v12;
  mat[2][0] = v20;
  mat[2][1] = v21;
  mat[2][2] = v22;
}

Matrix::Matrix(const Matrix &other) {

  _size = other._size;

  // copy content
  for (int i = 0; i < _size; i++)
    for (int j = 0; j < _size; j++)
      this->mat[i][j] = other.mat[i][j];
}

double Matrix::getVal(int row, int col) const { 
  
  assert(row >= 0 && row < _size);
  assert(col >= 0 && col < _size);

  return mat[row][col];
}

void Matrix::setVal(int row, int col, double val) { 

  assert(row >= 0 && row < _size);
  assert(col >= 0 && col < _size);

  mat[row][col] = val;
}

const Matrix & Matrix::operator = (const Matrix &other) {
  
  // avoid copying from self
  if (&other == this)
    return *this;;

  _size = other._size;

  /* Create matrix */
  for (int i = 0; i < _size; i++)
    for (int j = 0; j < _size; j++)
      this->mat[i][j] = other.mat[i][j];
     
  return *this;
}

Matrix Matrix::mult(const Matrix *other) const {

  assert(_size == other->_size);
  
  Matrix result(_size);
  
  // Multiply matrices
  for (int i = 0; i < _size; i++) {
    for (int j = 0; j < _size; j++) {
      result.mat[i][j] = 0.0;
      for (int k = 0; k < _size; k++)
        result.mat[i][j] += this->mat[i][k] * other->mat[k][j];
    }
  }

  return result;
}

double Matrix::transformVal(double val) const {

  assert(_size == 2);

  return val * mat[0][0] + mat[0][1];
}

Point Matrix::transformPoint(const Point *p) const {

  assert(_size == 3);

  Point newp;
  newp.x = p->x*mat[0][0] + p->y*mat[0][1] + mat[0][2];
  newp.y = p->x*mat[1][0] + p->y*mat[1][1] + mat[1][2];

  return newp;
}

void Matrix::print() const {
  
  /* Print matrix */
  for (int i = 0; i < _size; i++) {
    for (int j = 0; j < _size; j++) {
      cout << mat[i][j] << "\t";
    }
    cout << "\n";
  }
}

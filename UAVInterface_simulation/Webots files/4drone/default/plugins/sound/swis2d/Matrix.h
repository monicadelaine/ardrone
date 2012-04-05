#ifndef MATRIX_H
#define MATRIX_H

//----------------------------------------------------------------------------------------------------------------------------
// File:     Matrix.h
// Purpose:  Describes a 2D matrix
// Authors:  James Pugh
//           This software was initially developped at the:
//             Distributed Intelligent Systems and Algorithms Laboratory (DISAL), EPFL, Switzerland
//           Please add a reference to the original publication if you publish any article derived from this work:
//             C. M. Cianci, T. Lochmatter, J. Pugh, and A. Martinoli.
//             Toward Multi-Level Modeling of Robotic Sensor Networks: A Case Study in Acoustic Event Monitoring.
//             In International Conference on Robot Communication and Coordination (ROBOCOMM), pages 1-8, Athens, Greece, 2007
//----------------------------------------------------------------------------------------------------------------------------

#include "Point.h"

class Matrix {
public:
  // creates an empty matrix.
  Matrix() { _size = 0; }

  // creates an identity matrix of the given size.
  Matrix(int size);

  // creates a 2x2 matrix with the given values
  Matrix(double v00, double v01, double v10, double v11);
 
  // creates a 3x3 matrix with the given values
  Matrix(double v00, double v01, double v02,
         double v10, double v11, double v12,
         double v20, double v21, double v22);
  
  // copy constructor
  Matrix(const Matrix &mat);

  // gets a value
  double getVal(int row, int col) const;

  // returns the size of the matrix
  int getSize() const { return _size; }

  // returns a pointer to the matrix values
  const double *getMat() const { return (const double *)mat; }

  // sets a value
  void setVal(int row, int col, double val);

  // sets the matrix dimensions and values to that of the given matrix
  const Matrix & operator = (const Matrix &mat);

  // multiply the matrix by the given matrix
  // @param mat pointer to the matrix by which to be multiplied
  // @return the multiplied matrix
  Matrix mult(const Matrix *mat) const;

  // transforms the given number with the 2x2 matrix
  // @param val the value to be tranformed
  // @return the transformed value
  double transformVal(double val) const;

  // transforms the given given with the 3x3 matrix
  // @param p pointer to the point to be tranformed
  // @return the transformed point
  Point transformPoint(const Point *p) const;

  // prints the point.
  void print() const;
  
protected:
  // the size of the matrix
  int _size;
  
  enum { MAXSIZE = 3 };

  // the matrix values
  double mat[MAXSIZE][MAXSIZE];
};

#endif

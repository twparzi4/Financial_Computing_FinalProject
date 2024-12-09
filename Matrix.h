# pragma once
# include <vector>
# include <iomanip>
# include <cmath>
# include <stdexcept>

typedef std::vector<double> Vector;
typedef std::vector<Vector> Matrix;

// // Matrix and Vector Operations
Vector operator*(const Matrix& C, const Vector& V);     // Matrix * Vector
Vector operator*(const double& a, const Vector& V);     // Scalar * Vector
Vector operator*(const Vector& V, const Vector& W);     // Element-wise Vector * Vector
Vector operator+(const double& a, const Vector& V);     // Scalar + Vector
Vector operator+(const Vector& V, const Vector& W);     // Vector + Vector
Vector exp(const Vector& V);                            // Element-wise exponential
double operator^(const Vector& V, const Vector& W);     // Dot product (scalar operator)
std::ostream& operator<<(std::ostream& out, Vector& V); // Overload cout for Vector
std::ostream& operator<<(std::ostream& out, Matrix& W); // Overload cout for Matrix
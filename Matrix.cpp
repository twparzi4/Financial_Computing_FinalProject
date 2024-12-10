# include "Matrix.h"
# include <iomanip>
// Multiply a Matrix by a Vector
// Parameters:
//   - C: The input matrix (MxN dimensions)
//   - V: The input vector (length N)
// Returns:
//   - A Vector (length M) which is the result of the multiplication
Vector operator*(const Matrix& C, const Vector& V) {
    if (C.empty() || C[0].size() != V.size()) {
        throw std::invalid_argument("Matrix and Vector size mismatch.");
    }
    Vector result(C.size(), 0.0); // Initialize result vector with zeros
    for (size_t i = 0; i < C.size(); ++i) { // Loop over each row of the matrix
        for (size_t j = 0; j < V.size(); ++j) { // Loop over each column of the row
            result[i] += C[i][j] * V[j]; // Compute the dot product of the row and vector
        }
    }
    return result;
}

// Multiply a Scalar by a Vector
// Parameters:
//   - a: The scalar value
//   - V: The input vector
// Returns:
//   - A Vector where each element is the result of the scalar multiplication
Vector operator*(const double& a, const Vector& V) {
    Vector result = V; // Copy the input vector to avoid modifying it
    for (auto& val : result) { // Loop over each element of the vector
        val *= a; // Multiply each element by the scalar
    }
    return result;
}

// Perform Element-wise Multiplication of Two Vectors
// Parameters:
//   - V: First input vector
//   - W: Second input vector (must have the same size as V)
// Returns:
//   - A Vector where each element is the product of the corresponding elements of V and W
Vector operator*(const Vector& V, const Vector& W) {
    if (V.size() != W.size()) {
        throw std::invalid_argument("Vector size mismatch.");
    }
    Vector result = V; // Copy the first vector
    for (size_t i = 0; i < V.size(); ++i) { // Loop over each element
        result[i] *= W[i]; // Multiply corresponding elements
    }
    return result;
}

// Add a Scalar to a Vector
// Parameters:
//   - a: The scalar value
//   - V: The input vector
// Returns:
//   - A Vector where each element is the sum of the scalar and the corresponding vector element
Vector operator+(const double& a, const Vector& V) {
    Vector result = V; // Copy the input vector
    for (auto& val : result) { // Loop over each element of the vector
        val += a; // Add the scalar to each element
    }
    return result;
}

// Perform Element-wise Addition of Two Vectors
// Parameters:
//   - V: First input vector
//   - W: Second input vector (must have the same size as V)
// Returns:
//   - A Vector where each element is the sum of the corresponding elements of V and W
Vector operator+(const Vector& V, const Vector& W) {
    if (V.size() != W.size()) {
        throw std::invalid_argument("Vector size mismatch.");
    }
    Vector result = V; // Copy the first vector
    for (size_t i = 0; i < V.size(); ++i) { // Loop over each element
        result[i] += W[i]; // Add corresponding elements
    }
    return result;
}

// Compute the Element-wise Exponential of a Vector
// Parameters:
//   - V: The input vector
// Returns:
//   - A Vector where each element is the exponential of the corresponding element in V
Vector exp(const Vector& V) {
    Vector result = V; // Copy the input vector
    for (auto& val : result) { // Loop over each element
        val = std::exp(val); // Compute the exponential
    }
    return result;
}

// Compute the Dot Product of Two Vectors
// Parameters:
//   - V: First input vector
//   - W: Second input vector (must have the same size as V)
// Returns:
//   - A double representing the dot product (sum of element-wise products)
double operator^(const Vector& V, const Vector& W) {
    if (V.size() != W.size()) {
        throw std::invalid_argument("Vector size mismatch.");
    }
    double result = 0.0; // Initialize the result to zero
    for (size_t i = 0; i < V.size(); ++i) { // Loop over each element
        result += V[i] * W[i]; // Accumulate the product of corresponding elements
    }
    return result;
}

// Overload the Output Operator for Vectors
// Prints the vector elements separated by spaces
// Parameters:
//   - out: The output stream (e.g., std::cout)
//   - V: The vector to print
// Returns:
//   - The modified output stream
std::ostream& operator<<(std::ostream& out, const Vector& V) {
    for (const auto& val : V) {
        out << val << " ";
    }
    out << '\n'; 
    return out;
}

// Overload the Output Operator for Matrices
// Prints the matrix as rows of elements, each separated by spaces
// Parameters:
//   - out: The output stream (e.g., std::cout)
//   - W: The matrix to print
// Returns:
//   - The modified output stream
std::ostream& operator<<(std::ostream& out, const Matrix& W) {
    for (const auto& row : W) {
        for (const auto& val : row) {
            out << std::setw(10) << val << " ";
        }
        out << '\n'; 
    }
    return out;
}

#pragma once

#include <vector>
#include <stdexcept>

template <typename T>
class Matrix {
public:
    // Contructor
    Matrix(const std::vector<T>& data, size_t rows, size_t cols)
        : m_data(data), m_rows(rows), m_cols(cols)
    {
        if (m_data.size() != m_rows * m_cols) {
            throw std::invalid_argument("The size of the data vector does not correspond to X * Y.");
        }
    }

    // Getters
    size_t getRows() const { return m_rows; }
    size_t getCols() const { return m_cols; }

    // Setters : we need convention [x][y]
    class RowProxy {
    public:
        RowProxy(Matrix<T>& matrix, size_t row_index)
            : m_matrix(matrix), m_row_index(row_index) {}

        T& operator[](size_t col_index) {
            if (m_row_index >= m_matrix.m_rows || col_index >= m_matrix.m_cols) {
                throw std::out_of_range("Out-of-bounds matrix indices (bounds check failed).");
            }
            return m_matrix.m_data[m_row_index * m_matrix.m_cols + col_index];
        }
    private:
        Matrix<T>& m_matrix;
        size_t m_row_index;
    };

    class ConstRowProxy {
    public:
        ConstRowProxy(const Matrix<T>& matrix, size_t row_index)
            : m_matrix(matrix), m_row_index(row_index) {}

        const T& operator[](size_t col_index) const {
            if (m_row_index >= m_matrix.m_rows || col_index >= m_matrix.m_cols) {
                throw std::out_of_range("Out-of-bounds matrix indices (bounds check failed).");
            }
            return m_matrix.m_data[m_row_index * m_matrix.m_cols + col_index];
        }
    private:
        const Matrix<T>& m_matrix;
        size_t m_row_index;
    };

    RowProxy operator[](size_t row_index) {
        return RowProxy(*this, row_index);
    }
    ConstRowProxy operator[](size_t row_index) const {
        return ConstRowProxy(*this, row_index);
    }
private:
    std::vector<T> m_data;
    size_t m_rows;
    size_t m_cols;
};

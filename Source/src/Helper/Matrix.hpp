/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

#include <iostream>
#include <vector>

typedef std::vector<std::vector<double>> MatNxNd;

class Matrix
{
public:
    Matrix(){}

    Matrix(const MatNxNd a_Mat)
        : m_Mat(a_Mat), m_Rows(a_Mat.size()), m_Cols(a_Mat[0].size()){}

    Matrix(const int a_Rows, const int a_Cols)
        : m_Rows(a_Rows), m_Cols(a_Cols)
    {
        initMatrix(m_Mat, m_Rows, m_Cols);
    }

    int getRows() const { return m_Rows; }
    int getCols() const { return m_Cols; }

    std::vector<double>& operator()(int a_Row)
    {
        return m_Mat[a_Row];
    }

    const std::vector<double>& operator()(int a_Row) const
    {
        return m_Mat[a_Row];
    }

    double& operator()(int a_Row, int a_Col)
    {
        return m_Mat[a_Row][a_Col];
    }

    const double& operator()(int a_Row, int a_Col) const
    {
        return m_Mat[a_Row][a_Col];
    }

    Matrix operator*(Matrix a_Input) const
    {
        // check dimensions
        if(m_Cols!=a_Input.getRows())
        {
            std::cout << "Can't multiply two matrices!" << std::endl;
        }

        Matrix t_Output = Matrix(m_Rows, a_Input.getCols());

        for(int i=0; i < m_Rows; i++)
        {
            for(int j=0; j < a_Input.getCols(); j++)
            {
                for (int k=0; k < m_Cols; k++)
                {
                    t_Output(i, j) += m_Mat[i][k] * a_Input(k, j);
                }
            }
        }
        return t_Output;
    }

private:
    int m_Rows, m_Cols;
    MatNxNd m_Mat;

    void initMatrix(MatNxNd& a_Mat, const int a_Rows, const int a_Cols)
    {
        for(int i=0; i<a_Rows; i++)
        {
            std::vector<double> t_RowVec;
            for(int j=0; j<a_Cols; j++)
            {
                t_RowVec.push_back(0);
            }
            a_Mat.push_back(t_RowVec);
        }
    }

};

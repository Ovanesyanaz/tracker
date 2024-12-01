#include "AssignmentOptimal.h"
#include <Eigen/Dense>

using Eigen::MatrixXd;

MatrixXd step2(MatrixXd &, MatrixXd &, MatrixXd &,
               MatrixXd &, MatrixXd &, int64_t);
MatrixXd step3(MatrixXd &, MatrixXd &, MatrixXd &,
               MatrixXd &, MatrixXd &, int64_t);
MatrixXd step4(MatrixXd &, MatrixXd &, MatrixXd &,
               MatrixXd &, MatrixXd &, int64_t, int64_t, int64_t);
MatrixXd step5(MatrixXd &, MatrixXd &, MatrixXd &,
               MatrixXd &, MatrixXd &, int64_t);
MatrixXd buildassignmentvector(MatrixXd);
double computeassignmentcost(MatrixXd &, MatrixXd, int64_t);

MatrixXd step2(MatrixXd &distMatrix, MatrixXd &starMatrix, MatrixXd &primeMatrix,
               MatrixXd &coveredColumns, MatrixXd &coveredRows, int64_t minDim)
// Возвращает assignment

{
    MatrixXd assignment;
    // cover every column containing a starred zero
    coveredColumns = (starMatrix.colwise().maxCoeff().array() == 1).select(1, coveredColumns);

    if (coveredColumns.sum() == minDim)
    {
        // algorithm finished
        assignment = buildassignmentvector(starMatrix);
    }
    else
    {
        // move to step 3
        assignment = step3(distMatrix, starMatrix, primeMatrix, coveredColumns, coveredRows, minDim);
    }
    return assignment;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

MatrixXd step3(MatrixXd &distMatrix, MatrixXd &starMatrix, MatrixXd &primeMatrix,
               MatrixXd &coveredColumns, MatrixXd &coveredRows, int64_t minDim)
// Возвращает assignment
{
    MatrixXd assignment;

    int zerosFound = 1;
    while (zerosFound)
    {
        zerosFound = 0;

        for (int col = 0; col < coveredColumns.cols(); col++)
        {
            if (coveredColumns(0, col) == 0)
            {
                for (int row = 0; row < coveredRows.rows(); row++)
                {
                    if (coveredRows(row, 0) == 0)
                    {
                        if (distMatrix(row, col) == 0)
                        {
                            MatrixXd starCol(1, starMatrix.cols());
                            primeMatrix(row, col) = 1;
                            starCol = (starMatrix.row(row).array() != 0).select(1, starCol);
                            if (starCol.sum() == 0)
                            {
                                // move to step 4
                                assignment = step4(distMatrix, starMatrix, primeMatrix, coveredColumns, coveredRows, row, col, minDim);
                                return assignment;
                            }
                            else
                            {
                                coveredRows(row, 0) = 1;
                                coveredColumns = (starCol.array() == 1).select(0, coveredColumns);
                                zerosFound = 1;
                                break; // go on in next column
                            }
                        }
                    }
                }
            }
        }
    }

    assignment = step5(distMatrix, starMatrix, primeMatrix, coveredColumns, coveredRows, minDim);
    return assignment;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

MatrixXd step4(MatrixXd &distMatrix, MatrixXd &starMatrix, MatrixXd &primeMatrix,
               MatrixXd &coveredColumns, MatrixXd &coveredRows, int64_t row, int64_t col, int64_t minDim)
{
    MatrixXd assignment;

    MatrixXd newStarMatrix = starMatrix;
    newStarMatrix(row, col) = 1;

    int64_t starCol = col;
    int64_t starRow;
    bool row_was_found = 0;
    for (int row = 0; row < starMatrix.rows(); row++)
    {
        if (starMatrix(row, starCol) == 1)
        {
            starRow = row;
            row_was_found = 1;
        }
    }

    int64_t primeRow;
    int64_t primeCol;

    while (row_was_found)
    {
        // unstar the starred zero
        newStarMatrix(starRow, starCol) = 0;

        // find primed zero in row
        primeRow = starRow; // максимум - одна единица
        for (int col = 0; col < starMatrix.cols(); col++)
        {
            if (primeMatrix(primeRow, col) == 1)
            {
                primeCol = col;
            }
        }

        // star the primed zero
        newStarMatrix(primeRow, primeCol) = 1;

        // find starred zero in column
        starCol = primeCol;
        row_was_found = 0;
        for (int row = 0; row < starMatrix.rows(); row++)
        {
            if (starMatrix(row, starCol) == 1)
            {
                starRow = row;
                row_was_found = 1;
            }
        }
    }

    starMatrix = newStarMatrix;
    primeMatrix = MatrixXd::Zero(primeMatrix.rows(), primeMatrix.cols());
    coveredRows = MatrixXd::Zero(coveredRows.rows(), coveredRows.cols());

    assignment = step2(distMatrix, starMatrix, primeMatrix, coveredColumns, coveredRows, minDim);
    return assignment;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

MatrixXd step5(MatrixXd &distMatrix, MatrixXd &starMatrix, MatrixXd &primeMatrix,
               MatrixXd &coveredColumns, MatrixXd &coveredRows, int64_t minDim)
{
    MatrixXd assignment;
    // find smallest uncovered element
    double minValue = distMatrix.maxCoeff();
    for (int row = 0; row < distMatrix.rows(); row++)
    {
        if (coveredRows(row, 0) == 0) // uncoveredRows
        {
            for (int col = 0; col < distMatrix.cols(); col++)
            {
                if (coveredColumns(0, col) == 0) // uncoveredColumns
                {
                    if (distMatrix(row, col) < minValue)
                    {
                        minValue = distMatrix(row, col);
                    }
                }
            }
        }
    }
    double h = minValue;

    // add h to each covered row
    for (int row = 0; row < distMatrix.rows(); row++)
    {
        if (coveredRows(row, 0) == 1)
        {
            for (int col = 0; col < distMatrix.cols(); col++)
            {
                distMatrix(row, col) = distMatrix(row, col) + h;
            }
        }
    }

    // subtract h from each uncovered columnm
    for (int row = 0; row < distMatrix.rows(); row++)
    {
        for (int col = 0; col < distMatrix.cols(); col++)
        {
            if (coveredColumns(0, col) == 0)
            {
                distMatrix(row, col) = distMatrix(row, col) - h;
            }
        }
    }

    assignment = step3(distMatrix, starMatrix, primeMatrix, coveredColumns, coveredRows, minDim);

    return assignment;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

MatrixXd buildassignmentvector(MatrixXd starMatrix)
/* Функция создает матрицу assignment, находя максимальные значения в starmatrix */
{
    int nOfRows = starMatrix.rows();

    MatrixXd maxValueV = MatrixXd::Zero(nOfRows, 1);
    MatrixXd assignment = MatrixXd::Zero(nOfRows, 1);

    for (int row = 0; row < nOfRows; row++)
    {
        int64_t _, col;
        maxValueV(row, 0) = starMatrix.row(row).maxCoeff(&_, &col);
        if (maxValueV(row, 0) != 0)
            assignment(row, 0) = col + 1; // индексация как в matlab
        else
            assignment(row, 0) = 0;
    }
    return assignment;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double computeassignmentcost(MatrixXd &assignment, MatrixXd distMatrix, int64_t nOfRows)
/* Функция находит значения в матрице соответсвующие маске assignment, возвращает сумму конечных найденных значений,
    при этом измеияет assignment: индесы которым соответствуют бесконечные значения заменяет значением 0*/
{
    double cost = 0;
    for (int row = 0; row < nOfRows; row++)
    {
        if (assignment(row, 0) > 0)
        {
            int64_t col_index = assignment(row, 0);
            if (distMatrix(row, col_index - 1) < std::numeric_limits<double>::infinity())
            {
                cost += distMatrix(row, col_index - 1);
            }
            else
            {
                assignment(row, 0) = 0;
            }
        }
    }
    return cost;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

std::pair<MatrixXd, double> assignmentoptimal(MatrixXd distMatrix)
{
    // save original distMatrix for cost computation
    MatrixXd originalDistMatrix = distMatrix;

    // check for negative elements
    if ((originalDistMatrix.array() < 0).any())
        throw std::logic_error("All matrix elements have to be non-negative.");

    // get matrix dimensions
    int64_t nOfRows = distMatrix.rows(), nOfColumns = distMatrix.cols();

    // check for infinite values
    MatrixXd finiteIndex = distMatrix.array().isFinite().cast<double>();
    MatrixXd infiniteIndex = distMatrix.array().isInf().cast<double>();

    if (!infiniteIndex.isZero())
    {
        // set infinite values to large finite value
        double maxFiniteValue = (distMatrix.array().isInf()).select(-1, distMatrix).maxCoeff();
        double infValue;
        if (maxFiniteValue > 0)
        {
            infValue = abs(10 * maxFiniteValue * nOfRows * nOfColumns);
        }
        else
        {
            infValue = 10;
        }

        if (distMatrix.array().isInf().all())
        {
            // all elements are infinite
            MatrixXd assignment = MatrixXd::Zero(nOfRows, 1);
            double cost = 0;
            std::pair<MatrixXd, double> res(assignment, cost);
            return res;
        }
        distMatrix = (distMatrix.array().isInf()).select(infValue, distMatrix);
    }

    // memory allocation
    MatrixXd coveredColumns = MatrixXd::Zero(1, nOfColumns);
    MatrixXd coveredRows = MatrixXd::Zero(nOfRows, 1);
    MatrixXd starMatrix = MatrixXd::Zero(nOfRows, nOfColumns);
    MatrixXd primeMatrix = MatrixXd::Zero(nOfRows, nOfColumns);

    MatrixXd assignment = MatrixXd::Zero(nOfRows, 1);
    int64_t minDim;

    // preliminary steps
    if (nOfRows <= nOfColumns)
    {
        minDim = nOfRows;

        // find the smallest element of each row
        MatrixXd minVector = distMatrix.rowwise().minCoeff();
        // subtract the smallest element of each row from the row
        for (int row = 0; row < nOfRows; row++)
        {
            for (int col = 0; col < nOfColumns; col++)
            {
                distMatrix(row, col) = distMatrix(row, col) - minVector(row, 0);
            }
        }

        // Steps 1 and 2
        for (int row = 0; row < nOfRows; row++)
        {
            double minValue = distMatrix.row(row).minCoeff();
            for (int col = 0; col < nOfColumns; col++)
            {
                if (distMatrix(row, col) == minValue)
                {
                    if (!coveredColumns.col(col).value())
                    {
                        starMatrix(row, col) = 1;
                        coveredColumns(0, col) = 1;
                        break;
                    }
                }
            }
        }
    }
    else // nOfRows > nOfColumns
    {
        minDim = nOfColumns;

        // find the smallest element of each column
        MatrixXd minVector = distMatrix.colwise().minCoeff();
        // subtract the smallest element of each column from the column
        for (int col = 0; col < nOfColumns; col++)
        {
            for (int row = 0; row < nOfRows; row++)
            {
                distMatrix(row, col) = distMatrix(row, col) - minVector(0, col);
            }
        }

        // Steps 1 and 2
        for (int col = 0; col < nOfColumns; col++)
        {
            double minValue = distMatrix.col(col).minCoeff();
            for (int row = 0; row < nOfRows; row++)
            {
                if (distMatrix(row, col) == minValue)
                {
                    if (!coveredRows.row(row).value())
                    {
                        starMatrix(row, col) = 1;
                        coveredColumns(0, col) = 1;
                        coveredRows(row, 0) = 1;
                        break;
                    }
                }
            }
        }
        coveredRows = MatrixXd::Zero(nOfRows, 1); // was used auxiliary above
    }

    if (coveredColumns.sum() == minDim)
    {
        // algorithm finished
        assignment = buildassignmentvector(starMatrix);
    }
    else
    {
        // move to step 3
        assignment = step3(distMatrix, starMatrix, primeMatrix, coveredColumns, coveredRows, minDim);
    }

    double cost = computeassignmentcost(assignment, originalDistMatrix, nOfRows); // assigment передается по ссылке, соответсвенно, меняется
    std::pair<MatrixXd, double> res(assignment, cost);
    return res;
}

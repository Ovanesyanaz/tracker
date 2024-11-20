#include "Filter.h"
#include <vector>
#include <iostream>
#include <Eigen/Dense>

class ArrayController
{
private:
    int capacity = 5;         // емкость по умолчанию
    int free;                 // количество свободных элементов
    int used;                 // количество использованных элементов
    std::vector<int> list(5); // массив индексов
    void Increment(int n) {}  // пересчет free и used

public:
    ArrayController(nTrackMax) {}  // конструктор
    MatrixXd Allocate(int nNew) {} // добавление нового элемента
    MatrixXd Allocate() {}
    void Deallocate(std::vector<int> idx) {} // удаление элемента
};
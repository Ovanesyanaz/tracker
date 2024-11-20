#ifndef ARRAYCONTROLLER_H
#define ARRAYCONTROLLER_H

#include <vector>
#include <iostream>
#include <numeric>
#include <execution>
#include <limits>

class ArrayController
{
private:
    int capacity;          // емкость по умолчанию
    int free;              // количество свободных элементов
    int used;              // количество использованных элементов
    std::vector<int> list; // массив индексов
    void Increment(int n); // пересчет free и used

public:
    ArrayController(int nTrackMax = 5);  // конструктор
    std::vector<int> Allocate(int nNew); // добавление нового элемента
    std::vector<int> Allocate();
    void Deallocate(std::vector<int> idx); // удаление элемента
};

#endif // ARRAYCONTROLLER_H
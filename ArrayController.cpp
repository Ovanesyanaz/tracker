#include "ArrayController.h"

ArrayController::ArrayController(int nTrackMax) : capacity(nTrackMax), free(nTrackMax), used(0), index(nTrackMax) {Getlist();}

void ArrayController::Increment(int n)
{
    this->free = this->free - n;
    this->used = this->used + n;
}

void ArrayController::Getlist() {
    this->list.resize(0);
    for (int i = 0; i < this->capacity; i++) {
        if (this->index[i] != 0) {
            this->list.push_back(i);
        }
    }
}
std::vector<int> ArrayController::Allocate(int nNew)
{

    std::vector<int> idx(capacity);

    if (this->used + nNew > this->capacity)
    {
        std::cerr << "Can not allocate" << nNew << "objects" << std::endl;
        idx.resize(0);
        return idx;
    }
    int nNewCopy = nNew;
    for (int i = 0; i < capacity; i++)
    {
        if ((nNewCopy > 0) and (this->index[i] == 0))
        {
            idx[i] = 1;
            this->index[i] = 1;
            nNewCopy--;
        }
    }

    Increment(nNew);
    Getlist();
    std::vector<int> ind;
    for (int i = 0; i < idx.size(); i++){
        if (idx[i] != 0){
            ind.push_back(i);
        }
    }
    return ind;
}

std::vector<int> ArrayController::Allocate()
{

    int nNew = 1;
    std::vector<int> idx(capacity);

    if (this->used + nNew > this->capacity)
    {
        std::cerr << "Can not allocate" << nNew << "objects";
        idx.resize(0);
        return idx;
    }

    int nNewCopy = nNew;
    for (int i = 0; i < capacity; i++)
    {
        if (nNewCopy && this->index[i] == 0)
        {
            idx[i] = 1;
            this->index[i] = 1;
            nNewCopy--;
        }
    }

    Increment(nNew);
    Getlist();
    return idx;
}

void ArrayController::Deallocate(std::vector<int> idx)
{

    int numel = std::accumulate(idx.begin(), idx.end(), 0LL);

    if (!numel) // если в idx все 0
    {
        return;
    }

    if (idx.size() > this->capacity) // если количество элементов в idx больше capacity
    {
        std::cerr << "Can not delete " << numel << " objects" << std::endl;
        exit(1);
    }

    for (int i = 0; i < capacity; i++)
    {
        if (idx[i] == 1 && this->index[i] == 1)
        {
            this->index[i] = 0;
        }
        else {} // обработка ошибки при неправильном idx
    }

    Increment(-numel);
    Getlist();
}
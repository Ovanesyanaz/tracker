#include "ArrayController.h"

ArrayController::ArrayController(int nTrackMax) : capacity(nTrackMax), free(nTrackMax), used(0), list(nTrackMax) {}

void ArrayController::Increment(int n)
{
    this->free = this->free - n;
    this->used = this->used + n;
}

std::vector<int> ArrayController::Allocate(int nNew)
{

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
        if (nNewCopy && this->list[i] == 0)
        {
            idx[i] = 1;
            this->list[i] = 1;
            nNewCopy--;
        }
    }

    Increment(nNew);
    return idx;
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
        if (nNewCopy && this->list[i] == 0)
        {
            idx[i] = 1;
            this->list[i] = 1;
            nNewCopy--;
        }
    }

    Increment(nNew);
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
        if (idx[i] == 1)
        {
            this->list[i] = 0;
        }
    }

    Increment(-numel);
}
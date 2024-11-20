#include "ArrayController.h"

using Eigen::MatrixXd;

class ArrayController
{
private:
    int capacity = 5;         // емкость по умолчанию
    int free;                 // количество свободных элементов
    int used;                 // количество использованных элементов
    std::vector<int> list(5); // массив индексов

    void Increment(int n) // добавить эелементы
    {
        this.free = this.free - n;
        this.used = this.used + n;
    }

public:
    ArrayController(nTrackMax)
    {
        this.capacity = nTrackMax;
        this.free = this.capacity;
        this.used = 0;
        this.list.resize(0);
        this.list.resize(capacity);
    };

    MatrixXd Allocate(int nNew)
    {

        std::vector<int> idx(capacity);

        if (this.used + nNew > this.capacity)
        {
            std::cerr << "Can not allocate" << nNew << "objects";
            idx.resize(0);
            return;
        }

        int nNewCopy = nNew;
        for (int i = 0; i < capacity; i++)
        {
            if (nNewCopy && this.list[i] == 0)
            {
                idx[i] = 1;
                this.list[i] = 1;
                nNewCopy--;
            }
        }

        Increment(nNew);
        return idx;
    }

    MatrixXd Allocate()
    {

        int nNew = 1;
        std::vector<int> idx(capacity);

        if (this.used + nNew > this.capacity)
        {
            std::cerr << "Can not allocate" << nNew << "objects";
            idx.resize(0);
            return;
        }

        int nNewCopy = nNew;
        for (int i = 0; i < capacity; i++)
        {
            if (nNewCopy && this.list[i] == 0)
            {
                idx[i] = 1;
                this.list[i] = 1;
                nNewCopy--;
            }
        }

        Increment(nNew);
        return idx;
    }

    void Deallocate(std::vector<int> idx) // в матлабе подается массив idx = {2 5 6}
    {

        int numel = std::accumulate(vec.begin(), vec.end(), 0LL);

        if (!numel) // если в idx все 0
        {
            return;
        }

        if (idx.size() > this.capacity) // если количество элементов в idx больше capacity
        {
            std::cerr << "Can not delete " << numel << " objects" << std::endl;
            exit(1);
        }

        for (int i = 0; i < capacity; i++)
        {
            if (dx[i] == 1)
            {
                this.list[i] = 0;
            }
        }

        Increment(-numel);
    }
};
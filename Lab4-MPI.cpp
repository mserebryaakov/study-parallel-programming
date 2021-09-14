#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include "mpi.h"

#define FIRST_THREAD 0
#define LEN 95000

using namespace std;

clock_t startTime;
clock_t endTime;

struct Data {
    char str[LEN];
    char txt[LEN];
    long pPower[LEN];
    long hashs_t[LEN];
    long hash_s = 0;
};

//Считавание текста из файла и ввод строки, возврат длинны строки и текста
int* InputFile(char* strCopy, char* txtCopy)
{
    int* size = new int[2];
    string txt, str;
    ifstream file("text.txt");
    while (file)
        getline(file, txt);
    if (txt.length() == 0)
        cout << "File is empty";
    cout << "Input string: ";
    cin >> str;

    for (size_t i = 0; i < str.length(); i++)
        strCopy[i] = str[i];
    strCopy[str.length()] = static_cast<char>('\n');

    for (size_t i = 0; i < txt.length(); i++)
        txtCopy[i] = txt[i];
    txtCopy[txt.length()] = static_cast<char>('\n');

    size[0] = str.length();
    size[1] = txt.length();
    return size;
}

//Расчет степеней Р для хэша
void initializePPower(Data* myData, int* size)
{
    const int P = 31;
    myData->pPower[0] = 1;
    for (int i = 1; i < size[1]; i++)
        myData->pPower[i] = myData->pPower[i - 1] * P;
}

//Расчет хэшей для подстрок текста
void createHashsT(Data* myData, int* size)
{
    for (int i = 0; i < size[1]; i++)
    {
        myData->hashs_t[i] = (static_cast<int>(myData->txt[i]) - 97 + 1) * myData->pPower[i];
        if (i) myData->hashs_t[i] += myData->hashs_t[i - 1];
    }
}

//Расчет хэша для строки
void createHashS(Data* myData, int* size)
{
    for (int i = 0; i < size[0]; i++)
        myData->hash_s += (static_cast<int>(myData->str[i]) - 97 + 1) * myData->pPower[i];
}

//Расчет хэшей в интервале и поиск подстроки
void parallelCreateResult(int fromWhere, int toWhere, Data* myData, int* size)
{
    bool result = false;
    for (int i = fromWhere; i + size[0] - 1 < toWhere; i++)
    {
        long cur_h = myData->hashs_t[i + size[0] - 1];
        if (i) cur_h -= myData->hashs_t[i - 1];
        if (cur_h == myData->hash_s * myData->pPower[i])
        {
            cout << i << " ";
            result = true;
        }
    }
    if (!result) cout << "No matches";
}

//Инициализация данных
int* InitData(struct Data* myData)
{
    int* size = new int[2];
    size = InputFile(myData->str, myData->txt);
    startTime = clock();
    initializePPower(myData, size);
    createHashsT(myData, size);
    createHashS(myData, size);
    return size;
}

//mpiexec -n 3 Lab4ParallelMPI.exe
int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    int thread_size, thread;
    int forWhere;
    int toWhere;
    int step;
    Data myData;
    int* interval = new int[2];
    int* size = new int[2];

    MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD, &thread_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &thread);

    MPI_Datatype structMPI;
    int len[5] = { LEN, LEN, LEN, LEN, 1 };
    MPI_Aint pos[5] = { 0,
        LEN,
        LEN * 2,
        LEN * 2 + LEN * 4,
        LEN * 2 + LEN * 4 + LEN * 4 };
    MPI_Datatype type[5] = { MPI_CHAR, MPI_CHAR, MPI_LONG, MPI_LONG, MPI_LONG };
    MPI_Type_create_struct(5, len, pos, type, &structMPI);
    MPI_Type_commit(&structMPI);

    if (thread == FIRST_THREAD)
        size = InitData(&myData);

    MPI_Bcast(size, 2, MPI_INT, FIRST_THREAD, MPI_COMM_WORLD);
    MPI_Bcast(&myData, 1, structMPI, FIRST_THREAD, MPI_COMM_WORLD);

    if (thread == FIRST_THREAD)
    {
        printf("Num threads: %d\n", thread_size);
        forWhere = 0;
        toWhere = size[1] / (thread_size - 1);
        step = toWhere;
        toWhere += size[1] % thread_size;
        for (int which_thread = 1; which_thread < thread_size; which_thread++)
        {
            interval[0] = forWhere;
            interval[1] = toWhere;
            MPI_Ssend(interval, 2, MPI_INT, which_thread, 0, MPI_COMM_WORLD);
            forWhere = toWhere - size[0] + 1;
            toWhere += step;
        }
    }

    if (thread != FIRST_THREAD)
    {
        MPI_Recv(interval, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        parallelCreateResult(interval[0], interval[1], &myData, size);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (thread == FIRST_THREAD)
    {
        endTime = clock();
        printf("Execution time - %f\n", (double)(endTime - startTime) / CLOCKS_PER_SEC);
    }
    MPI_Finalize();
}
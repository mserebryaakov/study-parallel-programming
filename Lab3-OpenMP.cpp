#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <ctime>
#include <windows.h>
#include <omp.h>

using namespace std;

string str, txt;
vector<long long> pPower;
vector<long long> hashs_t;
long long hash_s = 0;

void openMPInput(int i) //Критическая секция
{
#pragma omp critical
    {
        cout << i << " ";
    }
}

void RabinKarpInitialization(string what, string where)
{
    hashs_t.resize(txt.length());
    pPower.resize(max(str.length(), txt.length()));
}

void initializePPower() //Расчет степеней p
{
    const int P = 31;
    pPower[0] = 1;
    for (size_t i = 1; i < pPower.size(); i++)
        pPower[i] = pPower[i - 1] * P;
}

void createHashsT() //Расчет хэшей для подстрок текста
{
    for (size_t i = 0; i < txt.length(); i++)
    {
        hashs_t[i] = (txt[i] - 'a' + 1) * pPower[i];
        if (i) hashs_t[i] += hashs_t[i - 1];
    }
}

void createHashS() //Расчет хэша для строки
{
    for (size_t i = 0; i < str.length(); i++)
        hash_s += (str[i] - 'a' + 1) * pPower[i];
}

void createResult()
{
    bool result = false;
    for (size_t i = 0; i + str.length() - 1 < txt.length(); i++)
    {
        long long cur_h = hashs_t[i + str.length() - 1];
        if (i) cur_h -= hashs_t[i - 1];
        if (cur_h == hash_s * pPower[i])
        {
            cout << i << " ";
            result = true;
        }
    }
    if (!result) cout << "Вхождений не найдено";
}

void parallelCreateResult(int fromWhere, int toWhere)
{
    bool result = false;
    for (size_t i = fromWhere; i + str.length() - 1 < toWhere; i++)
    {
        long long cur_h = hashs_t[i + str.length() - 1];
        if (i) cur_h -= hashs_t[i - 1];
        if (cur_h == hash_s * pPower[i])
        {
            openMPInput(i);
            result = true;
        }
    }
    if (!result) cout << "Вхождений не найдено";
}

int inputCountThreads()
{
    int countThreads;
    do
    {
        cout << "Введите количество потоков: ";
        cin >> countThreads; cout << '\n';
    } while ((txt.length() / str.length() < countThreads) || (countThreads < 2));
    return countThreads;
}

void start()
{
    initializePPower();
    createHashsT();
    createHashS();
    createResult();
}

void openMPStart(int countThreads)
{
    initializePPower();
    omp_set_num_threads(countThreads);
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            createHashS();
        }
        #pragma omp section
        {
            createHashsT();
        }
    }

    int step = txt.length() / countThreads;
    #pragma omp barrier

    #pragma omp parallel
    {
    #pragma omp for schedule(guided)
        for (int i = 0; i < txt.length(); i += step)
        {
            parallelCreateResult(i - (str.length() + 1) * (i & 1), i + step);
        }
    }

    /*#pragma omp parallel
    {
    #pragma omp for
        for (int i = 0; i < txt.length(); i += step)
        {
            parallelCreateResult(i - (str.length() + 1) * (i & 1), i + step);
        }
    }*/
}

int main()
{
    setlocale(LC_ALL, "Russian");
    ifstream file("text.txt");
    while (file)
    {
        getline(file, txt);
    }
    if (txt.length() == 0)
    {
        cout << "Файл пуст";
        return 0;
    }
    cout << "Введите строку для поиска: ";
    cin >> str;

    RabinKarpInitialization(str, txt);
    int countThreads = inputCountThreads();
    clock_t leadTime = clock();
    start();
    clock_t endTime = clock();
    cout << endl << (double)(endTime - leadTime) / CLOCKS_PER_SEC << endl;

    leadTime = clock();
    hash_s = 0;
    openMPStart(countThreads);
    endTime = clock();
    cout << endl << (double)(endTime - leadTime) / CLOCKS_PER_SEC << endl;

    cin >> str;
}

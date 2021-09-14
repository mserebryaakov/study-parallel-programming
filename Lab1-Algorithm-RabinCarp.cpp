#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <ctime>

using namespace std;

class RabinKarp {
public:
    RabinKarp(string what, string where)
    {
        str = what;
        txt = where;
    }

    void start()
    {
        pPower = initializePPower();
        hashs_t = createHashsT();
        hash_s = createHashS();
        createResult();
    }

private:
    string str, txt;
    vector<long long> pPower;
    vector<long long> hashs_t;
    long long hash_s;

    vector<long long> initializePPower() //Расчет степеней p
    {
        const int P = 31;
        vector<long long> pPower(max(str.length(), txt.length()));
        pPower[0] = 1;
        for (size_t i = 1; i < pPower.size(); i++)
            pPower[i] = pPower[i - 1] * P;
        return pPower;
    }

    vector<long long> createHashsT() //Расчет хэшей для подстрок текста
    {
        vector<long long> hashs_t(txt.length());
        cout << "\nХэши текста: ";
        for (size_t i = 0; i < txt.length(); i++)
        {
            hashs_t[i] = (txt[i] - 'a' + 1) * pPower[i];
            if (i) hashs_t[i] += hashs_t[i - 1];
            cout << hashs_t[i] << " ";
        }
        return hashs_t;
    }

    long long createHashS() //Расчет хэша для строки
    {
        long long hash_s = 0;
        cout << "\nХэш строки: ";
        for (size_t i = 0; i < str.length(); i++)
            hash_s += (str[i] - 'a' + 1) * pPower[i];
        cout << hash_s;
        return hash_s;
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
};

class NaiveMethod {
public:
    NaiveMethod(string what, string where)
    {
        str = what;
        txt = where;
    }

    void start()
    {
        hashs_t = createHashsT();
        hash_s = createHashS();
        createResult();
    }

private:
    string str, txt;
    vector<long long> hashs_t;
    long long hash_s;
    
    vector<long long> createHashsT() //Расчет хэшей для каждой подстроки
    {
        long long hash;
        vector<long long> hashs_t(txt.length() - str.length() + 1);
        for (size_t i = 0; i < txt.length() - str.length() + 1; i++)
        {
            for (size_t j = i; j < i + str.length(); j++)
            {
                hash = txt[j] - 'a' + 1;
                hashs_t[i] += hash;
            }
        }
        return hashs_t;
    }

    long long createHashS() //Расчет хэши для строки
    {
        long long hash_s = 0;
        for (size_t i = 0; i < str.length(); i++)
            hash_s += (str[i] - 'a' + 1);
        return hash_s;
    }

    bool compareStrings(size_t fwhere) //Посимвольное сравнение строк
    {
        for (size_t i = fwhere; i < fwhere + str.length(); i++)
        {
            if (str[i - fwhere] != txt[i])
                return false;
        }
        return true;
    }

    void createResult()
    {
        for (size_t i = 0; i < txt.length() - str.length() + 1; i++)
        {
            if (hashs_t[i] == hash_s)
            {
                if (compareStrings(i) == true) cout << i << " ";
            }
        }
    }
};

int main()
{
    setlocale(LC_ALL, "Russian");
    string str, txt;
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
    cout << "Строка в файле: " << txt << endl << "Введите строку для поиска: ";
    cin >> str;

    RabinKarp algoritm(str, txt);
    clock_t leadTime = clock();
    algoritm.start();
    clock_t endTime = clock();
    cout << endl << (double)(endTime - leadTime) / CLOCKS_PER_SEC << endl;

    NaiveMethod algoritm2(str, txt);
    leadTime = clock();
    algoritm2.start();
    endTime = clock();
    cout << endl << (double)(endTime - leadTime) / CLOCKS_PER_SEC << endl;

    cin.get();
}

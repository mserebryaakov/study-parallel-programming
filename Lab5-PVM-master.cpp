#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <pvm3.h>
#include <stdio.h>

#define LEN 1000
#define COUNTTHREAD 3

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
    strCopy[str.length()] = static_cast<char>('\0');

    for (size_t i = 0; i < txt.length(); i++)
        txtCopy[i] = txt[i];
    txtCopy[txt.length()] = static_cast<char>('\0');

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

int main(int argc, char** argv)
{
    int forWhere;
    int toWhere;
    int step;
    Data myData;
    int* interval = new int[2];
    int* size = new int[2];
	
	int* tid = new int[COUNTTHREAD-1];

	int inum = pvm_joingroup("final");

	numt = pvm_spawn("sslave",0,0,0,COUNTTHREAD-1,&tid[0]);

	if (numt != 1)
	{
		printf("Error start slave");
		return 0;
	}
	
    size = InitData(&myData);

	int info;
	pvm_initsend(PvmDataDefault);
	info = pvm_pkstr(myData.str)
	info = pvm_pkstr(myData.txt)
	info = pvm_pklong(myData.pPower,LEN,1);
	info = pvm_pklong(myData.hashs_t,LEN,1);
	info = pvm_pklong(myData.hash_s,1,1);
	info = pvm_pkint(size,2,1);
	info = pvm_bcast("final",1);

    forWhere = 0;
    toWhere = size[1] / (COUNTTHREAD - 1);
    step = toWhere;
    toWhere += size[1] % COUNTTHREAD;
    for (int which_thread = 1; which_thread < COUNTTHREAD; which_thread++)
    {
        interval[0] = forWhere;
        interval[1] = toWhere;
		
		pvm_initsend(PvmDataDefault);
		pvm_pkint(interval,2,1);
		pvm_send(ptid,2);

        forWhere = toWhere - size[0] + 1;
        toWhere += step;
	}

    int infob = pvm_barrier("final",COUNTTHREAD);
	
    endTime = clock();
    printf("Execution time - %f\n", (double)(endTime - startTime) / CLOCKS_PER_SEC);
	
    pvm_exit();
	exit(0);
}
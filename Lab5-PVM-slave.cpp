#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <pvm3.h>
#include <stdio.h>

#define LEN 1000
#define COUNTTHREAD 3

using namespace std;

struct Data {
    char str[LEN];
    char txt[LEN];
    long pPower[LEN];
    long hashs_t[LEN];
    long hash_s = 0;
};

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
			printf("%d",i)
            result = true;
        }
    }
    if (!result) cout << "No matches";
}

int main(int argc, char** argv)
{
    Data myData;
    int* interval = new int[2];
    int* size = new int[2];

	int info;
	int suminfo = 0;
	int ptid = pvm_parent();
	
	int inum = pvm_joingroup("final");
	
	pvm_recv(ptid,1)
	
	info = pvm_upkstr(myData.str)
	suminfo += info;
	
	info = pvm_upkstr(myData.txt)
	suminfo += info;
	
	info = pvm_upklong(myData.pPower,LEN,1);
	suminfo += info;
	
	info = pvm_upklong(myData.hashs_t,LEN,1);
	suminfo += info;
	
	info = pvm_upklong(myData.hash_s,1,1);
	suminfo += info;
	
	info = pvm_upkint(size,2,1);
	suminfo += info;
	
	pvm_recv(ptid,2)
	
	info = pvm_upkint(interval,2,1);
	suminfo += info;
	
	if (suminfo != 0)
	{
		printf("Unpack error %d", suminfo);
		return 0;
	}

    parallelCreateResult(interval[0], interval[1], &myData, size);

	int infob = pvm_barrier("final",COUNTTHREAD);

    pvm_exit();
	return 0;
}
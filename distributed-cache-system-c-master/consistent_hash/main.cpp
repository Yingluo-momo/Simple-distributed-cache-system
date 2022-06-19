#include <iostream>
#include <string.h>
#include <cstdlib>
#include <ctime>
#include "consistenthash.h"

using namespace std;

int main()
{
    ConsistentHash ch(100);
//    ch.TestBalance(10000);
//    cout << ch.TestDelete();
    cout << ch.TestAdd();
    return 0;
}

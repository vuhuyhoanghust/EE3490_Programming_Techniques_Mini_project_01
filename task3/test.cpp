#include <iostream>
#include <fstream>
#include <string> // std::string, std::stof
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <vector>
#include <cstring> // memset
#include "check_string.h"
using namespace std;

int main()
{
    string a = "2023:01:12 01:00:00";
    cout << isTime(a);
}
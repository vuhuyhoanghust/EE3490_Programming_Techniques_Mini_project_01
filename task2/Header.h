#pragma once
#include <iostream>
#include <fstream>
#include <string> // std::string, std::stof
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <vector>
#include <cstring> // memset

using namespace std;

class dust_data
{
public:
    string time;
    float value;
    int index;
    struct tm tm;

    void disp()
    {
        cout << this->index << "\t" << this->time << "\t" << this->value << "\n";
    }

    void analy_time()
    {
        std::istringstream ss(time);
        ss >> std::get_time(&tm, "%Y:%m:%d %H:%M:%S");
    }
};

struct aqi
{
    int aqi_index;
    string aqi_level;
    int aqi_encode;
};

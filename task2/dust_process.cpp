#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <vector>


using namespace std;

class dust_data
{
public:
    int id;
    vector<string> time;
    vector<float> value;
    vector<string> level;
    int index_max;
    int index_min;
    string time_period;

};

main(int argc, char **argv)
{

    string filename;
    if (argc == 1)
    {
        filename = "dust_sim.csv";
    }
    else
    {
        filename = argv[1];
    }

    dust_data *sensor = new dust_data[10000];

    ifstream myfile;
    myfile.open(filename, ios::in);

    myfile.close();
}
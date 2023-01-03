#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

/* Function to check a number */
bool isNumber(const string &s)
{
    for (char const &ch : s)
    {
        if (std::isdigit(ch) == 0)
            return false;
    }
    return true;
}

/* Function to check error input */
int check_err(int count, char **args)
{
    if (count%2==0){
        return 1;
    }
    
}

/* main */
int main(int argc, char **argv)
{
    const int dust_con_max = 600;
    /* Read input arguments */
    cout << "You have entered " << argc
         << " arguments:"
         << "\n";

    for (int i = 0; i < argc; ++i)
        cout << argv[i] << "\n";

    /*Check error arguments*/
    ofstream task1_err;
    task1_err.open("task1.log", ios::trunc);

    /* Write error */
    const char *err_note[] = {"No error", "Invalid command", "Invalid argument",
                              "Denied access dust_sensor.csv"};

    if (check_err(argc, argv) != 0)
    {
        task1_err << "Error " << setw(2) << setfill('0') <<
        check_err(argc, argv) << ": " << err_note[check_err(argc, argv)];
        task1_err.close();
        return 1;
    }

    /* Declare variable */
    int n, st, si;
    n = atoi(argv[2]);
    st = atoi(argv[4]);
    si = atoi(argv[6]);
    int count = si * 3600 / st; // number of sampling times
    float dust_con;             // dust concentration

    /* Current Time*/
    time_t now = time(0); // current time from 1970 (seconds)
    srand(now);
    tm *ltm = localtime(&now); // struct time tm
    /* Start time from 00:00:00*/
    ltm->tm_hour = 0;
    ltm->tm_min = 0;
    ltm->tm_sec = 0;
    now = mktime(ltm);

    /* Open file dust_sensor */
    ofstream myfile;
    myfile.open("dust_sensor.csv", ios::trunc);

    /* Write title */
    myfile << "id,time,values" << endl;

    /* Write result */
    for (int i = 0; i < count; i++)
    {
        for (int j = 0; j < n; j++)
        {
            dust_con = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / dust_con_max));
            myfile << j + 1 << "," << 1900 + ltm->tm_year << ":" << setw(2) << setfill('0') << 1 + ltm->tm_mon << ":" << setw(2) << setfill('0') << ltm->tm_mday << " " << setw(2) << setfill('0') << ltm->tm_hour << ":" << setw(2) << setfill('0') << ltm->tm_min << ":" << setw(2) << setfill('0') << ltm->tm_sec << "," << fixed << setprecision(1) << dust_con << endl;
        }
        now = now + st; // next time point
        ltm = localtime(&now);
    }

    myfile.close();
    return 0;
}
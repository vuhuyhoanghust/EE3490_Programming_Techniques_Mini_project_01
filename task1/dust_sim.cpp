#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

/* Function to check a number */
bool isNumber(const string &s);

/* Function to check error input */
int check_err(int count, char **args, int check_file);

/* Write to file error*/
void write_err(ofstream &file_err, ofstream &file_src, int check_err);

/* Assign value for variables*/
void assign(string mer_str, int *vari, int argc, char **argv);

/* Export data of sensor to myfile*/
void exp_data(ofstream &myfile, int *vari);

/* main */
int main(int argc, char **argv)
{

    string mer_str;
    int vari[3]; // vari[0]: number of sensors, vari[1]: sampling, vari[2]: interval

    for (int i = 0; i < argc; ++i)
        mer_str = mer_str + argv[i];

    /* Open file dust_sensor */
    ofstream myfile;
    myfile.open("dust_sensor.csv", ios::trunc);

    /*Check error arguments*/
    ofstream task1_err;
    task1_err.open("task1.log", ios::trunc);

    /* Write error */
    int chk = check_err(argc, argv, !myfile);
    write_err(task1_err, myfile, chk);
    if (chk != 0)
        return 0;

    /* Assign value for variables*/
    assign(mer_str, &vari[0], argc, argv);

    /* Export data of sensor to myfile*/
    exp_data(myfile, &vari[0]);

    return 0;
}

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
int check_err(int count, char **args, int check_file)
{
    if (check_file == 1)
    {
        return 3;
    }
    if (count % 2 == 0)
    {
        return 1;
    }
    for (int u = 1; u < count / 2; u++)
    {
        if (atoi(args[2 * u]) < 1)
            return 2;
    }
    return 0;
}

/* Write to file error*/
void write_err(ofstream &file_err, ofstream &file_src, int check_err)
{
    const char *err_note[] = {"No error", "Invalid command", "Invalid argument",
                              "Denied access dust_sensor.csv"};

    file_err << "Error " << setw(2) << setfill('0') << check_err << ": " << err_note[check_err];
    file_err.close();
}

/* Assign value for variables*/
void assign(string mer_str, int *vari, int argc, char **argv)
{
    int val_def[3] = {1, 30, 24}; // Default value for n, st, si
    int index[3];
    index[0] = mer_str.find("-n");
    index[1] = mer_str.find("-st");
    index[2] = mer_str.find("-si");

    for (int i = 0; i < 3; i++)
    {
        int sum = 0;
        if (index[i] != string::npos)
        {
            index[i] = 1;
            for (int j = 0; j <= i; j++)
            {
                sum = sum + index[j];
            }
            vari[i] = atoi(argv[2 * sum]);
        }
        else
        {
            index[i] = 0;
            vari[i] = val_def[i];
        }
    }
}

/* Export data of sensor to myfile*/
void exp_data(ofstream &myfile, int *vari)

{
    int dust_con_max = 600;
    int count = vari[2] * 3600 / vari[1]; // number of sampling times
    float dust_con;                       // dust concentration

    /* Current Time*/
    time_t now = time(0); // current time from 1970 (seconds)
    srand(now);
    time_t past = now - vari[2] * 3600;
    tm *ltm = localtime(&past); // struct time tm

    /* Write title */
    myfile << "id,time,values" << endl;

    /* Write result */
    for (int i = 0; i <= count; i++)
    {
        for (int j = 0; j < vari[0]; j++)
        {
            dust_con = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / dust_con_max));
            myfile << j + 1 << "," << 1900 + ltm->tm_year << ":" << setw(2) << setfill('0') << 1 + ltm->tm_mon << ":" << setw(2) << setfill('0') << ltm->tm_mday << " " << setw(2) << setfill('0') << ltm->tm_hour << ":" << setw(2) << setfill('0') << ltm->tm_min << ":" << setw(2) << setfill('0') << ltm->tm_sec << "," << fixed << setprecision(1) << dust_con << endl;
        }
        past = past + vari[1]; // next time point
        ltm = localtime(&past);
    }
    myfile.close();
}
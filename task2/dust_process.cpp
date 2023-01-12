#include <iostream>
#include <fstream>
#include <string> // std::string, std::stof
#include <cstdlib>
#include <ctime>

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

    void analy_time(){
        memset(&tm, 0, sizeof(struct tm));
        strptime("2001-11-12 18:31:01", "%Y-%m-%d %H:%M:%S", &tm);
    }
};

/* Set filename*/
string set_filename(int argc, char **argv);

/* Import data to data_valid, data_outlier*/
void import_data(vector<dust_data> &data_valid, vector<dust_data> &data_outlier, int &count_valid, int &count_outlier, int &count1, ifstream &myfile, int &num_sen);

/* Export outlier data */
void export_outlier(vector<dust_data> &data_outlier, int count_outlier, ofstream &oulier_data);

int main(int argc, char **argv)
{
    string filename = set_filename(argc, argv);

    /* Open file of sensor's data (Read file) */
    ifstream myfile;
    myfile.open(filename, ios::in);

    /* Open file to export outlier data */
    ofstream oulier_data;
    oulier_data.open("dust_outlier.csv", ios::out);

    /* Creat vector of data sensor */
    vector<dust_data> data_valid;
    vector<dust_data> data_outlier;

    int count1 = 0;
    int count_valid = 0;   // number of valid data
    int count_outlier = 0; // number of outlier data

    int num_sen = 0; // number of sensor

    /* Import data*/
    import_data(data_valid, data_outlier, count_valid, count_outlier, count1, myfile, num_sen);

    /* Export outlier data to dust_outlier.csv */
    export_outlier(data_outlier, count_outlier, oulier_data);

    cout << count_valid << "\t" << count_outlier << "\t" << num_sen;

    return 0;
}
/* Set filename*/
string set_filename(int argc, char **argv)
{
    if (argc == 1)
    {
        return "dust_sim.csv";
    }
    else
    {
        return argv[1];
    }
}

/* Import data to data_valid, data_outlier*/
void import_data(vector<dust_data> &data_valid, vector<dust_data> &data_outlier, int &count_valid, int &count_outlier, int &count1, ifstream &myfile, int &num_sen)
{
    dust_data temp;
    string line, word;
    while (1)
    {

        getline(myfile, line);
        stringstream s(line);
        while (getline(s, word, ','))
        {
            switch (count1)
            {
            case 0:
                temp.index = stoi(word);
                break;
            case 1:
                temp.time = word;
                break;
            case 2:
                temp.value = stof(word);
                break;
            default:
                break;
            }
            count1 = (count1 + 1) % 3;
        }
        if (temp.value >= 5 && temp.value <= 550.5)
        {
            data_valid.push_back(temp);
            count_valid++;
        }
        else
        {
            data_outlier.push_back(temp);
            count_outlier++;
        }
        if (temp.index > num_sen)
            num_sen = temp.index;
        if (myfile.eof())
            break;
    }
}

/* Export outlier data */
void export_outlier(vector<dust_data> &data_outlier, int count_outlier, ofstream &oulier_data)
{
    oulier_data << "number of outliers: " << count_outlier << endl;
    oulier_data << "id,time,values" << endl;
    for (int i = 0; i < count_outlier; i++)
    {
        oulier_data << data_outlier[i].index << "," << data_outlier[i].time << "," << data_outlier[i].value << endl;
    }
    oulier_data.close();
}
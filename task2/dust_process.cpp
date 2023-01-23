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

string level_def[] = {"Good", "Moderate", "Slightly unhealthy", "Unhealthy", "Very unhealthy", "Hazardous", "Extremely hazardous"};

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
/* Set filename*/
string set_filename(int argc, char **argv);

/* Import data to data_valid, data_outlier*/
void import_data(vector<dust_data> &data_valid, vector<dust_data> &data_outlier, int &count_valid, int &count_outlier, int &count1, ifstream &myfile, int &num_sen);

/* Export outlier data */
void export_outlier(vector<dust_data> &data_outlier, int count_outlier, ofstream &oulier_data);

/* AQI calculate*/
aqi aqi_cal(float n);

int main(int argc, char **argv)
{
    string filename = set_filename(argc, argv);

    /* Open file of sensor's data (Read file) */
    ifstream myfile;
    myfile.open(filename, ios::in);

    /* Open file to export outlier data */
    ofstream oulier_data;
    oulier_data.open("dust_outlier.csv", ios::out);

    /* Open file to export outlier data */
    ofstream aqi_data;
    aqi_data.open("dust_aqi.csv", ios::trunc);

    /* Open file to export dust_summary */
    ofstream dust_summary;
    dust_summary.open("dust_summary.csv", ios::out);

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

    /* Array of min, max, mean*/
    float *min_index = new float[num_sen];
    float *max_index = new float[num_sen];
    float *sum_sen = new float[num_sen];
    float *count_sen = new float[num_sen];

    for (int i = 0; i < num_sen; i++)
    {
        min_index[i] = 0;
        max_index[i] = 0;
        sum_sen[i] = 0;
        count_sen[i] = 0;
    }

    /* Define start and finish time point */
    time_t aqi_timet_start = mktime(&data_valid[0].tm), aqi_timet_end = mktime(&data_valid[count_valid - 1].tm);
    aqi_timet_start = ((aqi_timet_start / 3600) + 1) * 3600;
    aqi_timet_end = ((aqi_timet_end / 3600) + 1) * 3600;

    int num_pt_time = (aqi_timet_end - aqi_timet_start) / 3600 + 1; // number of times checking aqi index

    float **sum_per_hour = new float *[num_sen]; // array stores sum of value of sensors per hour
    for (int i = 0; i < num_sen; i++)
    {
        sum_per_hour[i] = new float[num_pt_time];
        for (int j = 0; j < num_pt_time; j++)
        {
            sum_per_hour[i][j] = 0; // initial condition
        }
    }

    int **num_per_hour = new int *[num_sen]; // array stores number of times the sensor retrieves the result per hour
    for (int i = 0; i < num_sen; i++)
    {
        num_per_hour[i] = new int[num_pt_time];
        for (int j = 0; j < num_pt_time; j++)
        {
            num_per_hour[i][j] = 0; // initial condition
        }
    }

    int row_index, col_index;

    /* Read all valid-data line and store in 2 arrays above with respective index*/
    for (int i = 0; i < count_valid; i++)
    {
        row_index = data_valid[i].index - 1;
        col_index = (mktime(&data_valid[i].tm) - aqi_timet_start + 3600) / 3600;
        sum_per_hour[row_index][col_index] += data_valid[i].value;
        num_per_hour[row_index][col_index]++;
        sum_sen[row_index] += data_valid[i].value;
        count_sen[row_index]++;
        if (data_valid[min_index[row_index]].value > data_valid[i].value)
        {
            min_index[row_index] = i;
        }
        else if (data_valid[max_index[row_index]].value < data_valid[i].value)
        {
            max_index[row_index] = i;
        }
    }
    /* Export aqi index to file*/
    char buffer[80];
    aqi_data << "id,time,values,aqi,pollution" << endl;
    struct aqi aqi_temp;
    float mean_per_hour;
    for (int j = 0; j < num_pt_time; j++)
    {
        for (int i = 0; i < num_sen; i++)
        {
            aqi_data << i + 1 << ",";
            strftime(buffer, 26, "%Y:%m:%d %H:%M:%S", localtime(&aqi_timet_start));
            mean_per_hour = sum_per_hour[i][j] / num_per_hour[i][j];
            aqi_temp = aqi_cal(mean_per_hour);
            aqi_data << buffer << "," << fixed << setprecision(1) << mean_per_hour << "," << aqi_temp.aqi_index << "," << aqi_temp.aqi_level << endl;
        }
        aqi_timet_start += 3600;
    }

    /* Export to summary file*/
    struct dust_data temp;
    double dist = abs(difftime(mktime(&data_valid[0].tm), mktime(&data_valid[count_valid - 1].tm)));
    uint64_t hour = floor(dist / 3600);
    int minu = floor(dist / 60) - 60 * hour;
    int seco = dist - hour * 3600 - minu * 60;
    cout << dist;

    dust_summary << "id, parameters, time, values" << endl;
    for (int i = 0; i < num_sen; i++)
    {
        temp = data_valid[max_index[i]];
        dust_summary << i + 1 << ", max," << temp.time << "," << temp.value << endl;
        temp = data_valid[min_index[i]];
        dust_summary << i + 1 << ", min," << temp.time << "," << temp.value << endl;
        dust_summary << i + 1 << ", mean,";
        dust_summary << std::setfill('0') << std::setw(2) << hour << ":" << std::setfill('0') << std::setw(2) << minu << ":" << std::setfill('0') << std::setw(2) << seco;
        dust_summary << "," << fixed << setprecision(1) << sum_sen[i] / count_sen[i] << endl;
    }

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
    getline(myfile, line);
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
            data_valid[count_valid].analy_time();
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

/* AQI calculate*/
aqi aqi_cal(float n)
{
    // n: average value per hour
    aqi temp;
    float slope;
    float concen_def[] = {0, 12, 35.5, 55.5, 150.5, 250.5, 350.5, 550.5};
    float aqi_def[] = {0, 50, 100, 150, 200, 300, 400, 500};
    int length_arr = sizeof(concen_def) / sizeof(float);
    for (int i = 1; i < length_arr; i++)
    {
        if (n < concen_def[i])
        {
            slope = (aqi_def[i] - aqi_def[i - 1]) / (concen_def[i] - concen_def[i - 1]);
            temp.aqi_index = aqi_def[i - 1] + slope * (n - concen_def[i - 1]);
            temp.aqi_level = level_def[i - 1];
            temp.aqi_encode = i-1;
            return temp;
        }
    }
    temp.aqi_index = 0;
    temp.aqi_level = "Error";
    return temp;
}
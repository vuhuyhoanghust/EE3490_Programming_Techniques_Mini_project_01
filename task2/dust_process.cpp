#include "Header.h"
#include "export_lib.h"
#include "check_string.h"

string level_def[] = {"Good", "Moderate", "Slightly unhealthy", "Unhealthy", "Very unhealthy", "Hazardous", "Extremely hazardous"};
int num_of_level = sizeof(level_def) / sizeof(level_def[0]);

/* Set filename*/
string set_filename(int argc, char **argv);

/* Check invalid data*/
bool check_err_dataline(string word, int count1);

/* Import data to data_valid, data_outlier*/
void import_data(vector<dust_data> &data_valid, vector<dust_data> &data_outlier, int &count_valid, int &count_outlier, ifstream &myfile, int &num_sen, ofstream &error_file);

/* dynamically allocate 2 dimensional array */
void dynam_2D_float(float **&array, int row, int col);
void dynam_2D_int(int **&array, int row, int col);

int main(int argc, char **argv)
{
    string filename = set_filename(argc, argv);
    /* Open error file to write */
    ofstream error_file;
    error_file.open("task2.log", ios::trunc);

    /* Open file of sensor's data (Read file) */
    ifstream myfile;
    myfile.open(filename, ios::in);
    if (!myfile == 1)
    {
        error_file << "Error 01: file not found or cannot be accesse";
        return 1;
    }

    /* Open file to export outlier data */
    ofstream oulier_data;
    oulier_data.open("dust_outlier.csv", ios::out);

    /* Open file to export outlier data */
    ofstream aqi_data;
    aqi_data.open("dust_aqi.csv", ios::trunc);

    /* Open file to export dust_summary */
    ofstream dust_summary;
    dust_summary.open("dust_summary.csv", ios::trunc);

    /* Open file to export dust_statistics */
    ofstream dust_statistics;
    dust_statistics.open("dust_statistics.csv", ios::trunc);

    /* Creat vector of data sensor */
    vector<dust_data> data_valid;
    vector<dust_data> data_outlier;

    int count_valid = 0;   // number of valid data
    int count_outlier = 0; // number of outlier data

    int num_sen = 0; // number of sensor

    /* Import data*/
    import_data(data_valid, data_outlier, count_valid, count_outlier, myfile, num_sen, error_file);

    /* Export outlier data to dust_outlier.csv */
    export_outlier(data_outlier, count_outlier, oulier_data, level_def);

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

    float **sum_per_hour; // array stores sum of value of sensors per hour
    dynam_2D_float(sum_per_hour, num_sen, num_pt_time);

    int **num_per_hour; // array stores number of times the sensor retrieves the result per hour
    dynam_2D_int(num_per_hour, num_sen, num_pt_time);

    int **level_stat; // array stores statistics level of pollution
    dynam_2D_int(level_stat, num_sen, num_of_level);

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
    export_aqi(aqi_data, sum_per_hour, num_per_hour, num_pt_time, num_sen, level_stat, aqi_timet_start, level_def);

    /* Export to summary file*/
    export_summary(data_valid, num_sen, max_index, min_index, sum_sen, count_sen, count_valid, dust_summary);

    /* Export to statistics file*/
    export_stat(dust_statistics, num_sen, level_stat, level_def, num_of_level);

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
void import_data(vector<dust_data> &data_valid, vector<dust_data> &data_outlier, int &count_valid, int &count_outlier, ifstream &myfile, int &num_sen, ofstream &error_file)
{
    int count1;          // Index of data per line: 0 is id, 1 is time, 2 is values, 3 is aqi, 4 is pollution
    int count_line = 0;  // line index
    bool check_err_data; // check error of data
    dust_data temp;
    string line, word;
    getline(myfile, line);
    while (!myfile.eof())
    {
        check_err_data = 1;
        getline(myfile, line);
        count1 = 0;
        count_line++;
        stringstream s(line);

        while (getline(s, word, ','))
        {
            check_err_data = check_err_dataline(word, count1);
            if (check_err_data == 0)
                break; // if data is invalid, exit the loop
            else
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
        }
        if (check_err_data == 0)
            error_file << "Error 02: data is missing at line " << count_line << endl;
        else
        {
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
        }
    }
}

/* dynamically allocate 2 dimensional array */
void dynam_2D_float(float **&array, int row, int col)
{

    array = new float *[row];
    for (int i = 0; i < row; i++)
    {
        array[i] = new float[col];
        for (int j = 0; j < col; j++)
        {
            array[i][j] = 0; // initial condition
        }
    }
}

void dynam_2D_int(int **&array, int row, int col)
{

    array = new int *[row];
    for (int i = 0; i < row; i++)
    {
        array[i] = new int[col];
        for (int j = 0; j < col; j++)
        {
            array[i][j] = 0; // initial condition
        }
    }
}

bool check_err_dataline(string word, int count1)
{
    // return 0 if (string) word is invalid data
    if (isBlank(word) == 1)
        return 0;
    switch (count1)
    {
    case 0:
        return isPosIntNumber(word);
    case 1:
        return isTime(word);
    case 2:
        return isPosRealNumber(word);
    default:
        return 1;
    }
}
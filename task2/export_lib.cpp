#include "export_lib.h"


/* AQI calculate*/
aqi aqi_cal(float n, string level_def[])

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
            temp.aqi_encode = i - 1;
            return temp;
        }
    }
    temp.aqi_index = 0;
    temp.aqi_level = "Error";
    return temp;
}
/* Export outlier data */
void export_outlier(vector<dust_data> &data_outlier, int count_outlier, ofstream &oulier_data, string level_def[])
{
    oulier_data << "number of outliers: " << count_outlier << endl;
    oulier_data << "id,time,values" << endl;
    for (int i = 0; i < count_outlier; i++)
    {
        oulier_data << data_outlier[i].index << "," << data_outlier[i].time << "," << data_outlier[i].value << endl;
    }
    oulier_data.close();
}



/* Export to statistics file*/
void export_stat(ofstream &dust_statistics, int num_sen, int **level_stat, string level_def[], int num_of_level)
{
    dust_statistics << "id, pollution,duration" << endl;
    for (int i = 0; i < num_sen; i++)
    {
        for (int j = 0; j < num_of_level; j++)
        {
            dust_statistics << i + 1 << "," << level_def[j] << "," << level_stat[i][j] << endl;
        }
    }
}

/* Export to summary file*/
void export_summary(vector<dust_data> &data_valid, int num_sen, float *max_index, float *min_index, float *sum_sen, float *count_sen, int count_valid, ofstream &dust_summary)
{
    struct dust_data temp;
    double dist = abs(difftime(mktime(&data_valid[0].tm), mktime(&data_valid[count_valid - 1].tm)));
    uint64_t hour = floor(dist / 3600);
    int minu = floor(dist / 60) - 60 * hour;
    int seco = dist - hour * 3600 - minu * 60;

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
}

/* Export to aqi file*/
void export_aqi(ofstream &aqi_data, float **sum_per_hour, int **num_per_hour, int num_pt_time, int num_sen, int **&level_stat, time_t aqi_timet_start, string level_def[])
{

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
            aqi_temp = aqi_cal(mean_per_hour, level_def);
            aqi_data << buffer << "," << fixed << setprecision(1) << mean_per_hour << "," << aqi_temp.aqi_index << "," << aqi_temp.aqi_level << endl;
            level_stat[i][aqi_temp.aqi_encode]++;
        }
        aqi_timet_start += 3600;
    }
}
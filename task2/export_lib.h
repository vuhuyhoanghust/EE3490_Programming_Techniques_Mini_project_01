#pragma once
#include "Header.h"


/* Export outlier data */
void export_outlier(vector<dust_data> &data_outlier, int count_outlier, ofstream &oulier_data, string level_def[]);
/* Export to statistics file*/
void export_stat(ofstream &dust_statistics, int num_sen, int **level_stat, string level_def[],int num_of_level);
/* Export to summary file*/
void export_summary(vector<dust_data> &data_valid, int num_sen, float *max_index, float *min_index, float *sum_sen, float *count_sen, int count_valid, ofstream &dust_summary);
/* Export to aqi file*/
void export_aqi(ofstream &aqi_data, float **sum_per_hour, int **num_per_hour, int num_pt_time, int num_sen, int **&level_stat, time_t aqi_timet_start, string level_def[]);
/* AQI calculate*/
aqi aqi_cal(float n, string level_def[]);
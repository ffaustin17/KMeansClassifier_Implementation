#pragma once
#include <string>
#include "data_containers.h"
using namespace std;

void getDataset(string input_file_name, DataPointList& list_of_data_points);
void normalize(const DataPointList& original_dataset, DataPointList& normalized_dataset);
DataPointList standardize(const DataPointList& original_dataset);
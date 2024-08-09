#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include "data_containers.h"
using namespace std;

void getDataset(string input_file_name, DataPointList& list_of_data_points) {
    ifstream input_file(input_file_name);
    int line_counter = 1;
    string temp_line = "";
    string first_line = "";
    unsigned int dimensionality = 0;


    if (input_file.is_open()) {
        while (getline(input_file, temp_line)) {
            //read the file line by line
            if (line_counter == 1) {
                first_line = temp_line;

                //read into the first line to store the dimensionality listed in it
                bool space_encountered = false;
                string str_ver_dimensionality = "";

                for (char character : first_line) {
                    if (space_encountered) {
                        str_ver_dimensionality += character;
                    }


                    if (character == ' ')
                        space_encountered = true;
                }

                dimensionality = stoi(str_ver_dimensionality);
            }
            else {
                //cout << "point " << line_counter - 1 << " added" << endl;
                DataPoint new_point(dimensionality, temp_line, line_counter - 2);
                list_of_data_points.addDataPoint(new_point);
            }

            line_counter++;
        }

        input_file.close();
    }
    else {
        cout << "Unable to open file." << endl;
    }

}

void normalize(const DataPointList& original_dataset, DataPointList& normalized_dataset) {
    //Empty DataPointList, which will be the normalized version of the original dataset and will be what will be returned
    vector<double> new_attributes;
    new_attributes.reserve(original_dataset.getDataPoint(0).getDimensionality());

    double new_attribute = 0;
    int num_points = original_dataset.getNumDataPoints();

    //in the case where the original dataset is not empty
    if (num_points != 0) {

        int dimensionality = original_dataset.getDataPoint(0).getDimensionality();
        vector<double> max_attributes = original_dataset.getMax();
        vector<double> min_attributes = original_dataset.getMin();

        double min = 0;
        double max = 0;
        double curr = 0;

        for (int i = 0; i < num_points; i++) {

            for (int j = 0; j < dimensionality; j++) {

                curr = original_dataset.getDataPoint(i).getAttributeAt(j);
                max = max_attributes[j];
                min = min_attributes[j];

                if (max == min) {
                    new_attribute = 0.5;
                }
                else {
                    new_attribute = (curr - min) / (max - min);
                }

                new_attributes.push_back(new_attribute);
            }
            DataPoint new_point(new_attributes);
            normalized_dataset.addDataPoint(DataPoint(new_point.getDimensionality(), new_point.toString(), i));

            new_attributes.clear();
        }
    }
}

DataPointList standardize(const DataPointList& original_dataset) {
    DataPoint mean_point = original_dataset.getMeanDataPoint();
    int num_points = original_dataset.getNumDataPoints();
    int dimensionality = mean_point.getDimensionality();

    DataPointList standardized_list;

    double curr_point_attribute = 0;
    double attribute_mean = 0;

    double new_attribute = 0;
    vector<double> new_attributes;

    double dist_from_mean = 0;
    double column_sse = 0;
    double std_dev = 0;
    vector<double> std_devs;

    for (int i = 0; i < dimensionality; i++) {
        attribute_mean = mean_point.getAttributes()[i];
        for (int j = 0; j < num_points; j++) {
            curr_point_attribute = original_dataset.getDataPoint(j).getAttributes()[i];

            dist_from_mean = (curr_point_attribute - attribute_mean) * (curr_point_attribute - attribute_mean);
            column_sse += dist_from_mean;
        }

        std_dev = sqrt(column_sse / num_points);
        std_devs.push_back(std_dev);
        column_sse = 0;
    }


    for (int i = 0; i < num_points; i++) {
        for (int j = 0; j < dimensionality; j++) {
            attribute_mean = mean_point.getAttributes()[j];
            std_dev = std_devs[j];
            curr_point_attribute = original_dataset.getDataPoint(i).getAttributes()[j];

            if (std_dev != 0)
                new_attribute = (curr_point_attribute - attribute_mean) / std_dev;
            else
                new_attribute = 0;

            new_attributes.push_back(new_attribute);
        }

        DataPoint new_point(new_attributes);
        standardized_list.addDataPoint(DataPoint(dimensionality, new_point.toString(), i));

        new_attributes.clear();
    }

    return standardized_list;
}
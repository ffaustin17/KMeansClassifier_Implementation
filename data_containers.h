#pragma once
#include <vector>
#include <iostream>
#include <string>
using namespace std;

// object that stores a data point of n dimensionality
class DataPoint {
private:
    vector<double> attributes;
    unsigned int dimensionality = 0;

    //Determines where the datapoint is located in its original dataset (dataset in the file containing all the points).
    //It is stored as a double, because we calculate for mean points/centroids
    //of a group of data points/clusters that may not be in the original dataset but still need to be 
    //given an index. 
    //In this case, the index will be the mean of the indices of all the data points in
    //the group containing this centroid.
    double index = 0;

    //this representation is not only used for printing the point in text output,
    //but serves as the 'unique' string key for a given datapoint in its dataset.
    //Essentially, if two different datapoints (different index) in the same dataset 
    //have the same string representation,then they are basically two duplicate points.
    string string_representation = "";

public:

    //constructors-------------------------------------------------------


    //creates an origin point (form (0,0,...,0)) of n dimensionality
    DataPoint(const unsigned int dimensionality);

    //creates a datapoint with the attributes specified by the parameter
    DataPoint(const vector<double>& attributes);
    //creates a datapoint of n dimensionality based on the string/line representation of a datapoint found in a txt file,
    //the dimensionality of the datapoint, and the index of the line/datapoint being read in the dataset found in the file
    DataPoint(const unsigned int dimensionality, const string& str_list_of_attributes, double index);

    //setters-----------------------------------

    //used mainly in this program to set the index of the updated centroids (not the initial ones)
    void setIndex(const double index);


    //getters----------------------------------------

    double getIndex() const;

    int getDimensionality() const;

    vector<double> getAttributes() const;

    double getAttributeAt(const int attribute_index) const;

    //returns the attributes of the point in a string format where the attributes are separated by one space.
    string toString() const;

    // operator overload--------------------------------------------

    DataPoint operator+(const DataPoint& p);

    void operator+=(const DataPoint& p);

    DataPoint operator/(const double divisor);


    // miscellaneous----------------------------------------------------

    //calculates and returns the squared euclidean distance of the point from another point
    double getEucliDistanceSqrdFrom(const DataPoint& point) const;
};

// container of DataPoint objects
class DataPointList {
private:
    vector<DataPoint> data_point_list;
    vector<double> sum_attributes;
    bool first_point_not_added = true;

    int num_data_points = 0;

public:
    DataPointList(const int num_data_points = 20000);

    void addDataPoint(const DataPoint& data_point);

    void removeDataPoint(int index);

    void emptyList();

    int getNumDataPoints() const;

    DataPoint getMeanDataPoint() const;

    DataPoint getSumOfDataPoints() const;

    vector<DataPoint> getList() const;
    vector<double> getMin() const;

    vector<double> getMax() const;


    string toString() const;

    DataPoint getDataPoint(unsigned int index) const;

};
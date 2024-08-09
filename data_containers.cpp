#include "data_containers.h"
#include <vector>
#include <string>
using namespace std;

//creates an origin point (form (0,0,...,0)) of n dimensionality
DataPoint::DataPoint(const unsigned int dimensionality) {
    this->dimensionality = dimensionality;
    this->attributes.reserve(dimensionality);

    for (int i = 0; i < dimensionality; i++) {
        this->attributes.push_back(0);
        if (i == 0)
            this->string_representation += '0';
        else
            this->string_representation += " 0";
    }
}

//creates a datapoint with the attributes specified by the parameter
DataPoint::DataPoint(const vector<double>& attributes) {
    this->attributes.reserve(attributes.capacity());
    int iteration_num = 0;

    for (double attribute : attributes) {
        this->attributes.push_back(attribute);
        this->dimensionality++;
        if (iteration_num == 0) {
            this->string_representation += to_string(attribute);
        }
        else {
            this->string_representation += (" " + to_string(attribute));
        }

        iteration_num++;
    }
}

//creates a datapoint of n dimensionality based on the string/line representation of a datapoint found in a txt file,
//the dimensionality of the datapoint, and the index of the line/datapoint being read in the dataset found in the file
DataPoint::DataPoint(const unsigned int dimensionality, const string& str_list_of_attributes, double index) {
    this->dimensionality = dimensionality;
    this->attributes.reserve(dimensionality);
    this->string_representation = str_list_of_attributes;
    this->index = index;

    // used to store one completed (string) attribute found in the string line of attributes given
    string str_number = "";

    for (char character : str_list_of_attributes) {
        // the attributes are space-separated in the input file. Therefore, everytime the current character in the iteration is not a space, it should be one of the digits (or decimal point)
        // which form an attribute. We make sure to store all of these important characters and ignore the spaces
        if (character != ' ') {

            str_number += character;
        }
        else {

            // once a space is encountered, we can add the double-precision floating point conversion of the str_number to the data point's list
            // of attributes. We then reset the string number for the next attribute if there any.
            attributes.push_back(stod(str_number));

            str_number = "";
        }
    }

    // only accounting for the spaces leaves out the last attribute, which will not be added to the list even though it is stored in str_number.
    // we make sure to add it after we are done with the line.
    this->attributes.push_back(stod(str_number));
}

//setters-----------------------------------

//used mainly in this program to set the index of the updated centroids (not the initial ones)
void DataPoint::setIndex(const double index) {
    this->index = index;
}


//getters----------------------------------------

double DataPoint::getIndex() const {
    return this->index;
}

int DataPoint::getDimensionality() const
{
    return this->dimensionality;
}

vector<double> DataPoint::getAttributes() const
{
    return this->attributes;
}

double DataPoint::getAttributeAt(const int attribute_index) const
{
    return this->attributes[attribute_index];
}

//returns the attributes of the point in a string format where the attributes are separated by one space.
string DataPoint::toString() const {
    return string_representation;
}

// operator overload--------------------------------------------

DataPoint DataPoint::operator+(const DataPoint& p) {
    vector<double> resulting_attributes;

    for (int i = 0; i < this->dimensionality; i++) {
        resulting_attributes.push_back(this->attributes[i] + p.attributes[i]);
    }

    return DataPoint(resulting_attributes);
}

void DataPoint::operator+=(const DataPoint& p) {
    for (int i = 0; i < this->dimensionality; i++) {
        this->attributes[i] += p.attributes[i];
    }
}

DataPoint DataPoint::operator/(const double divisor) {
    vector<double> resulting_attributes;

    for (int i = 0; i < this->dimensionality; i++) {
        resulting_attributes.push_back(this->attributes[i] / divisor);
    }

    return DataPoint(resulting_attributes);
}

// miscellaneous----------------------------------------------------

//calculates and returns the squared euclidean distance of the point from another point
double DataPoint::getEucliDistanceSqrdFrom(const DataPoint& point) const {
    double dist_sqrd = 0;
    double temp_val = 0;

    for (int i = 0; i < (this->attributes).size(); i++) {
        temp_val = (this->attributes[i] - point.attributes[i]);
        dist_sqrd += (temp_val * temp_val);
    }

    return dist_sqrd;
}









DataPointList::DataPointList(const int num_data_points) {
    this->data_point_list.reserve(num_data_points);
    //creates an empty list of data points - Does nothing essentially
}

void DataPointList::addDataPoint(const DataPoint& data_point) {
    this->data_point_list.push_back(data_point);
    this->num_data_points++;

    if (first_point_not_added) {
        this->sum_attributes.reserve(data_point.getDimensionality());

        first_point_not_added = false;
    }

    //after pushing a point into the DataPointList, we automatically calculate the minimum and maximum values in each attribute

    // initialize the minimum and maximum attributes when the first point is being added to the list before updating them with the attributes of the points

    if (this->sum_attributes.empty()) {
        for (int i = 0; i < data_point.getDimensionality(); i++) {
            sum_attributes.push_back(0);
        }
    }



    double attribute = 0;
    for (int i = 0; i < data_point.getDimensionality(); i++) {
        attribute = data_point.getAttributeAt(i);

        sum_attributes[i] += attribute;
    }
}

void DataPointList::removeDataPoint(int index) {
    this->data_point_list.erase(data_point_list.begin() + index);
    this->num_data_points--;
}

void DataPointList::emptyList() {
    this->data_point_list.clear();
    this->num_data_points = 0;
    for (int i = 0; i < sum_attributes.size(); i++) {
        this->sum_attributes[i] = 0;
    }
}

int DataPointList::getNumDataPoints() const {
    return this->num_data_points;
}

DataPoint DataPointList::getMeanDataPoint() const {
    DataPoint centroid(DataPoint(this->sum_attributes) / this->num_data_points);
    centroid.setIndex(this->num_data_points + 0.5);
    return (centroid);
}

DataPoint DataPointList::getSumOfDataPoints() const {
    return (DataPoint(this->sum_attributes));
}

vector<DataPoint> DataPointList::getList() const {
    return this->data_point_list;
}

vector<double> DataPointList::getMin() const {
    vector<double> min_attributes;
    min_attributes.reserve(this->data_point_list[0].getDimensionality());

    for (int i = 0; i < this->data_point_list[0].getDimensionality(); i++) {
        min_attributes.push_back(numeric_limits<double>::infinity());
    }

    for (int i = 0; i < this->num_data_points; i++) {
        for (int j = 0; j < this->data_point_list[0].getDimensionality(); j++) {
            if (this->data_point_list[i].getAttributeAt(j) < min_attributes[j]) {
                min_attributes[j] = this->data_point_list[i].getAttributeAt(j);
            }
        }
    }


    return min_attributes;
}

vector<double> DataPointList::getMax() const {
    vector<double> max_attributes;
    max_attributes.reserve(this->data_point_list[0].getDimensionality());

    for (int i = 0; i < this->data_point_list[0].getDimensionality(); i++) {
        max_attributes.push_back(numeric_limits<double>::infinity() * (-1));
    }

    for (int i = 0; i < this->num_data_points; i++) {
        for (int j = 0; j < this->data_point_list[0].getDimensionality(); j++) {
            if (this->data_point_list[i].getAttributeAt(j) > max_attributes[j]) {
                max_attributes[j] = this->data_point_list[i].getAttributeAt(j);
            }
        }
    }


    return max_attributes;
}

string DataPointList::toString() const {
    string str_rep = "";
    for (DataPoint data_point : data_point_list) {
        str_rep += data_point.toString();
        str_rep += '\n';
    }

    return str_rep;
}

DataPoint DataPointList::getDataPoint(unsigned int index) const {
    return data_point_list[index];
}
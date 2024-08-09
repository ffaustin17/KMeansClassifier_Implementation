#pragma once
#include <iostream>
#include <vector>
#include "data_containers.h"
using namespace std;

void getProximityMatrix(const DataPointList& original_dataset, vector<vector<double>>& proximity_matrix);
double getDistance(const double first_index, const double second_index, const vector<vector<double>>& proximity_matrix, const vector<vector<double>>& centroid_proximity_matrix, const int centroid_index);

DataPoint getGlobalCentroid(vector<DataPointList>& clusters);
double calculateBetweenClusterTrace(vector<DataPoint>& centroids, vector<DataPointList>& clusters, DataPoint& global_centroid);
double calculateCHIndex(double within_cluster_trace, double between_cluster_trace, int num_clusters, int total_num_datapoints);
void CHScore(vector<DataPointList>& clusters, vector<DataPoint>& centroids, double total_SSE, int num_clusters, int total_num_datapoints, double& ch_score);

double getInterClusterMeanDistance(const DataPoint& point, const DataPointList& cluster, vector<vector<double>>& proximity_matrix, const vector<vector<double>>& centroid_proximity_matrix, const int centroid_index);
double getIntraClusterMeanDistance(const DataPoint& point, const DataPointList& cluster, vector<vector<double>>& proximity_matrix, const vector<vector<double>>& centroid_proximity_matrix, const int centroid_index);
double getMinIntraClusterMeanDistance(const DataPoint& point, vector<DataPointList>& clusters, const int within_cluster_index, vector<vector<double>>& proximity_matrix, const vector<vector<double>>& centroid_proximity_matrix, const int centroid_index);
double getPointSilhouetteCoefficient(double min_intra_cluster_mean_dist, double inter_cluster_mean_dist);
void getMeanSilhouetteCoefficient(vector<DataPointList>& clusters, vector<vector<double>>& proximity_matrix, const vector<vector<double>>& centroid_proximity_matrix, double& mean_sw);

double getClusterDispersion(const DataPointList& cluster, const DataPoint& centroid);
double davies_bouldin_index(const DataPointList& cluster1, const DataPoint& cluster1_centroid, const DataPointList& cluster2, const DataPoint& cluster2_centroid);
double davies_bouldin_score(vector<DataPointList>& clusters, vector<DataPoint>& centroids);
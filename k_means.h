#pragma once
#include <vector>
#include "data_containers.h"
#include "k_means_stats.h"
using namespace std;



vector<DataPointList> divideIntoInitialClusters(const unsigned int num_clusters, const DataPointList& data_point_list);
void randomPartitionInitialization(const unsigned int num_clusters, const DataPointList& data_point_list, vector<DataPointList>& list_of_clusters);
KMeansStats performKMeans(const DataPointList& original_dataset, const int max_iteration_num, const double convergence_threshold, const int num_clusters, vector<DataPointList>& clusters, vector<DataPoint>& initial_centroids, vector<vector<double>>& proximity_matrix, bool rand_partition, double previous_best_sse);

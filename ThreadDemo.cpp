// ThreadDemo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//headers--------------------------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <random>
#include <cstdlib>
#include <iomanip>
#include "data_containers.h"
#include "preprocessing.h"
#include "internal_validation.h"
#include "k_means_stats.h"
#include "k_means.h"
using namespace std;



//main/driver------------------

int main(int argc, char* argv[])
{
    srand(time(nullptr)); //for our use of random generation later

    string input_file_name = "";
    unsigned int num_of_clusters = 0;
    unsigned int max_num_of_iterations = 0;
    double convergence_threshold = 0;
    unsigned int num_of_runs = 0;

    // for this program, we use asserts instead of exceptions because there is simply nothing to handle.
    // This program should only run under the condition that the command line arguments follow the contract for this program.
    assert(argc == 6);

    input_file_name = argv[1];
    num_of_clusters = stoi(argv[2]); assert(num_of_clusters > 1);
    max_num_of_iterations = stoi(argv[3]);  assert(max_num_of_iterations >= 1);
    convergence_threshold = stod(argv[4]); assert(convergence_threshold >= 0);
    num_of_runs = stoi(argv[5]);  assert(max_num_of_iterations >= 1);


    cout << "reading the data..." << endl;
    DataPointList list_of_data_points;
    getDataset(input_file_name, list_of_data_points);

    int total_num_of_points = list_of_data_points.getNumDataPoints();
    int max_num_clusters = round(sqrt(total_num_of_points / 2));

    cout << "normalizing the data..." << endl;
    // save the normalized version of the original dataset as a new dataset
    DataPointList normalized_dataset(list_of_data_points.getNumDataPoints());
    normalize(list_of_data_points, normalized_dataset);

    cout << "computing distance matrix..." << endl << endl;
    // get the proximity matrix of the normalized dataset
    vector<vector<double>> proximity_matrix;
    proximity_matrix.reserve(list_of_data_points.getNumDataPoints());
    getProximityMatrix(normalized_dataset, proximity_matrix);

    KMeansStats norm_rand_partition_best_stats;
    // since all of the runs are almost completely independent, we can consider multithreading computation of equal groups of runs
    vector<KMeansStats> best_stats;

    KMeansStats final_best_stats;

    vector<DataPointList> normrandPartitionClusters;
    normrandPartitionClusters.reserve(max_num_clusters);

    vector<DataPoint> normrandPartCentroids;
    normrandPartCentroids.reserve(max_num_clusters);

    for (int num_clusters = 2; num_clusters <= max_num_clusters; num_clusters++) {
        cout << "K = " << num_clusters << "[";

        //perform the k means for the number R of runs
        for (int i = 0; i < num_of_runs; i++) {
            //cout << "Run #" << i + 1 << endl << endl;
            //get our initial random partition clusters
            randomPartitionInitialization(num_clusters, normalized_dataset, normrandPartitionClusters);

            //get the centroids for the random partition clusters
            
            for (int i = 0; i < num_clusters; i++) {
                normrandPartCentroids.push_back(normrandPartitionClusters[i].getMeanDataPoint());
            }

            //update the best stats for each K mean implementation
            if (i == 0) {
                norm_rand_partition_best_stats = performKMeans(normalized_dataset, max_num_of_iterations, convergence_threshold, num_clusters, normrandPartitionClusters, normrandPartCentroids, proximity_matrix, true, norm_rand_partition_best_stats.getFinalSSE());

            }
            else {

                KMeansStats::updateBestStats(norm_rand_partition_best_stats, performKMeans(normalized_dataset, max_num_of_iterations, convergence_threshold, num_clusters, normrandPartitionClusters, normrandPartCentroids, proximity_matrix, true, norm_rand_partition_best_stats.getFinalSSE()));
            }

            normrandPartitionClusters.clear();
            normrandPartCentroids.clear();
        }

        // at this point we are done with all the runs.
        cout << "]" << endl;
        cout << std::setprecision(6);
        //cout << "SW(" << num_clusters << ") = " << norm_rand_partition_best_stats.getSilhouetteScore() << endl;
        cout << "CH(" << num_clusters << ") = " << norm_rand_partition_best_stats.getCHScore() << endl;
        cout << "DB(" << num_clusters << ") = " << norm_rand_partition_best_stats.getDBScore() << endl << endl;

    }



    return 0;
}







#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <cstdlib>
#include "data_containers.h"
#include "internal_validation.h"
#include "k_means_stats.h"
using namespace std;



// given a dataset and a desired number of clusters n, this function will return a list of n clusters, each containing a centroid data point
// which was uniformly and randomly chosen from the given dataset.
vector<DataPointList> divideIntoInitialClusters(const unsigned int num_clusters, const DataPointList& data_point_list) {
    unsigned int cluster_index = 0;
    const int original_list_size = data_point_list.getNumDataPoints();
    int updated_original_list_size = original_list_size - 1;
    int chosen_index = 0;
    vector<int> illegal_indices;
    bool can_be_chosen = true;

    //create an empty map that will contain the centroids and their respective/unique index in the dataset
    vector <string> unique_str_keys;

    //merse twister random generator engine initialized with both the current time and rand() as seed values
    //this is because some iterations would be so fast that they would occur at the same time returned (I presume)
    //and the random index would be returned for multiple iterations before changing.
    mt19937 gen(time(NULL) + rand());
    uniform_int_distribution<> data_point_picker{ 0,(original_list_size - 1) };

    //a counter variable to avoid infinite loops in case the entirety/majority of the points in the dataset/cluster are duplicates
    int failsafe = 0;

    //empty table that will hold the k clusters wanted for the run of k means.
    //each cluster within will contain the initial centroids chosen uniformly at random at the end of the function
    vector<DataPointList> list_of_clusters;


    // populate our empty list of clusters with a number of empty clusters equal to the num_clusters parameter.
    for (unsigned int i = 0; i < num_clusters; i++) {
        list_of_clusters.push_back(DataPointList());
    }

    //uniformly and randomly divide the original dataPointList amongst the clusters. 
    //Exit the operation if we are running an infinite loop (failsafe condition). This will cause an abort on the program.
    while (cluster_index < num_clusters && failsafe != (original_list_size * 10)) {
        //get a random index between the beginning and end of the data point list to divide
        chosen_index = data_point_picker(gen);

        //choose our our potential centroid data point at the random chosen index
        DataPoint potential_center = data_point_list.getDataPoint(chosen_index);

        //the unique string representation of a point at a position in n space.
        string potential_center_str_key = potential_center.toString();

        // check if the currently chosen point's index has not already been picked in a previous iteration.
        // if that is the case, we cannot use the point at this index as the centroid of another cluster.
        // Note that here we check for the specific record/instance of the datapoint (search by index).
        if (illegal_indices.size() != 0) {
            for (int illegal_index : illegal_indices) {
                if (illegal_index == chosen_index) {
                    can_be_chosen = false;
                    break;
                }
                else {
                    can_be_chosen = true;
                }
            }
        }

        //if the specific point at the chosen index has not already been chosen previously,we need to check if it is not a duplicate
        //of a point that has already been chosen as centroid before. In that case, the point cannot be chosen.
        //Note that if two points have the same string representation, then they are on top of each other (duplicates)
        if (can_be_chosen && unique_str_keys.size() != 0) {
            for (string key : unique_str_keys) {
                if (key == potential_center_str_key) {
                    can_be_chosen = false;
                    break;
                }
                else {
                    can_be_chosen = true;
                }
            }
        }

        //if the point at the chosen index passes both tests, then it can be chosen as an initial cluster
        if (can_be_chosen) {
            //add the DataPoint at this chosen index to the cluster at the computed cluster index
            list_of_clusters[cluster_index].addDataPoint(potential_center);


            // increment counter to go to the next iteration
            cluster_index++;
            failsafe = 0;
            illegal_indices.push_back(chosen_index);
            unique_str_keys.push_back(potential_center_str_key);
        }

        failsafe++;
    }


    return list_of_clusters;
}

void randomPartitionInitialization(const unsigned int num_clusters, const DataPointList& data_point_list, vector<DataPointList>& list_of_clusters) {
    unsigned int cluster_index = 0;
    const int original_list_size = data_point_list.getNumDataPoints();
    int num_points = data_point_list.getNumDataPoints();
    int chosen_index = 0;

    vector<double> available_indices;
    available_indices.reserve(num_points);

    for (int i = 0; i < num_points; i++) {
        available_indices.push_back(data_point_list.getDataPoint(i).getIndex());
    }

    //merse twister random generator engine initialized with both the current time and rand() as seed values
    //this is because some iterations would be so fast that they would occur at the same time returned (I presume)
    //and the random index would be returned for multiple iterations before changing.
    mt19937 gen(time(NULL) + rand());


    // populate our empty list of clusters with a number of empty clusters equal to the num_clusters parameter.
    for (unsigned int i = 0; i < num_clusters; i++) {
        list_of_clusters.push_back(DataPointList(original_list_size));
    }

    int i = 0;
    int points_added = 0;

    while (i < num_points) {
        uniform_int_distribution<int> data_point_picker{ (int)0, (int)(available_indices.size()) };
        cluster_index = i % num_clusters;

        chosen_index = (data_point_picker(gen));

        if (chosen_index == available_indices.size()) {
            chosen_index--;
        }

        if (points_added != num_points) {
            list_of_clusters[cluster_index].addDataPoint(data_point_list.getDataPoint(available_indices[chosen_index]));

            points_added++;


            available_indices.erase(available_indices.begin() + chosen_index);

        }

        i++;
    }

}

KMeansStats performKMeans(const DataPointList& original_dataset, const int max_iteration_num, const double convergence_threshold, const int num_clusters, vector<DataPointList>& clusters, vector<DataPoint>& initial_centroids, vector<vector<double>>& proximity_matrix, bool rand_partition, double previous_best_sse)
{
    KMeansStats final_results;
    char symbol = '.';
    double prev_total_SSE = 0;
    double total_SSE = 0;
    double convergence = numeric_limits<double>::infinity();
    double dist_w_centroid = 0;
    double smallest_dist = 0;
    double nearest_centroid_index = 0;
    int iteration_counter = 0;

    double mean_silhouette_index = 0;

    double CH_score = 0;
    double db_score = 0;

    int num_assignment_threads = 1;

    vector<vector<double>> centroid_proximity_matrix;
    centroid_proximity_matrix.reserve(num_clusters);


    //populate the centroid proximity_matrix with empty centroid distance vectors
    for (int i = 0; i < num_clusters; i++) {
        vector<double> new_centroid_dist_vector;
        new_centroid_dist_vector.reserve(original_dataset.getNumDataPoints());
        centroid_proximity_matrix.push_back(new_centroid_dist_vector);
    }

    //if a random partition initialization was used, our clusters are already populated in addition to already having our centers.
    //therefore we only need to compute the initial sse and move on to update our centers.
    if (rand_partition) {
        for (int i = 0; i < initial_centroids.size(); i++) {
            for (DataPoint& curr_point : clusters[i].getList()) {
                total_SSE += (curr_point.getEucliDistanceSqrdFrom(initial_centroids[i]));
            }
        }
    }

    while (iteration_counter < max_iteration_num && convergence >= convergence_threshold) {
        symbol = '.';
        //cout << "iteration #" << iteration_counter + 1 << ":" << endl << "--------------" << endl;
        if (rand_partition && iteration_counter == 0) {
            // do nothing as the clusters are already populated and the final sse as already been calculated
        }
        else {
            //cout << "assigning points to clusters..." << endl;
            // assign points to nearest centers and calculate final sse while doing so
            for (DataPoint& point : original_dataset.getList()) {

                //for each point we need to calculate the euclidean distance squared between it and all of the centroids.
                //we then assign the point to the cluster with the same index as the centroid with the smallest distance from said point.
                for (int centroid_index = 0; centroid_index < num_clusters; centroid_index++) {
                    dist_w_centroid = point.getEucliDistanceSqrdFrom(initial_centroids[centroid_index]);
                    centroid_proximity_matrix[centroid_index].push_back(dist_w_centroid);
                    if (centroid_index == 0) {
                        smallest_dist = dist_w_centroid;
                        nearest_centroid_index = centroid_index;
                    }
                    else {
                        if (dist_w_centroid < smallest_dist) {
                            smallest_dist = dist_w_centroid;
                            nearest_centroid_index = centroid_index;
                        }
                        else if (dist_w_centroid == smallest_dist) {
                            if (initial_centroids[centroid_index].getIndex() < initial_centroids[nearest_centroid_index].getIndex()) {
                                nearest_centroid_index = centroid_index;
                            }
                        }
                        else {
                            //do nothing
                        }
                    }


                }

                //at this point, we have found the nearest centroid for the point as well as in which index it is located (centroid_index)
                //now we can simply assign the point to the appropriate cluster
                //however, we must not duplicate the centroid which is already in the cluster.
                if (point.getIndex() != initial_centroids[nearest_centroid_index].getIndex() && point.toString() != initial_centroids[nearest_centroid_index].toString()) {
                    clusters[nearest_centroid_index].addDataPoint(point);
                    total_SSE += smallest_dist;
                }


            }
        }

        // at this point, we are done with the iteration.

        // our clusters are fully populated. So we can evaluate the validity of the clustering at k


        // calculate the convergence when we have passed the first iteration
        if (iteration_counter != 0) {
            convergence = (prev_total_SSE - total_SSE) / prev_total_SSE;
        }
        else {
            final_results.setInitialSSE(total_SSE);
        }

        if (convergence < convergence_threshold && total_SSE < previous_best_sse) {
           symbol = '!';

            //getMeanSilhouetteCoefficient(clusters, proximity_matrix, centroid_proximity_matrix, mean_silhouette_index);

            //ch index evaluation
            //cout << "calculating ch..." << endl;
            CHScore(clusters, initial_centroids, total_SSE, num_clusters, original_dataset.getNumDataPoints() + 3, CH_score);
            db_score = davies_bouldin_score(clusters, initial_centroids);

            final_results.setFinalSSE(total_SSE);

        }


        //cout << "updating centers..." << endl;

        //update my centers for the next iteration
        int centroid_pos = 0;
        for (DataPointList& cluster : clusters) {

            initial_centroids[centroid_pos] = cluster.getMeanDataPoint();

            centroid_pos++;
        }


        //cout << "emptying clusters..." << endl;
        // empty my clusters.
        centroid_pos = 0;
        for (int i = 0; i < num_clusters; i++) {
            clusters[i].emptyList(); //emptied the cluster
            clusters[i].addDataPoint(initial_centroids[centroid_pos]);
            centroid_pos++;
        }

        //cout << "clearing centroid proximity matrices..." << endl;
        // empty our centroid_dist_vectors within the centroid_proximity matrix for the next iteration
        for (int i = 0; i < num_clusters; i++) {
            centroid_proximity_matrix[i].clear();
        }


        prev_total_SSE = total_SSE;
        total_SSE = 0;
        iteration_counter++;
       //cout << "end of iteration." << endl << endl;
    }

    //final_results.setSilhouetteScore(mean_silhouette_index);
    final_results.setCHScore(CH_score);
    final_results.setDaviesBouwinScore(db_score);
    //final_results.setFinalSSE(prev_total_SSE);
    final_results.setNumIterations(iteration_counter);

    cout << symbol;
    return final_results;
}


void executeRuns(const unsigned int num_runs, const unsigned int num_clusters, const unsigned int max_num_iterations, const double convergence_threshold, const DataPointList& dataset, vector<vector<double>>& proximity_matrix, bool rand_partition, KMeansStats& best_stats) {
    //cout << "started a thread" << endl;
    for (int i = 0; i < num_runs; i++) {
        //get our initial random partition clusters
        vector<DataPointList> normrandPartitionClusters;
        randomPartitionInitialization(num_clusters, dataset, normrandPartitionClusters);

        //get the centroids for the random partition clusters
        vector<DataPoint> normrandPartCentroids;
        for (int i = 0; i < num_clusters; i++) {
            normrandPartCentroids.push_back(normrandPartitionClusters[i].getMeanDataPoint());
        }

        //update the best stats for each K mean implementation
        //cout << "going to start a kmeans" << endl;

        if (i == 0) {
            best_stats = performKMeans(dataset, max_num_iterations, convergence_threshold, num_clusters, normrandPartitionClusters, normrandPartCentroids, proximity_matrix, true, best_stats.getFinalSSE());

        }
        else {

            KMeansStats::updateBestStats(best_stats, performKMeans(dataset, max_num_iterations, convergence_threshold, num_clusters, normrandPartitionClusters, normrandPartCentroids, proximity_matrix, true, best_stats.getFinalSSE()));
        }

    }

    //cout << "thread finished successfully" << endl;
}




void assignToClusters(const DataPointList& dataset, const int num_points_to_assign, const int start_index, const int num_clusters, vector<DataPoint>& centroids, vector<DataPointList>& clusters, bool rand_partition, vector<vector<double>> centroid_proximity_matrix, double& total_SSE) {

    double dist_w_centroid = 0;
    double smallest_dist = 0;
    double nearest_centroid_index = 0;

    for (int i = start_index; i < start_index + num_points_to_assign; i++) {

        DataPoint point = dataset.getDataPoint(i);

        //for each point we need to calculate the euclidean distance squared between it and all of the centroids.
        //we then assign the point to the cluster with the same index as the centroid with the smallest distance from said point.
        for (int centroid_index = 0; centroid_index < num_clusters; centroid_index++) {
            dist_w_centroid = point.getEucliDistanceSqrdFrom(centroids[centroid_index]);
            centroid_proximity_matrix[centroid_index].push_back(dist_w_centroid);
            if (centroid_index == 0) {
                smallest_dist = dist_w_centroid;
                nearest_centroid_index = centroid_index;
            }
            else {
                if (dist_w_centroid < smallest_dist) {
                    smallest_dist = dist_w_centroid;
                    nearest_centroid_index = centroid_index;
                }
                else if (dist_w_centroid == smallest_dist) {
                    if (centroids[centroid_index].getIndex() < centroids[nearest_centroid_index].getIndex()) {
                        nearest_centroid_index = centroid_index;
                    }
                }
                else {
                    //do nothing
                }
            }


        }

        //at this point, we have found the nearest centroid for the point as well as in which index it is located (centroid_index)
        //now we can simply assign the point to the appropriate cluster
        //however, we must not duplicate the centroid which is already in the cluster.
        if (point.getIndex() != centroids[nearest_centroid_index].getIndex() && point.toString() != centroids[nearest_centroid_index].toString()) {
            clusters[nearest_centroid_index].addDataPoint(point);

            total_SSE += smallest_dist;
        }


    }
}


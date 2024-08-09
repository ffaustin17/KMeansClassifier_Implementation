#include <iostream>
#include <vector>
#include "data_containers.h"
using namespace std;




// -------------------------------Utility Functions-----------------------------------------------------------------

void getProximityMatrix(const DataPointList& original_dataset, vector<vector<double>>& proximity_matrix) {
    vector<double> curr_point_dist_vector;
    curr_point_dist_vector.reserve(original_dataset.getNumDataPoints());

    int curr_point_index = 0;
    int other_point_index = 0;
    int num_points = original_dataset.getNumDataPoints();

    double distance = 0;

    for (int i = 0; i < num_points - 1; i++) {
        for (int j = i + 1; j < num_points; j++) {
            distance = original_dataset.getDataPoint(i).getEucliDistanceSqrdFrom(original_dataset.getDataPoint(j));
            curr_point_dist_vector.push_back(distance);
        }
        // we are done getting the distance of all the other points from the current point
        // we can add this list of distance to the proximity matrix.
        proximity_matrix.push_back(curr_point_dist_vector);

        // we need to clear the curr_point_dist_vector for the next point
        curr_point_dist_vector.clear();
    }
}

double getDistance(const double first_index, const double second_index, const vector<vector<double>>& proximity_matrix, const vector<vector<double>>& centroid_proximity_matrix, const int centroid_index)
{
    double first = 0;
    double second = 0;

    if ((first_index - floor(first_index) == 0.5) || (second_index - floor(second_index) == 0.5))
    {

        if ((first_index - floor(first_index) == 0.5)) {
            second = second_index;
        }
        else if ((first_index - floor(first_index) == 0.5) && (second_index - floor(second_index) == 0.5))
        {
            return -1;
        }
        else
        {
            second = first_index;
        }

        return centroid_proximity_matrix[centroid_index][second];
    }
    else
    {
        if (first_index > second_index) {

            first = second_index;
            second = first_index - second_index - 1;
        }
        else if (first_index == second_index) {
            return 0;
        }
        else {
            first = first_index;
            second = second_index - first_index - 1;
        }

        return proximity_matrix[first][second];
    }
}

DataPoint getGlobalCentroid(vector<DataPointList>& clusters) {
    int dimensionality = clusters[0].getDataPoint(0).getDimensionality();
    int total_num_points = 0;
    DataPoint sum_of_points(dimensionality);

    for (int i = 0; i < clusters.size(); i++) {
        total_num_points += clusters[i].getNumDataPoints();
        sum_of_points += clusters[i].getSumOfDataPoints();
        /*for (int j = 0; j < clusters[i].getNumDataPoints(); j++) {
            sum_of_points += clusters[i].getDataPoint(j);
        }*/
    }

    return sum_of_points / total_num_points;
}

// -----------------------------------------------------------------------------------------------------------------



// -------------------------------CH Score--------------------------------------------------------------------------

double calculateBetweenClusterTrace(vector<DataPoint>& centroids, vector<DataPointList>& clusters, DataPoint& global_centroid) {
    int num_points_in_corresponding_cluster = 0;
    double result = 0;
    for (int i = 0; i < centroids.size(); i++) {
        num_points_in_corresponding_cluster = clusters[i].getNumDataPoints();

        result += (num_points_in_corresponding_cluster * (centroids[i].getEucliDistanceSqrdFrom(global_centroid)));

    }

    return result;
}

double calculateCHIndex(double within_cluster_trace, double between_cluster_trace, int num_clusters, int total_num_datapoints) {

    double coefficient = (total_num_datapoints - num_clusters) / (num_clusters - 1);

    return (between_cluster_trace / within_cluster_trace) * coefficient;
}

void CHScore(vector<DataPointList>& clusters, vector<DataPoint>& centroids, double total_SSE, int num_clusters, int total_num_datapoints, double& ch_score) {
    //get the global centroid
    DataPoint global_centroid(getGlobalCentroid(clusters));

    //get the between cluster trace
    double between_cluster_trace = calculateBetweenClusterTrace(centroids, clusters, global_centroid);

    // our within cluster trace is actually equal to our total sse
    double within_cluster_trace = total_SSE;

    // we can now get the CH score
    ch_score = calculateCHIndex(within_cluster_trace, between_cluster_trace, num_clusters, total_num_datapoints);

}



// -----------------------------------------------------------------------------------------------------------------



// -------------------------------Silhouette Score------------------------------------------------------------------

double getInterClusterMeanDistance(const DataPoint& point, const DataPointList& cluster, vector<vector<double>>& proximity_matrix, const vector<vector<double>>& centroid_proximity_matrix, const int centroid_index)
{
    double dist_to_all_points = 0;

    double curr_point_index = point.getIndex();

    if (cluster.getNumDataPoints() <= 1) {
        return 0;
    }

    for (int i = 0; i < cluster.getNumDataPoints(); i++) {
        dist_to_all_points += getDistance(cluster.getDataPoint(i).getIndex(), curr_point_index, proximity_matrix, centroid_proximity_matrix, centroid_index);
    }


    return (dist_to_all_points / (cluster.getNumDataPoints() - 1));
}

// essentially the same as the interclustermean distance function. This function assumes that the cluster given is a one that does not contain the datapoint.
double getIntraClusterMeanDistance(const DataPoint& point, const DataPointList& cluster, vector<vector<double>>& proximity_matrix, const vector<vector<double>>& centroid_proximity_matrix, const int centroid_index)
{
    double dist_to_all_points = 0;

    double curr_point_index = point.getIndex();

    //cout << "In inter mean distance:" << endl << endl;
    for (int i = 0; i < cluster.getNumDataPoints(); i++) {
        //cout << "at index " << i << " in the cluster" << endl;
        dist_to_all_points += getDistance(cluster.getDataPoint(i).getIndex(), curr_point_index, proximity_matrix, centroid_proximity_matrix, centroid_index);
    }


    return (dist_to_all_points / (cluster.getNumDataPoints()));
}

double getMinIntraClusterMeanDistance(const DataPoint& point, vector<DataPointList>& clusters, const int within_cluster_index, vector<vector<double>>& proximity_matrix, const vector<vector<double>>& centroid_proximity_matrix, const int centroid_index)
{
    double smallest_mean_distance = 0;
    double mean_distance = 0;

    bool first_time = true;

    for (int i = 0; i < clusters.size(); i++) {
        if (i != within_cluster_index)
        {
            mean_distance = getIntraClusterMeanDistance(point, clusters[i], proximity_matrix, centroid_proximity_matrix, centroid_index);

            if (first_time) {
                smallest_mean_distance = mean_distance;
                first_time = false;
            }
            else {
                if (mean_distance < smallest_mean_distance) {
                    smallest_mean_distance = mean_distance;
                }
            }
        }


    }

    return smallest_mean_distance;
}

double getPointSilhouetteCoefficient(double min_intra_cluster_mean_dist, double inter_cluster_mean_dist)
{
    return (min_intra_cluster_mean_dist - inter_cluster_mean_dist) / (max(min_intra_cluster_mean_dist, inter_cluster_mean_dist));
}

void getMeanSilhouetteCoefficient(vector<DataPointList>& clusters, vector<vector<double>>& proximity_matrix, const vector<vector<double>>& centroid_proximity_matrix, double& mean_sw)
{
    double mean_silhouette_index = 0;
    double inter_cluster_mean_dist = 0;
    double min_intra_cluster_mean_dist = 0;
    double silhouette_index = 0;
    double total_of_silhouette_indices = 0;
    int total_num_points = 0;

    for (int i = 0; i < clusters.size(); i++) {

        for (int j = 0; j < clusters[i].getNumDataPoints(); j++) {

            inter_cluster_mean_dist = getInterClusterMeanDistance(clusters[i].getDataPoint(j), clusters[i], proximity_matrix, centroid_proximity_matrix, i);
            min_intra_cluster_mean_dist = getMinIntraClusterMeanDistance(clusters[i].getDataPoint(j), clusters, i, proximity_matrix, centroid_proximity_matrix, i);
            silhouette_index = getPointSilhouetteCoefficient(min_intra_cluster_mean_dist, inter_cluster_mean_dist);
            total_of_silhouette_indices += silhouette_index;


            total_num_points++;

        }
    }

    mean_sw = total_of_silhouette_indices / total_num_points;
}


// -----------------------------------------------------------------------------------------------------------------


double getMinIntraClusterDistance(const DataPoint& point, vector<DataPointList>& clusters, const int within_cluster_index, vector<vector<double>>& proximity_matrix, const vector<vector<double>>& centroid_proximity_matrix, const int centroid_index)
{
    double smallest_distance = 0;
    double distance = 0;

    bool first_time = true;

    double curr_point_index = point.getIndex();

    for (int i = 0; i < clusters.size(); i++) {
        if (i != within_cluster_index)
        {
            distance = getDistance(clusters[centroid_index].getDataPoint(i).getIndex(), curr_point_index, proximity_matrix, centroid_proximity_matrix, centroid_index);

            if (first_time) {
                smallest_distance = distance;
                first_time = false;
            }
            else {
                if (distance < smallest_distance) {
                    smallest_distance = distance;
                }
            }
        }


    }

    return smallest_distance;
}

// -------------------------------------Dunn Score------------------------------------------------------------------

double getMinIntraClusterDistance(DataPointList& cluster_i, DataPointList& cluster_j, const int cluster_index, vector<vector<double>>& proximity_matrix, vector<vector<double>>& centroid_proximity_matrix, int centroid_index)
{
    double distance = 0;
    double smallest_pair_distance = numeric_limits<double>::infinity();

    for (int i = 0; i < cluster_i.getNumDataPoints(); i++) {
        for (int j = 0; j < cluster_j.getNumDataPoints(); j++) {
            distance = getDistance(cluster_i.getDataPoint(i).getIndex(), cluster_j.getDataPoint(j).getIndex(), proximity_matrix, centroid_proximity_matrix, centroid_index);
            
            //they are both centroids
            if (distance == -1) {
                distance = cluster_i.getDataPoint(i).getEucliDistanceSqrdFrom(cluster_j.getDataPoint(j));
            }

            if (distance < smallest_pair_distance) {
                smallest_pair_distance = distance;
            }
        }
    }

    return smallest_pair_distance;
}


// -----------------------------------------------------------------------------------------------------------------





// ----------------------------------Davies Bouwin Score------------------------------------------------------------

double getClusterDispersion(const DataPointList& cluster, const DataPoint& centroid) {
    double euclid_dist_sqrd = 0;
    double total_sse_sqrd = 0;
    double cluster_variance = 0;
    int num_points = cluster.getNumDataPoints();

    for (int i = 0; i < num_points; i++) {
        euclid_dist_sqrd = cluster.getDataPoint(i).getEucliDistanceSqrdFrom(centroid);
        total_sse_sqrd += ((euclid_dist_sqrd) * (euclid_dist_sqrd));
    }

    return (sqrt(total_sse_sqrd / num_points));
}

double davies_bouldin_index(const DataPointList& cluster1, const DataPoint& cluster1_centroid, const DataPointList& cluster2, const DataPoint& cluster2_centroid) {
    return ((getClusterDispersion(cluster1, cluster1_centroid) + getClusterDispersion(cluster2, cluster2_centroid)) / (cluster1_centroid.getEucliDistanceSqrdFrom(cluster2_centroid)));
}

double davies_bouldin_score(vector<DataPointList>& clusters, vector<DataPoint>& centroids) {
    int num_clusters = clusters.size();
    double max_db = numeric_limits<double>::infinity() * (-1);
    double db = 0;
    double sum_of_max_db = 0;

    vector<vector<double>> db_index_matrix;
    db_index_matrix.reserve(num_clusters);

    vector<double> db_indices;

    vector<double> cluster_variances;
    cluster_variances.reserve(num_clusters);
    int k = 0;

    //calculate and store the dispersion of each cluster
    for (int i = 0; i < num_clusters; i++) {
        cluster_variances.push_back(getClusterDispersion(clusters[i], centroids[i]));
    }

    //calculate the davis_bouldin index of every pair of clusters
    //as we compute the indices starting from cluster 1 (or 0), we dynamically populate the davies bouldin triangular matrix
    for (int i = 0; i < num_clusters - 1; i++) {
        max_db = numeric_limits<double>::infinity() * (-1);

        for (int j = i + 1; j < num_clusters; j++) {
            db = davies_bouldin_index(clusters[i], centroids[i], clusters[j], centroids[j]);
            db_indices.push_back(db);

            //cout << "C("<<i + 1 << ".)(" <<j+1<<") : " << db << endl;
            if (db > max_db) {
                max_db = db;
            }
        }
        
        db_index_matrix.push_back(db_indices);
        db_indices.clear();

        k = i - 1;
        if (i != 0) {
            for (int j = 0; j < i; j++) {
                //cout << "i : " << i << " - j : " << j << endl;
                //cout << "is the problem here???????????" << endl << endl;

                db = db_index_matrix[j][k];
                if (db > max_db) {
                    max_db = db;
                }

                k--;
            }
        }
        //cout << endl << "max db: " << max_db << endl;
        sum_of_max_db += max_db;
    }

    //cout << endl;

    //cout << "db: " << sum_of_max_db / num_clusters << endl;

    return (sum_of_max_db / num_clusters);

}
// -----------------------------------------------------------------------------------------------------------------
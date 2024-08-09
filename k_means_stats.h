#pragma once
#include <iostream>
#include <string>
using namespace std;

class KMeansStats {
private:
    double initial_sse = 0;
    double final_sse = std::numeric_limits<double>::infinity();
    int num_iterations = 0;
    double silhouette_score = 0;
    double CH_score = 0;
    double dunn_score = 0;
    double davies_bouwin_score = 0;
public:
    double getInitialSSE();

    double getFinalSSE();

    int getNumIterations();

    double getSilhouetteScore();

    double getCHScore();

    double getDBScore();

    void setSilhouetteScore(double silhouette_score);

    void setCHScore(double ch_score);

    void setDunnIndex(double dunn_index);

    void setDaviesBouwinScore(double davies_bouwin_score);

    void setInitialSSE(double initial_sse);

    void setFinalSSE(double final_sse);

    void setNumIterations(int num_iterations);

    string toString();

    static void updateBestStats(KMeansStats& best_stats, const KMeansStats& comparing_stats);
};

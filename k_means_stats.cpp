#include "k_means_stats.h"

double KMeansStats::getInitialSSE() {
    return this->initial_sse;
}

double KMeansStats::getFinalSSE() {
    return this->final_sse;
}

int KMeansStats::getNumIterations() {
    return this->num_iterations;
}

double KMeansStats::getSilhouetteScore() {
    return this->silhouette_score;
}

double KMeansStats::getCHScore() {
    return this->CH_score;
}

double KMeansStats::getDBScore() {
    return this->davies_bouwin_score;
}

void KMeansStats::setSilhouetteScore(double silhouette_score) {
    this->silhouette_score = silhouette_score;
}

void KMeansStats::setCHScore(double ch_score) {
    this->CH_score = ch_score;
}

void KMeansStats::setDunnIndex(double dunn_index) {
    this->dunn_score = dunn_index;
}

void KMeansStats::setDaviesBouwinScore(double davies_bouwin_score) {
    this->davies_bouwin_score = davies_bouwin_score;
}

void KMeansStats::setInitialSSE(double initial_sse) {
    this->initial_sse = initial_sse;
}

void KMeansStats::setFinalSSE(double final_sse) {
    this->final_sse = final_sse;
}

void KMeansStats::setNumIterations(int num_iterations) {
    this->num_iterations = num_iterations;
}

string KMeansStats::toString() {
    return (to_string(initial_sse) + " " + to_string(final_sse) + " " + to_string(num_iterations));
}

void KMeansStats::updateBestStats(KMeansStats& best_stats, const KMeansStats& comparing_stats) {
    if (comparing_stats.final_sse < best_stats.final_sse) {
        best_stats.final_sse = comparing_stats.final_sse;
        best_stats.silhouette_score = comparing_stats.silhouette_score;
        best_stats.CH_score = comparing_stats.CH_score;
        best_stats.davies_bouwin_score = comparing_stats.davies_bouwin_score;
    }

    if (comparing_stats.initial_sse < best_stats.initial_sse) {
        best_stats.initial_sse = comparing_stats.initial_sse;
    }

    if (comparing_stats.num_iterations < best_stats.num_iterations) {
        best_stats.num_iterations = comparing_stats.num_iterations;
    }
}
# K-Means Clustering Evaluator

This C++ project implements and evaluates the performance of K-Means clustering using internal validation metrics such as the Calinski-Harabasz (CH) Index and Davies-Bouldin (DB) Score. It also includes preprocessing steps like data normalization and supports running multiple randomized experiments to find optimal cluster configurations.

## Features

- Custom K-Means clustering implementation
- Data normalization and standardization
- Support for multiple clustering runs
- Internal validation using:
  - Calinski-Harabasz Index (CH)
  - Davies-Bouldin Index (DB)
  - (Optionally) Silhouette Score
- Proximity matrix precomputation for faster metric evaluation
- Optimized for performance with minimal STL overhead

## Project Structure
- src/
  - data_containers.cpp # DataPoint and DataPointList utility classes implementations
  - preprocessing.cpp # Functions to read, normalize, and standardize datasets.
  - internal_validation.cpp # Evaluation metrics (CH, DB, Silhouette, Dunn)
  - k_means.cpp # Core K-Means clustering algorithm
  - k_means_stats.cpp # struct to collect and update clustering statistics
  - k_means_evaluator.cpp # main driver and orchestrator
- include/
  - data_containers.h
  - ...
- data/
  - iris_bezdek.txt
  

 
## Usage

### Command-Line Arguments
Run the program with:

```bash
./kmeans_evaluator <dataset_path> <num_clusters> <max_iterations> <convergence_threshold> <num_runs>
```

## Input Format
The dataset should follow this format:
```bash
<Some metadata line that ends with dimensionality>
<space-separated data point 1>
<space-separated data point 2>
...
```

## Output
For each number of clusters `K`, you'll see:
- A loading bar `[.....]` representing progress over multiple runs
- Best CH(k) and DB(k) scores across runs

Example output:
```bash
K = 3 [.............................]
CH(3) = 412.34
DB(3) = 0.5621
```

## Compilation Instructions
Use any modern C++ compiler:
```bash
g++ -std=c++17 -o kmeans_evaluator *.cpp
```

## Future Expansions:
- Add support for multithreaded executions of runs.
- Add visualization tools (CSV export)

## Author
This project was developed by me (Fabrice Faustin) as an exploration of clustering techniques and internal validation metrics. Contributions and feedback are welcome!

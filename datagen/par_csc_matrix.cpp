#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <tuple>
#include <chrono>
#include <cmath>
#include <numeric>


/**
 * @namespace parallel
 * @brief Contains functions and data structures for parallel graph processing.
 */
namespace parallel {
    // ------------------ Matrix struct ------------------

    /**
     * @struct CSC_Matrix
     * @brief Represents a graph adjacency matrix in Compressed Sparse Column (CSC) format.
     */
    struct CSC_Matrix {
        long n, m, NNZ, num_null_cols;
        std::vector<long> ROW_INDEX;         // len = NNZ
        std::vector<long> COL_PTR;           // len = n + 1
        std::vector<long> OUT_DEGREE;        // len = n
        std::vector<long> indexes_null_cols; // len = num_null_cols


        /**
         * @brief Initializes a CSC_Matrix object with the given number of nodes and non-zero elements.
         * 
         * @param n The number of nodes in the graph.
         * @param NNZ The number of non-zero elements in the graph.
         */
        CSC_Matrix(long n, long m) : n(n), m(m) {
            NNZ = 0;
            COL_PTR.resize(n + 1);
            COL_PTR[0] = 0;
        }


        /**
         * Adds an edge between two nodes.
         * 
         * @param from_node_id The ID of the node where the edge starts.
         * @param to_node_id The ID of the node where the edge ends.
         */
        void add_edge(long from_node_id, long to_node_id) {
            NNZ++;
            ROW_INDEX.push_back(to_node_id);
        }


        
        /**
         * @brief Adds a column to the compressed sparse column (CSC) matrix.
         * 
         * This function updates the column pointer array `COL_PTR` by setting the value at index `i` to the current number of non-zero elements (`NNZ`).
         * 
         * @param i The index of the column to be added.
         */
        void add_col(long i) {
            COL_PTR[i] = NNZ;
        }
        

        /**
         * @brief Calculates the value of the element in position (i, j) of the matrix.
         * 
         * @param i The index of the source node.
         * @param j The index of the destination node.
         * @return 1 if there is an edge from i to j, 0 otherwise.
         */
        int access(long i, long j) {
            for (long k = COL_PTR[j]; k < COL_PTR[j+1]; k++) {
                if (ROW_INDEX[k] == i) {
                    return 1;
                }
            }

            return 0;
        }


        /**
         * Multiplies the vector with the given vector `v`.
         * 
         * @param v The vector to be multiplied with.
         * @return A pointer to a new vector that is the result of the multiplication.
         */
        std::vector<double>* operator*(std::vector<double> &v) {
            std::vector<double> *result = new std::vector<double>(m, 0);

            for (long i = 0; i < n; i++) {
                for (long j = COL_PTR[i]; j < COL_PTR[i + 1]; j++) {
                    (*result)[ROW_INDEX[j]] += v[i] / OUT_DEGREE[i];
                }
            }

            return result;
        }


        /**
         * @brief Prints the matrix elements up to a specified maximum element.
         * 
         * @param max_el The maximum number of elements to print. Defaults to 10.
         */
        void print(int max_el = 30) {
            int max_n = max_el < n ? max_el : n;
            int max_m = max_el < m ? max_el : m;

            for (long i = 0; i < max_m; i++) {
                for (long j = 0; j < max_n; j++) {
                    std::cout << access(i, j) << " ";
                }
                std::cout << std::endl;
            }

            std::cout << std::endl;
        }


        /**
         * @brief Prints all the matrix information.
         * 
         * @param max_el The maximum number of elements to print. Defaults to 10.
         */
        void print_info(int max_el = 20) {
            std::cout << "Nodes: " << n << std::endl;
            std::cout << "Edges: " << NNZ << std::endl;

            int max1 = max_el < n ? max_el : n;
            int max2 = max_el < NNZ ? max_el : NNZ;
            int max3 = max_el < n + 1 ? max_el : n + 1;
            int max4 = max_el < num_null_cols ? max_el : num_null_cols;

            std::cout << "Row index: ";
            for (long i = 0; i < max2; i++) {
                std::cout << ROW_INDEX[i] << " ";
            }
            std::cout << std::endl;

            std::cout << "Column pointer: ";
            for (long i = 0; i < max3; i++) {
                std::cout << COL_PTR[i] << " ";
            }
            std::cout << std::endl;

            std::cout << "Out degree: ";
            for (long i = 0; i < max1; i++) {
                std::cout << OUT_DEGREE[i] << " ";
            }
            std::cout << std::endl;

            std::cout << "Null columns: " << num_null_cols << std::endl;
            std::cout << "Null columns indexes: ";
            for (long i = 0; i < max4; i++) {
                std::cout << indexes_null_cols[i] << " ";
            }
            std::cout << std::endl;
            std::cout << std::endl;
        }
    };


    // ------------------ Load graph from file ------------------

    /**
     * @brief Reads the header of the graph file and returns the number of nodes and edges.
     * 
     * @param file The input file stream.
     * @return A tuple containing the number of nodes and edges.
     */
    std::tuple<long, long> read_header(std::ifstream &file) {
        std::cout << "Reading header" << std::endl;

        std::string line, nodes, edges;

        // Discard first two lines
        std::getline(file, line);
        std::getline(file, line);

        // Read the number of nodes and edges
        file >> line >> line >> nodes >> line >> edges;

        long n = std::stoi(nodes);
        long NNZ = std::stoi(edges);

        // Discard header line
        std::getline(file, line);
        std::getline(file, line);

        return std::make_tuple(n, NNZ);
    }


    /**
     * @brief Loads a graph from a file and returns its adjacency matrix in CSC format.
     * 
     * @param filename The name of the graph file.
     * @return A pointer to the CSC_Matrix object representing the graph.
     */
    std::vector<CSC_Matrix*> load_graph_CSC(const char *filename, int cores) {
        std::ifstream file(filename);

        // Store the graph adjacency matrix in CSC format
        if (file.is_open()) {
            std::cout << "File opened" << std::endl;

            long n, NNZ;
            std::tie(n, NNZ) = read_header(file);
            std::cout << "Nodes: " << n << std::endl;
            std::cout << "Edges: " << NNZ << std::endl;

            // TO-DO: calculate number of rows per core
            long m = n%cores == 0 ? n/cores : std::ceil(n/cores) + 1; // Number of rows per core
            
            std::cout << "m: " << m << std::endl;

            // Initialize the matrix
            std::vector<CSC_Matrix*> matrices(cores);
            for (int i = 0; i < cores; i++) {
                if (i != cores - 1) {
                    matrices[i] = new CSC_Matrix(n, m);
                } else {
                    matrices[i] = new CSC_Matrix(n, n - m*(cores-1));
                }
            }

            // Read the graph
            std::string from, to;
            long from_node_id, to_node_id;
            long i = 0, sum_rows = 0, col_els = 0;
            std::vector<long> indexes_null_cols, out_degree(n, 0);

            std::cout << "Reading graph" << std::endl;
            while (file >> from >> to) {
                from_node_id = std::stoi(from);
                to_node_id = std::stoi(to); 

                while (i != from_node_id) {
                    i++; 

                    for (int j = 0; j < cores; j++) {
                        (*matrices[j]).add_col(i);
                    }

                    if (col_els == 0) {
                        indexes_null_cols.push_back(i-1);
                    }

                    col_els = 0;
                }

                (*matrices[std::floor(to_node_id / m)]).add_edge(from_node_id, to_node_id % m);
                out_degree[from_node_id]++;
                col_els++;
            }

            std::cout << "End of file" << std::endl;

            while (i != n) {
                i++;
                for (int j = 0; j < cores; j++) {
                    matrices[j]->add_col(i);
                }

                if (col_els == 0) {
                    indexes_null_cols.push_back(i-1);
                }

                col_els = 0;
            }

            std::cout << "Graph loaded" << std::endl;

            // Set the number of null columns and their indexes
            (*matrices[0]).num_null_cols = indexes_null_cols.size();
            (*matrices[0]).indexes_null_cols = indexes_null_cols;
            
            for (int i = 0; i < cores; i++) {
                (*matrices[i]).OUT_DEGREE = out_degree;
            }

            return matrices;

        } else {
            std::cout << "Unable to open file" << std::endl;
            exit(1);
        }
    }

}
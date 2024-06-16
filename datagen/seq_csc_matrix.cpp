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
 * @namespace sequential
 * @brief Contains functions and data structures for sequential graph processing.
 */
namespace sequential {

    // ------------------ Matrix struct ------------------

    /**
     * @struct CSC_Matrix
     * @brief Represents a graph adjacency matrix in Compressed Sparse Column (CSC) format.
     */
    struct CSC_Matrix {
        long n, NNZ, num_null_cols;
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
        CSC_Matrix(long n, long NNZ) : n(n), NNZ(NNZ) {
            ROW_INDEX.resize(NNZ);
            COL_PTR.resize(n + 1);
            OUT_DEGREE.resize(n);
        }


        /**
         * @brief Calculates the value of the element in position (i, j) of the matrix.
         * 
         * @param i The index of the source node.
         * @param j The index of the destination node.
         * @return 1 if there is an edge from i to j, 0 otherwise.
         */
        int access(long i, long j) {
            long start = COL_PTR[j];
            long end = COL_PTR[j + 1];

            for (long k = start; k < end; k++) {
                if (ROW_INDEX[k] == i) {
                    return 1;
                }
            }

            return 0;
        }


        /**
         * @brief Prints the matrix elements up to a specified maximum element.
         * 
         * @param max_el The maximum number of elements to print. Defaults to 10.
         */
        void print(int max_el = 15) {
            int max = max_el < n ? max_el : n;

            for (long i = 0; i < max; i++) {
                for (long j = 0; j < max; j++) {
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
        void print_info(int max_el = 15) {
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
    CSC_Matrix* load_graph_CSC(const char *filename) {
        std::ifstream file(filename);

        // Store the graph adjacency matrix in CSC format
        if (file.is_open()) {
            std::cout << "File opened" << std::endl;

            long n, NNZ;
            std::tie(n, NNZ) = read_header(file);
            std::cout << "Nodes: " << n << std::endl;
            std::cout << "Edges: " << NNZ << std::endl;

            static CSC_Matrix M(n, NNZ);

            // Read the graph
            std::string from, to;
            long from_node_id, to_node_id;
            long i = 0, sum_rows = 0;

            M.COL_PTR[0] = 0;

            std::cout << "Reading graph" << std::endl;
            while (file >> from >> to) {
                from_node_id = std::stoi(from);
                to_node_id = std::stoi(to); 

                while (i != from_node_id) {
                    M.COL_PTR[++i] = sum_rows;
                    
                    M.OUT_DEGREE[i-1] = sum_rows - M.COL_PTR[i-1];

                    if (M.OUT_DEGREE[i-1] == 0) {
                        M.indexes_null_cols.push_back(i-1);
                        M.num_null_cols++;
                    }
                }

                M.ROW_INDEX[sum_rows++] = to_node_id;

                //std::cout << "From: " << from_node_id << " To: " << to_node_id << std::endl;
            }

            std::cout << "End of file" << std::endl;

            while (i != n) {
                M.COL_PTR[++i] = sum_rows;
                M.OUT_DEGREE[i-1] = sum_rows - M.COL_PTR[i-1];

                if (M.OUT_DEGREE[i-1] == 0) {
                    M.indexes_null_cols.push_back(i-1);
                    M.num_null_cols++;
                }
            }

            std::cout << "Graph loaded" << std::endl;

            return &M;

        } else {
            std::cout << "Unable to open file" << std::endl;
            exit(1);
        }
    }


}
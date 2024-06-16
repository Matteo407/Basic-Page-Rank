/**
 * @file main.cpp
 * @brief This file contains the implementation of a program that loads a graph from a file and represents it using the Compressed Sparse Column (CSC) format.
 * It also provides functions to access and print the matrix elements.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <tuple>
#include <chrono>
#include <cmath>
#include <numeric>

#include "../src/seq_page_rank.cpp"

// g++ -O3 main.cpp -o main
// "C:\Users\matte\OneDrive - unive.it\Unive\Cs 1 - Learning with massive data\Code implementations\Assignment 1 - Page Rank\datagen\test_file.txt"


// ------------------ Main ------------------
int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <graph_file>" << std::endl;
        return 1;
    }

    const char *filename = argv[1];

    auto start = std::chrono::high_resolution_clock::now();
    sequential::CSC_Matrix *M = sequential::load_graph_CSC(filename);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time to load the file: " << elapsed.count() << " s" << std::endl << std::endl;
    M->print(); M->print_info(); 

    start = std::chrono::high_resolution_clock::now();
    std::vector<double> *result = Page_Rank(M);
    end = std::chrono::high_resolution_clock::now();

    // Print the result
    std::cout << "v_50: [ ";
    for (int i = 0; i < 10; i++) {
        std::cout << (*result)[i] << " ";
    }
    std::cout << "]" << std::endl << std::endl;

    // Verify if it's still normalized 
    double sum = 0;
    for (long i = 0; i < M->n; i++) {
        sum += (*result)[i];
    }
    std::cout << "Sum: " << sum << std::endl;

    elapsed = end - start;
    std::cout << "Time: " << elapsed.count() << " s" << std::endl;

    return 0;
}


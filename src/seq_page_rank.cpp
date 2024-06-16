#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <tuple>
#include <chrono>
#include <cmath>
#include <numeric>

#include "../datagen/seq_csc_matrix.cpp"

namespace sequential {
    
    // ------------------ Page Rank ------------------

    /**
     * @brief Generates a random vector of the specified size.
     * 
     * @param n The size of the vector.
     * @return A pointer to the generated vector.
     */
    std::vector<double>* gen_random_vector(long n) {
        static std::vector<double> *v = new std::vector<double>(n);

        std::srand(std::time(0));
        for (int i = 0; i < n; i++) {
            (*v)[i] = (double)rand() / RAND_MAX;
        }

        // Normalize the vector
        double sum = 0;
        for (long i = 0; i < n; i++) {
            sum += (*v)[i];
        }

        for (long i = 0; i < n; i++) {
            (*v)[i] /= sum;
        }

        return v;
    }


    std::vector<double>* page_rank_iter(CSC_Matrix *M, std::vector<double> *v) {

        // std::cout << "Starting Page-Rank Iteration" << std::endl;
        
        // Calculate the sum of the contribution of the dangling ends
        double sum = 0;
        for (int i = 0; i < M->num_null_cols; i++) {
            sum += (*v)[M->indexes_null_cols[i]]/M->n;
        }    

        // std::cout << "Sum: " << sum << std::endl;
        
        std::vector<double> *output = new std::vector<double>(M->n, 0.85*sum+0.15/M->n);

        // Matrix multiplication
        for (long i = 0; i < M->n; i++) {
            for (long j = M->COL_PTR[i]; j < M->COL_PTR[i + 1]; j++) {
                (*output)[M->ROW_INDEX[j]] += 0.85 * (*v)[i] / M->OUT_DEGREE[i];
            }
        }

        return output;
    }

    std::vector<double>* Page_Rank(CSC_Matrix *M) {
        std::vector<double> *result, *temp = gen_random_vector(M->n);

        double norm = 1;
        while (norm >= 1e-6) {
            result = page_rank_iter(M, temp);

            // Norm of the difference
            norm = sqrt(inner_product(result->begin(), result->end(), temp->begin(), 0.0, std::plus<>(), [](double a, double b) { return (a - b) * (a - b); }));

            temp = result;
        }

        return result;
    }
}
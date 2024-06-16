#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <tuple>
#include <chrono>
#include <cmath>
#include <numeric>

#include "../datagen/par_csc_matrix.cpp"

// ------------------ Page Rank ------------------
// The Page Rank algorithm is a link analysis algorithm used by Google Search to rank websites in their search engine results.


namespace parallel {
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
            (*v)[i] = rand() % 100;
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

    /**
     * @brief Performs a single iteration of the Page Rank algorithm.
     * 
     * @param matrices A vector of CSC_Matrix pointers.
     * @param v The input vector.
     * @param cores The number of cores to use for parallelization.
     * @return A pointer to the resulting vector.
     */
    std::vector<double>* page_rank_iter(std::vector<CSC_Matrix*> matrices, std::vector<double> *v, int cores) { 
        // Calculate contribution of null columns
        double sum = 0;

        for (int i = 0; i < matrices[0]->num_null_cols; i++) {
            sum += (*v)[matrices[0]->indexes_null_cols[i]]/matrices[0]->n;
        }

        std::vector<double> *result = new std::vector<double>(matrices[0]->n, 0);

        #pragma omp parallel for num_threads(cores) 
        for (int i = 0; i < cores; i++) {
            // std::cout << (*matrices[i]).NNZ << std::endl;
            std::vector<double> *temp = (*matrices[i]) * (*v);

            for (int j = 0; j < (*matrices[i]).m; j++) {
                (*result)[j + i*(*matrices[i]).m] = 0.85*(*temp)[j] + 0.85*sum + 0.15/matrices[0]->n;
            }
        }

        return result;
    }

    /**
     * @brief Performs the Page Rank algorithm using parallel computation.
     * 
     * @param matrices A vector of CSC_Matrix pointers.
     * @param cores The number of cores to use for parallelization.
     * @return A pointer to the final Page Rank vector.
     */
    std::vector<double>* Page_Rank(std::vector<CSC_Matrix*> matrices, int cores) {
        std::vector<double> *result, *temp = gen_random_vector(matrices[0]->n);

        double norm = 1;
        while (norm >= 1e-6) { 
            result = page_rank_iter(matrices, temp, cores);

            // Norm of the difference
            norm = sqrt(inner_product(result->begin(), result->end(), temp->begin(), 0.0, std::plus<>(), [](double a, double b) { return (a - b) * (a - b); }));
            
            temp = result;
        }

        return result;
    }
}
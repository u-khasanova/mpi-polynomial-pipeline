#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <cmath>
#include <mpi.h>

/**
 * Polynomial class representing a mathematical polynomial
 * Coefficients are stored as [a0, a1, a2, ...] for a0 + a1*x + a2*x² + ...
 */
class Polynomial {
private:
    std::vector<float> coefficients;
    
public:
    Polynomial(const std::vector<float>& coeffs) : coefficients(coeffs) {}
    
    /**
     * Evaluate polynomial at point x using Horner's method
     */
    float evaluate(float x) const {
        float result = 0.0f;
        float power = 1.0f;  // x^0 = 1
        
        for (size_t i = 0; i < coefficients.size(); ++i) {
            result += coefficients[i] * power;
            power *= x;
        }
        return result;
    }
    
    /**
     * Compute partial sum for a specific term
     */
    float compute_term(int term_index, float x) const {
        if (term_index < 0 || static_cast<size_t>(term_index) >= coefficients.size()) {
            return 0.0f;
        }
        float power = 1.0f;
        for (int i = 0; i < term_index; ++i) {
            power *= x;
        }
        return coefficients[term_index] * power;
    }
    
    /**
     * String representation of the polynomial for display
     */
    std::string toString() const {
        if (coefficients.empty()) return "0";
        
        std::string result;
        size_t n = coefficients.size();
        
        for (int i = static_cast<int>(n) - 1; i >= 0; --i) {
            if (coefficients[i] == 0) continue;
            
            if (!result.empty()) result += " + ";
            
            if (i == 0) {
                result += std::to_string(coefficients[i]);
            } else if (i == 1) {
                result += std::to_string(coefficients[i]) + "*x";
            } else {
                result += std::to_string(coefficients[i]) + "*x^" + std::to_string(i);
            }
        }
        return result;
    }
    
    // Getters
    int degree() const { return static_cast<int>(coefficients.size()) - 1; }
    const std::vector<float>& getCoefficients() const { return coefficients; }
};

/**
 * Parse command line arguments and extract polynomial coefficients and x
 */
bool parse_arguments(int argc, char* argv[], float& x, std::vector<float>& coefficients) {
    if (argc < 3) {
        std::cerr << "Error: Insufficient arguments provided." << std::endl;
        return false;
    }
    
    try {
        x = std::stof(argv[1]);
        
        // Parse coefficients (remaining arguments)
        for (int i = 2; i < argc; ++i) {
            coefficients.push_back(std::stof(argv[i]));
        }
        
        if (coefficients.empty()) {
            std::cerr << "Error: No polynomial coefficients provided." << std::endl;
            return false;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error parsing arguments: " << e.what() << std::endl;
        return false;
    }
    
    return true;
}

/**
 * Print program usage information
 */
void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " <x> <coefficients...>" << std::endl;
    std::cout << "  x           - Point at which to evaluate polynomial" << std::endl;
    std::cout << "  coefficients - Polynomial coefficients (a0 a1 a2 ...)" << std::endl;
    std::cout << std::endl;
    std::cout << "Example: " << std::endl;
    std::cout << "  mpiexec -n 4 " << program_name << " 2.0 1 2 3 4" << std::endl;
    std::cout << "  This computes P(2.0) for polynomial 4x^3 + 3x^2 + 2x + 1" << std::endl;
}

int main(int argc, char* argv[]) {
    int rank, size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    float x;
    std::vector<float> coeffs;
    Polynomial* poly = nullptr;
    
    // Process 0 - handle input and validation
    if (rank == 0) {
        if (!parse_arguments(argc, argv, x, coeffs)) {
            print_usage(argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        
        poly = new Polynomial(coeffs);
        std::cout << "Polynomial: " << poly->toString() << std::endl;
        std::cout << "Degree: " << poly->degree() << std::endl;
        std::cout << "Evaluation point: x = " << x << std::endl;
        std::cout << "Number of processes: " << size << std::endl;
    }
    
    // Broadcast parameters to all processes
    MPI_Bcast(&x, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
    // Broadcast polynomial coefficients
    int degree;
    if (rank == 0) {
        degree = static_cast<int>(coeffs.size());
    }
    MPI_Bcast(&degree, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    std::vector<float> broadcast_coeffs(degree);
    if (rank == 0) {
        broadcast_coeffs = coeffs;
    }
    MPI_Bcast(broadcast_coeffs.data(), degree, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
    if (rank != 0) {
        poly = new Polynomial(broadcast_coeffs);
    } else {
        poly = new Polynomial(coeffs);
    }
    
    float partial_sum = 0.0f;
    float accumulated_sum = 0.0f;
    
    if (rank == 0) {
        /**
         * Process 0 - compute first term and start the pipeline
         */
        int terms_per_process = (degree + size - 1) / size;
        int start_term = rank * terms_per_process;
        int end_term = std::min(start_term + terms_per_process, degree);
        
        for (int i = start_term; i < end_term; ++i) {
            float term_value = poly->compute_term(i, x);
            partial_sum += term_value;
            std::cout << "Process " << rank << ": computed term " << i 
                      << " (a" << i << "*x^" << i << ") = " 
                      << std::fixed << std::setprecision(6) << term_value << std::endl;
        }
        
        accumulated_sum = partial_sum;
        std::cout << "Process " << rank << ": partial sum = " 
                  << std::fixed << std::setprecision(6) << partial_sum << std::endl;
        
        if (size > 1) {
            MPI_Send(&accumulated_sum, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
        }
    }
    
    if (rank > 0 && rank < size) {
        /**
         * Intermediate processes - compute terms and accumulate sum
         */
        MPI_Recv(&accumulated_sum, 1, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        int terms_per_process = (degree + size - 1) / size; // ceiling division
        int start_term = rank * terms_per_process;
        int end_term = std::min(start_term + terms_per_process, degree);
        
        for (int i = start_term; i < end_term; ++i) {
            float term_value = poly->compute_term(i, x);
            partial_sum += term_value;
            std::cout << "Process " << rank << ": computed term " << i 
                      << " (a" << i << "*x^" << i << ") = " 
                      << std::fixed << std::setprecision(6) << term_value << std::endl;
        }
        
        accumulated_sum += partial_sum;
        std::cout << "Process " << rank << ": partial sum = " 
                  << std::fixed << std::setprecision(6) << partial_sum 
                  << ", accumulated sum = " << accumulated_sum << std::endl;
        
        if (rank < size - 1) {
            MPI_Send(&accumulated_sum, 1, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD);
        }
    }
    
    // Final result - displayed by the last process
    if (rank == size - 1) {
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "FINAL RESULT: P(" << x << ") = " 
                  << std::fixed << std::setprecision(6) << accumulated_sum << std::endl;
        
        float direct_result = 0.0f;
        if (rank == 0) {
            direct_result = poly->evaluate(x);
        } else {
            if (rank == size - 1) {
                int signal = 1;
                MPI_Send(&signal, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
                MPI_Recv(&direct_result, 1, MPI_FLOAT, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
        
        std::cout << "Verification (direct computation): " << direct_result << std::endl;
        std::cout << "Difference: " << std::abs(accumulated_sum - direct_result) << std::endl;
    }
    
    if (rank == 0 && size > 1) {
        MPI_Status status;
        int signal;
        // Check if there's a verification request
        MPI_Probe(MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
        if (status.MPI_TAG == 2) {
            MPI_Recv(&signal, 1, MPI_INT, status.MPI_SOURCE, 2, MPI_COMM_WORLD, &status);
            float direct_result = poly->evaluate(x);
            MPI_Send(&direct_result, 1, MPI_FLOAT, status.MPI_SOURCE, 3, MPI_COMM_WORLD);
        }
    }
    
    delete poly;
    MPI_Finalize();
    
    return 0;
}
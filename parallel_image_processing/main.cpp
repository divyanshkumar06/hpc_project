#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace cv;

// Defines a 2D Kernel structure
struct Kernel {
    int size;
    vector<vector<float>> data;
};

// Generates a Gaussian Kernel
Kernel createGaussianKernel(int size, float sigma) {
    Kernel k;
    k.size = size;
    k.data.resize(size, vector<float>(size));
    float sum = 0.0f;
    int half = size / 2;

    for (int i = -half; i <= half; i++) {
        for (int j = -half; j <= half; j++) {
            float r = sqrt(i * i + j * j);
            k.data[i + half][j + half] = (exp(-(r * r) / (2 * sigma * sigma))) / (M_PI * 2 * sigma * sigma);
            sum += k.data[i + half][j + half];
        }
    }
    // Normalize the kernel
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            k.data[i][j] /= sum;
        }
    }
    return k;
}

// Sobel Kernels for Edge Detection
Kernel createSobelKernelX() {
    Kernel k;
    k.size = 3;
    k.data = {{-1, 0, 1},
              {-2, 0, 2},
              {-1, 0, 1}};
    return k;
}

Kernel createSobelKernelY() {
    Kernel k;
    k.size = 3;
    k.data = {{-1, -2, -1},
              { 0,  0,  0},
              { 1,  2,  1}};
    return k;
}

// --------------------------------------------------------------------------------------
// SERIAL CONVOLUTION
// --------------------------------------------------------------------------------------
void serialConvolution(const Mat& input, Mat& output, const Kernel& kernel) {
    int rows = input.rows;
    int cols = input.cols;
    int kSize = kernel.size;
    int halfK = kSize / 2;
    int channels = input.channels();

    // Cache Locality: Traversing the image in Row-Major Order (y, then x).
    // This perfectly matches how data is laid out in memory, minimizing cache misses.
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            for (int c = 0; c < channels; c++) {
                float sum = 0.0f;

                for (int ky = -halfK; ky <= halfK; ky++) {
                    for (int kx = -halfK; kx <= halfK; kx++) {
                        int iy = y + ky;
                        int ix = x + kx;

                        // Boundary Handling: Zero-Padding
                        // If the index is out of bounds, we simply do not add to the sum (effectively padding with 0)
                        if (iy >= 0 && iy < rows && ix >= 0 && ix < cols) {
                            float pixelVal = input.at<Vec3b>(iy, ix)[c];
                            float kernelVal = kernel.data[ky + halfK][kx + halfK];
                            sum += pixelVal * kernelVal;
                        }
                    }
                }
                
                // Clamp values between 0 and 255
                sum = max(0.0f, min(255.0f, sum));
                output.at<Vec3b>(y, x)[c] = static_cast<uchar>(sum);
            }
        }
    }
}

// --------------------------------------------------------------------------------------
// PARALLEL CONVOLUTION (OpenMP)
// --------------------------------------------------------------------------------------
void parallelConvolution(const Mat& input, Mat& output, const Kernel& kernel) {
    int rows = input.rows;
    int cols = input.cols;
    int kSize = kernel.size;
    int halfK = kSize / 2;
    int channels = input.channels();

    // OpenMP Parallelization
    // collapse(2): Flattens the nested (y, x) loops into a single linear iteration space
    // schedule(dynamic): Dynamically assigns chunks of iterations to threads.
    // This is useful for load balancing, especially if edge processing (boundary checks) takes slightly different times.
    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            for (int c = 0; c < channels; c++) {
                // 'sum' is a private variable for each thread, preventing False Sharing.
                float sum = 0.0f;

                for (int ky = -halfK; ky <= halfK; ky++) {
                    for (int kx = -halfK; kx <= halfK; kx++) {
                        int iy = y + ky;
                        int ix = x + kx;

                        // Zero-Padding Boundary Handling
                        if (iy >= 0 && iy < rows && ix >= 0 && ix < cols) {
                            float pixelVal = input.at<Vec3b>(iy, ix)[c];
                            float kernelVal = kernel.data[ky + halfK][kx + halfK];
                            sum += pixelVal * kernelVal;
                        }
                    }
                }
                
                sum = max(0.0f, min(255.0f, sum));
                output.at<Vec3b>(y, x)[c] = static_cast<uchar>(sum);
            }
        }
    }
}

// Fast Sobel Magnitude combining X and Y
void computeSobelMagnitude(const Mat& sobelX, const Mat& sobelY, Mat& output) {
    int rows = sobelX.rows;
    int cols = sobelX.cols;
    int channels = sobelX.channels();

    #pragma omp parallel for collapse(2) schedule(static)
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            for (int c = 0; c < channels; c++) {
                float valX = sobelX.at<Vec3b>(y, x)[c];
                float valY = sobelY.at<Vec3b>(y, x)[c];
                float mag = sqrt(valX*valX + valY*valY);
                output.at<Vec3b>(y, x)[c] = static_cast<uchar>(min(255.0f, mag));
            }
        }
    }
}


int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Usage: ./image_filter <image_path>" << endl;
        return -1;
    }

    string imagePath = argv[1];
    
    // Read the image using OpenCV. We use OpenCV EXCLUSIVELY for I/O.
    Mat image = imread(imagePath, IMREAD_COLOR);
    if (image.empty()) {
        cerr << "Error: Could not open or find the image." << endl;
        return -1;
    }

    cout << "Loaded image: " << image.cols << "x" << image.rows << " (" << image.channels() << " channels)\n" << endl;

    // Create an output matrix of the same size and type, initialized to zeros.
    Mat serialOutput = Mat::zeros(image.size(), image.type());
    Mat parallelOutput = Mat::zeros(image.size(), image.type());

    // Create kernels
    Kernel gaussianTask = createGaussianKernel(7, 1.5f); // 7x7 Gaussian

    cout << "--- Benchmarking Gaussian Blur ---" << endl;

    // 1. Serial Execution
    double start_serial = omp_get_wtime();
    serialConvolution(image, serialOutput, gaussianTask);
    double end_serial = omp_get_wtime();
    double time_serial = end_serial - start_serial;
    cout << "Serial Time: " << fixed << setprecision(4) << time_serial << " seconds." << endl;

    // 2. Parallel Execution
    int target_threads = omp_get_max_threads();
    double start_parallel = omp_get_wtime();
    parallelConvolution(image, parallelOutput, gaussianTask);
    double end_parallel = omp_get_wtime();
    double time_parallel = end_parallel - start_parallel;
    cout << "Parallel Time (" << target_threads << " threads): " << time_parallel << " seconds." << endl;

    // 3. Compute Metrics
    double speedup = time_serial / time_parallel;
    double efficiency = speedup / target_threads;
    
    cout << "Speedup: " << speedup << "x" << endl;
    cout << "Parallel Efficiency: " << efficiency * 100 << "%" << endl;

    // Write output images
    imwrite("output_gaussian_parallel.jpg", parallelOutput);

    // Demonstration of Sobel Edge Detection 
    // Creating intermediate matrices
    Mat sobelX_out = Mat::zeros(image.size(), image.type());
    Mat sobelY_out = Mat::zeros(image.size(), image.type());
    Mat sobel_mag  = Mat::zeros(image.size(), image.type());

    Kernel sobelX = createSobelKernelX();
    Kernel sobelY = createSobelKernelY();

    // Only run parallel Sobel to save execution time in benchmarking script
    parallelConvolution(image, sobelX_out, sobelX);
    parallelConvolution(image, sobelY_out, sobelY);
    computeSobelMagnitude(sobelX_out, sobelY_out, sobel_mag);
    imwrite("output_sobel_edges.jpg", sobel_mag);


    // Append metrics to CSV for the plotting script
    ofstream csvFile("benchmark_results.csv", ios_base::app);
    if (csvFile.is_open()) {
        csvFile << target_threads << "," << time_serial << "," << time_parallel << "," << speedup << "," << efficiency << "\n";
        csvFile.close();
    } else {
        cerr << "Failed to open benchmark_results.csv for writing." << endl;
    }

    return 0;
}

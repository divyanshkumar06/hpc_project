#include <cmath>
#include <fstream>
#include <immintrin.h>
#include <iomanip>
#include <iostream>
#include <omp.h>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

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
      k.data[i + half][j + half] =
          (exp(-(r * r) / (2 * sigma * sigma))) / (M_PI * 2 * sigma * sigma);
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
  k.data = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
  return k;
}

Kernel createSobelKernelY() {
  Kernel k;
  k.size = 3;
  k.data = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
  return k;
}

// Sharpen Kernel
Kernel createSharpenKernel() {
  Kernel k;
  k.size = 3;
  k.data = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
  return k;
}

// Image struct to hold pixel data in flat 1D array
struct Image {
  unsigned char *data;
  int width;
  int height;
  int channels;

  Image(int w, int h, int c) : width(w), height(h), channels(c) {
    data = new unsigned char[width * height * channels]();
  }
  ~Image() {
    if (data)
      delete[] data;
  }

  // Copy constructor
  Image(const Image &other)
      : width(other.width), height(other.height), channels(other.channels) {
    data = new unsigned char[width * height * channels];
    std::copy(other.data, other.data + (width * height * channels), data);
  }
};

// --------------------------------------------------------------------------------------
// SERIAL CONVOLUTION
// --------------------------------------------------------------------------------------
void serialConvolution(const Image &input, Image &output,
                       const Kernel &kernel) {
  int rows = input.height;
  int cols = input.width;
  int kSize = kernel.size;
  int halfK = kSize / 2;
  int channels = input.channels;

  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < cols; x++) {
      for (int c = 0; c < channels; c++) {
        if (c == 3) { // Preserve alpha channel
          output.data[(y * cols + x) * channels + c] =
              input.data[(y * cols + x) * channels + c];
          continue;
        }

        float sum = 0.0f;
        for (int ky = -halfK; ky <= halfK; ky++) {
          for (int kx = -halfK; kx <= halfK; kx++) {
            int iy = y + ky;
            int ix = x + kx;

            // Zero-Padding Boundary Handling
            if (iy >= 0 && iy < rows && ix >= 0 && ix < cols) {
              float pixelVal = input.data[(iy * cols + ix) * channels + c];
              float kernelVal = kernel.data[ky + halfK][kx + halfK];
              sum += pixelVal * kernelVal;
            }
          }
        }

        // Clamp values between 0 and 255
        sum = max(0.0f, min(255.0f, sum));
        output.data[(y * cols + x) * channels + c] =
            static_cast<unsigned char>(sum);
      }
    }
  }
}

// --------------------------------------------------------------------------------------
// PARALLEL CONVOLUTION (OpenMP)
// --------------------------------------------------------------------------------------
void parallelConvolution(const Image &input, Image &output,
                         const Kernel &kernel, int threads) {
  int rows = input.height;
  int cols = input.width;
  int kSize = kernel.size;
  int halfK = kSize / 2;
  int channels = input.channels;

  omp_set_num_threads(threads);

  // [Hinglish Comment for Presentation]
  // #pragma omp parallel for: Yeh batata hai ki agli loop ko alag-alag threads (cores) mein baant do.
  // collapse(2): Image 2D array hai (x aur y). Is command se dono loops (y and x) flatten ho kar 1D ban jate hain.
  // schedule(dynamic): Agar koi core jaldi free ho jaye, toh use agla pixel de do taaki koi CPU idle (khali) na baithay.
#pragma omp parallel for collapse(2) schedule(dynamic)
  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < cols; x++) {
      for (int c = 0; c < channels; c++) {
        if (c == 3) { // Preserve alpha channel
          output.data[(y * cols + x) * channels + c] =
              input.data[(y * cols + x) * channels + c];
          continue;
        }

        float sum = 0.0f;
        for (int ky = -halfK; ky <= halfK; ky++) {
          for (int kx = -halfK; kx <= halfK; kx++) {
            int iy = y + ky;
            int ix = x + kx;

            if (iy >= 0 && iy < rows && ix >= 0 && ix < cols) {
              float pixelVal = input.data[(iy * cols + ix) * channels + c];
              float kernelVal = kernel.data[ky + halfK][kx + halfK];
              sum += pixelVal * kernelVal;
            }
          }
        }

        sum = max(0.0f, min(255.0f, sum));
        output.data[(y * cols + x) * channels + c] =
            static_cast<unsigned char>(sum);
      }
    }
  }
}

// --------------------------------------------------------------------------------------
// CACHE-TILED PARALLEL CONVOLUTION (OpenMP)
// --------------------------------------------------------------------------------------
void tiledParallelConvolution(const Image &input, Image &output,
                              const Kernel &kernel, int threads,
                              int TILE_SIZE = 64) {
  int rows = input.height;
  int cols = input.width;
  int kSize = kernel.size;
  int halfK = kSize / 2;
  int channels = input.channels;

  omp_set_num_threads(threads);

  // [Hinglish Comment for Presentation]
  // Yahan 4 For-Loops hain. Pehle 2 loops image (wall) ko 64x64 ke 'Tiles' (blocks) mein todte hain.
  // Yeh 64x64 tile CPU ke 'L1 Cache' mein puri tarah fit ho jata hai.
  // Jiske karan CPU ko slow RAM memory par baar-baar jana nahi padta (We overcome Memory Bound limit!).
#pragma omp parallel for collapse(2) schedule(dynamic)
  for (int ty = 0; ty < rows; ty += TILE_SIZE) {
    for (int tx = 0; tx < cols; tx += TILE_SIZE) {

      // Andar ke 2 loops sirf usi chhote 64x64 tile ke andar convolution (math) apply karte hain.
      int y_end = min(ty + TILE_SIZE, rows);
      int x_end = min(tx + TILE_SIZE, cols);

      for (int y = ty; y < y_end; y++) {
        for (int x = tx; x < x_end; x++) {
          for (int c = 0; c < channels; c++) {
            if (c == 3) {
              output.data[(y * cols + x) * channels + c] =
                  input.data[(y * cols + x) * channels + c];
              continue;
            }

            float sum = 0.0f;
            for (int ky = -halfK; ky <= halfK; ky++) {
              for (int kx = -halfK; kx <= halfK; kx++) {
                int iy = y + ky;
                int ix = x + kx;

                if (iy >= 0 && iy < rows && ix >= 0 && ix < cols) {
                  float pixelVal = input.data[(iy * cols + ix) * channels + c];
                  float kernelVal = kernel.data[ky + halfK][kx + halfK];
                  sum += pixelVal * kernelVal;
                }
              }
            }

            sum = max(0.0f, min(255.0f, sum));
            output.data[(y * cols + x) * channels + c] =
                static_cast<unsigned char>(sum);
          }
        }
      }
    }
  }
}

// Point-wise filter: Inversion (Scalar)
void parallelInversion(const Image &input, Image &output, int threads) {
  int total_bytes = input.width * input.height * input.channels;
  omp_set_num_threads(threads);

#pragma omp parallel for schedule(static)
  for (int i = 0; i < total_bytes; i++) {
    // Skip alpha channel logic for simplicity assuming 3 channel RGB for our
    // tests
    output.data[i] = 255 - input.data[i];
  }
}

// Point-wise filter: Inversion (AVX2 SIMD explicitly loaded)
void avx2ParallelInversion(const Image &input, Image &output, int threads) {
  int total_bytes = input.width * input.height * input.channels;
  omp_set_num_threads(threads);

  // 32-byte blocks
  int blocks = total_bytes / 32;
  int remainder = total_bytes % 32;

  // [Hinglish Comment for Presentation]
  // Yeh Masterclass step hai 'Data-Level Parallelism' (SIMD) ka.
  // Normal loop 1 baar mein ek byte process karta hai.
  // Yahan hum explicitly CPU ko bol rahe hain ki 32 bytes ek sath load karo (_mm256_loadu_si256)
  // aur ek hi cycle mein 255 se sabko ek sath minus kar do (_mm256_sub_epi8). Time gets close to 0.000s!
#pragma omp parallel for schedule(static)
  for (int b = 0; b < blocks; b++) {
    int i = b * 32;
    __m256i vec_255 = _mm256_set1_epi8((char)255);
    __m256i pixels = _mm256_loadu_si256(
        (__m256i *)&input.data[i]);                 // 32 bytes ek sath CPU ke vector register me aate hain.
    __m256i inv = _mm256_sub_epi8(vec_255, pixels); // 255 - pixel ek sath saare 32 bytes ke liye hota hai!
    _mm256_storeu_si256((__m256i *)&output.data[i], inv); // Store 32 bytes wapas memory mein.
  }

  // Handle remainder (scalar for leftover bytes)
  if (remainder > 0) {
    int start = blocks * 32;
    for (int i = start; i < total_bytes; i++) {
      output.data[i] = 255 - input.data[i];
    }
  }
}
void parallelGrayscale(const Image &input, Image &output, int threads) {
  int total_pixels = input.width * input.height;
  int channels = input.channels;
  omp_set_num_threads(threads);

#pragma omp parallel for schedule(static)
  for (int i = 0; i < total_pixels; i++) {
    unsigned char r = input.data[i * channels + 0];
    unsigned char g = input.data[i * channels + 1];
    unsigned char b = input.data[i * channels + 2];

    // Luminosity method
    float gray = 0.299f * r + 0.587f * g + 0.114f * b;
    unsigned char gray_val =
        static_cast<unsigned char>(max(0.0f, min(255.0f, gray)));

    output.data[i * channels + 0] = gray_val;
    output.data[i * channels + 1] = gray_val;
    output.data[i * channels + 2] = gray_val;
    if (channels == 4)
      output.data[i * channels + 3] = input.data[i * channels + 3];
  }
}

// Point-wise filter: Sepia (Parallel)
void parallelSepia(const Image &input, Image &output, int threads) {
  int total_pixels = input.width * input.height;
  int channels = input.channels;
  omp_set_num_threads(threads);

#pragma omp parallel for schedule(static)
  for (int i = 0; i < total_pixels; i++) {
    unsigned char r = input.data[i * channels + 0];
    unsigned char g = input.data[i * channels + 1];
    unsigned char b = input.data[i * channels + 2];

    float tr = 0.393f * r + 0.769f * g + 0.189f * b;
    float tg = 0.349f * r + 0.686f * g + 0.168f * b;
    float tb = 0.272f * r + 0.534f * g + 0.131f * b;

    output.data[i * channels + 0] =
        static_cast<unsigned char>(max(0.0f, min(255.0f, tr)));
    output.data[i * channels + 1] =
        static_cast<unsigned char>(max(0.0f, min(255.0f, tg)));
    output.data[i * channels + 2] =
        static_cast<unsigned char>(max(0.0f, min(255.0f, tb)));
    if (channels == 4)
      output.data[i * channels + 3] = input.data[i * channels + 3];
  }
}

// Fast Sobel Magnitude combining X and Y
void computeSobelMagnitude(const Image &sobelX, const Image &sobelY,
                           Image &output, int threads) {
  int total_pixels = sobelX.width * sobelX.height;
  int channels = sobelX.channels;
  omp_set_num_threads(threads);

#pragma omp parallel for schedule(static)
  for (int i = 0; i < total_pixels; i++) {
    for (int c = 0; c < channels; c++) {
      if (c == 3) {
        output.data[i * channels + c] = sobelX.data[i * channels + c];
        continue;
      }
      float valX = sobelX.data[i * channels + c];
      float valY = sobelY.data[i * channels + c];
      float mag = sqrt(valX * valX + valY * valY);
      output.data[i * channels + c] =
          static_cast<unsigned char>(min(255.0f, mag));
    }
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    cout << "Usage: ./image_filter <image_path>" << endl;
    return -1;
  }

  string imagePath = argv[1];

  // Load image via STB
  int width, height, channels;
  unsigned char *img_data =
      stbi_load(imagePath.c_str(), &width, &height, &channels, 0);
  if (!img_data) {
    cerr << "Error: Could not open or find the image." << endl;
    return -1;
  }

  cout << "Loaded image: " << width << "x" << height << " (" << channels
       << " channels)\n"
       << endl;

  Image image(width, height, channels);
  std::copy(img_data, img_data + (width * height * channels), image.data);
  stbi_image_free(img_data); // Free the STB allocated memory

  // Open CSV for benchmarking
  ofstream csvFile("benchmark_results.csv");
  if (!csvFile.is_open()) {
    cerr << "Failed to open benchmark_results.csv for writing." << endl;
    return -1;
  }
  csvFile
      << "filter_name,threads,serial_time,parallel_time,speedup,efficiency\n";

  // Kernels
  Kernel gaussianK = createGaussianKernel(7, 1.5f);
  Kernel sobelX = createSobelKernelX();
  Kernel sobelY = createSobelKernelY();
  Kernel sharpenK = createSharpenKernel();

  vector<int> thread_counts = {1, 2, 4, 8, 16};

  auto runBenchmark = [&](const string &filterName, auto serialFunc,
                          auto parallelFunc, Image &final_out) {
    cout << "\n=== Benchmarking: " << filterName << " ===" << endl;
    Image temp_out(width, height, channels);

    // 1. Serial Execution
    double start_serial = omp_get_wtime();
    serialFunc(temp_out);
    double end_serial = omp_get_wtime();
    double time_serial = end_serial - start_serial;
    cout << "Serial Time: " << fixed << setprecision(4) << time_serial << " s"
         << endl;

    // 2. Parallel Executions
    for (int t : thread_counts) {
      double start_parallel = omp_get_wtime();
      parallelFunc(final_out, t);
      double end_parallel = omp_get_wtime();
      double time_parallel = end_parallel - start_parallel;

      double speedup = time_serial / time_parallel;
      double efficiency = speedup / t;

      cout << "Threads: " << setw(2) << t << " | Time: " << fixed
           << setprecision(4) << time_parallel << " s"
           << " | Speedup: " << fixed << setprecision(2) << speedup << "x"
           << " | Eff: " << fixed << setprecision(1) << (efficiency * 100)
           << "%" << endl;

      csvFile << filterName << "," << t << "," << time_serial << ","
              << time_parallel << "," << speedup << "," << efficiency << "\n";
    }
  };

  // Output folders or filenames
  Image out_gaussian(width, height, channels);
  Image out_gray(width, height, channels);
  Image out_sepia(width, height, channels);
  Image out_sharpen(width, height, channels);

  Image out_tiled(width, height, channels);
  Image out_inv(width, height, channels);

  // BENCHMARK 1: Gaussian Blur (Row-Major vs Tiled)
  runBenchmark(
      "Gaussian Blur (Scalar)",
      [&](Image &out) { serialConvolution(image, out, gaussianK); },
      [&](Image &out, int t) { parallelConvolution(image, out, gaussianK, t); },
      out_gaussian);
  stbi_write_jpg("output_gaussian.jpg", width, height, channels,
                 out_gaussian.data, 90);

  runBenchmark(
      "Gaussian Blur (Tiled 64x64)",
      [&](Image &out) {
        tiledParallelConvolution(image, out, gaussianK, 1, 64);
      },
      [&](Image &out, int t) {
        tiledParallelConvolution(image, out, gaussianK, t, 64);
      },
      out_tiled);
  // Tiled output is visually identical, no need to save again

  // BENCHMARK 2: Image Inversion (Scalar vs AVX2)
  runBenchmark(
      "Inversion (Scalar)",
      [&](Image &out) { parallelInversion(image, out, 1); },
      [&](Image &out, int t) { parallelInversion(image, out, t); }, out_inv);

  runBenchmark(
      "Inversion (AVX2 SIMD)",
      [&](Image &out) { avx2ParallelInversion(image, out, 1); },
      [&](Image &out, int t) { avx2ParallelInversion(image, out, t); },
      out_inv);
  stbi_write_jpg("output_inversion.jpg", width, height, channels, out_inv.data,
                 90);

  // BENCHMARK 3: Grayscale
  runBenchmark(
      "Grayscale",
      [&](Image &out) {
        parallelGrayscale(image, out, 1);
      }, // simulate serial with 1 thread
      [&](Image &out, int t) { parallelGrayscale(image, out, t); }, out_gray);
  stbi_write_jpg("output_grayscale.jpg", width, height, channels, out_gray.data,
                 90);

  // BENCHMARK 3: Sepia
  runBenchmark(
      "Sepia tone", [&](Image &out) { parallelSepia(image, out, 1); },
      [&](Image &out, int t) { parallelSepia(image, out, t); }, out_sepia);
  stbi_write_jpg("output_sepia.jpg", width, height, channels, out_sepia.data,
                 90);

  // BENCHMARK 4: Sharpen
  runBenchmark(
      "Sharpening",
      [&](Image &out) { serialConvolution(image, out, sharpenK); },
      [&](Image &out, int t) { parallelConvolution(image, out, sharpenK, t); },
      out_sharpen);
  stbi_write_jpg("output_sharpen.jpg", width, height, channels,
                 out_sharpen.data, 90);

  // BENCHMARK 5: Sobel Edge Detection (Pipeline)
  cout << "\n=== Benchmarking: Sobel Edges (Pipeline) ===" << endl;
  Image out_sobelX(width, height, channels);
  Image out_sobelY(width, height, channels);
  Image out_sobelMag(width, height, channels);

  // Serial
  double start_serial = omp_get_wtime();
  serialConvolution(out_gray, out_sobelX, sobelX);
  serialConvolution(out_gray, out_sobelY, sobelY);
  computeSobelMagnitude(out_sobelX, out_sobelY, out_sobelMag, 1);
  double end_serial = omp_get_wtime();
  double time_serial = end_serial - start_serial;
  cout << "Serial Time: " << fixed << setprecision(4) << time_serial << " s"
       << endl;

  // Parallel
  for (int t : thread_counts) {
    double start_parallel = omp_get_wtime();
    parallelConvolution(out_gray, out_sobelX, sobelX, t);
    parallelConvolution(out_gray, out_sobelY, sobelY, t);
    computeSobelMagnitude(out_sobelX, out_sobelY, out_sobelMag, t);
    double end_parallel = omp_get_wtime();
    double time_parallel = end_parallel - start_parallel;

    double speedup = time_serial / time_parallel;
    double efficiency = speedup / t;

    cout << "Threads: " << setw(2) << t << " | Time: " << fixed
         << setprecision(4) << time_parallel << " s"
         << " | Speedup: " << fixed << setprecision(2) << speedup << "x"
         << " | Eff: " << fixed << setprecision(1) << (efficiency * 100) << "%"
         << endl;

    csvFile << "Sobel Edges," << t << "," << time_serial << "," << time_parallel
            << "," << speedup << "," << efficiency << "\n";
  }
  stbi_write_jpg("output_sobel.jpg", width, height, channels, out_sobelMag.data,
                 90);

  csvFile.close();
  cout << "\nAll processing complete. Imagery and metrics saved." << endl;

  return 0;
}

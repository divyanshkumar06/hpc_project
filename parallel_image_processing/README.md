# Real-Time Parallel Image Processing Filter

## Project Objective
This project demonstrates high-performance computing techniques for image processing. It focuses on implementing robust 2D convolutions from scratch—specifically Gaussian Blur and Sobel Edge Detection—and accelerating them using shared-memory parallelism via **OpenMP**. The implementation is designed with a practical application context in **Intelligent Transport Systems (ITS)**, making it highly suitable for real-time traffic or highway monitoring where high-resolution edge detection acts as a pre-processing step for feature detection (e.g., license plates, vehicle tracking).

## Architecture & Implementation Details

### Data Decomposition & Cache Optimization
Image processing using 2D convolution is an inherently parallelizable task because the computation for each output pixel is independent of the others. 

1. **Row-Major Traversal:** In C++, 2D arrays (or OpenCV matrices) are stored continuously in row-major order. Our convolution loops strictly iterate over $Y$ (rows) first, then $X$ (columns). This guarantees maximum **Spatial Locality**, significantly reducing cache misses.
2. **Boundary Handling:** To guarantee that the output image dimensions match the input dimensions exactly, **Zero-Padding** boundary handling is utilized. Filter responses extending beyond the physical limits of the image bounds simulate transparent black pixels (values of 0).
3. **OpenMP Strategy:** 
   - `#pragma omp parallel for collapse(2)` is used to flatten the nested $Y$ and $X$ iteration loops into a single linear iteration space, greatly increasing the pool of distributable tasks.
   - `schedule(dynamic)` is used to allocate loop chunks on the fly. Since boundary conditions branch code logic (e.g., omitting bounds), different pixels take slightly different processing times. Dynamic scheduling ensures no threads sit idle due to load imbalance.
   - **False Sharing Prevention**: A private accumulator variable (`float sum`) is isolated per thread when looping through the kernel window. This avoids heavy atomic operations and invalidation of cache lines between threads.

## Expected Results (Amdahl's Law)

According to **Amdahl's Law**, the theoretical maximum speedup of an application is dictated by its serial portion. 

$$ S_{max} = \frac{1}{(1 - P) + \frac{P}{N}} $$

Where $P$ is the parallelizable portion. Since image I/O (reading from disk, allocating memory) is strictly serial in this implementation, our measured speedup will approach an asymptote and will not perfectly equal the number of active threads $N$. However, since the convolution algorithm is heavily computational $O(w \times h \times k^2)$, the parallel portion dominates the execution time for large inputs (like our high-resolution highway dataset). You should expect near-linear speedups up to 4 threads, with slightly diminishing returns at 8 threads due to thread creation overhead and the memory bandwidth limits of the shared architecture.

## Setup & Compilation Instructions

### Prerequisites (Linux/Ubuntu Environment)
- GNU C++ Compiler (`g++`)
- OpenMP (`libomp-dev`)
- OpenCV 4 (`libopencv-dev`)
- Python 3 with Matplotlib and Pandas (for benchmarking visibility)

```bash
sudo apt update
sudo apt install build-essential libomp-dev libopencv-dev pkg-config
sudo apt install python3 python3-pip
sudo apt install python3-matplotlib python3-pandas
```

### Quick Start
To build the application, download a sample ITS highway photo, execute benchmarks across multiple threads, and generate a performance graph, simply run the bash automation script:

```bash
chmod +x run.sh
./run.sh
```

### Outputs Generated
- **`output_gaussian_parallel.jpg`**: Result after applying the 7x7 Gaussian blur kernel.
- **`output_sobel_edges.jpg`**: Sobel edge magnitude response (combining Gx and Gy).
- **`benchmark_results.csv`**: Raw performance data (Execution Time, Speedup, Efficiency).
- **`speedup_graph.png`**: Visual analytic plot evaluating OpenMP parallel behavior.

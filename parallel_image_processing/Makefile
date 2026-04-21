CXX = g++
CXXFLAGS = -O3 -fopenmp -mavx2 -mfma -Wall -Wextra -std=c++17
TARGET = image_filter
SRCS = main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET) output_*.jpg benchmark_results.csv dashboard/data.json *.png test_traffic.jpg *.exe

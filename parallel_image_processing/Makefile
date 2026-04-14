CXX = g++
CXXFLAGS = -O3 -fopenmp -Wall -Wextra -std=c++17
INCLUDES = `pkg-config --cflags opencv4`
LIBS = `pkg-config --libs opencv4`
TARGET = image_filter
SRCS = main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SRCS) $(LIBS)

clean:
	rm -f $(TARGET) output_*.jpg benchmark_results.csv *.png

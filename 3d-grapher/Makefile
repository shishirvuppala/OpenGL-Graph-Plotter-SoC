CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -I.
LDFLAGS = -lGL -lglfw -lGLEW

# The final executable name
TARGET = 3d-grapher

# All the .cpp files from your directory
SRCS = main.cpp cubelet.cpp graph.cpp gl_framework.cpp shader_util.cpp
OBJS = $(SRCS:.cpp=.o)

# Default rule
all: $(TARGET)

# Link the object files into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile each .cpp file into a .o object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)
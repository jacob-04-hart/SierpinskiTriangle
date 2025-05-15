CXX = g++
CXXFLAGS = -std=c++17
CFLAGS =
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lglfw -lGL -lm -lX11 -lpthread -lXrandr -lXi -ldl

SRC = src/main.cpp src/glad.c
OBJ = $(filter %.o, $(SRC:.cpp=.o) $(SRC:.c=.o))
TARGET = SierpinskiTriangle

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LDFLAGS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/%.o: src/%.c
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

debug:
	@echo "OBJ: $(OBJ)"
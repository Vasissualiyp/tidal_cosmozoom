CXX    = g++
CFLAGS = 
LFLAGS = 

TARGET = main
OBJS   = build/main.o

builddir:
	mkdir build

build/%.o: builddir %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LFLAGS)

all: $(OBJS) $(TARGET)

clean:
	rm -rf $(OBJS)
	rm -rf $(TARGET)

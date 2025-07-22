CXX    = g++
CFLAGS = 
LFLAGS = 

TARGET = main
OBJS   = build/main.o

build/%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LFLAGS)

all: $(OBJS) $(TARGET)

clean:
	rm -rf $(OBJS)
	rm -rf $(TARGET)

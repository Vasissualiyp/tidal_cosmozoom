CXX    = g++
CFLAGS = 
LFLAGS = 
BUILDDIR = build

TARGET = main
OBJS   = build/main.o \
         build/params.o

builddir:
	mkdir -p build

$(BUILDDIR)/%.o: %.cpp | build
	$(CXX) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LFLAGS)

all: $(OBJS) $(TARGET)

clean:
	rm -rf $(OBJS)
	rm -rf $(TARGET)

.PHONY: all clean

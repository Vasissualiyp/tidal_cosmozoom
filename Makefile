CXX    = g++
CFLAGS = -Wall -Wextra
LFLAGS = 
BUILDDIR = build
SRCDIR = src

TARGET = main
OBJS   = $(BUILDDIR)/main.o \
         $(BUILDDIR)/params.o

all: $(TARGET)

builddir:
	mkdir -p build

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | build
	$(CXX) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LFLAGS)


clean:
	rm -rf $(OBJS)
	rm -rf $(TARGET)

.PHONY: all clean

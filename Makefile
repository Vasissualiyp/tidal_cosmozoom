CXX    = g++
CFLAGS = -Wall -Wextra -I$(FFTW_PATH)/include
LFLAGS = -L$(FFTW_PATH)/lib
BUILDDIR = build
SRCDIR   = src

TARGET = main
OBJS   = $(BUILDDIR)/main.o \
         $(BUILDDIR)/params.o

all: $(TARGET)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LFLAGS)

clean:
	rm -rf $(OBJS)
	rm -rf $(TARGET)

.PHONY: all clean

# =============================================================================
#  SPARTAN-16  –  Build System
# =============================================================================
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
TARGET   := cpu
SRCDIR   := src

# Collect every .cpp in src/
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(SRCS:.cpp=.o)

# ── Default target ─────────────────────────────────────────────────────────────
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "Build complete → $(TARGET)"

# ── Compile each .cpp to an object file ────────────────────────────────────────
$(SRCDIR)/%.o: $(SRCDIR)/%.cpp $(wildcard $(SRCDIR)/*.h)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# ── Convenience targets ────────────────────────────────────────────────────────
hello: all
	./$(TARGET) exec programs/hello.asm

fibonacci: all
	./$(TARGET) exec programs/fibonacci.asm

timer: all
	./$(TARGET) exec programs/timer.asm

trace-fibonacci: all
	./$(TARGET) exec programs/fibonacci.asm --trace

dump-hello: all
	./$(TARGET) exec programs/hello.asm --dump

# ── Clean ──────────────────────────────────────────────────────────────────────
clean:
	rm -f $(SRCDIR)/*.o $(TARGET) programs/*.bin

.PHONY: all clean hello fibonacci timer trace-fibonacci dump-hello

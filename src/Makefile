MPICXX ?= mpicxx
MPIEXEC ?= mpiexec

CXXFLAGS = -Wall -Wextra -O2 -std=c++17

TARGET = polynomial_pipeline
SRCDIR = src
SOURCES = $(SRCDIR)/polynomial_pipeline.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(MPICXX) $(CXXFLAGS) $(OBJECTS) -o $@
	@echo ""
	@echo "Build successfull: $(TARGET)"
	@echo "Student: Khasanova Uma, No 61"
	@echo "Pattern: Linear Pipeline (#6)"
	@echo "Data Type: float"
	@echo "MPI Compiler: $(MPICXX)"

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "Compiling $<..."
	$(MPICXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Cleaning build artifacts..."
	rm -f $(TARGET) $(OBJECTS)
	@echo "Clean complete."

# Run with custom parameters
run-custom: $(TARGET)
	@echo ""
	@echo "Running with custom parameters"
	@echo "Format: processes x coefficients..."
	@echo "Example: make run-custom processes=4 x=2.0 coeffs='1 2 3 4'"
	$(MPIEXEC) -n $(processes) ./$(TARGET) $(x) $(coeffs)

# Single example
run-example: $(TARGET)
	@echo ""
	@echo "Running: P(2.0) for 4x^3 + 3x^2 + 2x + 1"
	@echo "Using 4 processes"
	$(MPIEXEC) -n 4 ./$(TARGET) 2.0 1 2 3 4

info:
	@echo "Build Information:"
	@echo "  Student: Khasanova Uma, No 61"
	@echo "  Task: Compute polynomial using Pipeline pattern"
	@echo "  Pattern: Linear Pipeline (#6)"
	@echo "  Data Type: float"
	@echo "  MPI Compiler: $(MPICXX)"
	@echo "  MPI Runtime: $(MPIEXEC)"
	@echo "  Target: $(TARGET)"

help:
	@echo ""
	@echo "MPI Polynomial Pipeline - Makefile Help"
	@echo "========================================"
	@echo ""
	@echo "Available targets:"
	@echo "  all        - Build the program (default)"
	@echo "  clean      - Remove build artifacts"
	@echo "  run-custom - Run with custom parameters"
	@echo "  run-example- Run example with 4 processes"
	@echo "  info       - Display build information"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Custom run example:"
	@echo "  make run-custom processes=4 x=2.0 coeffs='1 2 3 4'"

.PHONY: all clean run-custom run-example info help
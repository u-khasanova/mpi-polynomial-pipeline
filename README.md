# MPI Polynomial Pipeline Project

## Project Overview

This project implements a **linear pipeline pattern** using MPI for parallel computation of polynomial values. The application distributes polynomial evaluation across multiple processes in a pipeline fashion, where each process computes a portion of the terms and passes the accumulated sum to the next process.

## Student Information

- **Student**: Khasanova Uma, No. 61
- **Pattern**: Linear Pipeline (#6)
- **Data Type**: float
- **Technology**: MPI (Message Passing Interface)

## Prerequisites

### Required Software
- MPI implementation (MPICH, OpenMPI, or Microsoft MPI)
- C++ compiler with C++17 support
- Make build system

### Environment Variables Setup

**You must set the following environment variables before building and running:**

#### For Linux/macOS:
```bash
export MPICXX=/path/to/your/mpicxx
export MPIEXEC=/path/to/your/mpiexec
```

#### For Windows (Command Prompt):
```cmd
set MPICXX=C:\Path\To\mpicxx.exe
set MPIEXEC=C:\Path\To\mpiexec.exe
```

## Why Linear Pipeline Pattern is Suitable for This Task

### Natural Data Flow Dependencies

The linear pipeline pattern is perfectly suited for polynomial evaluation due to the **inherently sequential nature** of accumulation operations. Polynomial computation requires summing terms in a specific order, creating natural data dependencies that align perfectly with the pipeline paradigm:

```
P(x) = a₀ + a₁·x + a₂·x² + ... + aₙ·xⁿ
```

## Project Functionality

### Core Features

**Polynomial Representation**
   - Stores coefficients as `[a0, a1, a2, ...]` for polynomial: a0 + a1*x + a2*x² + ...
   - Supports polynomials of any degree
   - Provides string representation for display

**Parallel Evaluation**
   - Distributes polynomial terms across multiple MPI processes
   - Each process computes its assigned terms
   - Accumulates results in a pipeline fashion

### Mathematical Basis

The project evaluates polynomials using the standard form:
```
P(x) = a0 + a1*x + a2*x² + a3*x³ + ... + an*xⁿ
```

Each process in the pipeline computes:
```
partial_sum = Σ(ai * xⁱ) for assigned terms i
```

## Build Instructions

### Available Make Targets

```bash
# Build the project
make all

# Clean build artifacts
make clean

# Run with custom parameters
make run-custom processes=4 x=2.0 coeffs='1 2 3 4'

# Run predefined example
make run-example

# Display build information
make info

# Show help message
make help
```
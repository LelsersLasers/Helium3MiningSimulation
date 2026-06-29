# Helium 3 Mining Simulation

Vast Take Home Coding Exercise - Flight SWE

## Overview

A discrete-event simulation of a 72-hour lunar Helium-3 mining operation.

The simulations runs faster than real-time by simulating only state transistion events rather than advancing a clock at fixed ticks.

## Build/Run

Requires CMake 3.14 or higher and a C++17-capable compiler

### Build

```bash
cmake -S . -B build
cmake --build build
```

This creates two executables under `build/`:
- `helium_3_mining_sim` - the simulation itself
- `helium_3_mining_sim_tests` - runs the unit tests

### Run Simulation

```bash
./build/helium_3_mining_sim [options]

Options:
  -n <trucks>     Number of mining trucks   (default: 10)
  -m <stations>   Number of unload stations (default:  2)
  -s <seed>       RNG seed                  (default: random)
  -h              Show help
```

### Run Tests

```bash
./build/helium_3_mining_sim_tests
```

## Parameters

### Hard-coded parameters

| Parameter | Value |
|-----------|-------|
| Simulation duration | 72 hours |
| Mining duration per trip | Uniform random: 1-5 hours |
| Travel time between site and station) | 30 minutes |
| Unload time per truck | 5 minutes |
| Station capacity | 1 truck at a time. Unlimited trucks allowed to wait in queue |
| Queue assignment | Lowest expected wait time (ties broken by station id) |

### Command-line parameters

| Parameter | Flag | Default |
|-----------|------|---------|
| Number of trucks | -n | 10 |
| Number of stations | -m | 2 |
| Random seed | -s | random via `std::random_device` |


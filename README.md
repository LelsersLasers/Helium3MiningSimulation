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

## Design

### Event-driven simulation

```mermaid
flowchart TD
    subgraph Simulator
        EventQueue["Priority Queue<br/>(Future Events)"]
        Scheduler["Event Dispatcher"]
        EventQueue --> Scheduler
    end

    Truck["MiningTruck"]
    Station["UnloadStation"]

    Scheduler --> Truck
    Scheduler --> Station

    Truck -->|Schedules next event| EventQueue
    Station -->|Schedules truck start| EventQueue

    Truck <-->|"join_queue() <br/> assigned_station()"| Station
```

### Mining Truck States

```mermaid
stateDiagram
    [*] --> Mining : t = 0

    Mining --> TravelingToStation : mining complete
    TravelingToStation --> WaitingInQueue : arrival (t + 30 min)
    WaitingInQueue --> Unloading : station free (t + wait)
    Unloading --> TravelingToSite : done (t + 5 min)
    TravelingToSite --> Mining : arrival (t + 30 min)
```

### Code structure

| Class/File | Responsibility |
|-------|----------------|
| `sim_config.h` | Constants, truct states definition, `SimConfig` |
| `MiningTruck` | State machine; tracks time per state |
| `UnloadStation` | FIFO queue; manages busy and idle transitions |
| `Simulator` | Event loop; assignment logic; owns all entities |
| `stats_reporter` | Formats and prints statistics |

### Test coverage

| Suite | What is tested |
|-------|----------------|
| `MiningTruck` | All state transitions; state durations accumulate correctly (mining, travel, queue, unloading) |
| `UnloadStation` | Idle/busy transitions; projected wait time; FIFO queue ordering; queue advancement after unloading; utilization and busy/idle statistics |
| `Simulator` | Determinism (same seed = same total trips); time accounting (per-truck state times sum to simulation duration); every truck completes at least one trip; single-truck and single-station edge case (no queueing); total truck trips == total station services |


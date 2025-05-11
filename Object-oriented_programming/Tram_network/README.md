# Tram Network Simulation

A Java project simulating tram traffic and passenger movement based on user-provided input.

## Overview

This object-oriented program models tram operations across multiple days, handling:
- Dynamic scheduling of trams
- Passenger assignments and travel
- Real-time event processing (boarding, arrivals)
- Statistics generation (travel count, wait times)

Time is modeled in **minutes from 6:00 AM**, and all simulation data (stations, routes, trams, passengers) is read from standard input.

## Features

- Event-driven simulation using a custom event queue
- Modular class structure (`Symulacja`, `Pasażer`, `Linia`, `Tramwaj`, `Zdarzenie`, etc.)
- Tracking of per-day and overall statistics
- Full simulation loop: from parsing input to reporting results

## Example

Run the simulation by compiling and executing `Main.java`:

```bash
javac Main.java
java Main < input.txt

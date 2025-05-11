# Stock Exchange Simulator

An object-oriented simulation of a stock exchange system with multiple investors and companies.

## Overview

This project simulates a simplified financial market, where:
- Investors make buy/sell decisions using different strategies.
- Stock orders are processed over discrete time turns.
- The system enforces transaction logic and market invariants.

Two investor types are supported:
- **RandomInvestor** - places random buy/sell orders.
- **SMAInvestor** - makes decisions based on Simple Moving Average (SMA5 and SMA10).

## Features

- Priority-based transaction system (price + submission order)
- Buy/sell order queues per stock
- Matching algorithm for executing transactions
- Portfolio and cash balance tracking per investor
- Market invariants and consistency checks

## How to Run

Compile and run with a valid input file and number of turns:

```bash
javac Main/Stock_market.java
java Main.Stock_market input.txt 20
```
## Input Format
Input file includes:

- Investor types per line (e.g., `RRSS`)
- List of companies and shares (e.g., `ABC:100 XYZ:200`)
- Initial cash and portfolio per investor (e.g., `1000 ABC:10 XYZ:5`)

## Output
At the end of the simulation, each investor's final portfolio and cash balance is printed.


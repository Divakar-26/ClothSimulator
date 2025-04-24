# 🧵 Cloth Simulator (Verlet Integration) - C++ / SDL / ImGui

A lightweight and interactive cloth simulation built using C++ with SDL2 and ImGui, powered by Verlet integration for realistic physics. Inspired by soft body physics, this project simulates a dynamic fabric that reacts to gravity, wind, and user interaction.

## Features

- 🔗 Verlet-based point mass system
- 🧶 Realistic cloth behavior (tearing, pinning, swinging)
- 🖱️ Mouse interaction: tear, and pin nodes
  - MIDDLE MOUSE BUTTON -> pin
  - LEFT -> place a node
  - RIGHT -> to connect two nodes and remove connection
- 🖥️ UI controls via ImGui for:
  - Gravity toggle
  - Iteration count
  - Reset / pause simulation

## Tech Stack

- **Language**: C++
- **Graphics**: SDL3
- **UI**: ImGui
- **Physics**: Verlet Integration

## Preview



```bash
git clone https://github.com/Divakar-26/ClothSimulator
cd ClothSimulator
make 
./ClothSimulator

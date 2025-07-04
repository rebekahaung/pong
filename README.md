# Pong SDL2 Game


This is a custom implementation of the classic Pong game built with C++ and SDL2/SDL_ttf.

## Features

- **Welcome Screen**: Displays “Welcome to Pong” and prompts users to press the spacebar to begin.
- **Mode Selection**:  
  - `1`: Two-Player  
  - `2`: Player vs. AI (Easy)  
  - `3`: Player vs. AI (Hard)
- **Game UI**:
  - Score counter
  - “You” label above player paddle in AI mode
  - Red boxed text for countdown and prompts
  - Restart button (clickable with mouse) that also allows mode re-selection
- **Pause Option**: Press `P` to pause/unpause.
- **Countdown**: Triggered by pressing spacebar. Disables paddle movement until it ends.

## Requirements

- SDL2  
- SDL2_ttf  
- C++ compiler supporting C++11

## Setup (macOS - Homebrew Path)

## Install dependencies:

brew install sdl2 sdl2_ttf

## Compile the game:

clang++ main.cpp -o pong -std=c++11 \
  -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE \
  -L/opt/homebrew/lib -lSDL2 -lSDL2_ttf

## Launch the game:

./pong

## Controls:

W / S: Move Player 1 paddle

↑ / ↓: Move Player 2 or AI paddle (if 2-player)

Space: Start game / trigger countdown

P: Pause/Unpause

Mouse Click: Click "Restart" to reset game and reselect mode

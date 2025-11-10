# 🎮 Tic-Tac-Toe Game in C
A two player game of X and Os.
#### Play smart, think fast — Tic-Tac-Toe in C with color, sound, and fun!

### 📖 Overview
This is an **enhanced console-based Tic-Tac-Toe game in C**, featuring:
- **Colorful interface (ANSI colors)**
- **Sound effects** for moves, wins, and draws
- **Two game modes:** Player vs Player & Player vs Computer
- **Smart terminal display** with centered text and clear formatting

The game demonstrates the use of:
- Loops, arrays, and conditional logic
- Functions and modular programming
- Platform-specific features (Windows & Linux)
- Basic logic for computer moves

---

### ⚙️ Features
✅ **Two Game Modes**
- Player vs Player  
- Player vs Computer (with random AI moves)

✅ **Cross-Platform Support**
- Works on both **Windows** and **Linux**  
- Uses ANSI color codes and sound via `Beep()` / `printf("\a")`

✅ **User-Friendly Console**
- Centered layout and numbered grid (1–9)  
- Input validation and quit option (`Q`)  
- Clear screen updates for each move  

✅ **Interactive Experience**
- **Sound effects** on moves, wins, and draws  
- **Dynamic messages** and colorful output  

---

### 🧩 Game Rules
1. The board is a 3×3 grid numbered **1 to 9**.  
2. Player **X** always starts first.  
3. Players choose a number to mark that cell.  
4. The first player with three in a row (horizontally, vertically, or diagonally) wins.  
5. Enter **Q** to quit anytime.  
6. If all cells are filled with no winner → **Draw**.

---

### 🧠 Code Structure

| Function | Purpose |
|-----------|----------|
| `initializeBoard()` | Sets up the numbered grid (1–9) |
| `printBoard()` | Displays the board with colors |
| `makeMove()` | Places the player's move on the grid |
| `checkWin()` | Checks for any winning condition |
| `isDraw()` | Detects draw |
| `computerMove()` | Random move for AI mode |
| `playInputSound()`, `playWinSound()` | Adds sound feedback |
| `centerText()` | Centers text dynamically based on terminal width |
| `enableANSI()` | Enables color support on Windows |
| `main()` | Controls the entire game flow |

---

### 💻 How to Run

#### On Windows:
```bash
gcc tic_tac_toe.c -o tic_tac_toe -lwinmm
tic_tac_toe.exe
```
#### On Linux:
```
gcc tic_tac_toe.c -o tic_tac_toe
./tic_tac_toe
```
# Project Members:
```
 1. Aditi Gupta
 2. Asmita Marandi
 3. Dhawal Gilke
 4. Durgesh Chouhan
 5. Ira Bhardwaj
 6. Poulami Maji
 7. Prachi Gupta
 8. Pradeep Patidar
 9. Rahul Sondhiya
 10. Sahil Tamang
 11. Shriram Patel
 12. Shubham Patidar

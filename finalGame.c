/**************************************************************************************
 *                          TIC TAC TOE GAME (C Version)
 *  -----------------------------------------------------------------------------------
 *  Description:
 *      A colorful terminal-based Tic Tac Toe game for Windows, macOS, and Linux.
 *      Supports Player vs Player and Player vs Computer modes with sound feedback.
 *
 *  Features:
 *      - Cross-platform ANSI color support
 *      - Centered text output based on terminal width
 *      - Sound feedback for input, win, and draw events
 *      - Input validation and replay option
 *      - NEW: Quit anytime during the game by pressing 'Q'
 *
 *  Quit-Anywhere Overview:
 *      - Typing 'Q' (or 'q') at any input prompt cleanly exits to THE_END label.
 *      - Detection is done by is_quit() -> prompt_int_or_quit() helpers.
 *      - Call sites check for a "quit" status (return 0) and jump to THE_END.
 **************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#else
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#endif

/**************************************************************************************
 *                                   COLOR CODES
 **************************************************************************************/
#define CLEAR "\033[2J"
#define HOME  "\033[H"
#define RESET "\033[0m"
#define RED   "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE  "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN  "\033[36m"

/**************************************************************************************
 *                               GLOBAL VARIABLES
 **************************************************************************************/
char board[3][3];          // Game board (3x3)
char currentPlayer = 'X';  // Current player ('X' or 'O')
int mode = 1;              // Game mode: 1 = PvP, 2 = PvC

/**************************************************************************************
 *                         ENABLE ANSI COLORS ON WINDOWS
 **************************************************************************************/
void enableANSI()
{
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode))
    {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
#endif
}

/**************************************************************************************
 *                                   INPUT HELPERS
 *
 *  These helpers implement the "Quit anytime with 'Q'" feature:
 *    - read_line(): safe line input
 *    - is_quit(): detects if the user entered 'Q' or 'q'
 *    - parse_int(): parses numeric input
 *    - prompt_int_or_quit(): unified prompt that returns:
 *          1  = success (valid int in range)
 *          0  = user requested quit (typed 'Q' / 'q')
 *         -1  = invalid input (not int or out of range)
 **************************************************************************************/
void flushInput() { int c; while ((c = getchar()) != '\n' && c != EOF) ; }

static void trim_newline(char *s) {
    size_t n = strlen(s);
    if (n && s[n-1] == '\n') s[n-1] = '\0';
}

static bool read_line(char *buf, size_t sz) {
    if (!fgets(buf, (int)sz, stdin)) return false;
    trim_newline(buf);
    return true;
}

static bool is_quit(const char *s) {
    // Treat leading whitespace as ignorable; first non-space 'q' or 'Q' triggers quit
    while (*s==' '||*s=='\t') s++;
    return (*s=='q'||*s=='Q');
}

static bool parse_int(const char *s, int *out) {
    char *end=NULL; long v=strtol(s,&end,10);
    if (s==end) return false;               // no digits found
    while (*end==' '||*end=='\t') end++;
    if (*end!='\0') return false;           // trailing junk => invalid
    *out=(int)v; return true;
}

/* Prompt for an int in [min,max]; allow 'Q' to quit.
   Returns: 1 success, 0 user quit, -1 invalid input */
static int prompt_int_or_quit(const char *prompt, int *out, int min, int max) {
    char buf[64];
    printf("%s", prompt);
    if (!read_line(buf, sizeof(buf))) return -1;
    if (is_quit(buf)) return 0;             // <<< Quit-anywhere detection here
    int val;
    if (!parse_int(buf, &val)) return -1;
    if (val < min || val > max) return -1;
    *out = val;
    return 1;
}

/**************************************************************************************
 *                                   SOUND EFFECTS
 **************************************************************************************/
void playInputSound()
{
#ifdef _WIN32
    Beep(750, 120);
#else
    printf("\a"); fflush(stdout);
#endif
}

void playWinSound()
{
#ifdef _WIN32
    Beep(1000, 150); Beep(1200, 150); Beep(1500, 200);
#elif APPLE
    system("afplay /System/Library/Sounds/Tink.aiff 2>/dev/null &");
#else
    system("play -q -n synth 0.3 tri 800 fade 0 0.1 0.2 2>/dev/null &");
#endif
}

void playDrawSound()
{
#ifdef _WIN32
    Beep(700, 200); Beep(700, 200);
#elif APPLE
    system("afplay /System/Library/Sounds/Pop.aiff 2>/dev/null &");
#else
    printf("\a"); fflush(stdout);
#endif
}

/**************************************************************************************
 *                             TERMINAL DISPLAY HELPERS
 **************************************************************************************/
int getTerminalWidth()
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int width = 80;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleScreenBufferInfo(hOut, &csbi))
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return width;
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
        return w.ws_col > 0 ? w.ws_col : 80;
    return 80;
#endif
}

void centerText(const char *text)
{
    int width = getTerminalWidth();
    int len = (int)strlen(text);
    int spaces = (width - len) / 2;
    if (spaces < 0) spaces = 0;
    for (int i = 0; i < spaces; i++) printf(" ");
    printf("%s\n", text);
}

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    printf(CLEAR HOME);
#endif
}

/**************************************************************************************
 *                                 GAME LOGIC
 **************************************************************************************/
void initializeBoard()
{
    int n = 1;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            board[i][j] = '0' + n++;
}

void printBoard()
{
    clearScreen();
    printf("\n\n\n\n");

    centerText(CYAN "============================================" RESET);
    centerText(CYAN "            TIC TAC TOE GAME                " RESET);
    centerText(CYAN "============================================" RESET);
    printf("\n");
    centerText(YELLOW "(Enter 1-9 for move, or 'Q' to quit)" RESET); // <<< UI hint for quit
    printf("\n");

    int width = getTerminalWidth();
    int indent = (width - 25) / 2;
    if (indent < 0) indent = 0;

    for (int i = 0; i < 3; i++)
    {
        for (int s = 0; s < indent; s++) printf(" ");

        for (int j = 0; j < 3; j++)
        {
            char mark = board[i][j];
            if (mark == 'X') printf(YELLOW " %c " RESET, mark);
            else if (mark == 'O') printf(RED " %c " RESET, mark);
            else printf(" %c ", mark);

            if (j < 2) printf(CYAN "|" RESET);
        }
        printf("\n");
        if (i < 2)
        {
            for (int s = 0; s < indent; s++) printf(" ");
            printf(CYAN "---+---+---" RESET "\n");
        }
    }
    printf("\n");
}

bool checkWin()
{
    for (int i = 0; i < 3; i++)
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return true;

    for (int i = 0; i < 3; i++)
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i])
            return true;

    if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) return true;
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0]) return true;

    return false;
}

bool isDraw()
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] != 'X' && board[i][j] != 'O')
                return false;
    return true;
}

bool makeMove(int block)
{
    if (block < 1 || block > 9) return false;

    int row = (block - 1) / 3;
    int col  = (block - 1) % 3;

    if (board[row][col] == 'X' || board[row][col] == 'O') return false;

    board[row][col] = currentPlayer;
    return true;
}

void computerMove()
{
    int block;
    do { block = rand() % 9 + 1; } while (!makeMove(block));

    centerText(MAGENTA "Computer chose a move." RESET);

#ifdef _WIN32
    Beep(650, 150);
    Sleep(800);
#else
    printf("\a"); fflush(stdout);
    sleep(1);
#endif
}

/**************************************************************************************
 *                                     MAIN FUNCTION
 *
 *  Quit-anytime flow notes:
 *    - Game mode prompt: if user types 'Q', we set playAgain=0 and goto THE_END.
 *    - During turns: prompt_int_or_quit() returns 0 on 'Q'; we show a message and exit.
 *    - Replay prompt: 'Q' also exits immediately (treated same as "No").
 **************************************************************************************/
int main()
{
    enableANSI();
    srand((unsigned)time(NULL));

    // Welcome message
    clearScreen();
    printf("\n\n\n\n");
    centerText(CYAN "=============================================================" RESET);
    centerText(CYAN "                  WELCOME TO TIC TAC TOE GAME!               " RESET);
    centerText(CYAN "=============================================================" RESET);
    printf("\n");
    centerText(YELLOW "~ Press ENTER to Start the Game ~" RESET);
    getchar();

    int playAgain = 1;

    while (playAgain)
    {
        // Game mode selection
        clearScreen();
        printf("\n\n\n");
        centerText(MAGENTA "       ***************** Select Game Mode ***************\n" RESET);
        centerText(CYAN "     1. Player vs Player\n" RESET);
        centerText(CYAN "     2. Player vs Computer" RESET);

        while (1) {
            int ok = prompt_int_or_quit(
                YELLOW "\nEnter your choice (1 or 2) or 'Q' to quit: " RESET,
                &mode, 1, 2
            );
            if (ok == 1) break;
            if (ok == 0) { playAgain = 0; goto THE_END; }  // <<< Quit-anytime from mode menu
            centerText(RED "Invalid input! Please enter 1, 2, or Q." RESET);
#ifdef _WIN32
            Beep(500, 150);
#else
            printf("\a"); fflush(stdout);
#endif
        }

        initializeBoard();
        currentPlayer = 'X';
        bool gameOver = false;

        // Main game loop
        while (!gameOver)
        {
            printBoard();

            if (mode == 2 && currentPlayer == 'O')
            {
                computerMove();
            }
            else
            {
                int block;
                int status = prompt_int_or_quit(
                    CYAN "Player X, enter block (1-9) or 'Q' to quit: " RESET,
                    &block, 1, 9
                );

                if (status == 0) { // quit mid-game (player typed Q/q)
                    centerText(RED "You quit the game. Goodbye!" RESET);
                    playAgain = 0;
                    goto THE_END;   // <<< Centralized graceful exit
                } else if (status == -1) {
                    centerText(RED "Invalid input! Press ENTER to continue..." RESET);
                    getchar();
                    continue;
                }

                playInputSound();

                if (!makeMove(block))
                {
                    centerText(RED "Invalid move! Press ENTER to continue..." RESET);
                    getchar();
                    continue;
                }
            }

            // Check for win or draw
            if (checkWin())
            {
                playWinSound();
                printBoard();
                printf("\n");

                if (mode == 2 && currentPlayer == 'O')
                    centerText(RED "**************** COMPUTER WINS! ****************" RESET);
                else {
                    char msg[96];
                    sprintf(msg, GREEN "************ PLAYER %c WINS! ************" RESET, currentPlayer);
                    centerText(msg);
                }

                gameOver = true;
            }
            else if (isDraw())
            {
                playDrawSound();
                printBoard();
                centerText(GREEN "**************** It's a Draw! ****************" RESET);
                gameOver = true;
            }
            else
            {
                currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
            }
        }

        // Ask if player wants to play again
        while (1) {
            int againVal;
            int ok = prompt_int_or_quit(
                RED "\nPlay again? (1 = Yes / 0 = No) : " RESET,
                &againVal, 0, 1
            );
            if (ok == 0) { playAgain = 0; break; }   // <<< 'Q' here exits like "No"
            if (ok == 1) { playAgain = againVal; break; }
            centerText(RED "Invalid input! Enter 1 or 0." RESET);
        }
    }

THE_END:
    // Exit message (keep all '\n' as requested)
    clearScreen();
    printf("\n\n\n\n\n\n\n\n\n");
    centerText(RED "   ----------------------------------- THE END -----------------------------------------\n\n\n" RESET);
    centerText(GREEN "          ------------------ Thanks for playing Tic Tac Toe! ------------------" RESET);
    printf("\n\n\n");
    return 0;
}
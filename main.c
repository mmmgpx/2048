
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

void noicanon(int i);
int init(int *yader,int *xader,int board[4][4]);
int print(int board[4][4]);
void game(char a,int board[4][4]);
bool moveLeft(int board[4][4]);
bool moveRight(int board[4][4]);
bool moveUp(int board[4][4]);
bool moveDown(int board[4][4]);
bool slideAndMergeRow(int row[4]);
void addRandomTile(int board[4][4]);
struct termios oled;
struct termios new;

int main() {
    char in;
    int y;
    int x;
    int *yader = &y;
    int *xader = &x;
    int board[4][4] = {0};

    srand(time(NULL));
    noicanon(1);

    init(yader, xader, board);
    system("clear");
    print(board);

    while (1) {
        if (read(STDIN_FILENO, &in, 1) > 0) {
            system("clear");
            game(in, board);
            print(board);
        }
    }
}
int init(int *yader,int *xader,int board[4][4]) {
    while(1) {
    *yader=rand()%4;
    *xader=rand()%4;
    if(board[*yader][*xader]==0) {
        board[*yader][*xader]=2;
        break;
    } 
    }
   
    
    return 0;
}
int print(int board[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%4d", board[i][j]);
        }
        printf("\n");
    }
    printf("\nUse WASD or arrow keys to move, q to quit.\n");
    return 0;
}
void noicanon(int i)
{
    if (i == 1) {
        tcgetattr(STDIN_FILENO, &oled);
        new = oled;
        new.c_lflag &= ~(ICANON | ECHO);
        new.c_cc[VMIN] = 0;
        new.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &new);
    }
    else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oled);
    }
}
void game(char a, int board[4][4]) {
    bool moved = false;
    if (a == '\033') {
        char buf[2];
        if (read(STDIN_FILENO, &buf[0], 1) > 0 && read(STDIN_FILENO, &buf[1], 1) > 0) {
            if (buf[0] == '[') {
                if (buf[1] == 'A') moved = moveUp(board);
                else if (buf[1] == 'B') moved = moveDown(board);
                else if (buf[1] == 'C') moved = moveRight(board);
                else if (buf[1] == 'D') moved = moveLeft(board);
            }
        }
    } else {
        if (a == 'w' || a == 'W') moved = moveUp(board);
        else if (a == 's' || a == 'S') moved = moveDown(board);
        else if (a == 'a' || a == 'A') moved = moveLeft(board);
        else if (a == 'd' || a == 'D') moved = moveRight(board);
        else if (a == 'q' || a == 'Q') {
            noicanon(0);
            exit(0);
        }
    }
    if (moved) {
        addRandomTile(board);
    }
}

bool slideAndMergeRow(int row[4]) {
    int original[4];
    memcpy(original, row, sizeof(original));

    int write = 0;
    for (int i = 0; i < 4; i++) {
        if (row[i] != 0) {
            row[write++] = row[i];
        }
    }
    for (int i = write; i < 4; i++) {
        row[i] = 0;
    }

    for (int i = 0; i < 3; i++) {
        if (row[i] != 0 && row[i] == row[i + 1]) {
            row[i] *= 2;
            row[i + 1] = 0;
        }
    }

    write = 0;
    for (int i = 0; i < 4; i++) {
        if (row[i] != 0) {
            row[write++] = row[i];
        }
    }
    for (int i = write; i < 4; i++) {
        row[i] = 0;
    }

    return memcmp(original, row, sizeof(original)) != 0;
}

bool moveLeft(int board[4][4]) {
    bool changed = false;
    for (int i = 0; i < 4; i++) {
        if (slideAndMergeRow(board[i])) changed = true;
    }
    return changed;
}

bool moveRight(int board[4][4]) {
    bool changed = false;
    int row[4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            row[j] = board[i][3 - j];
        }
        if (slideAndMergeRow(row)) changed = true;
        for (int j = 0; j < 4; j++) {
            board[i][3 - j] = row[j];
        }
    }
    return changed;
}

bool moveUp(int board[4][4]) {
    bool changed = false;
    int col[4];
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            col[i] = board[i][j];
        }
        if (slideAndMergeRow(col)) changed = true;
        for (int i = 0; i < 4; i++) {
            board[i][j] = col[i];
        }
    }
    return changed;
}

bool moveDown(int board[4][4]) {
    bool changed = false;
    int col[4];
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            col[i] = board[3 - i][j];
        }
        if (slideAndMergeRow(col)) changed = true;
        for (int i = 0; i < 4; i++) {
            board[3 - i][j] = col[i];
        }
    }
    return changed;
}

void addRandomTile(int board[4][4]) {
    int empties[16][2];
    int count = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (board[i][j] == 0) {
                empties[count][0] = i;
                empties[count][1] = j;
                count++;
            }
        }
    }
    if (count == 0) return;
    int index = rand() % count;
    int value = (rand() % 10 == 0) ? 4 : 2;
    board[empties[index][0]][empties[index][1]] = value;
}

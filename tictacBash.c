/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char baseCommand[] = "mosquitto_pub -h 104.198.150.251 -t esp32/tictac -m ";
    char fullCommand[256];


    printf("\nTicTacToe \n1: PvB(Bot), \n2: PvP(Local), \n3: Auto(BotvBot)): ");
    int option;
    int moves;
    scanf("%d", &option);

    // Handle input commands
    switch (option)
    {
    case 1: 
        snprintf(fullCommand, sizeof(fullCommand), "%s\"1\"", baseCommand);
        printf("\nPlayer vs Botn");
        system(fullCommand);
        break;

    case 2: 
        snprintf(fullCommand, sizeof(fullCommand), "%s\"2\"", baseCommand);
        printf("\nLocal Play\n");
        system(fullCommand);
        break;

    case 3:
        snprintf(fullCommand, sizeof(fullCommand), "%s\"3\"", baseCommand);
        printf("\nAuto Play\n");
        system(fullCommand);
        break;

    case 4: // Exit program
        printf("Exiting program.\n");
        system("pause");
        exit(0);

    default: // Invalid input
        printf("Invalid input received. Please enter 1, 2, or 3.\n");
    }

    if (option == 2) 
    {

    }
    return 0;
}

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char board[9];
int turn = 1; // 1 = X, 0 = O

void resetBoard() {
    for (int i = 0; i < 9; i++) {
        board[i] = '1' + i;
    }
}

void printBoard() {
    printf("\n");
    printf(" %c | %c | %c \n", board[0], board[1], board[2]);
    printf("---+---+---\n");
    printf(" %c | %c | %c \n", board[3], board[4], board[5]);
    printf("---+---+---\n");
    printf(" %c | %c | %c \n", board[6], board[7], board[8]);
    printf("\n");
}

void updateBoard(int move) {
    if (move < 1 || move > 9) return;
    board[move - 1] = (turn == 1) ? 'X' : 'O';
    turn = 1 - turn;
    printBoard();
}

void listenForMoves() {
    FILE* fp = popen("mosquitto_sub -h 104.198.150.251 -t move", "r");
    if (!fp) {
        perror("mosquitto_sub failed");
        return;
    }

    char line[128];
    resetBoard();
    printBoard();

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0; // remove newline

        if (strncmp(line, "X wins", 6) == 0 || strncmp(line, "O wins", 6) == 0 || strncmp(line, "Draw", 4) == 0) {
            printf("\nGame Over: %s\n", line);
            break;
        }

        int move = atoi(line);
        if (move >= 1 && move <= 9) {
            updateBoard(move);
        }
    }

    pclose(fp);
}

void sendMove() {
    int move;
    char cmd[128];

    while (1) {
        printf("Enter your move (1–9): ");
        scanf("%d", &move);
        if (move < 1 || move > 9) {
            printf("Invalid move.\n");
            continue;
        }

        snprintf(cmd, sizeof(cmd), "mosquitto_pub -h 104.198.150.251 -t player/move -m \"%d\"", move);
        system(cmd);
    }
}

int main() {
    int option;
    char cmd[256];

    printf("TicTacToe Menu\n1: PvB (Bot)\n2: PvP (Local)\n3: Auto (Bot vs Bot)\n4: Exit\nChoose: ");
    scanf("%d", &option);

    switch (option) {
    case 1:
        snprintf(cmd, sizeof(cmd), "mosquitto_pub -h 104.198.150.251 -t esp32/tictac -m \"1\"");
        system(cmd);
        break;

    case 2:
        snprintf(cmd, sizeof(cmd), "mosquitto_pub -h 104.198.150.251 -t esp32/tictac -m \"2\"");
        system(cmd);

        // Start listener in new terminal
        system("start cmd /k mosquitto_sub -h 104.198.150.251 -t move");

        resetBoard();
        printBoard();
        sendMove(); // Continue sending moves until game ends
        break;

    case 3:
        snprintf(cmd, sizeof(cmd), "mosquitto_pub -h 104.198.150.251 -t esp32/tictac -m \"3\"");
        system(cmd);
        break;

    case 4:
        printf("Exiting.\n");
        return 0;

    default:
        printf("Invalid input.\n");
    }

    return 0;
}

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
int turn = 1;

// Reset the board to starting state
void resetBoard() {
    for (int i = 0; i < 9; i++) {
        board[i] = '1' + i;
    }
}

// Display the current state of the board
void printBoard() {
    printf("\n");
    printf(" %c | %c | %c \n", board[0], board[1], board[2]);
    printf("---+---+---\n");
    printf(" %c | %c | %c \n", board[3], board[4], board[5]);
    printf("---+---+---\n");
    printf(" %c | %c | %c \n\n", board[6], board[7], board[8]);
}

// Update the board with the current player's move
void updateBoard(int move) {
    if (move < 1 || move > 9) return;
    if (board[move - 1] == 'X' || board[move - 1] == 'O') return;
    board[move - 1] = (turn == 1) ? 'X' : 'O';
    turn = 1 - turn;
    printBoard();
}

// Send move and receive confirmation from ESP32
void sendMove() {
    int move;
    char pubCmd[128];
    char result[64];
    FILE* fp;
    char rematch;

    do {
        resetBoard();
        turn = 1;
        printBoard();

        while (1) {
            // CORRECT (with ASCII dash)
            printf("Enter your move (1-9): ");
            scanf("%d", &move);

            if (move < 1 || move > 9) {
                printf("Invalid move. Try again.\n");
                continue;
            }

            snprintf(pubCmd, sizeof(pubCmd), "mosquitto_pub -h 104.198.150.251 -t player/move -m \"%d\"", move);
            system(pubCmd);

            fp = popen("mosquitto_sub -h 104.198.150.251 -t move -C 1", "r");
            if (!fp) {
                perror("mosquitto_sub failed");
                continue;
            }

            if (fgets(result, sizeof(result), fp) != NULL) {
                result[strcspn(result, "\r\n")] = 0;

                if (strncmp(result, "X wins", 6) == 0 || strncmp(result, "O wins", 6) == 0 || strncmp(result, "Draw", 4) == 0) {
                    printf("\nGame Over: %s\n", result);
                    char rematch;
                    printf("Play again? (y/n): ");
                    scanf(" %c", &rematch);

                    if (rematch == 'y' || rematch == 'Y') {
                    system("mosquitto_pub -h 104.198.150.251 -t player/rematch -m \"Y\"");
                    } 
                    else 
                    {
                    system("mosquitto_pub -h 104.198.150.251 -t player/rematch -m \"N\"");
                    }

                    pclose(fp);
                    break; // Exit inner move loop, ask for rematch
                }

                int confirmedMove = atoi(result);
                if (confirmedMove >= 1 && confirmedMove <= 9) {
                    updateBoard(confirmedMove);
                }
            }

            pclose(fp);
        }

        // Ask for rematch
        printf("Play again? (y/n): ");
        scanf(" %c", &rematch); // Note space before %c to skip newline

    } while (rematch == 'y' || rematch == 'Y');
}



int main() {
    int option;
    char cmd[256];

    while (1) {
        printf("\n=== Tic-Tac-Toe Menu ===\n");
        printf("1: PvB (Bot)\n");
        printf("2: PvP (Local via ESP32)\n");
        printf("3: Auto (Bot vs Bot)\n");
        printf("4: Exit\n");
        printf("Choose an option: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                snprintf(cmd, sizeof(cmd), "mosquitto_pub -h 104.198.150.251 -t esp32/tictac -m \"1\"");
                system(cmd);
                printf("PvB mode started. Waiting for ESP32...\n");
                break;

            case 2:
                snprintf(cmd, sizeof(cmd), "mosquitto_pub -h 104.198.150.251 -t esp32/tictac -m \"2\"");
                system(cmd);
                sendMove(); // Run PvP game
                break;

            case 3:
                snprintf(cmd, sizeof(cmd), "mosquitto_pub -h 104.198.150.251 -t esp32/tictac -m \"3\"");
                system(cmd);
                printf("Auto mode started. Watch ESP32 for progress.\n");
                break;

            case 4:
                printf("Exiting the game.\n");
                return 0;

            default:
                printf("Invalid option. Please choose 1–4.\n");
        }
    }

    return 0;
}

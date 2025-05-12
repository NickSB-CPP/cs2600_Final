#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    char baseCommand[] = "mosquitto_pub -h 34.71.227.204 -t esp32/tictac -m ";
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
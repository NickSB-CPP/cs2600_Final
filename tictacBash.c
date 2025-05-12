#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    char baseCommand[] = "mosquitto_pub -h 34.71.227.204 -t esp32/tictac -m ";
    char fullCommand[256];

    while (1)
    {
        printf("\nTicTacToe \n1: LOCAL PLAY, \n2: REMOTE PLAY, \n3: EXIT): ");
        int option;
        scanf("%d", &option);

        // Handle input commands
        switch (option)
        {
        case 1: // Turn on LED
            snprintf(fullCommand, sizeof(fullCommand), "%s\"1\"", baseCommand);
            printf("\nbot combat initiated\n");
            system(fullCommand);
            break;

        case 2: // Turn off LED
            snprintf(fullCommand, sizeof(fullCommand), "%s\"2\"", baseCommand);
            printf("WIP\n");
            system(fullCommand);
            break;

        case 3: // Exit program
            printf("Exiting program.\n");
            system("pause");
            exit(0);

        default: // Invalid input
            printf("Invalid input received. Please enter 1, 2, or 3.\n");
        }
    }

    return 0;
}
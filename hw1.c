#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEFAULT_INPUT_FILE "processes.in"

#define FCFS 0;
#define SJF 1;
#define RR 2;

int main(void)
{
    int processCount = 0;
    int runTime = 0;
    int schedulingAlgorithm = FCFS;
    int timeQuantum = 0;

    char **processNames;
    int *arrivalTimes;
    int *burstTimes;

    char line[256];
    char tempToken[256];
    char tempValue[256];

    int headerComplete = 0;

    int i;

    FILE *inputFile = fopen(DEFAULT_INPUT_FILE, "r");

    if (inputFile == NULL) {
        printf("Input file could not be opened.");
        return 1;
    }

    while (fgets(line, sizeof(line), inputFile)) {
        printf("%s", line);

        if (strcmp(line, "end") == 0) {
            break;
        }

        sscanf(line, "%s %s", tempToken, tempValue);

        if (strcmp(tempToken, "processcount") == 0) {
            processCount = atoi(tempValue);
        } else if (strcmp(tempToken, "runfor") == 0) {
            runTime = atoi(tempValue);
        } else if (strcmp(tempToken, "use") == 0) {
            if (strcmp(tempValue, "fcfs") == 0) {
                schedulingAlgorithm = FCFS;
                headerComplete = 1;
            } else if (strcmp(tempValue, "sjf") == 0) {
                schedulingAlgorithm = SJF;
                headerComplete = 1;
            } else if (strcmp(tempValue, "rr") == 0) {
                schedulingAlgorithm = RR;
            }
        } else if (strcmp(tempToken, "quantum") == 0) {
            timeQuantum = atoi(tempValue);
            headerComplete = 1;
        }

        if (headerComplete) {
            break;
        }
    }

    processNames = malloc(sizeof(char *) * processCount);

    for (i = 0; i < processCount; i++) {
        processNames[i] = malloc(sizeof(char *) * 256);
    }

    arrivalTimes = malloc(sizeof(int) * processCount);
    burstTimes = malloc(sizeof(int) * processCount);

    // EXAMPLE: process name P1 arrival 3 burst 5
    //          process name P2 arrival 0 burst 9
    for (i = 0; i < processCount; i++) {
        fgets(line, sizeof(line), inputFile);

        if (strcmp(line, "end") == 0) {
            break;
        }

        sscanf(line, "%*s %*s %s %*s %d %*s %d", processNames[i], &arrivalTimes[i], &burstTimes[i]);
    }

    printf("\n\n");
    printf("Process Count        : %d\n", processCount);
    printf("Run Time             : %d\n", runTime);
    printf("Scheduling Algorithm : %d\n", schedulingAlgorithm);
    printf("Time Quantum         : %d\n", timeQuantum);

    for (i = 0; i < processCount; i++) {
        printf("%s %d %d\n", processNames[i], arrivalTimes[i], burstTimes[i]);
    }

    fclose(inputFile);

    return 0;
}

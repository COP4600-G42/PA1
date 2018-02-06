#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEFAULT_INPUT_FILE "processes.in"

#define FCFS 0;
#define SJF 1;
#define RR 2;

// Holds information for a single process
typedef struct Process {
    char *processName;
    int arrivalTime;
    int burstTime;
} Process;

// Holds information for a
typedef struct Timeline {
    int processCount;
    int runTime;
    int schedulingAlgorithm;
    int timeQuantum;

    Process **processes;
} Timeline;

void readInputFile(Timeline *inputTimeline)
{
    char line[256];
    char tempToken[256];
    char tempValue[256];

    int headerComplete = 0;
    int i;

    FILE *inputFile = fopen(DEFAULT_INPUT_FILE, "r");

    if (inputFile == NULL) {
        printf("Input file could not be opened.");
        return;
    }

    while (fgets(line, sizeof(line), inputFile)) {
        printf("%s", line);

        if (strcmp(line, "end") == 0) {
            break;
        }

        sscanf(line, "%s %s", tempToken, tempValue);

        if (strcmp(tempToken, "processcount") == 0) {
            inputTimeline->processCount = atoi(tempValue);
        } else if (strcmp(tempToken, "runfor") == 0) {
            inputTimeline->runTime = atoi(tempValue);
        } else if (strcmp(tempToken, "use") == 0) {
            if (strcmp(tempValue, "fcfs") == 0) {
                inputTimeline->schedulingAlgorithm = FCFS;
                headerComplete = 1;
            } else if (strcmp(tempValue, "sjf") == 0) {
                inputTimeline->schedulingAlgorithm = SJF;
                headerComplete = 1;
            } else if (strcmp(tempValue, "rr") == 0) {
                inputTimeline->schedulingAlgorithm = RR;
            }
        } else if (strcmp(tempToken, "quantum") == 0) {
            inputTimeline->timeQuantum = atoi(tempValue);
            headerComplete = 1;
        }

        if (headerComplete) {
            break;
        }
    }

    // Make space for the processes
    inputTimeline->processes = malloc(sizeof(Process *) * inputTimeline->processCount);

    // Make space for the process names
    for (i = 0; i < inputTimeline->processCount; i++) {
        inputTimeline->processes[i] = malloc(sizeof(Process));
        inputTimeline->processes[i]->processName = malloc(sizeof(char *) * 256);
    }

    // EXAMPLE: process name P1 arrival 3 burst 5
    //          process name P2 arrival 0 burst 9
    for (i = 0; i < inputTimeline->processCount; i++) {
        fgets(line, sizeof(line), inputFile);

        if (strcmp(line, "end") == 0) {
            break;
        }

        sscanf(line, "%*s %*s %s %*s %d %*s %d", inputTimeline->processes[i]->processName, &inputTimeline->processes[i]->arrivalTime, &inputTimeline->processes[i]->burstTime);
    }

    printf("\n\n");
    printf("Process Count        : %d\n", inputTimeline->processCount);
    printf("Run Time             : %d\n", inputTimeline->runTime);
    printf("Scheduling Algorithm : %d\n", inputTimeline->schedulingAlgorithm);
    printf("Time Quantum         : %d\n", inputTimeline->timeQuantum);

    fclose(inputFile);
}

int main(void)
{
    Timeline *inputTimeline = malloc(sizeof(Timeline));

    int i;

    readInputFile(inputTimeline);

    for (i = 0; i < inputTimeline->processCount; i++) {
        printf("%s %d %d\n", inputTimeline->processes[i]->processName, inputTimeline->processes[i]->arrivalTime, inputTimeline->processes[i]->burstTime);
    }

    for (i = 0; i < inputTimeline->processCount; i++) {
        // free(inputTimeline->processes[i]->processName);
    }
    // free(inputTimeline->processes);

    return 0;
}

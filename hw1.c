#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEFAULT_INPUT_FILE "processes.in"

// Set to 1 to print debug statements
#define DEBUG 1

#define FCFS  0
#define SJF   1
#define RR    2

// Holds information for a single process
typedef struct Process {
    char *processName;
    int arrivalTime;
    int burstTime;
    int timeLeft;
    int endTime;
} Process;

// Holds information for a timeline of processes
typedef struct Timeline {
    int processCount;
    int runTime;
    int schedulingAlgorithm;
    int timeQuantum;

    Process **processes;
} Timeline;


Timeline *createTimeline(char *inputFileName)
{
    char line[256];
    char tempToken[256];
    char tempValue[256];

    int headerComplete = 0;
    int i;

    Timeline *inputTimeline = malloc(sizeof(Timeline));

    FILE *inputFile = fopen(inputFileName, "r");

    if (inputFile == NULL) {
        printf("Input file could not be opened.");
        return NULL;
    }

    while (fgets(line, sizeof(line), inputFile)) {
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
            inputTimeline->timeQuantum = 0;
            break;
        }
    }

    // Allocate memory for the processes array
    inputTimeline->processes = malloc(sizeof(Process *) * inputTimeline->processCount);

    // Allocate memory for each individual process
    for (i = 0; i < inputTimeline->processCount; i++) {
        inputTimeline->processes[i] = malloc(sizeof(Process));
        inputTimeline->processes[i]->processName = malloc(sizeof(char *) * 256);
    }

    // Populate information for each individual process
    for (i = 0; i < inputTimeline->processCount; i++) {
        fgets(line, sizeof(line), inputFile);

        if (strcmp(line, "end") == 0) {
            break;
        }

        sscanf(line, "%*s %*s %s %*s %d %*s %d",
               inputTimeline->processes[i]->processName,
               &inputTimeline->processes[i]->arrivalTime,
               &inputTimeline->processes[i]->burstTime
               );
    }

    if (DEBUG) {
        printf("\nTIMELINE INFORMATION\n\n");
        printf("Process Count        : %d\n", inputTimeline->processCount);
        printf("Total Run Time       : %d\n", inputTimeline->runTime);
        printf("Scheduling Algorithm : %d\n", inputTimeline->schedulingAlgorithm);
        printf("Time Quantum         : %d\n", inputTimeline->timeQuantum);
        printf("\n---------------------\n\n");
    }

    fclose(inputFile);

    return inputTimeline;
}

void freeTimeline(Timeline *timeline)
{
    int i;
    int processCount = timeline->processCount;

    // Free memory for each individual process
    for (i = 0; i < processCount; i++) {
        free(timeline->processes[i]->processName);
        free(timeline->processes[i]);
    }

    // Free memory for the processes array
    free(timeline->processes);

    // Free memory for the timeline
    free(timeline);
}

void swapProcesses(Process **xp, Process **yp)
{
    Process temp = **xp;
    **xp = **yp;
    **yp = temp;
}

void sortByArrivalTime(Timeline *timeline)
{
   int i,j,a;
   int processCount = timeline->processCount;

   for (i = 0; i < processCount-1; i++) {
       for (j = 0; j < processCount-i-1; j++) {
           if (timeline->processes[j]->arrivalTime > timeline->processes[j+1]->arrivalTime) {
              swapProcesses(&timeline->processes[j], &timeline->processes[j+1]);
           }
       }
   }
}

void roundRobin(Timeline *timeline)
{
   
}

void firstComeFirstServed(Timeline *timeline)
{
    int i;
    int timeElasped = 0;
    int processIndex = 0;

    sortByArrivalTime(timeline);

    while (timeElasped <= timeline->runTime)
    {
        if (timeline->processes[processIndex]->arrivalTime == timeElasped) {
            printf("Time %d: %s arrived\n", timeElasped, timeline->processes[processIndex]->processName);
            processIndex++;
        }

        timeElasped++;
    }

    // while (timeElasped <= timeline->runTime) {
    //     for (i = 0; i < timeline->processCount; i++) {
    //         if (timeline->processes[i]->arrivalTime == timeElasped) {
    //             printf("Time %d: %s arrived\n", timeElasped, timeline->processes[i]->processName);
    //         } else {
    //             printf("Time %d: %s selected (burst %d)\n", timeElasped, timeline->processes[i]->processName, timeline->processes[i]->burstTime);
    //             timeElasped += timeline->processes[i]->burstTime;
    //         }
    //     }
    // }
}

void shortestJobFirst(Timeline *timeline)
{

}

int main(int argc, char *argv[])
{
    Timeline *timeline;

    int i;

    if (argv[1] != NULL) {
        timeline = createTimeline(argv[1]);
    } else {
        timeline = createTimeline(DEFAULT_INPUT_FILE);
    }

    // Print all stored processes (for debug purposes)
    if (DEBUG) {
        printf("PROCESSES INFORMATION\n\n");
        for (i = 0; i < timeline->processCount; i++) {
            printf("Process Name: %s\n", timeline->processes[i]->processName);
            printf("Arrival Time: %d\n", timeline->processes[i]->arrivalTime);
            printf("Burst Time  : %d\n\n", timeline->processes[i]->burstTime);
        }
    }

    switch (timeline->schedulingAlgorithm) {
        case FCFS:
            printf("FCFS\n");
            firstComeFirstServed(timeline);
            break;
        case SJF:
            printf("SJF\n");
            shortestJobFirst(timeline);
            break;
        case RR:
            printf("RR\n");
            roundRobin(timeline);
            break;
        default:
            printf("ERROR: Invalid scheduling algorithm\n");
            break;
    }

    freeTimeline(timeline);

    return 0;
}

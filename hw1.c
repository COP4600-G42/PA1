#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEFAULT_INPUT_FILE "processes.in"

// Set to 1 to print debug statements
#define DEBUG 0

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
    int waitTime;
    int turnAroundTime;
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

// sorts the process by process burst time left. looks at the list and will bubble sort smallest burst going to index 0. ignores list items on index count and higher
void sortByburst (int *processListIn, int count, Timeline *timeline)
{
   int i,j,a;
   int tmpProcessNum = -1;
	for(i=count-1; i > 0; i-- ){
		if(timeline->processes[processListIn[i]]->burstTime < timeline->processes[processListIn[i-1]]->burstTime ){
			tmpProcessNum = processListIn[i];
			processListIn[i] = processListIn[i-1];
			processListIn[i-1] =tmpProcessNum;
		}
	}
	return;

}

//preemptive Shortest Job First
void shortestJobFirst (Timeline *timeline)
{
    int time = 0; // current time in sequence
    int j = 0, k = 0, i = 0; // looping variables
    int runTime = timeline->runTime; // this holds the run for time
    int processCount = timeline->processCount;

	int inCount = 0;
	int change = -1;

	// malloc bc c is not cool like java.
	int *processListIn = malloc(sizeof(int) * processCount); // [processnumber] this is like a queue

    // sortByArrivalTime(timeline);

	// logic: preemptive so pay attention to a processArrivel time and burst left
    while (time < runTime) {
		//check arrivals
		for (k = 0; k < timeline->processCount; k++) {
			if (timeline->processes[k]->arrivalTime == time) {
				printf("Time %i: %s arrived\n", time, timeline->processes[k]->processName);
				processListIn[inCount] = k;
				inCount++;
				sortByburst(processListIn, inCount, timeline);
			}
		}

		// if inCount == 0 stack is empty so we idle
		if (inCount > 0) {
			k = processListIn[0]; // holds the location of the current process in the timeline

			//look at wait times, if process is waiting increment it
			for (i=1; i < inCount; i++) {
				timeline->processes[processListIn[i]]->waitTime += 1;
			}

			// process ran
			if (change != processListIn[0]) {
				printf("Time %i: %s selected (burst %i)\n", time,timeline->processes[k]->processName, timeline->processes[k]->burstTime);
				change = processListIn[0];
			}

			// process ran so update burst
			timeline->processes[k]->burstTime -= 1;
			timeline->processes[k]->timeLeft = timeline->processes[k]->burstTime;

			// check if complete
			if (timeline->processes[k]->burstTime == 0) {
				printf("Time %i: %s finished\n", time + 1, timeline->processes[k]->processName);
				timeline->processes[k]->endTime = time + 1;
				timeline->processes[k]->turnAroundTime = timeline->processes[k]->endTime - timeline->processes[k]->arrivalTime;
				timeline->processes[k]->burstTime = 2147483000;
				sortByburst(processListIn, inCount, timeline);
				inCount--;
				timeline->processes[k]->burstTime = 0;
			}
		} else {
			printf("Time %i: Idle\n", time);
		}

		time++;
    }

	printf("Finished at time %i\n\n", time);

	for (i = 0; i < timeline->processCount; i++) {
		printf("%s wait %i turnaround %i\n", timeline->processes[i]->processName, timeline->processes[i]->waitTime, timeline->processes[i]->turnAroundTime);
    }

	free(processListIn);
}

void printHeader(Timeline *timeline)
{
    printf("%d processes\n", timeline->processCount);

    switch (timeline->schedulingAlgorithm) {
        case FCFS:
            printf("Using First-Come First-Served\n");
            break;
        case SJF:
            printf("Using Shortest Job First\n");
            break;
        case RR:
            printf("Using Round-Robin\n");
            printf("Quantum %d", timeline->timeQuantum);
            break;
        default:
            printf("ERROR: Invalid scheduling algorithm\n");
            break;
    }

    printf("\n");
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

    printHeader(timeline);

    switch (timeline->schedulingAlgorithm) {
        case FCFS:
            if (DEBUG) printf("FCFS\n");
            firstComeFirstServed(timeline);
            break;
        case SJF:
            if (DEBUG) printf("SJF\n");
            shortestJobFirst(timeline);
            break;
        case RR:
            if (DEBUG) printf("RR\n");
            roundRobin(timeline);
            break;
        default:
            printf("ERROR: Invalid scheduling algorithm\n");
            break;
    }

    freeTimeline(timeline);

    return 0;
}

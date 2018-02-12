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
                inputTimeline->timeQuantum = 0;
                headerComplete = 1;
            } else if (strcmp(tempValue, "sjf") == 0) {
                inputTimeline->schedulingAlgorithm = SJF;
                inputTimeline->timeQuantum = 0;
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
    int i, j, a;
    int processCount = timeline->processCount;

    for (i = 0; i < processCount - 1; i++) {
        for (j = 0; j < processCount - i - 1; j++) {
            if (timeline->processes[j]->arrivalTime > timeline->processes[j + 1]->arrivalTime) {
                swapProcesses(&timeline->processes[j], &timeline->processes[j + 1]);
            }
        }
    }
}

void roundRobin(Timeline *timeline)
{
    int time = 0;
    int j = 0;
    int i = 0;
    int arrived = 0;
    int k = 0;
    int finishProcesses = 0;
    int turnaround = 0;
    int waitTime = 0;

    int runTime = timeline->runTime;
    int processCount = timeline->processCount;
    int quantumTime  = timeline->timeQuantum;
    sortByArrivalTime(timeline);

    while (time < runTime)
    {
        if (timeline->processes[j % processCount]->arrivalTime > time) //.....if the first process arrival time is bigger idle until start time
        {
            printf("Time is %d: Idle \n", time);
            time++;
        }

        for (j = 0; j < processCount ; j++)
        {
            // when the arrival time equals the current time or more process gets selected
            if (timeline->processes[j % processCount]->arrivalTime <= time)
            {
                if (timeline->processes[i % processCount]->arrivalTime == time) //only print arrival for equal time
                {
                    printf("Time is %d: %s arrived  \n", timeline->processes[i % processCount]->arrivalTime, timeline->processes[i % processCount]->processName);
                    timeline->processes[i % processCount]->timeLeft = timeline->processes[i % processCount]->burstTime;
                    i++;
                    arrived++;
                }

                if (timeline->processes[j % processCount]->timeLeft > 0  )
                {
                    printf("Time is %d: %s selected (burst %d) \n", time, timeline->processes[j % processCount]->processName, timeline->processes[j % processCount]->timeLeft);

                    if (timeline->processes[j % processCount]->timeLeft <= quantumTime ) {

                        time = time + timeline->processes[j % processCount]->timeLeft;
                        timeline->processes[j % processCount]->timeLeft = 0;
                        timeline->processes[j % processCount]->endTime = time;
                        printf("Time is %d: %s finished  \n", time, timeline->processes[j % processCount]->processName);
                        finishProcesses++;
                    } else if (timeline->processes[j % processCount]->timeLeft > quantumTime ) {
                        time = time + quantumTime;
                        timeline->processes[j % processCount]->timeLeft = (timeline->processes[j % processCount]->timeLeft) - quantumTime;
                    }

                    if (timeline->processes[i % processCount]->arrivalTime < time && arrived < processCount) {
                        printf("Time is %d: %s arrived  \n", timeline->processes[i % processCount]->arrivalTime, timeline->processes[i % processCount]->processName);
                        timeline->processes[i % processCount]->timeLeft = timeline->processes[i % processCount]->burstTime;
                        i++;
                        arrived++;
                    }
                }

                if (timeline->processes[j % processCount]->timeLeft == 0  )
                {
                    //processes is finish
                    if (finishProcesses == processCount)
                    {
                        if (time < runTime)
                        {
                            printf("Time is %d: Idle \n", time);
                            time++;
                        } else if (time >= runTime) {
                            printf("Finished at time : %d \n", time);
                        }
                    }
                }
                //printf("line 217\n");
            }
        }
    }

    //turnaround time = endtime- arrival time
    //wait time = turnaround - bursttime

    for (k = 0; k < processCount ; k++) {
        turnaround = (timeline->processes[k]->endTime) - (timeline->processes[k]->arrivalTime);
        waitTime = turnaround - (timeline->processes[k]->burstTime);
        printf("%s wait %d turnaround %d \n", timeline->processes[k]->processName, waitTime, turnaround);
    }
}

void firstComeFirstServed(Timeline *timeline)
{
    Process **processes = timeline->processes;
    int runTime = timeline->runTime;
    Process *currentProcess = NULL;

    int i;
    int pIndex = 0;

    sortByArrivalTime(timeline);

    // Evaluate each unit of time...
    for (i = 0; i <= timeline->runTime; i++) {
        // Check for newly arrived processes
        if (processes[pIndex]->arrivalTime == i) {
            // Make sure to setup the initial process
            if (currentProcess == NULL) {
                currentProcess = processes[pIndex];
            }

            // Announce the process arrival
            printf("Time %d: %s arrived\n", i, processes[pIndex]->processName);

            // Setup the initial timeLeft (which is the burstTime)
            processes[pIndex]->timeLeft = processes[pIndex]->burstTime;

            // Increase the process index to look for the next process
            pIndex++;
        }

        // Check for the start of a process
        if (currentProcess->timeLeft == currentProcess->burstTime) {
            // Announce the process selection
            printf("Time %d: %s selected (burst %d)\n", i, currentProcess->processName, currentProcess->burstTime);
        }

        // Decrement the time left for the current process
        currentProcess->timeLeft--;

        // If the current process is out of time, then select the new current process
        if (currentProcess->timeLeft == 0) {
            currentProcess = processes[pIndex - 1];
        }

        // Check for the end of the overall run time
        if (i == timeline->runTime) {
            printf("Finished at time %d\n", i);
        }
    }
}

// sorts the process by process burst time left. looks at the list and will bubble sort smallest burst going to index 0. ignores list items on index count and higher
void sortByburst (int *processListIn, int count, Timeline *timeline)
{
    int i, j, a;
    int tmpProcessNum = -1;

    for (i = (count - 1); i > 0; i--) {
        if (timeline->processes[processListIn[i]]->burstTime < timeline->processes[processListIn[i - 1]]->burstTime) {
            tmpProcessNum = processListIn[i];
            processListIn[i] = processListIn[i - 1];
            processListIn[i - 1] = tmpProcessNum;
        }
    }
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
            for (i = 1; i < inCount; i++) {
                timeline->processes[processListIn[i]]->waitTime += 1;
            }

            // process ran
            if (change != processListIn[0]) {
                printf("Time %i: %s selected (burst %i)\n", time, timeline->processes[k]->processName, timeline->processes[k]->burstTime);
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

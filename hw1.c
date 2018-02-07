#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEFAULT_INPUT_FILE "processes.in"

// Set to 1 to print debug statements
#define DEBUG 1

#define FCFS 0;
#define SJF 1;
#define RR 2;

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
      

Timeline *createTimeline()
{
    char line[256];
    char tempToken[256];
    char tempValue[256];

    int headerComplete = 0;
    int i;

    Timeline *inputTimeline = malloc(sizeof(Timeline));

    FILE *inputFile = fopen(DEFAULT_INPUT_FILE, "r");

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
void swapJobs(Process *xp, Process *yp)
{
    Process temp = *xp;
    *xp = *yp;
    *yp = temp;
}
Process * arrivalList (Timeline *timeline)
{  
   int i,j,a; 
   int processCount = timeline->processCount;
   
   Process* jobs = malloc(sizeof(Process *) * inputTimeline->processCount);

   for (a=0; a<processCount); a++ ){

    timeline->processes[a]->processName = jobs[a]->processName;
    timeline->processes[a]->arrivalTime= jobs[a]->arrivalTime;
    timeline->processes[a]->burstTime = jobs[a]->burstTime;
    timeline->processes[a]->timeLeft = jobs[a]->timeLeft;
    
   }
  

   for (i = 0; i < processCount-1; i++)        
       for (j = 0; j < processCount-i-1; j++) 
           if (jobs[j]->arrivalTime > jobs[j+1]->arrivalTime)
              swapJobs(&jobs[j], &jobs[j+1]);

  
return jobs;


}
int roundRobin (Timeline *timeline)
{
    int time=0;
    int j=0; 
    int i=0;
    int a=1;
    int runTime = timeline->runTime;
    int processCount = timeline->processCount;
    int quantumTime  = timeline->timeQuantum;

    
    while (time < =runTime) {
        
        if (jobs[i]->arrivalTime >= time){

        printf("Time is %d: %s arrived  ",jobs[i]->arrivalTime,jobs[i++]->processName);  
       

        }

        for (j=0; j< processCount ; j++){

            if (jobs[j%processCount]->timeLeft>0){
            
            printf("Time is %d: %s selected (burst %d) ",time,jobs[j%processCount]->processName, jobs[j%processCount]->burstTime);

            if (jobs[j%processCount]->timeLeft <= quantumTime && (time + jobs[j%processCount]->timeLeft<= runTime)){
                time = time + jobs[j%processCount]->timeLeft;
                jobs[j%processCount]->timeLeft = 0;
                jobs[j%processCount]->endTime = time;
                printf("Time is %d: %s finished  ",time,jobs[j%processCount]->processName); 
            }
            else {
                time = time + quantumTime;
                jobs[j%processCount]->timeLeft = (jobs[j%processCount]->timeLeft) - quantumTime;   
            }


         }

        }
        
        
    }
}

int main(void)
{
    Timeline *timeline = createTimeline();

    int i;

    // Print all stored processes (for debug purposes)
    if (DEBUG) {
        printf("PROCESSES INFORMATION\n\n");
        for (i = 0; i < timeline->processCount; i++) {
            printf("Process Name: %s\n", timeline->processes[i]->processName);
            printf("Arrival Time: %d\n", timeline->processes[i]->arrivalTime);
            printf("Burst Time  : %d\n\n", timeline->processes[i]->burstTime);
        }
    }

    freeTimeline(timeline);

    return 0;
}

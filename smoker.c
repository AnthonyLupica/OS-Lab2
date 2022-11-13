/* 
    smoker.c

    Operating Systems (Fall 2022) Lab 2
    pthreads and semaphores 

    The Cigarette Smokers’ Problem 

    How to Compile and Run:
    In Ubuntu, or another Linux distribution, create a build directory in the folder containing 
    the .c program and CMakeLists.txt. From within the build directory, type... 
        
        cmake ..

    to create your Make files, followed by 
        
        make run 
           or 
        ./smoker 

    to execute the program.
*/

#include <stdio.h>      // printf     
#include <pthread.h>    // pthreads
#include <semaphore.h>  // semaphores
#include <time.h>       // time 
#include <stdlib.h>     // rand and srand
#include <unistd.h>     // usleep

// 3 agent functions
void *agent_tobaccoPaper(void *param);
void *agent_tobaccoMatch(void *param);
void *agent_paperMatch(void *param);

// 3 pusher functions 
void *pushTobacco(void *param);
void *pushPaper(void *param);
void *pushMatch(void *param);

// 6 smoker functions
void *smokerWithTobaccoOne(void *param);
void *smokerWithPaperOne(void *param);
void *smokerWithMatchOne(void *param);
void *smokerWithTobaccoTwo(void *param);
void *smokerWithPaperTwo(void *param);
void *smokerWithMatchTwo(void *param);

/* Semaphores */

// semaphores for the agents
sem_t agentSem;
sem_t tobacco;
sem_t paper;
sem_t match;

// semaphores for pushers to signal smokers
sem_t tobaccoSem;
sem_t paperSem;
sem_t matchSem;
sem_t mutex;

/* Booleans */

// these signal whether an ingredient is on the table; init false.
int isTobacco = 0;
int isPaper = 0;
int isMatch = 0;

int main ()
{
    srand(time(0));

    printf(" - - - - - - - - - - - - - - - - - - - \n");
    printf("|                                     |\n");
    printf("|     OPERATING SYSTEMS: LAB TWO      |\n");
    printf("|      PTHREADS AND SEMAPHORES        |\n");
    printf("|                                     |\n");
    printf("|   THE CIGARETTE SMOKERS' PROBLEM    |\n");
    printf("|                                     |\n");
    printf(" - - - - - - - - - - - - - - - - - - - \n\n");

    /* INITIALIZE SEMAPHORES */

    sem_init(&agentSem, 0, 1);
    sem_init(&tobacco, 0, 0);
    sem_init(&paper, 0, 0);
    sem_init(&match, 0, 0);

    sem_init(&tobaccoSem, 0, 0);
    sem_init(&paperSem, 0, 0);
    sem_init(&matchSem, 0, 0);
    sem_init(&mutex, 0, 1);

    /* DECLARE THREADS */

    // 3 agent threads
    pthread_t tobaccoPaper, tobaccoMatch, paperMatch;
    
    // 3 pusher threads
    pthread_t tobaccoPush, paperPush, matchPush;

    // 6 smoker threads
    pthread_t tobaccoSmokerOne, paperSmokerOne, matchSmokerOne;
    pthread_t tobaccoSmokerTwo, paperSmokerTwo, matchSmokerTwo;

    /* SPAWN THREADS */

    // spawn 3 agent threads 
    pthread_create(&tobaccoPaper, NULL, agent_tobaccoPaper, NULL);
    pthread_create(&tobaccoMatch, NULL, agent_tobaccoMatch, NULL);
    pthread_create(&paperMatch, NULL, agent_paperMatch, NULL);

    // spawn 3 pusher threads
    pthread_create(&tobaccoPush, NULL, pushTobacco, NULL);
    pthread_create(&paperPush, NULL, pushPaper, NULL);
    pthread_create(&matchPush, NULL, pushMatch, NULL);

    // spawn 6 smoker threads
    pthread_create(&tobaccoSmokerOne, NULL, smokerWithTobaccoOne, NULL);
    pthread_create(&paperSmokerOne, NULL, smokerWithPaperOne, NULL);
    pthread_create(&matchSmokerOne, NULL, smokerWithMatchOne, NULL);
    pthread_create(&tobaccoSmokerTwo, NULL, smokerWithTobaccoTwo, NULL);
    pthread_create(&paperSmokerTwo, NULL, smokerWithPaperTwo, NULL);
    pthread_create(&matchSmokerTwo, NULL, smokerWithMatchTwo, NULL);
    
    /* JOIN THREADS */

    // join 6 smokers
    pthread_join(tobaccoSmokerOne, NULL);
    pthread_join(paperSmokerOne, NULL);
    pthread_join(matchSmokerOne, NULL);
    pthread_join(tobaccoSmokerTwo, NULL);
    pthread_join(paperSmokerTwo, NULL);
    pthread_join(matchSmokerTwo, NULL);

    // join 3 pushers
    pthread_join(tobaccoPush, NULL);
    pthread_join(paperPush, NULL);
    pthread_join(matchPush, NULL);

    // join 3 agents
    pthread_join(tobaccoPaper, NULL);
    pthread_join(tobaccoMatch, NULL);
    pthread_join(paperMatch, NULL);

    printf("\nAll 6 smokers have been satisfied. One question remains... at what cost?\n\n");

    return 0;
}

/* agent one tables tobacco and a rolling paper */
void *agent_tobaccoPaper(void *param)
{
    for (int i = 0; i < 6; ++i) 
    {
        usleep(rand() % 200000 + 1);
        
        sem_wait(&agentSem);
        sem_post(&tobacco);
        sem_post(&paper);
    }
    
    pthread_exit(0);
}

/* agent two tables tobacco and a match */
void *agent_tobaccoMatch(void *param)
{
    for (int i = 0; i < 6; ++i) 
    {
        usleep(rand() % 200000 + 1);
        
        sem_wait(&agentSem);
        sem_post(&tobacco);
        sem_post(&match);
    }
    
    pthread_exit(0);
}

/* agent three tables a rolling paper and a match */
void *agent_paperMatch(void *param)
{
    for (int i = 0; i < 6; ++i) 
    {
        usleep(rand() % 200000 + 1);

        sem_wait(&agentSem);
        sem_post(&paper);
        sem_post(&match);
    }

    pthread_exit(0);
}

/* pusher one wakes up when tobacco is on the table */
void *pushTobacco(void *param)
{
    for (int i = 0; i < 12; ++i) 
    {
        sem_wait(&tobacco);
        sem_wait(&mutex);

        if (isPaper) 
        {
            printf("\nAttention all MATCH smokers, TOBACCO and a PAPER are on the table!\n\n\n");
            isPaper = 0;
            sem_post(&matchSem);
        }
        else if (isMatch) 
        {
            printf("\nAttention all PAPER smokers, TOBACCO and a MATCH are on the table!\n\n\n");
            isMatch = 0;
            sem_post(&paperSem);
        }
        else
        {
            isTobacco = 1;
        }

        sem_post(&mutex);
    }

    pthread_exit(0); 
}

/* pusher two wakes up when a rolling paper is on the table */
void *pushPaper(void *param)
{
    for (int i = 0; i < 12; ++i) 
    {
        sem_wait(&paper);
        sem_wait(&mutex);

        if (isTobacco) 
        {
            printf("\nAttention all MATCH smokers, a PAPER and TOBACCO are on the table!\n\n\n");
            isTobacco = 0;
            sem_post(&matchSem);
        }
        else if (isMatch) 
        {
            printf("\nAttention all TOBACCO smokers, a PAPER and a MATCH are on the table!\n\n\n");
            isMatch = 0;
            sem_post(&tobaccoSem);
        }
        else
        {
            isPaper = 1;
        }

        sem_post(&mutex);
    }

    pthread_exit(0); 
}

/* pusher three wakes up when a match is on the table */
void *pushMatch(void *param)
{
    for (int i = 0; i < 12; ++i) 
    {
        sem_wait(&match);
        sem_wait(&mutex);

        if (isTobacco) 
        {
            printf("\nAttention all PAPER smokers, a MATCH and TOBACCO are on the table!\n\n\n");
            isTobacco = 0;
            sem_post(&paperSem);
        }
        else if (isPaper) 
        {
            printf("\nAttention all TOBACCO smokers, a MATCH and a PAPER are on the table!\n\n\n");
            isPaper = 0;
            sem_post(&tobaccoSem);
        }
        else
        {
            isMatch = 1;
        }

        sem_post(&mutex);
    }

    pthread_exit(0); 
}

/* the first smoker with infinite tobacco */
void *smokerWithTobaccoOne(void *param)
{
    int smokeCount = 0;
    
    while (smokeCount < 3) 
    {
        sem_wait(&tobaccoSem);
        
        /* Make a cigarette */
        usleep(rand() % 50000 + 1);
        ++smokeCount;
        printf("Smoker ONE with INFINITE TOBACCO rolled up cigarette number %d\n\n", smokeCount);
        
        sem_post(&agentSem);
        
        /* Smoke the cigarette */
        usleep(rand() % 50000 + 1);
        printf("Smoker ONE with INFINITE TOBACCO is blowing smoke on cigarette number %d\n\n", smokeCount);
    }

    printf("Smoker ONE with INFINITE TOBACCO got hungry and went home\n\n");
    pthread_exit(0);
}

/* the second smoker with infinite tobacco */
void *smokerWithTobaccoTwo(void *param)
{
    int smokeCount = 0;
    
    while (smokeCount < 3) 
    {
        sem_wait(&tobaccoSem);
        
        /* Make a cigarette */
        usleep(rand() % 50000 + 1);
        ++smokeCount;
        printf("Smoker TWO with INFINITE TOBACCO rolled up cigarette number %d\n\n", smokeCount);
        
        sem_post(&agentSem);
        
        /* Smoke the cigarette */
        usleep(rand() % 50000 + 1);
        printf("Smoker TWO with INFINITE TOBACCO is blowing smoke on cigarette number %d\n\n", smokeCount);
    }

    printf("Smoker TWO with INFINITE TOBACCO got hungry and started snoring\n\n");
    pthread_exit(0);
}

/* the first smoker with infinite paper */
void *smokerWithPaperOne(void *param)
{
    int smokeCount = 0;

    while (smokeCount < 3) 
    {
        sem_wait(&paperSem);
        
        /* Make a cigarette */
        usleep(rand() % 50000 + 1);
        ++smokeCount;
        printf("Smoker ONE with INFINITE PAPER rolled up cigarette number %d\n\n", smokeCount);
    
        sem_post(&agentSem);
       
        /* Smoke the cigarette */
        usleep(rand() % 50000 + 1);
        printf("Smoker ONE with INFINITE PAPER is blowing smoke on cigarette number %d\n\n", smokeCount);
    }

    printf("Smoker ONE with INFINITE PAPER got hungry and went to Taco Bell\n\n");
    pthread_exit(0);
}

/* the second smoker with infinite paper */
void *smokerWithPaperTwo(void *param)
{
    int smokeCount = 0;

    while (smokeCount < 3) 
    {
        sem_wait(&paperSem);
        
        /* Make a cigarette */
        usleep(rand() % 50000 + 1);
        ++smokeCount;
        printf("Smoker TWO with INFINITE PAPER rolled up cigarette number %d\n\n", smokeCount);
    
        sem_post(&agentSem);
       
        /* Smoke the cigarette */
        usleep(rand() % 50000 + 1);
        printf("Smoker TWO with INFINITE PAPER is blowing smoke on cigarette number %d\n\n", smokeCount);
    }

    printf("Smoker TWO with INFINITE PAPER got hungry and started convulsing\n\n");
    pthread_exit(0);
}

/* the first smoker with infinite matches */
void *smokerWithMatchOne(void *param)
{
    int smokeCount = 0;

    while (smokeCount < 3) 
    {
        sem_wait(&matchSem);
       
        /* Make a cigarette */
        usleep(rand() % 50000 + 1);
        ++smokeCount;
        printf("Smoker ONE with INFINITE MATCHES rolled up cigarette number %d\n\n", smokeCount);
        
        sem_post(&agentSem);

        /* Smoke the cigarette */
        usleep(rand() % 50000 + 1);
        printf("Smoker ONE with INFINITE MATCHES is blowing smoke on cigarette number %d\n\n", smokeCount);
    }

    printf("Smoker ONE with INFINITE MATCHES got hungry and started eating the rolling paper\n\n");
    pthread_exit(0);
}

/* the second smoker with infinite matches */
void *smokerWithMatchTwo(void *param)
{
    int smokeCount = 0;

    while (smokeCount < 3) 
    {
        sem_wait(&matchSem);
       
        /* Make a cigarette */
        usleep(rand() % 50000 + 1);
        ++smokeCount;
        printf("Smoker TWO with INFINITE MATCHES rolled up cigarette number %d\n\n", smokeCount);
        
        sem_post(&agentSem);

        /* Smoke the cigarette */
        usleep(rand() % 50000 + 1);
        printf("Smoker TWO with INFINITE MATCHES is blowing smoke on cigarette number %d\n\n", smokeCount);
    }

    printf("Smoker TWO with INFINITE MATCHES got hungry and ate a granola bar\n\n");
    pthread_exit(0);
}

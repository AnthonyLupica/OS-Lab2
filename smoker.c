/* 
    smoker.c

    Operating Systems (Fall 2022) Lab 2
    pthreads and semaphores 

    The Cigarette Smokers’ Problem 
*/

#include <stdio.h>      // printf     
#include <pthread.h>
#include <semaphore.h> 
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

    /* Initialize Semaphores */

    sem_init(&agentSem, 0, 1);
    sem_init(&tobacco, 0, 0);
    sem_init(&paper, 0, 0);
    sem_init(&match, 0, 0);

    sem_init(&tobaccoSem, 0, 0);
    sem_init(&paperSem, 0, 0);
    sem_init(&matchSem, 0, 0);
    sem_init(&mutex, 0, 1);

    /* Declare Threads */

    // agent threads
    pthread_t tobaccoPaper, tobaccoMatch, paperMatch;
    
    // pusher threads
    pthread_t tobaccoPush, paperPush, matchPush;

    // smoker threads
    pthread_t tobaccoSmoker, paperSmoker, matchSmoker;

    // spawn agent threads 
    pthread_create(&tobaccoPaper, NULL, agent_tobaccoPaper, NULL);
    pthread_create(&tobaccoMatch, NULL, agent_tobaccoMatch, NULL);
    pthread_create(&paperMatch, NULL, agent_paperMatch, NULL);

    // spawn pusher threads
    pthread_create(&tobaccoPush, NULL, pushTobacco, NULL);
    pthread_create(&paperPush, NULL, pushPaper, NULL);
    pthread_create(&matchPush, NULL, pushMatch, NULL);

    // spawn smoker threads
    pthread_create(&tobaccoSmoker, NULL, smokerWithTobaccoOne, NULL);
    pthread_create(&paperSmoker, NULL, smokerWithPaperOne, NULL);
    pthread_create(&matchSmoker, NULL, smokerWithMatchOne, NULL);
    
    // join smokers
    pthread_join(tobaccoSmoker, NULL);
    pthread_join(paperSmoker, NULL);
    pthread_join(matchSmoker, NULL);

    // join agents
    pthread_join(tobaccoPaper, NULL);
    pthread_join(tobaccoMatch, NULL);
    pthread_join(paperMatch, NULL);

    // join pushers
    pthread_join(tobaccoPush, NULL);
    pthread_join(paperPush, NULL);
    pthread_join(matchPush, NULL);
    
    return 0;
}

/* The agent tables tobacco and a rolling paper */
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

/* The agent tables tobacco and a match */
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

/* The agent tables a rolling paper and a match */
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

void *pushTobacco(void *param)
{
    for (int i = 0; i < 12; ++i) 
    {
        sem_wait(&tobacco);
        sem_wait(&mutex);

        if (isPaper) 
        {
            isPaper = 0;
            sem_post(&matchSem);
        }
        else if (isMatch) 
        {
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

void *pushPaper(void *param)
{
    for (int i = 0; i < 12; ++i) 
    {
        sem_wait(&paper);
        sem_wait(&mutex);

        if (isTobacco) 
        {
            isTobacco = 0;
            sem_post(&matchSem);
        }
        else if (isMatch) 
        {
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

void *pushMatch(void *param)
{
    for (int i = 0; i < 12; ++i) 
    {
        sem_wait(&match);
        sem_wait(&mutex);

        if (isTobacco) 
        {
            isTobacco = 0;
            sem_post(&paperSem);
        }
        else if (isPaper) 
        {
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

void *smokerWithTobaccoOne(void *param)
{
    int smokeCount = 0;
    
    while (smokeCount < 3) 
    {
        sem_wait(&tobaccoSem);
        
        /* Make a cigarette */
        usleep(rand() % 50000 + 1);
        ++smokeCount;
        printf("Smoker one with INFINITE TOBACCO rolled up blunt number %d\n\n", smokeCount);
        
        sem_post(&agentSem);
        
        /* Smoke the cigarette */
        usleep(rand() % 50000 + 1);
        printf("Smoker one with INFINITE TOBACCO is blowing smoke on blunt number %d\n\n", smokeCount);
    }

    printf("Smoker one with INFINITE TOBACCO got hungry and went home\n\n");
    pthread_exit(0);
}

void *smokerWithPaperOne(void *param)
{
    int smokeCount = 0;

    while (smokeCount < 3) 
    {
        sem_wait(&paperSem);
        
        /* Make a cigarette */
        usleep(rand() % 50000 + 1);
        ++smokeCount;
        printf("Smoker one with INFINITE PAPER rolled up blunt number %d\n\n", smokeCount);
    
        sem_post(&agentSem);
       
        /* Smoke the cigarette */
        usleep(rand() % 50000 + 1);
        printf("Smoker one with INFINITE PAPER is blowing smoke on blunt number %d\n\n", smokeCount);
    }

    printf("Smoker one with INFINITE PAPER got hungry and went to Taco Bell\n\n");
    pthread_exit(0);
}

void *smokerWithMatchOne(void *param)
{
    int smokeCount = 0;

    while (smokeCount < 3) 
    {
        sem_wait(&matchSem);
       
        /* Make a cigarette */
        usleep(rand() % 50000 + 1);
        ++smokeCount;
        printf("Smoker one with INFINITE MATCHES rolled up blunt number %d\n\n", smokeCount);
        
        sem_post(&agentSem);

        /* Smoke the cigarette */
        usleep(rand() % 50000 + 1);
        printf("Smoker one with INFINITE MATCHES is blowing smoke on blunt number %d\n\n", smokeCount);
    }

    printf("Smoker one with INFINITE MATCHES got hungry and started eating the rolling paper\n\n");
    pthread_exit(0);
}

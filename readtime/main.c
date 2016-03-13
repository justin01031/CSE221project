//
//  main.c
//  readtime
//
//  Created by justin01031 on 1/31/16.
//  Copyright © 2016 justin01031. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "timing.c"

static double readtime(int itera){
    uint64_t start;
    uint64_t end;
    uint64_t elapsed;
    static mach_timebase_info_data_t    sTimebaseInfo;
    double total_time;
    // If this is the first time we've run, get the timebase.
    // We can use denom == 0 to indicate that sTimebaseInfo is
    // uninitialised because it makes no sense to have a zero
    // denominator is a fraction.
    // ref: https://developer.apple.com/library/mac/qa/qa1398/_index.html
    
    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }
    
    
    for(int i=0;i<itera;i++){
        start=rdtsc();
        end= rdtsc();
        elapsed = end - start;
       // printf("%llu\n",elapsed);
        uint64_t elapsedNano = elapsed*0.3847;
        total_time += elapsedNano;
    }
    double average= total_time/itera;
    
    return average;
}

static double loopreadtime(int itera){
    uint64_t start;
    uint64_t end;
    uint64_t elapsed;
    int loop_time=10000;
    static mach_timebase_info_data_t    sTimebaseInfo;
    double total_time;
    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }
    
    
    for(int i=0;i<itera;i++){
        start=rdtsc();
        for (int i=0; i<loop_time; i++) {
            end= rdtsc();  //can change to any procedure
        }
        end= rdtsc();
        elapsed = end - start;
        uint64_t elapsedNano = elapsed*0.3847/loop_time;
        total_time += elapsedNano;
    }
    double average= total_time/itera;
    
    return average;
}

static void zero(void){}
static void one(int a){}
static void two(int a, int b){}
static void three(int a, int b,int c){}
static void four(int a, int b, int c, int d){}
static void five(int a, int b,int c, int d, int e){}
static void six(int a, int b, int c, int d, int e, int f){}
static void seven(int a, int b, int c, int d, int e, int f, int g){}

static double procedure_overhead(int itera, int para_num){
    uint64_t start;
    uint64_t end;
    uint64_t elapsed;
    static mach_timebase_info_data_t    sTimebaseInfo;
    uint64_t total_elapsed=0;
    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }
    int innerloop=25;
    for (int i=0; i<itera/innerloop; i++) {
        switch (para_num) {
            case 0:
                start=rdtsc();
                for(int j=0;j<innerloop;j++){
                    zero();
                    zero();
                    zero();
                    zero();
                }
                end= rdtsc();
                break;
            case 1:
                start=rdtsc();
                for(int j=0;j<innerloop;j++){
                    one(0);
                    one(0);
                    one(0);
                    one(0);
                }
                end= rdtsc();
                break;
            case 2:
                start=rdtsc();
                for(int j=0;j<innerloop;j++){
                    two(0, 0);
                    two(0, 0);
                    two(0, 0);
                    two(0, 0);
                }
                end= rdtsc();
                break;
            case 3:
                start=rdtsc();
                for(int j=0;j<innerloop;j++){
                    three(0, 0, 0);
                    three(0, 0, 0);
                    three(0, 0, 0);
                    three(0, 0, 0);
                    
                }
                end= rdtsc();
                break;
            case 4:
                start=rdtsc();
                for(int j=0;j<innerloop;j++){
                    four(0, 0, 0, 0);
                    four(0, 0, 0, 0);
                    four(0, 0, 0, 0);
                    four(0, 0, 0, 0);
                }
                end= rdtsc();
                break;
            case 5:
                start=rdtsc();
                for(int j=0;j<innerloop;j++){
                    five(0, 0, 0, 0, 0);
                    five(0, 0, 0, 0, 0);
                    five(0, 0, 0, 0, 0);
                    five(0, 0, 0, 0, 0);
                }
                end= rdtsc();
                break;
            case 6:
                start=rdtsc();
                for(int j=0;j<innerloop;j++){
                    six(0, 0, 0, 0, 0, 0);
                    six(0, 0, 0, 0, 0, 0);
                    six(0, 0, 0, 0, 0, 0);
                    six(0, 0, 0, 0, 0, 0);
                }
                end= rdtsc();
                break;
            case 7:
                start=rdtsc();
                for(int j=0;j<innerloop;j++){
                    seven(0, 0, 0, 0, 0, 0, 0);
                    seven(0, 0, 0, 0, 0, 0, 0);
                    seven(0, 0, 0, 0, 0, 0, 0);
                    seven(0, 0, 0, 0, 0, 0, 0);
                }
                end= rdtsc();
                break;

            
            default:
                start=0;
                end=0;
                break;
        }
        elapsed = end - start;
        total_elapsed+=elapsed;
    }
    double total_time= ((double)(total_elapsed)*0.3847);
    double average= total_time/itera;
    return average;
}

double systemcall_overhead(unsigned long int itera){

    uint64_t start;
    uint64_t end;
    uint64_t elapsed;
    uint64_t total_elapsed=0;
    double average = 0.0;
    static mach_timebase_info_data_t    sTimebaseInfo;
    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }

    /* Toggling the gid doesn't work.
        The first call is much longer. Perhaps try to
        (1) fork a new process on each iteration or
        (2) use shell script. */
    // gid_t orig_gid = 20;
    // gid_t toggle_gid;

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 75000000;

    unsigned long int i;
    int innerloop=25;
    int outerloop=itera/(innerloop*4);
    for (i=0; i<outerloop; i++) {

        start = rdtsc();
        //syscall(SYS_getpid);
        for(int j=0;j<innerloop;j++){
            syscall(SYS_getpid);
            syscall(SYS_getpid);
            syscall(SYS_getpid);
            syscall(SYS_getpid);
        }
        end = rdtsc();
        
        //nanosleep(&ts, NULL);
        elapsed = end - start;
        total_elapsed +=elapsed;
    //    printf("%llu nsec\n", elapsed);


    }
    double total_time= ((double)(total_elapsed)*0.3847);
    average= total_time/itera;
    return average;
}
double systemcall_overhead_single_call(unsigned long int itera){
    
    uint64_t start;
    uint64_t end;
    uint64_t elapsed;
    uint64_t total_elapsed=0;


    
    start = rdtsc();
    syscall(SYS_getpid);
    end=rdtsc();
    for(int i=0;i<100000;i++){
        syscall(SYS_getgid);
    }
    uint64_t totalcycle=end-start;
    double total_time= (double)totalcycle*0.387;
    printf("%f\n", total_time);
    return total_time;
}

double process_creation_time(unsigned long int itera){

    uint64_t start;
    uint64_t end;
    uint64_t elapsed;
    uint64_t elapsedNano;
    double average = 0.0;
    static mach_timebase_info_data_t    sTimebaseInfo;
    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }

    pid_t child_pid;

    unsigned long int i;
    for (i=0; i<itera; i++) {

        /* Is this atomic? Can't think of a better way. */
        start = rdtsc();
        child_pid = fork();
        end = rdtsc();

        /* If maximum number of processes is reached,
            abort iteration and calculate average as is. */
        /* No longer needed after adding wait for child (see below). */
        // if (child_pid<0) {
        //     itera = i;
        //     break;
        // }

        if (child_pid!=0) {
            /* Parent */
            // printf("Parent with child %d\n", child_pid);
            elapsed = end - start;
            elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
            average += elapsedNano;
            // printf("End Parent.\n");

            /* To avoid congestion, wait for child to terminate. */
            int status;
            (void) waitpid(child_pid, &status, 0);
        }
        else {
            /* Child */
            // printf("Child #%d\n", getpid());
            // printf("End Child.\n");
            exit(0);
        }
    }

    // printf("itera ran: %lu\n", itera);
    average = average/itera;
    return average;
}

void *threadProcedure(){
    pthread_exit(NULL);
}

double pthread_creation_time(unsigned long int itera){

    uint64_t start;
    uint64_t end;
    uint64_t elapsed;
    double elapsedNano;
    double average = 0.0;
    static mach_timebase_info_data_t    sTimebaseInfo;
    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }

    int pt_status;
    pthread_t pthread;

    unsigned long int i;
    for (i=0; i<itera; i++) {

        start = mach_absolute_time();
        pt_status = pthread_create(&pthread, NULL, threadProcedure, NULL);
        end = mach_absolute_time();

        /* If maximum number of threads is reached,
            abort iteration and calculate average as is. */
        /* No longer needed after adding join for thread (see below). */
        // if (pt_status<0) {
        //     itera = i;
        //     break;
        // }
        elapsed = end - start;
        elapsedNano = (double)(end-start)*0.3847;
        average += elapsedNano;

        /* To avoid congestion, join the thread. */
        void *status;
        (void) pthread_join(pthread, &status);

    }

    // printf("itera ran: %lu\n", itera);
    // printf("average: %lf\n", average);
    average = average/itera;
    return average;
}
#define readout 0
#define writein 1
double contextswitch_time_two_pipe(int itera){
    

    uint64_t elapsed;
    uint64_t elapsedNano;
    static mach_timebase_info_data_t    sTimebaseInfo;
    double total_time;
    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }
    
    const char messageChild[] = "C";
    const char messagePar[]="P";
    for(int i=0;i<itera;i++){
        uint64_t start;
        uint64_t end;
        int pipe1[2];
        int pipe2[2];
        pid_t pid;
        pipe(pipe1);
        pipe(pipe2);

        
         /*
          
          p |write pipe1  read  |c
          p |read  pipe2  write |c
          */
        if((pid=fork())==-1){
            perror("fork error");
            exit(1);
        }
        if(pid==0){
            //child
            /* Child process closes up write side of pipe1 */
            /* Child process closes up read side of pipe2 */
            close(pipe1[writein]);
            close(pipe2[readout]);
            char holder[20]="";
            read(pipe1[readout],holder,sizeof(holder));
            write(pipe2[writein], messageChild, sizeof(messageChild));
            //kill(0,SIGSTOP);
            exit(0);
            
        }
        else{
            //parent
            //printf("%d\n",pid);
            /* Parent process closes up write side of pipe2 */
            /* Parent process closes up read side of pipe1 */
            close(pipe2[writein]);
            close(pipe1[readout]);
            char holder[20]="";
            start = mach_absolute_time();
            write(pipe1[writein], messagePar, sizeof(messagePar));
            read(pipe2[readout],holder,sizeof(holder));
            end = mach_absolute_time();
            wait(0);
            
        }
        elapsed = end - start;
        elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
        total_time += elapsedNano;
    }
    double average = total_time/itera;
    return average/2; //two way need to divide two

}
double contextswitch_time_one_pipe(int itera){

    uint64_t elapsed;
    uint64_t elapsedNano;
    double average = 0.0;
    static mach_timebase_info_data_t    sTimebaseInfo;
    double total_time;
    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }
    const char message[]="p";
    for(int i=0;i<itera;i++){
        uint64_t start;
        uint64_t end;
        int pipe1[2];
        pipe(pipe1);

        char holder[20]="";
        start = mach_absolute_time();
        write(pipe1[writein], message, sizeof(message));
        read(pipe1[readout],holder,sizeof(holder));
        end=mach_absolute_time();
        elapsed = end - start;
        elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
        total_time += elapsedNano;
        
    }
    average = total_time/itera;
    return average;
}
double contextswitch(int itera){
    return contextswitch_time_two_pipe(itera)-contextswitch_time_one_pipe(itera);
}


int main(int argc, const char * argv[]) {

    /*if ( argc < 2 ) {
        printf("Usage: ./main $ITERATIONS\n");
        exit(0);
    }*/
    //unsigned long int itera = strtoul(argv[1], NULL, 0);
    int itera=100000;
    double overhead = 0.0;
    /* Measurement Overhead */
     //overhead = readtime(itera);
     //printf("%lf nsec\n", overhead); //shouldn't use printf IO operation slow slow
   // overhead= loopreadtime(itera);
   // printf("Measurement Overhead %lf nsec\n", overhead);
    
    /* Procedure Call Overhead */
    /*for (int para_num=0; para_num<=8; para_num++) {
        overhead =procedure_overhead(itera,para_num);
        printf("Procedure Call Overhead %lf nsec\n", overhead);
    }*/
   

    /* System Call Overhead */
    //overhead = systemcall_overhead(itera);
  /*  for(int i=0;i<100000;i++){
        overhead+=systemcall_overhead_single_call(0);
    }
     printf("System Call Overhead %lf nsec\n", overhead);*/

    /* Process Creation Time */
    overhead = process_creation_time(itera);
    printf("%lf nsec\n", overhead);

    /* Kernel Thread Creation Time */
  //  overhead = pthread_creation_time(itera);
  //  printf("%lf nsec\n", overhead);

    /* Context Switch Time */
    
    //!!!!!missing thread swiching!?!?!?!?!
   // overhead = contextswitch(itera);
  //  printf("Context Switch Time %lf nsec\n", overhead);

    return 0;
}

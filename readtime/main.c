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
        start=mach_absolute_time();
        end= mach_absolute_time();
        elapsed = end - start;
        uint64_t elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
        total_time += elapsedNano;
    }
    double average= total_time/itera;
    
    return average;
}

static double loopreadtime(int itera){// not done
    uint64_t start;
    uint64_t end;
    uint64_t elapsed;
    static mach_timebase_info_data_t    sTimebaseInfo;
    double total_time;
    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }
    
    
    for(int i=0;i<itera;i++){
        start=mach_absolute_time();
        for (int i=0; i<10000; i++) {
        }
        end= mach_absolute_time();
        elapsed = end - start;
        uint64_t elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
        total_time += elapsedNano;
    }
    double average= total_time/itera;
    
    return average;
    return 0; 
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
    double total_time;
    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }
    for (int i=0; i<itera; i++) {
        switch (para_num) {
            case 0:
                start=mach_absolute_time();
                zero();
                end= mach_absolute_time();
                break;
            case 1:
                start=mach_absolute_time();
                one(0);
                end= mach_absolute_time();
                break;
            case 2:
                start=mach_absolute_time();
                two(0, 0);
                end= mach_absolute_time();
                break;
            case 3:
                start=mach_absolute_time();
                three(0, 0, 0);
                end= mach_absolute_time();
                break;
            case 4:
                start=mach_absolute_time();
                four(0, 0, 0, 0);
                end= mach_absolute_time();
                break;
            case 5:
                start=mach_absolute_time();
                five(0, 0, 0, 0, 0);
                end= mach_absolute_time();
                break;
            case 6:
                start=mach_absolute_time();
                six(0, 0, 0, 0, 0, 0);
                end= mach_absolute_time();
                break;
            case 7:
                start=mach_absolute_time();
                seven(0, 0, 0, 0, 0, 0, 0);
                end= mach_absolute_time();
                break;

            
            default:
                start=0;
                end=0;
                break;
        }
        elapsed = end - start;
        uint64_t elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
        total_time += elapsedNano;
    }
    double average= total_time/itera;
    return average;
}

double systemcall_overhead(unsigned long int itera){

    uint64_t start;
    uint64_t end;
    uint64_t elapsed;
    uint64_t elapsedNano;
    double average = 0.0;
    static mach_timebase_info_data_t    sTimebaseInfo;
    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }

    /* Toggling the gid doesn't work.
        The first call is much longer. Perhaps try to
        (1) fork a new process on each iteration or
        (2) use shell script. */
    gid_t orig_gid = 20;
    gid_t toggle_gid;

    unsigned long int i;
    for (i=0; i<itera; i++) {
        start = mach_absolute_time();
        (void) getgid();
        end = mach_absolute_time();
        elapsed = end - start;
        elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
        average += elapsedNano;

        toggle_gid = toggle_gid!=0 ? 0 : orig_gid;
        setgid(toggle_gid);
        // printf("set:%d, get:%d\n",setgid(toggle_gid),getgid());
    }

    average = average/itera;
    return average;
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
        start = mach_absolute_time();
        child_pid = fork();
        end = mach_absolute_time();

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

    printf("itera ran: %lu\n", itera);
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
    uint64_t elapsedNano;
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
        elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
        average += elapsedNano;

        /* To avoid congestion, join the thread. */
        void *status;
        (void) pthread_join(pthread, &status);

    }

    printf("itera ran: %lu\n", itera);
    // printf("average: %lf\n", average);
    average = average/itera;
    return average;
}

double contextswitch_time(){
    return 0;
}

int main(int argc, const char * argv[]) {

    if ( argc < 2 ) {
        printf("Usage: ./main $ITERATIONS\n");
        exit(0);
    }
    unsigned long int itera = strtoul(argv[1], NULL, 0);
    double overhead = 0.0;

    /* Measurement Overhead */
    // overhead = readtime(itera);
    // printf("%lf nsec\n", overhead); //shouldn't use printf IO operation slow slow

    /* Procedure Call Overhead */

    /* System Call Overhead */
    // overhead = systemcall_overhead(itera);
    // printf("%lf nsec\n", overhead);

    /* Process Creation Time */
    // overhead = process_creation_time(itera);
    // printf("%lf nsec\n", overhead);

    /* Kernel Thread Creation Time */
    // overhead = pthread_creation_time(itera);
    // printf("%lf nsec\n", overhead);

    /* Context Switch Time */
    overhead = context_switch_time(itera);
    printf("%lf nsec\n", overhead);

    return 0;
}
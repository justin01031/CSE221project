//
//  main.c
//  memorytime
//
//  Created by justin01031 on 2/15/16.
//  Copyright © 2016 justin01031. All rights reserved.
//

#include <stdio.h>
<<<<<<< HEAD
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <mach/thread_policy.h>
#include "timing.c"
#define MIN_STRIDE 64
#define MAX_STRIDE 1024
#define FLUSH_POWER 28
#define MIN_POWER 10
#define MAX_POWER 26
#define MEMORY_LIMIT 8000

double nano_to_cycle(double nano_sec) {
    return nano_sec*2.7;
}

char* random_char_array(unsigned long long entries) {
	srand((unsigned int)time(NULL));
	unsigned long long i = 0;
    unsigned long long arraySize = entries * sizeof(char);
    char* array = (char*)malloc(arraySize);
    for (i=0; i<entries; i++) {
        *(array+i) = rand()%26+'a';
    }
    return array;
}

void flush_cache() {
	char *char_array = random_char_array(pow((float)2, (int)FLUSH_POWER));
    free(char_array);
}

double mem_access_time(unsigned long int itera) {

    uint64_t start;
    uint64_t end;
    uint64_t elapsed;
    double elapsedNano;
    double totalNano;
    static mach_timebase_info_data_t    sTimebaseInfo;
    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }

    int array_stride = 0;
    for (array_stride=MIN_STRIDE; array_stride<=MAX_STRIDE; array_stride*=2) {

        int power = 0;
    	for (power=MIN_POWER; power<=MAX_POWER; power++) {

            long int array_entries = pow((float)2, power);
            char* char_array = random_char_array(array_entries);
            int* int_array = (int*)char_array;
            long int location = 0;
            unsigned long int i = 0;
            
            totalNano = 0.0;
            for (i=0; i<itera; i++) {

                // start = mach_absolute_time();
                // end = mach_absolute_time();
                // elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
                // totalNano -= elapsedNano;
                start = rdtsc();
                end = rdtsc();
                elapsed = end - start;
                totalNano -= elapsed;

                // start = mach_absolute_time();
                start = rdtsc();
                int tmp = int_array[location];
                // end = mach_absolute_time();
                end = rdtsc();
                elapsed = end - start;
                totalNano += elapsed;
                // elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
                // totalNano += elapsedNano;

                location += array_stride/sizeof(int);
                location = location % (array_entries/sizeof(int));
            }
     
            // printf("stride = %d, size = 2^%d, time = %lf, cycles = %lf\n", array_stride, power, totalNano/itera, nano_to_cycle(totalNano/itera));
            printf("stride = %d, size = 2^%d, cycles = %lf\n", array_stride, power, totalNano/itera);
            free(char_array);
            flush_cache();
        }
    }
    return 0.0;
}

double page_fault_service_time(unsigned long int itera) {

    uint64_t start;
    uint64_t end;
    uint64_t elapsed;
    double totalCycles;

    unsigned long i = 0;
    unsigned long entries = 1024 * 1024;
    char* arrays[MEMORY_LIMIT];
    for(i=0; i<MEMORY_LIMIT; i++) {
        arrays[i] = random_char_array(entries);
    }
 
    totalCycles = 0.0;
    for(i=0; i<entries; i+=4096){

        start = rdtsc();
        end = rdtsc();
        totalCycles -= end - start;

        start = rdtsc();
        char tmp = arrays[0][i];// + arrays[0][i];
        end = rdtsc();
        totalCycles += end - start;
    }
    // printf("%lf cycles avg from %d iterations.\n", totalCycles/(entries/4096), (int)entries/4096);
    printf("%lf cycles from %d iterations.\n", totalCycles, (int)entries/4096);
 
 
    for(i=0; i<MEMORY_LIMIT; i++) {
        free(arrays[i]);
    }
    return 0.0;

}

double readBandwidthTime(){
    return 0;
}
double writeBandwidthTime(){
    return 0;
}
int main(int argc, const char * argv[]) {
    
    /* Thread Affinity */
    thread_affinity_policy_data_t policy;
    policy.affinity_tag = 0;
    thread_policy_set(mach_thread_self(),
              THREAD_AFFINITY_POLICY,
              (thread_policy_t)&policy,
              THREAD_AFFINITY_POLICY_COUNT);

    
    unsigned long int itera = strtoul(argv[1], NULL, 0);    
    double measured_time = 0.0;

    /* Memory Access Time */
    flush_cache();
    measured_time = mem_access_time(itera);

    /* Page Fault Servicing Time */
    // flush_cache();
    // measured_time = page_fault_service_time(itera);

    return 0;
}

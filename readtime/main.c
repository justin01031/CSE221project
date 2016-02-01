//
//  main.c
//  readtime
//
//  Created by justin01031 on 1/31/16.
//  Copyright © 2016 justin01031. All rights reserved.
//

#include <stdio.h>
#include <mach/mach.h>
#include <mach/mach_time.h>

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
static double loopreadtime(double itera){// not done
   /* uint64_t start;
    uint64_t end;
    uint64_t elapsed;
    static mach_timebase_info_data_t    sTimebaseInfo;
    double total_time;
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
    
    return average;*/
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

int main(int argc, const char * argv[]) {

    double a=readtime(10);
    printf("%f",a);
    return 0;
}
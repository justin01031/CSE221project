#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include <mach/mach.h>
#include <mach/mach_time.h>
#include <mach/thread_policy.h>

#define NANOS_PER_SECF 1000000000.0
#define USECS_PER_SEC 1000000
#define R 5000000
#define B __asm__ __volatile__("" ::: "memory");

static uint64_t rdtsc_per_sec = 0;

static inline uint64_t rdtsc() {
	uint32_t hi, lo;
	asm volatile("rdtscp\n"
				 "movl %%edx, %0\n"
				 "movl %%eax, %1\n"
				 "cpuid"
				 : "=r" (hi), "=r" (lo) : : "%rax", "%rbx", "%rcx", "%rdx");
	return (((uint64_t)hi) << 32) | (uint64_t)lo;
}

static void __attribute__((constructor)) init_rdtsc_per_sec() {
	uint64_t before, after;
	before = rdtsc();
	usleep(USECS_PER_SEC);
	after = rdtsc();
	rdtsc_per_sec = after - before;
}

static double monotonic_seconds() {
	return (double) rdtsc() / (double) rdtsc_per_sec;
}

static double nano_to_cycle(long double nano_sec) {
	return nano_sec*2.7;
}

static double cycle_to_nano(uint64_t cycles) {
	return cycles/2.7;
}

static double nano_to_milli(long double nano_sec) {
	return nano_sec*pow(10.0,-9)/pow(10.0,-3);
}

static double nano_to_micro(long double nano_sec) {
	return nano_sec*pow(10.0,-9)/pow(10.0,-6);
}

static double nano_to_sec(long double nano_sec) {
	return nano_sec*pow(10.0,-9);
}

static double byte_to_mb(int bytes) {
	return bytes/pow(10.0,6);
}
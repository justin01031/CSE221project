#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>

#include <mach/mach_time.h>
#include <mach/mach.h>
#include <mach/thread_policy.h>

#define NANOS_PER_SECF 1000000000.0
#define USECS_PER_SEC 1000000
#define R 5000000

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
double monotonic_seconds() {
	return (double) rdtsc() / (double) rdtsc_per_sec;
}		
/*
int main(int argc, char* argv[]) {

	thread_affinity_policy_data_t policy;
	policy.affinity_tag = 0;
	thread_policy_set(mach_thread_self(),
			  THREAD_AFFINITY_POLICY,
			  (thread_policy_t)&policy,
			  THREAD_AFFINITY_POLICY_COUNT);

	uint64_t s, e, a, amt;
	uint64_t i, r;

	s = rdtsc();
	for (i = 0; i < R; i++) {
		r = mach_absolute_time();
		__asm__ __volatile__("" ::: "memory");
		r = mach_absolute_time();
		__asm__ __volatile__("" ::: "memory");
		r = mach_absolute_time();
		__asm__ __volatile__("" ::: "memory");
		r = mach_absolute_time();
		__asm__ __volatile__("" ::: "memory");
	}
	e = rdtsc();
	amt = (e - s) / (R * 4);

	s = rdtsc();
	for (i = 0; i < R; i++) {
		__asm__ __volatile__("movl $1, %%eax; cpuid" ::: "%eax", "%edx", "%ecx", "%ebx", "memory");
		__asm__ __volatile__("movl $1, %%eax; cpuid" ::: "%eax", "%edx", "%ecx", "%ebx", "memory");
		__asm__ __volatile__("movl $1, %%eax; cpuid" ::: "%eax", "%edx", "%ecx", "%ebx", "memory");
		__asm__ __volatile__("movl $1, %%eax; cpuid" ::: "%eax", "%edx", "%ecx", "%ebx", "memory");
	}
	e = rdtsc();
	a = (e - s) / (R * 4);

	printf("  a: %" PRIu64 "\n", a);
	printf("  mach_absolute_time: %12f (%" PRIu64 " / %" PRIu64 ")\n", (double)amt / (double)a, amt, a);


}*/
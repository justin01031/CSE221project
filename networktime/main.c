#include <mach/thread_policy.h>
#include "network.h"

int main(int argc, const char * argv[]) {
	
	/* Thread Affinity */
	thread_affinity_policy_data_t policy;
	policy.affinity_tag = 0;
	thread_policy_set(mach_thread_self(),
				 THREAD_AFFINITY_POLICY,
				 (thread_policy_t)&policy,
				 THREAD_AFFINITY_POLICY_COUNT);

	long double measuredTime;
	unsigned long int itera = strtoul(argv[1], NULL, 0);

	/* Round-Trip Time */
	// measuredTime = peak_bandwidth_time(itera, 64-16);
	measuredTime = round_trip_time(itera);
	printf("Round-trip time: %Lf ns\n", measuredTime/itera);

	/* Overhead */
	measuredTime = connect_time(itera);
	printf("Connect overhead: %Lf ns\n", measuredTime/itera);
	measuredTime = close_time(itera);
	printf("Teardown overhead: %Lf ns\n", measuredTime/itera);

	/* Peak Bandwidth */
	unsigned int msg_size = BUFFER_SIZE-100;
	measuredTime = peak_bandwidth_time(itera, msg_size);
	printf("Bytes: %lu\n", itera*msg_size);
	printf("Nanoseconds: %Lf\n", measuredTime);
	printf("Peak bandwidth: %lf MB/s\n", byte_to_mb(itera*msg_size)/nano_to_sec(measuredTime));

	return 0;
}

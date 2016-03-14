#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <mach/thread_policy.h>
#include "timing.c"

#define MIN_STRIDE 16
#define MAX_STRIDE 2048
#define FLUSH_POWER 28
#define MIN_POWER 10
#define MAX_POWER 26
#define MEMORY_LIMIT 8000
#define CACHE_LINE 64



char* random_char_array(unsigned long long entries) {

	srand((unsigned int)time(NULL));
	unsigned long long i = 0;
	char* array = (char*)malloc(sizeof(char)*entries);

	for (i=0; i<entries; i++) {
		*(array+i) = rand()%26+'a';
	}
	return array;
}

void flush_cache() {
	char *charArray = random_char_array(pow((float)2, (int)FLUSH_POWER));
	free(charArray);
}

double mem_access_time(unsigned long int itera) {

	uint64_t start;
	uint64_t end;
	double totalCycles;

	int array_stride = 0;
	for (array_stride=MIN_STRIDE; array_stride<=MAX_STRIDE; array_stride*=2) {

		int power = 0;
		for (power=MIN_POWER; power<=MAX_POWER; power++) {

			long int array_entries = pow((float)2, power);
			char* charArray = random_char_array(array_entries);
			int* int_array = (int*)charArray;
			long int location = 0;
			unsigned long int i = 0;
			
			totalCycles = 0.0;
			for (i=0; i<itera; i++) {

				start = rdtsc();
				end = rdtsc();
				totalCycles -= end - start;

				start = rdtsc();
				int tmp = int_array[location];
				end = rdtsc();
				totalCycles += end - start;

				location += array_stride/sizeof(int);
				location = location % (array_entries/sizeof(int));
			}
	 
			// printf("stride = %d, size = 2^%d, time = %lf, cycles = %lf\n", array_stride, power, totalCycles/itera, nano_to_cycle(totalCycles/itera));
			// printf("stride = %d, size = 2^%d, cycles = %lf\n", array_stride, power, totalCycles/itera);
			printf("%d %d %lf\n", array_stride, power, totalCycles/itera);
			free(charArray);
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

	/* Mmap Variables */
	char *addr;
	int fd;
	off_t offset;
	size_t length;
	ssize_t s;

	/* Open large file */
	fd = open("./8gfile", O_RDONLY);
	offset = 0;
	length = 8589934592;
	printf("length+offset: %llu\n", length+offset);
	addr = mmap(NULL, length + offset, PROT_READ,
                       MAP_PRIVATE, fd, offset);
	if (addr == MAP_FAILED) {
		printf("mmap failed\n");
	}
	// char buffer[256];
	// strncpy(buffer, addr, 10);
	// printf("%s\n", buffer);
	
 	unsigned long int i, j;
 	char tmp;
	totalCycles = 0.0;
	unsigned long int page_size = (unsigned long int)sysconf(_SC_PAGE_SIZE);
	unsigned long int page_end = (unsigned long int)length;
	printf("sysconf(_SC_PAGE_SIZE): %lu\n",page_size);
	printf("page_end: %lu\n",page_end);
	for (i=0; i<itera; i++) {
		for (j=0; j<page_end; j+=page_size) {

			// printf("(i,j)= (%lu,%lu)\n", i, j);

			start = rdtsc();
			end = rdtsc();
			totalCycles -= end - start;

			start = rdtsc();
			tmp = addr[j];
			end = rdtsc();
			totalCycles += end - start;

		}
	}
	printf("%lf cycles from %lu iterations.\n", totalCycles, itera);
	printf("%lf cycles per iteration.\n", totalCycles/itera);
	printf("%lu faults per iteration.\n", page_end/page_size);
	printf("%lf cycles per page fault.\n", totalCycles/itera/(page_end/page_size));
 
	munmap(addr, length);
	return 0.0;
}

double readBandwidthTime(unsigned int arraySizeP){

    unsigned int entries =pow(2,arraySizeP);
    char *testArray = random_char_array(entries);
    flush_cache();
    uint64_t start;
    uint64_t end;
    uint64_t elapsed;
    start= rdtsc();

    for (unsigned int i = 0; i < entries; i +=CACHE_LINE ) {
        char now = testArray[i];
    }
    end= rdtsc();
    free(testArray);
    elapsed=end-start;
    double bandwidthInBytePerNSec = (double)entries / (elapsed*0.417);
    double bandwidthInMBytePerSec = bandwidthInBytePerNSec * pow(10, 9) / pow(2, 20);
    return bandwidthInMBytePerSec;
}

double writeBandwidthTime(unsigned int arraySizeP){
    unsigned int entries = pow(2,arraySizeP);
    char *testArray = random_char_array(entries);
    flush_cache();
    uint64_t start;
    uint64_t end;
    uint64_t elapsed;
    char word='a';
   

    start= rdtsc();

    for (unsigned int i = 0; i < entries; i +=CACHE_LINE ) {
         testArray[i]=word;
    }
    end= rdtsc();
    
    
    free(testArray);
    elapsed=end-start;
    double bandwidthInBytePerNSec = (double)entries / (elapsed*0.417);
    double bandwidthInMBytePerSec = bandwidthInBytePerNSec * pow(10, 9) / pow(2, 20);
    return bandwidthInMBytePerSec;

}

void testArraySizeRead(int itera,int minSize,int maxSize){
    for(unsigned int arraySizeP=minSize;arraySizeP<=maxSize;arraySizeP++){
        double bandWidthTime=0;
        for(int i=0;i<itera;i++){
            bandWidthTime+=readBandwidthTime(arraySizeP);
        }
        printf("bandwidth of %lf MB: %lf Mbytes/sec\n",pow(2,arraySizeP-20),bandWidthTime/itera);
    }
    printf("write=========\n");


}

void testArraySizeWrite(int itera,int minSize,int maxSize){

    printf("write=========\n");
     for(unsigned int arraySizeP=minSize;arraySizeP<=maxSize;arraySizeP++){
         double bandWidthTime=0;
         for(int i=0;i<itera;i++){
             bandWidthTime+=writeBandwidthTime(arraySizeP);
         }
         printf("bandwidth of %lf MB: %lf Mbytes/sec\n",pow(2,arraySizeP-20),bandWidthTime/itera);
     }
  
}

int main(int argc, const char * argv[]) {
    
    /* Thread Affinity */
    thread_affinity_policy_data_t policy;
    policy.affinity_tag = 0;
    thread_policy_set(mach_thread_self(),
              THREAD_AFFINITY_POLICY,
              (thread_policy_t)&policy,
              THREAD_AFFINITY_POLICY_COUNT);

    
 //   unsigned long int itera = strtoul(argv[1], NULL, 0);
    unsigned long int itera=100000;
    double measuredTime = 0.0;

    /* Memory Access Time */
	// flush_cache();
	// measuredTime = mem_access_time(itera);

	/* Memory BandWidth */
    // testArraySizeRead(itera, 21, 30);
    // testArraySizeWrite(itera, 21, 30);
    
	/* Page Fault Servicing Time */
	flush_cache();
	measuredTime = page_fault_service_time(itera);

	return 0;
}

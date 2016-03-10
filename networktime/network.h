#ifndef NAME_H
#define NAME_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <netdb.h>
#include <netinet/in.h>

#include "timing.c"

#define HOST_ADDR "127.0.0.1"
#define SERVER_PORT 3490
#define BUFFER_SIZE 32768

char* random_char_array(unsigned long long entries);

double client(char* msg, int msg_len);

double client_connect_overhead();

double client_close_overhead();

double round_trip_time(unsigned long int itera);

double peak_bandwidth_time(unsigned long int itera, unsigned int msg_size);

double connect_time(unsigned long int itera);

double close_time(unsigned long int itera);

#endif

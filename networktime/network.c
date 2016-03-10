#include "network.h"

char* random_char_array(unsigned long long entries) {

	srand((unsigned int)time(NULL));
	unsigned long long i = 0;
    char* array = (char*)malloc(sizeof(char)*entries);

    for (i=0; i<entries; i++) {
        *(array+i) = rand()%26+'a';
    }
    return array;
}

double client(char* msg, int msg_len) {

	uint64_t start;
	uint64_t end;
	uint64_t totalCycles = 0;

	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char* write_buffer = (char*)malloc(BUFFER_SIZE);
	char* read_buffer = (char*)malloc(BUFFER_SIZE);
	
	/* Create a socket point */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}
	
	/* Specify server (Loopback or Remote) */
	portno = SERVER_PORT;
	server = gethostbyname(HOST_ADDR);
	
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	
	/* Now connect to the server */
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR connecting");
		exit(1);
	}

	/* Prepare random message */
	bzero(write_buffer, BUFFER_SIZE);
	strncpy(write_buffer, msg, sizeof(char)*msg_len);

	start = rdtsc();
	end = rdtsc();
	totalCycles -= end - start;

	/* Send message to the server */
	start = rdtsc();
	n = write(sockfd, write_buffer, strlen(write_buffer)); //Supposed to be blocking write
	end = rdtsc();
	totalCycles += end - start;

	if (n < 0) {
		perror("ERROR writing to socket");
		exit(1);
	}
	else {
		// printf("Random Message: %s\n", write_buffer);
		// printf("strlen(write_buffer): %d\n", (int)strlen(write_buffer));
		// printf("Bytes sent: %d\n", n);
	}
	
	/* Now read server response */
	bzero(read_buffer,BUFFER_SIZE);
	n = read(sockfd, read_buffer, BUFFER_SIZE-1);
	
	if (n < 0) {
		perror("ERROR reading from socket");
		exit(1);
	}
	else {
		// printf("Server Reply: %s \n",read_buffer);
	}
	
	close(sockfd);
	return cycle_to_nano(totalCycles);

}

double client_connect_overhead() {

	uint64_t start;
	uint64_t end;
	uint64_t totalCycles = 0;

	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char* write_buffer = (char*)malloc(BUFFER_SIZE);
	char* read_buffer = (char*)malloc(BUFFER_SIZE);
	
	/* Create a socket point */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}
	
	/* Specify server (Loopback or Remote) */
	portno = SERVER_PORT;
	server = gethostbyname(HOST_ADDR);
	
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	
	start = rdtsc();
	end = rdtsc();
	totalCycles -= end - start;

	/* Now connect to the server */
	start = rdtsc();
	n = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	end = rdtsc();
	totalCycles += end - start;

	if (n < 0) {
		perror("ERROR connecting");
		exit(1);
	}
	
	close(sockfd);

	return cycle_to_nano(totalCycles);
}

double client_close_overhead() {

	uint64_t start;
	uint64_t end;
	uint64_t totalCycles = 0;

	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char* write_buffer = (char*)malloc(BUFFER_SIZE);
	char* read_buffer = (char*)malloc(BUFFER_SIZE);
	
	/* Create a socket point */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}
	
	/* Specify server (Loopback or Remote) */
	portno = SERVER_PORT;
	server = gethostbyname(HOST_ADDR);
	
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	/* Now connect to the server */
	n = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	if (n < 0) {
		perror("ERROR connecting");
		exit(1);
	}

	start = rdtsc();
	end = rdtsc();
	totalCycles -= end - start;
	
	start = rdtsc();
	close(sockfd);
	end = rdtsc();
	totalCycles += end - start;

	return cycle_to_nano(totalCycles);

}

double round_trip_time(unsigned long int itera) {

	double totalNano = 0.0;
	char* charArray = random_char_array(itera);

	for(int i=0; i<itera; i++) {
		totalNano += client(charArray+i, 1);
	}
	return totalNano;
}

double connect_time(unsigned long int itera) {

	double totalNano = 0.0;

	for(int i=0; i<itera; i++) {
		totalNano += client_connect_overhead();
	}
	return totalNano;	
}

double close_time(unsigned long int itera) {

	double totalNano = 0.0;

	for(int i=0; i<itera; i++) {
		totalNano += client_close_overhead();
	}
	return totalNano;	
}

double peak_bandwidth_time(unsigned long int itera, unsigned int msg_size) {

	double totalNano = 0.0;
	char* charArray = random_char_array(itera*msg_size);

	for(int i=0; i<itera; i++) {
		totalNano += client(charArray+i*msg_size, msg_size);
		usleep(USECS_PER_SEC);
	}
	return totalNano;
}

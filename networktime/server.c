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

	/* Socket Variables */
	int sockfd, newsockfd, portno, clilen;
	char* buffer = (char*)malloc(BUFFER_SIZE);
	struct sockaddr_in serv_addr, cli_addr;
	int n, pid;

	/* First call to socket() function */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}
	
	/* Initialize socket structure */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = SERVER_PORT;
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	
	/* Bind the host address using bind() call.*/
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(1);
	}
	
	/* Now start listening for the clients, here
		* process will go in sleep mode and will wait
		* for the incoming connection
	*/

	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	bzero(buffer,BUFFER_SIZE);
	
	while (1) {

		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		
		// if (newsockfd < 0) {
		// 	perror("ERROR on accept");
		// 	exit(1);
		// }

		n = read(newsockfd,buffer,BUFFER_SIZE-1);
		
		// if (n < 0) {
		// 	perror("ERROR reading from socket");
		// 	exit(1);
		// }
		// else {
			// printf("Bytes read: %d\n", n);
		// }
		
		// printf("Here is the message: %s\n",buffer);
		// n = write(sockfd,"I got your message",18);
		n = write(newsockfd, "Y", 1);
		
		// if (n < 0) {
		// 	perror("ERROR writing to socket");
		// 	exit(1);
		// }
		// else {
			// printf("Bytes written: %d\n", n);
		// }

		close(newsockfd);

		bzero(buffer,BUFFER_SIZE);
		
		/* Create child process */
		/*pid = fork();
		
		if (pid < 0) {
			perror("ERROR on fork");
			exit(1);
		}
		
		if (pid == 0) {*/
			/* This is the child process hosting the client */
			/*close(sockfd);
			doprocessing(newsockfd);
			exit(0);
		}
		else {
			close(newsockfd);
		}*/
		
	} /* end of while */

	return 0;
}

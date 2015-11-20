#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>

#define BUF_SIZE 256

#define PORT 43210

int main(void)
{
	int ret;
	int sockfd;
	struct sockaddr_in servaddr, cliaddr;
	char sendstr[BUF_SIZE], recvstr[BUF_SIZE];

	/*
	 * Create socket
	 */
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror("socket");
		return -1;
	}

	/*
	 * Set client socket addr and bind it to socket
	 */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = bind(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
	if (ret < 0) {
		perror("bind");
		/*
		 * Go to label "err" to cleanup allocated objects
		 */
		goto err;
	}

	while (1) {
		int clilen;

		ret = recvfrom(sockfd, recvstr, BUF_SIZE, 0,
			       (struct sockaddr *) &cliaddr, &clilen);
		if (ret < 0) {
			perror("recvfrom");
			goto err;
		}

		printf("received: %s\n", recvstr);

		ret = sendto(sockfd, recvstr, strlen(recvstr) + 1, 0,
			     (struct sockaddr*) &cliaddr, clilen);
		if (ret < 0) {
			perror("sendto");
			goto err;
		}
	}
	
err:
	close(sockfd);
	return ret;
}

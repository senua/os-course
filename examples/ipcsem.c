#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>

/* ftok */
#define IPCSEM "ipcsem"
#define IPCSEM_INDEX 0
/* semget */
#define SEM_COUNT 2
#define IPCSEM_CREATE_FLAG IPC_CREAT|IPC_EXCL
#define IPCSEM_ACCESS_RIGHTS 0666
/* semop */
#define FIRST_SEM 0
#define SECOND_SEM 1

#define SEM_OP_FLAG 0
#define SEM_OP_SIZE 1

#define SEM_OP_DEC -1
#define SEM_OP_INC 1
/* semctl */
#define UNUSED 0

int semop_helper(int semid, int sem_num, int sem_op)
{
	int ret;
	struct sembuf semops;
	semops.sem_num = sem_num;
	semops.sem_op = sem_op;
	semops.sem_flg = SEM_OP_FLAG;

	ret = semop(semid, &semops, SEM_OP_SIZE);
	if (ret == -1) {
		perror("Failed semapthore operation");
		return -1;
	}
	return 0;
}

int main()
{
	int ret, semid, pid;
	key_t ipcsem_key;

	ipcsem_key = ftok(IPCSEM, IPCSEM_INDEX);
	if (ipcsem_key == -1) {
		perror("Failed to allocate ipcsem key");
		return 1;
	}

	semid = semget(ipcsem_key, SEM_COUNT,
	             IPCSEM_CREATE_FLAG | IPCSEM_ACCESS_RIGHTS);
	if (semid == -1) {
		perror("Failed to create new semapthore array");
		return 1;
	}

	/* Initialize semapthores */
	ret = semop_helper(semid, FIRST_SEM, SEM_OP_INC);
	if (ret == -1)
		return 1;

	ret = semop_helper(semid, SECOND_SEM, SEM_OP_INC);
	if (ret == -1)
		return 1;

	pid = fork();
	if (pid == -1) {
		perror("Failed to create new process");
		return 1;
	} else if (pid == 0) {
		/* Child */
		ret = semop_helper(semid, FIRST_SEM, SEM_OP_DEC);
		if (ret == -1)
			return 1;

		printf("Child got semaphore %d\n", FIRST_SEM);

		ret = semop_helper(semid, SECOND_SEM, SEM_OP_DEC);
		if (ret == -1)
			return 1;

		printf("Child got semaphore %d\n", SECOND_SEM);

		/* Got both semapthores here in Child */

		ret = semop_helper(semid, SECOND_SEM, SEM_OP_INC);
		if (ret == -1)
			return 1;

		printf("Child released semaphore %d\n", SECOND_SEM);

		ret = semop_helper(semid, FIRST_SEM, SEM_OP_INC);
		if (ret == -1)
			return 1;

		printf("Child released semaphore %d\n", FIRST_SEM);
	} else {
		/* Parent */
		ret = semop_helper(semid, SECOND_SEM, SEM_OP_DEC);
		if (ret == -1)
			return 1;

		printf("Parent got semaphore %d\n", SECOND_SEM);

		ret = semop_helper(semid, FIRST_SEM, SEM_OP_DEC);
		if (ret == -1)
			return 1;

		printf("Parent got semaphore %d\n", FIRST_SEM);

		/* Got both semapthores here in Parent */

		ret = semop_helper(semid, FIRST_SEM, SEM_OP_INC);
		if (ret == -1)
			return 1;

		printf("Parent released semaphore %d\n", FIRST_SEM);

		ret = semop_helper(semid, SECOND_SEM, SEM_OP_INC);
		if (ret == -1)
			return 1;

		printf("Parent released semaphore %d\n", SECOND_SEM);

		/* Wait for child and finish */
		pid = wait(&ret);
		if (pid == -1) {
			perror("Failed to wait for child");
			return 1;
		}

		if (ret)
			printf("Child failed\n");
		else
			printf("Child and Parent finished successfully\n");

		ret = semctl(semid, UNUSED, IPC_RMID);
		if (ret == -1) {
			perror("Failed to remove semapthores");
			return 1;
		}
	}

	return 0;
}

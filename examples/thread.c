#include <stdio.h>
#include <pthread.h>

#define MAX 10000

void count(int *a)
{
	int i;
	for (i = 0; i < MAX; i++) {
		(*a)++;
	}
}

void *my_pthread(void* a)
{
	count((int*)a);
	pthread_exit(NULL);
}

int main()
{
	pthread_t p;
	int b = 0;
	int ret;

	ret = pthread_create(&p, NULL, my_pthread, (void*)&b);
	if (ret) {
		printf("Failed to pthread_create errno=%d\n", ret);
		return 1;
	}

	count(&b);

	ret = pthread_join(p, NULL);
	if (ret) {
		printf("Failed to pthread_join errno=%d\n", ret);
		return 1;
	}

	printf("%d\n", b);
	return 0;
}

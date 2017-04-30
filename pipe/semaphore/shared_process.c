#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/sem.h>
#include<sys/shm.h>

static int semaphore_P(void);
static int semaphore_V(void);
static int set_semvalue(void);
static int del_semaphore(void);

static int sem_id;

union semun
{
	int val;
};

struct shared_memory
{
	int temp;
};

int main()
{
	void *shared_pointer = (void *)0;
	struct shared_memory *shared_ptr;
	int shmid;
	
	sem_id = semget((key_t)1234, 1, 0666|IPC_CREAT);
	set_semvalue();
	
	shmid = shmget((key_t)123, sizeof(struct shared_memory), 0666|IPC_CREAT);
	if (shmid == -1){
		perror("shmget failed");
		exit(EXIT_FAILURE);
	}
	
	shared_pointer = shmat(shmid, (void *)0, 0);
	if (shared_pointer == (void *)-1){
		perror("shmat failed");
		exit(EXIT_FAILURE);
	}
	
	printf("memory attached at %X\n",(int *)shared_pointer);		
	
	shared_ptr = (struct shared_memory *)shared_pointer;
	
	if (shmdt(shared_pointer) == -1){
		perror("shmdt failed");
		exit(EXIT_FAILURE);
	}
	
	if(shmctl(shmid, IPC_RMID, 0) == -1){
		perror("shmdt failed");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

static int set_semvalue(void)
{
	union semun sem_union;
	
	sem_union.val = 1;
	if (semctl(sem_id, 0, SETVAL, sem_union) == -1)
		return 0;

	
	return 1;
}

static int semaphore_P(void)     // This is "wait" operation
{
	struct sembuf sem_b;
	
	sem_b.sem_num = 0;
	sem_b.sem_op = -1;
	sem_b.sem_flg = SEM_UNDO;
	if (semop(sem_id, &sem_b, 1) == -1){
		perror("semop failed");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);	
}

static int semaphore_V(void)     // This is "release" operation
{
	struct sembuf sem_b;
	
	sem_b.sem_num = 0;
	sem_b.sem_op = 1;
	sem_b.sem_flg = SEM_UNDO;
	if (semop(sem_id, &sem_b, 1) == -1){
		perror("semop failed");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);	
}

static int del_semaphore(void)
{
	union semun sem_union;	

	if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
		perror("Failed to delete semaphore");

}
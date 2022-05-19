#ifndef MESSAGESEM
#define MESSAGESEM

#include <strstream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <iostream>

using namespace std;
enum {MSGKEY=186, MAX_LEN=256, SHMSIZE=1024, SEMSIZE=4};
enum {LOCAL_TIME = 1, UTC_TIME=2, QUIT_CMD=3, ILLEGAL_CMD =4,\
 SEM_RD = 0, SEM_WR=1, SEM_R2=2, SEM_W2=3};


struct mgbuf
{
	long mtype;
	char mtext[MAX_LEN];
};


class message
{
private:
	int shmId, semId;
	struct mgbuf *msgPtr;
	enum ipc_op {RESET_SEM, CLIENT_GET_MEM, CLIENT_SND_REQ, SERVER_RCV_REQ,\
	SERVER_GET_MEM, SERVER_SND_RPY, CLIENT_RCV_RPY, SRVR_SND_PART, CLNT_RCV_PART, SM, CM};
public:
	void getsem(enum ipc_op opType)
	{
		static struct sembuf args[SEMSIZE] = { {SEM_RD}, {SEM_WR}, {SEM_R2}, {SEM_W2}};
		switch (opType) {
			case SERVER_GET_MEM:
				return;
			case CLIENT_GET_MEM:
				args[SEM_RD].sem_op = 0;
				args[SEM_WR].sem_op = -1;
				args[SEM_R2].sem_op = 0;
				args[SEM_W2].sem_op = 0;
				break;
			case CLIENT_SND_REQ:
				args[SEM_RD].sem_op = 1;
				args[SEM_WR].sem_op = 0;
				args[SEM_R2].sem_op = 0;
				args[SEM_W2].sem_op = 0;
				break;
			case SERVER_RCV_REQ:
				args[SEM_RD].sem_op = -1;
				args[SEM_WR].sem_op = 0;
				args[SEM_R2].sem_op = 0;
				args[SEM_W2].sem_op = 1;
				break;
			case SERVER_SND_RPY:
				args[SEM_RD].sem_op = 1;
				args[SEM_WR].sem_op = 1;
				args[SEM_R2].sem_op = 0;
				args[SEM_W2].sem_op = -1;
				break;
			case CLIENT_RCV_RPY:
				args[SEM_RD].sem_op = -1;
				args[SEM_WR].sem_op = -1;
				args[SEM_R2].sem_op = 0;
				args[SEM_W2].sem_op = 0;
				break;
			case SM:
				args[SEM_RD].sem_op = 0;
				args[SEM_WR].sem_op = 0;
				args[SEM_R2].sem_op = 0;
				args[SEM_W2].sem_op = -1;
				break;
			case SRVR_SND_PART:
				args[SEM_RD].sem_op = 0;
				args[SEM_WR].sem_op = 0;
				args[SEM_R2].sem_op = 1;
				args[SEM_W2].sem_op = 0;
				break;
			case CM:
				args[SEM_RD].sem_op = 0;
				args[SEM_WR].sem_op = 0;
				args[SEM_R2].sem_op = 0;
				args[SEM_W2].sem_op = 1;
				break;
			case CLNT_RCV_PART:
				args[SEM_RD].sem_op = 0;
				args[SEM_WR].sem_op = 0;
				args[SEM_R2].sem_op = -1;
				args[SEM_W2].sem_op = 0;
				break;
			case RESET_SEM:
				args[SEM_RD].sem_op = 0;
				args[SEM_WR].sem_op = 1;
				args[SEM_R2].sem_op = 0;
				args[SEM_W2].sem_op = 0;
		}
		if (semop(semId, args, SEMSIZE) == -1) 
		{
			perror("semop");
		}
	}
	
	message(int key)
	{
		if ((shmId = shmget(key, SHMSIZE, 0)) == -1)
		{
			if (errno == ENOENT)
			{
				if ((shmId = shmget(key, SHMSIZE, IPC_CREAT|0666)) == -1)
					perror("shmget");
				else if ((semId = semget(key, SEMSIZE, IPC_CREAT|0666)) == -1)
					perror("semget");
				else getsem(RESET_SEM);
			}
			else perror("shmget");
		}
		else if ((semId=semget(key, 0, 0))==-1)
			perror ("semget");
		if (shmId >= 0 && !(msgPtr=(struct mgbuf*)shmat(shmId, 0,0)))
			perror("shmat");
	}

	int good() {return (shmId >= 0 && semId>=0) ?1 :0;}
	
	int rmQ()
	{
		if (shmdt((char*)msgPtr)<0) perror("semdt");
		if (!semctl(semId, 0, IPC_RMID, 0) && !shmctl(shmId, IPC_RMID,0))
			return 0;
		perror("shmctl or semctl");
			return -1;
	}

	int send(char *buf, int size)
	{
		getsem(SERVER_GET_MEM);
		memcpy(msgPtr->mtext, buf, size);
		getsem(SERVER_SND_RPY);
		return 0;
	}

	int sendpart(const char* buf, int size)
	{
		getsem(SM);
		memcpy(msgPtr->mtext, buf, size);
		getsem(SRVR_SND_PART);
		return 0;
	}
	
	int sendClient(char* buf, int size)
	{
		getsem(CLIENT_GET_MEM);
		memcpy(msgPtr->mtext, buf, size);
		getsem(CLIENT_SND_REQ);
		return 0;
	}
	
	
	int rcv(char *buf, int size)
	{
		getsem(SERVER_RCV_REQ);
		memcpy(buf, msgPtr->mtext, strlen(msgPtr->mtext) + 1);
		return 1;
	}

	
	int rcvClient(char *buf, int size)
	{
		getsem(CLIENT_RCV_RPY);
		memcpy(buf, msgPtr->mtext, strlen(msgPtr->mtext) + 1);
		getsem(RESET_SEM);
		return 1;
	}

	int rcvpart(char *buf, int size)
	{
		getsem(CLNT_RCV_PART);
		memcpy(buf, msgPtr->mtext, strlen(msgPtr->mtext) + 1);
		getsem(CM);
		return 0;
	}


	int reset()
	{
		getsem(RESET_SEM);
		return 1;
	}
};


#endif

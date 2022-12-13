#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/shm.h>
#include <time.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>

static struct sembuf buf;

void sig(int semid, int semnum){
    buf.sem_num = semnum;
    buf.sem_op = 1;
    buf.sem_flg = 0;
    if (semop(semid, &buf, 1) == -1){
        perror("Podnoszenie semafora");
        exit(1);
    }
}

void wait(int semid, int semnum){
    buf.sem_num = semnum;
    buf.sem_op = -1;
    buf.sem_flg = 0;
    if (semop(semid, &buf, 1) == -1){
        perror("Opuszczenie semafora");
        exit(1);
    }
}

int main(void){

    int shmid, i, semid;
    char *buf;
    pid_t k1, k2;
    srand(time(NULL));

    semid = semget(45287, 3, IPC_CREAT|0600);

    if(semid == -1){
        perror("blad tworzenia tablicy semaforow");
        exit(1);
    }

    if(semctl(semid, 0, SETVAL, (int)1) ==  -1){
        perror("blad nadawania wartosci semaforowi 0");
        exit(1);
    }
    if(semctl(semid, 1, SETVAL, (int)0) ==  -1){
        perror("blad nadawania wartosci semaforowi 1");
        exit(1);
    }
    if(semctl(semid, 2, SETVAL, (int)0) ==  -1){
        perror("blad nadawania wartosci semaforowi 2");
        exit(1);
    }



    shmid = shmget(45286, sizeof(char), IPC_CREAT|0600);
    if(shmid == -1){
        perror("blad tworzenia segmentu pamieci wspoldzielonej");
        exit(1);
    }

    buf = shmat(shmid, NULL, 0);

    if(buf == NULL){
        perror("blad przylaczenia segmentu pamieci wspoldzielonej");
        exit(1);
    }

    if((k2=fork()) == 0){
        execlp("./k2_jaworski", "./k2_jaworski", (char*)NULL);
    }else if((k1=fork()) == 0){
        execlp("./k1_jaworski","./k1_jaworski", (char*)NULL);
    }else{
        char character[2];
        for(i = 0; i < 5; i++){
            wait(semid, 0);
            character[0] = 'A' + (random()%26);
            character[1] = '\0';
            strcpy(buf, character);
            printf("Proces P1  - zapisuje do pamieci wartosc %s\n", buf);
            sig(semid, 1);
        }
    }

    sleep(1);
    kill(k2, SIGINT);
    kill(k1, SIGINT);
    shmdt(buf);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 1, IPC_RMID);
    return 0;

}
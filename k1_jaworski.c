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

    semid = semget(45287, 3, IPC_CREAT|0600);
    if (semid == -1) {
        perror("blad tworzenia tablicy semaforow");
        exit(1);
    }

    shmid = shmget(45286, sizeof(char), IPC_CREAT | 0600);
    if (shmid == -1) {
        perror("blad utworzenia segmentu pamieci wspoldzielonej");
        exit(1);
    }

    buf = shmat(shmid, NULL, 0);

    if (buf == NULL) {
        perror("blad przylaczenia segmentu pamieci wspoldzielonej");
        exit(1);
    }

    for(i = 0; i < 5; i++){
        wait(semid, 2);
        printf("Proces K1  - odczytuje z pamieci wartosc %s\n", buf);
        sig(semid, 0);
    }

    sleep(1);
    shmdt(buf);

    return 0;

}
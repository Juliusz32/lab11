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
#include <pthread.h>

#define steps 10

pthread_mutex_t mp, mk1,mk2;
pthread_t p, k1,k2;
int i = 1;
//struct buf;
char buf[2];

//static struct sembuf buf;
/*
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


void *Producent(void* v){
    int slp = 0;
    char character[2];
    while(i <= steps){
        //wait(semid, 0);
        pthread_mutex_lock(&mp);
        character[0] = 'A' + (random()%26);
        character[1] = '\0';
        strcpy(buf, character);
        printf("Watek P1  - zapisuje do pamieci wartosc %s\n", buf);
        i++;
        //sig(semid, 1);
        pthread_mutex_unlock(&mk1);
    }
    pthread_exit(0);
}
*/

void *Konsument1(void* v){
    while(i <= steps){
        //wait(semid, 2);
        pthread_mutex_lock(&mk2);
        printf("Proces K1  - odczytuje z pamieci wartosc %s\n", buf);
        //sig(semid, 0);
        pthread_mutex_unlock(&mp);
    }
    pthread_exit(0);
}

void *Konsument2(void* v){
    while(i <= steps){
        //wait(semid, 2);
        pthread_mutex_lock(&mk1);
        printf("Proces K1  - odczytuje z pamieci wartosc %s\n", buf);
        //sig(semid, 0);
        pthread_mutex_unlock(&mk2);
    }
    pthread_exit(0);
}



int main(void){

    int shmid, i;
    
    //pid_t k1, k2;
    srand(time(NULL));

/*
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

*/

    shmid = shmget(45286, sizeof(char), IPC_CREAT|0600);
    if(shmid == -1){
        perror("blad tworzenia segmentu pamieci wspoldzielonej");
        exit(1);
    }

    //buf = shmat(shmid, NULL, 0);

    if(buf == NULL){
        perror("blad przylaczenia segmentu pamieci wspoldzielonej");
        exit(1);
    }
/*
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
*/

    pthread_mutex_init(&mp, NULL);
    pthread_mutex_init(&mk1, NULL);
    pthread_mutex_init(&mk2, NULL);

    pthread_mutex_lock(&mk1);
    pthread_mutex_lock(&mk2);

    //pthread_create(&p, NULL, Producent, NULL);
    pthread_create(&k1, NULL, Konsument1, NULL);
    pthread_create(&k2, NULL, Konsument2, NULL);

    char character[2];
    while(i <= steps){
        //wait(semid, 0);
        pthread_mutex_lock(&mp);
        character[0] = 'A' + (random()%26);
        character[1] = '\0';
        strcpy(buf, character);
        printf("Watek P1  - zapisuje do pamieci wartosc %s\n", buf);
        i++;
        //sig(semid, 1);
        pthread_mutex_unlock(&mk1);
    }

    //pthread_join(p, NULL);
    pthread_join(k1, NULL);
    pthread_join(k2, NULL);

    pthread_mutex_destroy(&mp);
    pthread_mutex_destroy(&mk1);
    pthread_mutex_destroy(&mk2);
    printf("\nKoncze dzialanie...\n");


    return 0;

}
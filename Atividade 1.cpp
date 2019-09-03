#include <pthread.h>
#include <iostream>
#include <stdlib.h>

using namespace std;
#define NUM_THREADS 4
//#define NUM_THREADS2 4

int matriz1 [2][2];
int matriz2 [2][2];

int matrizsoma[2][2];
int matrizmulti[2][2];

int count = 0;

void GeraMatriz(int matriz[2][2], int v1, int v2, int v3, int v4){
    matriz[0][0] = v1;
    matriz[0][1] = v2;
    matriz[1][0] = v3;
    matriz[1][1] = v4;    
}

void PrintMatrix(int matriz[2][2]){
    int x, y;
    for(x=0;x<2;x++){
        for(y=0;y<2;y++){
            printf("X:%d Y:%d Valor: %d\n", x,y, matriz[x][y]);
        }
    }
    printf("\n\n");
}

void * SomaMatriz00(void * threadid){

    matrizsoma[0][0] = matriz1[0][0] + matriz2[0][0];
    matrizmulti[0][0] = ((matriz1[0][0] * matriz2[0][0]) + (matriz1[0][1] * matriz2[1][0]));

}
void * SomaMatriz01(void * threadid){

    matrizsoma[0][1] = matriz1[0][1] + matriz2[0][1];
    matrizmulti[0][1] = ((matriz1[0][0] * matriz2[0][1]) + (matriz1[0][1] * matriz2[1][1]));

}
void * SomaMatriz10(void * threadid){

    matrizsoma[1][0] = matriz1[1][0] + matriz2[1][0];
    matrizmulti[1][0] = ((matriz1[1][0] * matriz2[0][0]) + (matriz1[1][1] * matriz2[1][0]));

}
void * SomaMatriz11(void * threadid){

    matrizsoma[1][1] = matriz1[1][1] + matriz2[1][1];
    matrizmulti[1][1] = ((matriz1[1][0] * matriz2[0][1]) + (matriz1[1][1] * matriz2[1][1]));

}

void * alo_mundo(void * threadid) {
  long tid;
  tid = (long) threadid;

  printf("Print Matriz 1\n");
  GeraMatriz(matriz1, -1, 3, 4, 2);
  PrintMatrix(matriz1);

  printf("Print Matriz 2\n");
  GeraMatriz(matriz2, 1, 2, 3, 4);
  PrintMatrix(matriz2);
  
  system("echo \"Alo\" > /dev/null");
  system("echo \"Alo\" > /dev/null");
  system("echo \"Alo\" > /dev/null");
  system("echo \"Alo\" > /dev/null");
  system("echo \"Alo\" > /dev/null");
  cout << "Alo Mundo! Thread #" << tid << '\n';
  pthread_exit(NULL);
}

void SomaMatriz(){
    pthread_t threads[NUM_THREADS];
    int rc, rv,rt, ru;
    long t;
    rc = pthread_create( & threads[1], NULL, SomaMatriz00, NULL);
    rv = pthread_create( & threads[2], NULL, SomaMatriz01, NULL);
    rt = pthread_create( & threads[3], NULL, SomaMatriz10, NULL);
    ru = pthread_create( & threads[4], NULL, SomaMatriz11, NULL);
    
       for (int j = 1; j < 5; j++)
     {
        pthread_join (threads [j], NULL);
     }
    
  printf("\n\nRESP SOMA: \n");
  printf("%d %d\n", matrizsoma[0][0], matrizsoma[0][1]);
  printf("%d %d\n", matrizsoma[1][0], matrizsoma[1][1]);

  printf("\n\nRESP MULT: \n");
  printf("%d %d\n", matrizmulti[0][0], matrizmulti[0][1]);
  printf("%d %d\n", matrizmulti[1][0], matrizmulti[1][1]);
  pthread_exit(NULL);
}

int main(int argc, char * argv[]) {
  pthread_t threads[NUM_THREADS];
  int rc;
  long t;
    rc = pthread_create( & threads[1], NULL, alo_mundo, (void * ) t);
    if (rc) {
      cout << "ERROR:" << rc << '\n';
      return(-1);
    }
    
    pthread_join (threads [1], NULL);
  SomaMatriz();
  pthread_exit(NULL);

}
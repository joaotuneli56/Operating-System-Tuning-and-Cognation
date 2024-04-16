#include <stdio.h>
#include <stdlib.h>
#include <locale.h>     // necessário para setlocale()
#include <time.h>       // necessário para time_t
#include <pthread.h>    // necessário para pthread_t
#include <unistd.h>     // necessário para usleep()


#define     NUMEROS     512
#define     THREADS     8
#define     FALSE       0
#define     TRUE        1
#define     ECHO        0
#define     LOG         0
#define     LOG_FLUSH   0
#define     IO          0


struct stNumero
{
    unsigned long long int Numeros[NUMEROS];
};

int giThreads = 0;
pthread_mutex_t mutex;
FILE *fp;
int giLog = FALSE;

void* CalcularFatorial(void*);


int main(int argc, char *argv[])
{
    time_t segundos;
    struct tm *data_hora_atual;
    time_t tempo_inicial;
    time_t tempo_final;
    clock_t CLK1;
    clock_t CLK2;
    float fDelta;
    struct stNumero parametro[THREADS];
    int i, j;
    pthread_t thread[THREADS];


    setlocale(LC_ALL, "");
    printf("> AppMultiThread v1.2 - ");
    time(&segundos);
    data_hora_atual = localtime(&segundos);
    printf("%02d/", data_hora_atual->tm_mday);          // dia
    printf("%02d/", data_hora_atual->tm_mon + 1);       // mês
    printf("%d ",   data_hora_atual->tm_year + 1900);   // ano
    printf("%02d:", data_hora_atual->tm_hour);          // hora
    printf("%02d:", data_hora_atual->tm_min);           // minuto
    printf("%02dh", data_hora_atual->tm_sec);           // segundo
    printf("\n\n");

    for (j = 0; j < THREADS; j++)
    {
        for (i = 0; i < NUMEROS; i++)
        {
            parametro[j].Numeros[i] = 20;
        }
    }

    pthread_mutex_init(&mutex, NULL);

    if (LOG)
        if ((fp = fopen("LOG.txt", "a")) != NULL)
            giLog = TRUE;

    time(&tempo_inicial);
    CLK1 = clock();

    for (i = 0; i < THREADS; i++)
    {
        pthread_create(&thread[i], NULL, &CalcularFatorial, &parametro[i]);
    }
    usleep(1);

    // aguarda a conclusão de todas as threads
    while (giThreads != 0)
    {
        usleep(1);
    }

    CLK2 = clock();
    time(&tempo_final);

    if (giLog == TRUE)
        fclose(fp);

    pthread_mutex_destroy(&mutex);

    //printf("\n");
    printf("início: %I64d\n", tempo_inicial);
    printf("fim:    %I64d\n", tempo_final);
    printf("delta=  %I64d s\n", tempo_final-tempo_inicial);

    fDelta = ((CLK2 - CLK1) * 1000) / CLOCKS_PER_SEC;
    printf("delta=  %0.2f ms\n", fDelta);

    return (0);
}


void* CalcularFatorial(void *argumento)
{
    int iThreadId;
    struct stNumero *parametro = argumento;
    struct stNumero *resultado = malloc(sizeof(struct stNumero));
    int j;
    unsigned long long int i;
    int iIO;


    pthread_mutex_lock(&mutex);
    giThreads++;
    pthread_mutex_unlock(&mutex);
    iThreadId = giThreads;

    // simula a execução de operação de E/S de maneira aleatória
    if (IO)
    {
        srand(time(NULL));
        iIO = rand() % NUMEROS;
    }

    for (j = 0; j < NUMEROS; j++)
    {
        if (IO)
            if(j == iIO )
                usleep(3000);

        resultado->Numeros[j] = 1;
        for(i = parametro->Numeros[j]; i > 0; i--)
        {
            resultado->Numeros[j] = resultado->Numeros[j] * i;
        }
        if (ECHO)
        {
            pthread_mutex_lock(&mutex);
            printf("%d.%I64d", iThreadId, parametro->Numeros[j]);
            printf("!=\t%I64d\n", resultado->Numeros[j]);
            pthread_mutex_unlock(&mutex);
        }
    }

    free(resultado);
    resultado = NULL;

    if (giLog == TRUE)
    {
        pthread_mutex_lock(&mutex);
        fputs("thread finalizada\n", fp);
        if (LOG_FLUSH)
            fflush(fp);
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_lock(&mutex);
    giThreads--;
    pthread_mutex_unlock(&mutex);

    return ((void *)(resultado));
}

// https://www.khronos.org

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <CL/cl.h>

#define MAX_SOURCE_SIZE (0x100000)

int main()
{   
    int size_table = 3;
    clock_t tempo;
	tempo = clock();

    /* Variáveis para armazenamento de referências a objetos OpenCL */  
    cl_platform_id platform_id = NULL;         // OpenCL plataforma
    cl_device_id device_id = NULL;             // device ID
    cl_context context = NULL;                 // contexto
    cl_command_queue command_queue = NULL;     // fila de comandos
    cl_kernel kernel = NULL;                   // kernel
    cl_program program = NULL;                 // programa
    
    /* Memória do dispositivo usada para a matriz de entrada e saida */
    cl_mem Amobj = NULL;
    cl_mem Bmobj = NULL;
    cl_mem Cmobj = NULL;

    /* Tipos de dados escalares */
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret;

    int i, j;
    float *A;
    float *B;
    float *C;
    
    /* 
      Alocação dinâmica de memória
      Aloca espaço para um bloco de bytes consecutivos na memória RAM 
    */
    A = (float *)malloc(size_table*size_table*sizeof(float));
    B = (float *)malloc(size_table*size_table*sizeof(float));
    C = (float *)malloc(size_table*size_table*sizeof(float));

    FILE *fp;
    const char fileName[] = "./dataParallel.cl";
    size_t source_size;
    char *source_str;
    
    /* Carregar arquivo de origem do kernel */
    fp = fopen(fileName, "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char *)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp );
    fclose( fp );
    
    /* Inicializar dados de entrada */
    for (i=0; i<size_table; i++) {
        for (j=0; j<size_table; j++) {
            A[i*size_table+j] = i*size_table+j+1;
            B[i*size_table+j] = j*size_table+i+1;
        }
    }
    
    /* Exibir resultados A */
    printf("\n---- Resultados A ----\n");
    for (i=0; i<size_table; i++) {
        for (j=0; j<size_table; j++) {
            printf("%7.2f", A[i*size_table+j]);
        }
        printf("\n");
    }

    /* Exibir resultados B */
    printf("\n---- Resultados B ----\n");
    for (i=0; i<size_table; i++) {
        for (j=0; j<size_table; j++) {
            printf("%7.2f", B[i*size_table+j]);
        }
        printf("\n");
    }

    /* Obter informações da plataforma / dispositivo */
    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    // ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
    
    // GPU
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
    if(ret == CL_DEVICE_NOT_FOUND) {
        // CPU
        ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
    }
    
    /* Criar contexto OpenCL */
    context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
    
    /* Criar fila de comandos */
    command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
    
    /* Criar objeto de buffer */
    Amobj = clCreateBuffer(context, CL_MEM_READ_WRITE, size_table*size_table*sizeof(float), NULL, &ret);
    Bmobj = clCreateBuffer(context, CL_MEM_READ_WRITE, size_table*size_table*sizeof(float), NULL, &ret);
    Cmobj = clCreateBuffer(context, CL_MEM_READ_WRITE, size_table*size_table*sizeof(float), NULL, &ret);
  
    /* Copiar dados de entrada no buffer de memória */
    ret = clEnqueueWriteBuffer(command_queue, Amobj, CL_TRUE, 0, size_table*size_table*sizeof(float), A, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, Bmobj, CL_TRUE, 0, size_table*size_table*sizeof(float), B, 0, NULL, NULL);
   
    /* Criar programa do kernel a partir do arquivo de origem */
    program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
    ret     = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    
    /* Crie dados do kernel OpenCL paralelo */
    kernel = clCreateKernel(program, "dataParallel", &ret);
    
    /* Definir argumentos do kernel do OpenCL */
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&Amobj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&Bmobj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&Cmobj);
        
    size_t global_item_size = size_table; // Array contendo os tamanhos para cada dimensão do espaço de índices. 
    size_t local_item_size = 1; // Array de tamanhos dos grupos de trabalho em cada dimensão. 
    
    /* Execute o kernel do OpenCL como dados paralelos */
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, 
                                 &global_item_size, &local_item_size, 0, NULL, NULL);
     
    /* Transferencia de resultados para o hostpedeiro */
    ret = clEnqueueReadBuffer(command_queue, Cmobj, CL_TRUE, 0, size_table*size_table*sizeof(float), C, 0, NULL, NULL);

    /* Exibir resultados */
    printf("\n---- Resultados ----\n");
    for (i=0; i<size_table; i++) {
        for (j=0; j<size_table; j++) {
            printf("%7.2f", C[i*size_table+j]);
        }
        printf("\n");
    }
    printf("\n");

    /* Liberação de recursos e encerramento da aplicação */
    ret = clFlush(command_queue);                   // Liberação de Cache
    ret = clFinish(command_queue);                  // Bloqueia os comandos OpenCL enfileirados anteriormente, também é um ponto de sincronização. 
    ret = clReleaseKernel(kernel);                  // Decrementa a contagem de referência do kernel.
    ret = clReleaseProgram(program);                // Decrementa a contagem de referência do Programa.
    ret = clReleaseMemObject(Amobj);                // Decrementa a contagem de referência do objeto (Amobj) da memória.
    ret = clReleaseMemObject(Bmobj);                // Decrementa a contagem de referência do objeto (Bmobj) da memória.
    ret = clReleaseMemObject(Cmobj);                // Decrementa a contagem de referência do objeto (Cmobj) da memória.
    ret = clReleaseCommandQueue(command_queue);     // Decrementa a contagem de referência da fila de comandos (command_queue).
    ret = clReleaseContext(context);                // Reduz a contagem de referência de contexto.
    
    /* Desaloca a porção de memória alocada */
    free(source_str);

    free(A);
    free(B);
    free(C);

    printf("Tempo: %f\n\n",(clock() - tempo) / (double)CLOCKS_PER_SEC);
    return 0;
}

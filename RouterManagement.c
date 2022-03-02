#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Commands.c"

int main(int argc, char *argv[]) {
    int n;
    int connections[2];
    FILE *filePtr;
    
    if(argc > 2){
        if(access(argv[1], F_OK ) != -1 && access(argv[2], F_OK) != -1){
            filePtr = fopen(argv[1],"rb");
        } else{
            printf("File was not found\n");
            exit(1);
        }
        if(filePtr == NULL){ 
            printf("Memory not allocated.\n"); 
            exit(0); 
        }
    } else{
        printf("Correct usage: <topology file> <commands file> ");
    }

    fread(&n,1,sizeof(n),filePtr);
    number_of_routers = n;
    routers = malloc(n * sizeof(struct Router));
    for(int j = 0; j < n; j++){       
        fread(&routers[j], 6, 1, filePtr);
        fread(routers[j].model, routers[j].prod, 1, filePtr); 
        fread(&routers[j].nullbyte, sizeof(routers[j].nullbyte), 1, filePtr);
        routers[j].number_of_connections = 0;
    }
    while(fread(&connections, 8, 1, filePtr) == 1){
        addConnection(connections[0],connections[1]);
    }
    executeCommand(&argv[2]);
    fclose(filePtr);
    free(routers);
}

#define MAX_CONNECTIONS 10
#define MAX_MODEL_LENGTH 249

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "Commands.h"

struct Router {
    int router_id;
    unsigned char flag, prod;
    unsigned char model[MAX_MODEL_LENGTH];
    char nullbyte;
    struct Router *connections[MAX_CONNECTIONS];
    short number_of_connections;
};

struct Router *routers; // pointer to all the routers 
int number_of_routers; // total number of routers

void print(int router_id){
    int router_index = -1;
    printf("\n\nrouter_id: %d\n Connected to: ", router_id);
    for(int i = 0; i < number_of_routers; i++){
        if(router_id == routers[i].router_id){
            router_index = i;
            for(int k = 0; k < routers[i].number_of_connections; k++){
                printf("%d ", routers[i].connections[k]->router_id);
            }
        }
        for (int j = 0; j < routers[i].number_of_connections; j++){
            if(routers[i].connections[j]->router_id == router_id){
                printf("%d ", routers[i].router_id);
            }
        }
    }
    if(router_index != -1){
        routers[router_index].model[routers[router_index].prod] = '\0';
        printf("flag: %d model: %s" , routers[router_index].flag, routers[router_index].model);
    }
}

void setFlag(int router_id, unsigned char flag, unsigned char value){
    for(int i = 0; i < number_of_routers; i++){
        if(router_id == routers[i].router_id){
            if(flag == 0){
                if(value == 0){
                    routers[i].flag = routers[i].flag & 254; 
                } else if(value == 1){
                    routers[i].flag = routers[i].flag | 1;
                }
            } else if(flag == 1){
                if(value == 0){
                    routers[i].flag = routers[i].flag & 253; 
                } else if(value == 1){
                    routers[i].flag = routers[i].flag | 2;
                }
            } else if(flag == 2){
                if(value == 0){
                    routers[i].flag = routers[i].flag & 251; 
                }
                else if(value == 1){
                    routers[i].flag = routers[i].flag | 4;
                }
            } else if(flag == 4 && 15 + (16 * value) <= 255 ){
                routers[i].flag = 15 + (16 * value);
            }
            return;
        }
    }
}

void setModel(int router_id, char *model){
    for(int i = 0; i < number_of_routers; i++){
        if(router_id == routers[i].router_id){
            routers[i].prod = (unsigned char)strlen(model);
            memmove(routers[i].model, model, MAX_MODEL_LENGTH);
            return;
        }
    }
}

void addConnection(int router_id1, int router_id2) {
    int router_index1 = -1, router_index2 = -1;
    for(int i = 0; i < number_of_routers; i++){
        if(router_id1 == routers[i].router_id && routers[i].number_of_connections < MAX_CONNECTIONS){
            router_index1 = i;
        }
        if(router_id2 == routers[i].router_id){
            router_index2 = i;
        }   
    }
    if(router_index1 != -1 && router_index2 != -1){
        routers[router_index1].connections[routers[router_index1].number_of_connections] = &routers[router_index2];
        routers[router_index1].flag += 16;            
        routers[router_index1].number_of_connections ++;
    }
}

void deleteRouter(int router_id) {
    int router_index;
    for(int i = 0; i < number_of_routers; i++){
        for(int j = 0; j < routers[i].number_of_connections; j++){
            if(routers[i].connections[j]->router_id == router_id){
                for(int k = j; k < routers[i].number_of_connections-1; k++){
                    routers[i].connections[k] = routers[i].connections[k+1];
                }
                routers[i].number_of_connections --;
            }
        }
        if(router_id == routers[i].router_id){
            router_index = i;
        }
    }
    for(int i = router_index; i < number_of_routers-1; i++){
        routers[i] = routers[i + 1];
    }
    number_of_routers --;
}

void checkRoute(int router_id1, int router_id2){
    printf("\nChecking route from %d to %d\n", router_id1, router_id2);
    struct Router *to_visit[number_of_routers];
    int to_visit_idx = 0, visited_idx = 0, router_id1_index = -1;
    for(int i = 0; i < number_of_routers; i++){
        if(router_id1 == routers[i].router_id){
            router_id1_index = i;
            break;
        }
    }
    if(router_id1_index == -1){
        printf("The route does not exist");
        return;
    } else{
        to_visit[0] = &routers[router_id1_index];
        to_visit_idx++;
    }

    while(visited_idx < to_visit_idx){
        struct Router *target = to_visit[visited_idx];
        for(int i = 0; i < target->number_of_connections; i++){
            char visited = 0;
            if(target->connections[i]->router_id == router_id2){
                printf("The route exists");
                return;
            }
            for(int j = 0; j < visited_idx; j++){
                if(target->connections[i]->router_id == to_visit[j]->router_id){
                    visited = 1;
                    break;
                }
            }
            if(!visited){
                to_visit[to_visit_idx] = target->connections[i];
                to_visit_idx++;
            }
        }
        visited_idx++;
    }
    printf("The route does not exist");
}

void executeCommand(char *file[]){
    char str[15];
    int router_id;
    if(access(file[0], F_OK) != -1){  
        FILE *filePtr = fopen(file[0],"r");
        while(fscanf(filePtr, "%s", str) == 1){
            if(memcmp(str, "print", 5) == 0){
                fscanf(filePtr, "%d ", &router_id);
                print(router_id);
            } else if(memcmp(str, "setModel", 8) == 0){
                char model[MAX_MODEL_LENGTH];
                fscanf(filePtr, "%d %[^\n]s", &router_id, model);
                setModel(router_id, model);
            } else if(memcmp(str, "setFlag", 7) == 0){
                int flag, value;
                fscanf(filePtr, "%d %d %d", &router_id, &flag, &value);
                setFlag(router_id, (unsigned char)flag, (unsigned char)value);
            } else if(memcmp(str, "checkRoute",10) == 0){
                int router_id2;
                fscanf(filePtr,"%d %d ", &router_id, &router_id2);
                checkRoute(router_id,router_id2);
            } else if(memcmp(str, "addConnection",13) == 0){
                int router_id2;
                fscanf(filePtr,"%d %d", &router_id, &router_id2);
                addConnection(router_id, router_id2);
            } else if(memcmp(str, "deleteRouter", 12) == 0){
                fscanf(filePtr,"%d ",&router_id);
                deleteRouter(router_id);
            }
        }
        fclose(filePtr);
    } else {
        printf("File was not found");
    }
}

void writeToFile(char *filename) {
    FILE *filePtr = fopen(filename, "wb");
    if(filePtr == NULL){
        printf("Unable to create file.\n");
    } else{
        int numberOfConnections = 0;
        fwrite(&number_of_routers, sizeof(number_of_routers), 1, filePtr);
        for(int i = 0; i < number_of_routers; i++){
            fwrite(&routers[i].router_id, 4,1, filePtr);
            fwrite(&routers[i].flag, 1,1, filePtr);
            fwrite(&routers[i].prod, 1,1, filePtr);
            fwrite(routers[i].model, routers[i].prod, 1, filePtr);
            fwrite(&routers[i].nullbyte, 1,1, filePtr);
            numberOfConnections += routers[i].number_of_connections;
        }
        
        for (int i = 0; i < number_of_routers; i++){
            for(int j = 0; j < routers[i].number_of_connections; j++){
                fwrite(&routers[i].router_id, 4,1, filePtr);
                fwrite(&routers[i].connections[j]->router_id, 4,1, filePtr);
            }
        }
        fclose(filePtr);
        printf("\nFile created and saved successfully. Number of connections: %d \n", numberOfConnections);
    }
}

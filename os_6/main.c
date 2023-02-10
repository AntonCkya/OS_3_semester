#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zmq.h>
#include <assert.h>
#include <unistd.h>

typedef struct{
    char str[256];
    char sub[256];
    int command;
    int node;
} data_for_node;
/*
1 - exec 
2 - ping
3 - remove

1: если в ноду попадает комманда 1, то она проверяет совпадение data->node со своим CUR_NODE
при совпадении выполняет KMP, при несовпадении передает комманду своим дочерним нодам

2: если в ноду попадает комманда 2, то она делает пинг (Ok:node_number)
и отправляет дочерним нодам

3: если в ноду попадает комманда 3 с data->node == CUR_NODE или -1, то она отправляет эту комманду
своим дочерним нодам и удаляется (break; и программа нода завершается)
*/

int number_in_array (int number, int * array, int array_size){
    for(int i = 0; i < array_size; i++){
        if (array[i] == number)
            return 1;
    }
    return 0;
}

void print_array_of_nodes (int * array, int array_size){ // для отладки, вывод всех рабочих нод
    for(int i = 0; i < array_size; i++){
        if (array[i] != 0){
            printf("%d ", array[i]);
        }          
    }
    printf("\n");
}

void print_relations (int array[2][256], int array_size){ // для отладки, вывод всех связей между нодами
    for(int i = 0; i < array_size; i++){
        if (array[0][i] != 0 && array[1][i] != 0){
            printf("%d - %d\n", array[0][i], array[1][i]);
        }          
    }
}

int main(){

    void *context;
	void *publisher;

    context = zmq_ctx_new();
    publisher = zmq_socket(context, ZMQ_PUB);
    zmq_bind (publisher, "tcp://*:4040");

    //int timeout = 1500;
    //size_t timeout_size = sizeof timeout;
	//zmq_setsockopt(publisher, ZMQ_RCVTIMEO, &timeout, timeout_size);

    char s[256];

    int array_of_nodes[256];
    for(int i = 0; i < 256; i++){
        array_of_nodes[i] = 0;
    }

    array_of_nodes[0] = -1;
    int iter_array_of_nodes = 1;

    int relation[2][256];
    for(int i = 0; i < 256; i++){
        relation[0][i] = 0;
        relation[1][i] = 0;
    }
    int iter_relation = 0;

    while(scanf("%s", s) != EOF){


        if(!strcmp (s, "exec")){

            int node_number, is_exist;
            scanf("%d", &node_number);
            is_exist = number_in_array(node_number, array_of_nodes, 256);

            if(!is_exist){
                printf("Error:%d: Not found\n", node_number);
                continue;
            }

            char str[256];
            char sub[256];
            data_for_node* data = malloc(sizeof(data_for_node));

            scanf("%s", str);
            scanf("%s", sub);
            strcpy(data->str, str);
            strcpy(data->sub, sub);
            data->command = 1;
            data->node = node_number;

            //printf("server: str: %s, sub: %s\n", data->str, data->sub);
            //printf("server: command: %d, node: %d\n", data->command, data->node);

            zmq_msg_t message;
            zmq_msg_init_size(&message, sizeof(data_for_node));

            memcpy(zmq_msg_data(&message), data, zmq_msg_size(&message));

            zmq_msg_send(&message, publisher, 0);
            
		    zmq_msg_close(&message);

            free(data);
        }

        if(!strcmp (s, "create")){

            int node_number, parent_number, is_exist;
            scanf("%d", &node_number);
            is_exist = number_in_array(node_number, array_of_nodes, 256);

            if(is_exist){
                printf("Error: Already exists\n");
                continue;
            }
            
            scanf("%d", &parent_number);
            is_exist = number_in_array(parent_number, array_of_nodes, 256);

            if(!is_exist){
                printf("Error: Parent not found\n");
                continue;
            }

            relation[0][iter_relation] = parent_number;
            relation[1][iter_relation] = node_number;
            iter_relation++;

            int id = fork();

            if (id > 0){

                printf("OK: %d\n", id);

                array_of_nodes[iter_array_of_nodes] = node_number;
                iter_array_of_nodes++;
        
            } else if (id == 0){

                char str_node_number[256];
                snprintf(str_node_number, sizeof(str_node_number), "%d", node_number);

                char str_parent_number[256];
                snprintf(str_parent_number, sizeof(str_parent_number), "%d", parent_number);

                if (execl("b.out", "b.out", str_node_number, str_parent_number, (char *)NULL) == -1){
                    printf("Error in executing new node\n");
                    continue;
                }

            } else {
                printf("Fork raise error\n");
		        continue;
            }

        }
        if(!strcmp (s, "pingall")){

            data_for_node* data = malloc(sizeof(data_for_node));
            data->command = 2;

            zmq_msg_t message;
            zmq_msg_init_size(&message, sizeof(data_for_node));

            memcpy(zmq_msg_data(&message), data, zmq_msg_size(&message));

            zmq_msg_send(&message, publisher, 0);
            
		    zmq_msg_close(&message);

            free(data);

        }
        if(!strcmp (s, "remove")){

            int node_number, is_exist;
            scanf("%d", &node_number);
            is_exist = number_in_array(node_number, array_of_nodes, 256);

            if(!is_exist){
                printf("ID not found\n");
                continue;
            }

            data_for_node* data = malloc(sizeof(data_for_node));
            data->command = 3;
            data->node = node_number;

            zmq_msg_t message;
            zmq_msg_init_size(&message, sizeof(data_for_node));

            memcpy(zmq_msg_data(&message), data, zmq_msg_size(&message));

            zmq_msg_send(&message, publisher, 0);
            
		    zmq_msg_close(&message);

            free(data);

            for(int i = 0; i < 256; i++){
                if (array_of_nodes[i] == node_number){
                    array_of_nodes[i] = 0;
                    break;
                }
            }

            for(int i = 0; i < 256; i++){
                if (relation[0][i] == node_number){
                    for(int j = 0; j < 256; j++){
                        if (array_of_nodes[j] == relation[1][i]){
                            array_of_nodes[j] = 0;
                            break;
                        }
                    }
                }
                if (relation[1][i] == node_number){
                    relation[0][i] = 0;
                    relation[1][i] = 0;
                }
            }

            if(node_number == -1){
                printf("Programm stopped\n");
                break;
            }

            //print_array_of_nodes(array_of_nodes, 256);
            //print_relations(relation, 256);
        
        }

    }
    
    return 0;
}

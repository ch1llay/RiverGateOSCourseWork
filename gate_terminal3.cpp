#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include <time.h>
#include "structure.h"
#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

int speed = 40;
bool signal = true;
struct message_info termial_info;
struct message_info termial_config;
int msgid3;


void* sending(void*){
    while(true){
            sleep(100 / speed);
            termial_info.my_msg_type = 1;
            termial_info.from = rand() % 2;
            termial_info.speed = speed;
            if(signal){
                if (msgsnd (msgid3, &termial_info,sizeof(termial_info), 0) == -1)
                {
                    fprintf(stderr, "msgsnd faild\n");
                    exit (EXIT_FAILURE);
                }
            }
        }
}

int main (void)
{
	bool running = true;
    msgid3 = msgget ((key_t)0003,0666 | IPC_CREAT);

	if (msgid3 ==- 1)
	{
		fprintf(stderr, "msgget faild with error: %d\n",errno);
		exit(EXIT_FAILURE);
	}

        pthread_t th1;
        pthread_create(&th1, NULL, sending, NULL);

    while (1) {
        if(msgrcv(msgid3, &termial_config, sizeof(termial_config), 2, 0) != -1){
            cout << "-------------------------changing state------------------------------" << endl;

            if(termial_config.speed < 0){
                cout << "get uncorrect speed value: " << termial_config.speed << endl;
            }
            else{
                speed = termial_config.speed;
                cout << "new value of speed = " << speed << endl;
            }
            
            signal = termial_config.signal;
            cout << "new value of foreing signal is " << (signal ? "on" : "off") << endl;

        }


    }
}	


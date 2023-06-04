#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include <time.h>
#include "structure.h"
#include <iostream>

using namespace std;

int speed = 40;
bool signal = true;

int main (void)
{
	bool running = true;
	struct message_info termial_info;
    struct message_info termial_config;

	int msgid1;

	msgid1 = msgget ((key_t)0001,0666 | IPC_CREAT);

	if (msgid1 ==-1)
	{
		fprintf(stderr, "msgget faild with error: %d\n",errno);
		exit(EXIT_FAILURE);
	}

    pid_t terminal_pid = fork();
 
    if (terminal_pid == -1) { // если не удалось создать процесс, выводим сообщение об ошибке
        perror("Error creating train process");
        exit(EXIT_FAILURE);
    }
    // дочерний процесс - поезд, отправляем сигналы в родительский процесс
    else if (terminal_pid == 0) {
        while(true){
            sleep(100/speed);
            cout << "speed on send " << speed << endl;
            termial_info.my_msg_type = 1;
            termial_info.from = rand() % 2;
            termial_info.speed = speed;
            if(signal){
                if (msgsnd (msgid1, &termial_info,sizeof(termial_info), 0) == -1)
                {
                    fprintf(stderr, "msgsnd faild\n");
                    exit (EXIT_FAILURE);
                }
            }
        }
        exit(EXIT_SUCCESS);
    }    

    // родительский процесс - принимае сообщения для конфигурации
    else {
        while (1) {
            // sleep(1);
            cout << "-------------------------changing state------------------------------" << endl;
            if(msgrcv(msgid1, &termial_config, sizeof(termial_config), 2, 0) != -1){
                if(termial_config.speed < 0){
                    cout << "get uncorrect speed value: " << termial_config.speed << endl;
                }
                else{
                    speed = termial_config.speed;
                    cout << "new value of speed = " << speed << endl;
                }
                
                signal = termial_config.signal;
            }

            cout << "new value of foreing signal is" << (signal ? "on" : "off") << endl;

        }
    }	
}


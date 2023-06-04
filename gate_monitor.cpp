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

struct monitoring_info{
	int msgid;
	int terminal_number;
};

struct message_info termial_info;



void* monitoring(void* data){
	monitoring_info* info = (monitoring_info*) data;

	while(true){
		sleep(1);
	if(msgrcv(info->msgid, &termial_info, sizeof(termial_info), 1, 0) != -1){
            cout << "--------------------termainal " << info->terminal_number << ": ---------------------" << endl;
            cout << "sheep going from " << (termial_info.from == 1 ? "up" : "down") << endl;
            cout << "with speed " << termial_info.speed << endl << "signal is " << (termial_info.signal ? "on" : "off") << endl;
            cout << "--------------------end termainal " << info ->terminal_number << ": ---------------------" << endl << endl;
        }
		}
}
int main (void)
{
	bool running = true;

	int msgid1, msgid2, msgid3;

	msgid1 = msgget ((key_t)0001,0666 | IPC_CREAT);
	msgid2 = msgget ((key_t)0002,0666 | IPC_CREAT);
	msgid3 = msgget ((key_t)0003,0666 | IPC_CREAT);

	pthread_t th1, th2, th3;
	monitoring_info monitoring_info1;
	monitoring_info1.msgid = msgid1;
	monitoring_info1.terminal_number = 1;

	monitoring_info monitoring_info2;
	monitoring_info2.msgid = msgid2;
	monitoring_info2.terminal_number = 2;
	monitoring_info monitoring_info3;
	monitoring_info3.msgid = msgid3;
	monitoring_info3.terminal_number = 3;

    pthread_create(&th1, NULL, monitoring, &monitoring_info1);
	pthread_create(&th2, NULL, monitoring, &monitoring_info2);
	pthread_create(&th3, NULL, monitoring, &monitoring_info3);

	while(true);
}

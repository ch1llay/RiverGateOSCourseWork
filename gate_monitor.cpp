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

int main (void)
{
	bool running = true;
	struct message_info termial_info;

	int msgid1;

	msgid1 = msgget ((key_t)0001,0666 | IPC_CREAT);

	if (msgid1 ==-1)
	{
		fprintf(stderr, "msgget faild with error: %d\n",errno);
		exit(EXIT_FAILURE);
	}
        
	while (running)
	{	
        //sleep(1);

        if(msgrcv(msgid1, &termial_info, sizeof(termial_info), 1, 0) != -1){
            cout << "--------------------termainal1: ---------------------" << endl;
            cout << "sheep going from " << (termial_info.from == 1 ? "up" : "down") << endl;
            cout << "with speed " << termial_info.speed << endl << "signal is " << (termial_info.signal ? "on" : "off") << endl;
            cout << "--------------------end termainal1: ---------------------" << endl << endl;
        }

        
	}
}

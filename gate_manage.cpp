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

int msgid1, msgid2, msgid3;
bool running = true;
struct message_info termial_config;

void changing_state(int msg_id, int speed, bool signal){
    termial_config.my_msg_type = 2;
    termial_config.speed = speed;
    termial_config.signal = signal;

    if(msgsnd(msg_id, &termial_config, sizeof(termial_config), 0) == -1){
            fprintf(stderr, "msgsnd faild\n");
            //exit (EXIT_FAILURE);    
    }
    else{
        cout << "success " << endl;
    }
}
int main (void)
{
	

	msgid1 = msgget ((key_t)0001,0666 | IPC_CREAT);
    msgid2 = msgget ((key_t)0002,0666 | IPC_CREAT);
    msgid2 = msgget ((key_t)0003, 0666 | IPC_CREAT);



	if (msgid1 ==-1 || msgid2 == -1 || msgid3 ==-1)
	{
		fprintf(stderr, "msgget faild with error: %d\n",errno);
		exit(EXIT_FAILURE);
	}
    int terminal_number = 0;
    int new_speed = 1;
    bool new_signal = 1;
        
	while (running)
	{	
        cout << "choose a terminal" << endl;
        cout << "1 or 2 or 3" << endl;
        cin >> terminal_number;

        cout << "enter new speed value " << endl;
        cin >> new_speed;
        cout << "enter new signal value 0 or 1" << endl;
        cin >> new_signal;

        switch (terminal_number)
        {
        case 1:
            cout << "for 1" << endl;
            changing_state(msgid1, new_speed, new_signal);
            break;
        case 2:
            changing_state(msgid2, new_speed, new_signal);
            break;
        case 3:
            changing_state(msgid3, new_speed, new_signal);
            break;
        default:
            cout << "not correct terminal number " << endl;
            continue;
        }

        cout << "set speed for terminal " << terminal_number << endl;
        cout << "speed = " << new_speed << endl;
        cout << "foreing signal is " << new_signal;
        cout << endl << endl;

        
	}
}

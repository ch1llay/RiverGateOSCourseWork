#pragma once

struct message_info
	{
		long int my_msg_type;
        int speed; // speed of water // using 1/speed - pause time
        bool signal = true;
        int from;
        int terminal_number;
	};
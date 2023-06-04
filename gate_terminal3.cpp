#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <sys/msg.h>

#define MSG_SIZE 256

struct terminal_info {
    int id;
    char name[32];
    int status;
    int doorTop;
    int doorBottom;
};

struct door_info {
    int terminalId;
    int id;
    int status;
    int mode;
};

struct msgbuf {
    long mtype;
    char mtext[MSG_SIZE];
};

void showTerminalState(struct terminal_info* terminal) {
    printf("Terminal #%d: %s, Status: %d, Door (Top): %d, Door (Bottom): %d\n", terminal->id, terminal->name, terminal->status, terminal->doorTop, terminal->doorBottom);
}

void showDoorState(struct door_info* door) {
    printf("Door #%d (Terminal #%d): Mode: %d, Status: %d\n", door->id, door->terminalId, door->mode, door->status);
}

void GateControl() {
    int terminal_msgid = msgget(1234, 0666 | IPC_CREAT);
    int door_msgid = msgget(5678, 0666 | IPC_CREAT);

    if (terminal_msgid == -1) {
        perror("Error in creating message queue");
        exit(EXIT_FAILURE);
    }

    if (door_msgid == -1) {
        perror("Error in creating message queue");
        exit(EXIT_FAILURE);
    }

    int status = 0;
    struct msgbuf rcvMsg;

    // Wait for the messages
    while (1) {
        if (msgrcv(terminal_msgid, &rcvMsg, sizeof(rcvMsg.mtext), 1, MSG_NOERROR) == -1) {
            perror("msgrcv failed with error");
            exit(EXIT_FAILURE);
        }

        struct terminal_info *terminal = (struct terminal_info *)rcvMsg.mtext;
        showTerminalState(terminal);

        if (msgrcv(door_msgid, &rcvMsg, sizeof(rcvMsg.mtext), terminal->id + 1, MSG_NOERROR) == -1) {
            perror("msgrcv failed with error");
            exit(EXIT_FAILURE);
        }

        struct door_info *doorTop = (struct door_info *)rcvMsg.mtext;
        struct door_info *doorBottom = (struct door_info *)(rcvMsg.mtext + sizeof(struct door_info));

        showDoorState(doorTop);
        showDoorState(doorBottom);

        status = terminal->status * doorTop->status * doorBottom->status;
        printf("Gate Status: %d\n\n", status);
    }
    exit(EXIT_SUCCESS);
}

void TerminalControl() {
    int terminal_msgid = msgget(1234, 0666 | IPC_CREAT);
    int door_msgid = msgget(5678, 0666 | IPC_CREAT);

    if (terminal_msgid == -1) {
        perror("Error in creating message queue");
        exit(EXIT_FAILURE);
    }

    if (door_msgid == -1) {
        perror("Error in creating message queue");
        exit(EXIT_FAILURE);
    }

    struct terminal_info terminalList[2];
    strcpy(terminalList[0].name, "Terminal A");
    strcpy(terminalList[1].name, "Terminal B");
    terminalList[0].id = 0;
    terminalList[1].id = 1;

    terminalList[0].doorTop = 1;
    terminalList[0].doorBottom = 0;
    terminalList[0].status = 1;

    terminalList[1].doorTop = 0;
    terminalList[1].doorBottom = 1;
    terminalList[1].status = 1;

    while (1) {
        for (int i = 0; i < 2; i++) {
            struct msgbuf msg;
            msg.mtype = 1;

            memcpy(msg.mtext, &terminalList[i], sizeof(struct terminal_info));
            if (msgsnd(terminal_msgid, &msg, sizeof(struct terminal_info), 0) == -1) {
                perror("msgsnd failed with error");
            }

            struct door_info doorTop;
            doorTop.terminalId = i;
            doorTop.id = 0;
            doorTop.mode = 0;
            doorTop.status = terminalList[i].doorTop;

            struct door_info doorBottom;
            doorBottom.terminalId = i;
            doorBottom.id = 1;
            doorBottom.mode = 0;
            doorBottom.status = terminalList[i].doorBottom;

            msg.mtype = i + 2;
            memcpy(msg.mtext, &doorTop, sizeof(struct door_info));
            memcpy(msg.mtext + sizeof(struct door_info), &doorBottom, sizeof(struct door_info));

            if (msgsnd(door_msgid, &msg, 2 * sizeof(struct door_info), 0) == -1) {
                perror("msgsnd failed with error");
            }
        }

        sleep(2);
        printf("\n");
    }

    exit(EXIT_SUCCESS);
}

void DoorControl() {
    int door_msgid = msgget(5678, 0666 | IPC_CREAT);

    if (door_msgid == -1) {
        perror("Error in creating message queue");
        exit(EXIT_FAILURE);
    }

    int doorTopStatus[2] = {0, 1};
    int doorBottomStatus[2] = {1, 0};

    while (1) {
        struct msgbuf rcvMsg;
        if (msgrcv(door_msgid, &rcvMsg, sizeof(rcvMsg.mtext), 0, MSG_NOERROR) == -1) {
            perror("msgrcv failed with error");
            exit(EXIT_FAILURE);
        }

        struct door_info *doorTop = (struct door_info *)rcvMsg.mtext;
        struct door_info *doorBottom = (struct door_info *)(rcvMsg.mtext + sizeof(struct door_info));

        printf("DoorControl: Door Top status: %d, Door bottom status: %d\n", doorTop->status, doorBottom->status);

        doorTop->status = doorTopStatus[doorTop->terminalId];
        doorBottom->status = doorBottomStatus[doorBottom->terminalId];

        doorTop->mode = (doorBottom->status == 1 ? 2 : doorTop->mode);
        doorBottom->mode = (doorTop->status == 1 ? 2 : doorBottom->mode);

        printf("DoorControl: Door Top status: %d, Door bottom status: %d\n", doorTop->status, doorBottom->status);

        rcvMsg.mtype = doorTop->terminalId + 2;
        memcpy(rcvMsg.mtext, doorTop, sizeof(struct door_info));
        memcpy(rcvMsg.mtext + sizeof(struct door_info), doorBottom, sizeof(struct door_info));

        if (msgsnd(door_msgid, &rcvMsg, 2 * sizeof(struct door_info), IPC_NOWAIT) == -1) {
            perror("msgsnd failed with error");
        }

        sleep(2);
    }

    exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[]) {
    pid_t pid1, pid2, pid3;

    pid1 = fork();
    if (pid1 == 0){
        TerminalControl();
    } else {
        pid2 = fork();
        if (pid2 == 0) {
            DoorControl();
        } else {
            pid3 = fork();
            if (pid3 == 0) {
                GateControl();
            } else {
                waitpid(pid1, NULL, 0);
                waitpid(pid2, NULL, 0);
                waitpid(pid3, NULL, 0);
            }
        }
    }

    return 0;
}



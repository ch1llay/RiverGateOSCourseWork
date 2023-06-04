#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

using namespace std;

// Сообщение для управления шлюзом
struct GatewayMessage {
    int msgtype;
    int terminalNumber; // Номер терминала
    int gateNumber; // Номер створа
    int level; // Уровень воды
    bool isOpen; // Открыт ли створ
    bool isWorking; // Работает ли терминал
    bool isEmergency; // Аварийная ли ситуация
    int speed; // Скорость изменения уровня воды
    bool isSignalEnabled; // Включены ли звуковые и визуальные сигналы
};

// Идентификаторы для взаимодействия посредством сообщений
const int gatewayMessageQueueKey = 100; // Очередь сообщений для управления шлюзом
const int statusMessageQueueKey = 200; // Очередь сообщений для отчетности о состоянии терминалов и створов

 bool terminalsWorking[4] = {true, true, true, true};
int gateLevels[4][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
bool gateOpen[4][2] = {{false, false}, {false, false}, {false, false}, {false, false}};
bool gateSignalEnabled[4][2] = {{true, true}, {true, true}, {true, true}, {true, true}};

// Функция для отправки сообщений в очередь
void sendMessage(int queueId, GatewayMessage message) {
    message.msgtype = 1;
    msgsnd(queueId, &message, sizeof(message), IPC_NOWAIT);
}

// Функция для получения сообщений из очереди
GatewayMessage receiveMessage(int queueId) {
    GatewayMessage message;
    msgrcv(queueId, &message, sizeof(message), 1, 0);
    return message;
}

void printInfo(){
    cout << "--------------------------------------------" << endl;
        cout << "terminals " << endl;

        for(int i =0; i < 4; i++){
            cout << terminalsWorking[i] << " ";
        }

        cout << endl;
            cout << "gateLevels" << endl;

        for(int i =0; i < 4; i++){
                        cout << "#" << i+1 << " ";

            for(int j = 0; j < 2; j++){
            cout << gateLevels[i][j] << " ";

            }
            cout << endl;
        }
            cout << "gateOpen" << endl;

        for(int i =0; i < 4; i++){
                        cout << "#" << i+1 << " ";
            for(int j = 0; j < 2; j++){
            cout << gateOpen[i][j] << " ";

            }
            cout << endl;
        }
            cout << "gateSignalEnabled" << endl;

        for(int i =0; i < 4; i++){
                        cout << "#" << i+1 << " ";

            for(int j = 0; j < 2; j++){
            cout << gateSignalEnabled[i][j] << " ";

            }
            cout << endl;
        }
        cout << "--------------------------------------------" << endl;

}
    

int main() {
    // Создаем очереди сообщений
    int gatewayMessageQueueId = msgget(gatewayMessageQueueKey, IPC_CREAT | 0666);
    int statusMessageQueueId = msgget(statusMessageQueueKey, IPC_CREAT | 0666);

    // Инициализируем состояние терминалов и створов
   

    GatewayMessage message;
    while (true) {
        
        printInfo();
        // Получаем сообщения для управления шлюзом
        if (msgrcv(gatewayMessageQueueId, NULL, 0, 0, IPC_NOWAIT) >= 0) {
            message = receiveMessage(gatewayMessageQueueId);
            int terminalNumber = message.terminalNumber;
            int gateNumber = message.gateNumber;

            // Обрабатываем сообщение
            if (terminalNumber >= 0 && terminalNumber < 4 && gateNumber >= 0 && gateNumber < 2) {
                terminalsWorking[terminalNumber] = message.isWorking;
                gateLevels[terminalNumber][gateNumber] = message.level;
                gateOpen[terminalNumber][gateNumber] = message.isOpen;
                gateSignalEnabled[terminalNumber][gateNumber] = message.isSignalEnabled;
            }
        }

        // Обновляем состояние терминалов и створов
        for (int i = 0; i < 4; i++) {
            if (terminalsWorking[i]) {
                for (int j = 0; j < 2; j++) {
                    if (gateOpen[i][j]) {
                        gateLevels[i][j] += message.speed;
                        if (gateLevels[i][j] >= 100) {
                            gateLevels[i][j] = 100;
                            gateOpen[i][j] = false;
                            sendMessage(statusMessageQueueId, {i, j, gateLevels[i][j], gateOpen[i][j], terminalsWorking[i], false, message.speed, gateSignalEnabled[i][j]});
                        }
                    } else {
                        gateLevels[i][j] -= message.speed;
                        if (gateLevels[i][j] <= 0) {
                            gateLevels[i][j] = 0;
                            gateOpen[i][j] = true;
                            sendMessage(statusMessageQueueId, {i, j, gateLevels[i][j], gateOpen[i][j], terminalsWorking[i], false, message.speed, gateSignalEnabled[i][j]});
                        }
                    }
                }
            }
        }

        // Отправляем информацию о состоянии термина


        for (int i = 0; i < 4; i++) {
            sendMessage(statusMessageQueueId, {i, 0, gateLevels[i][0], gateOpen[i][0], terminalsWorking[i], false, 0, gateSignalEnabled[i][0]});
            sendMessage(statusMessageQueueId, {i, 1, gateLevels[i][1], gateOpen[i][1], terminalsWorking[i], false, 0, gateSignalEnabled[i][1]});
        }

        // Ждем 1 секунду
        sleep(1);
    }

    return 0;
}
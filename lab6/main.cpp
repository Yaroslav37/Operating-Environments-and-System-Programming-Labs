#include <windows.h>
#include <iostream>
#include <queue>

bool isClosed = false;

std::queue<int> buffer;
HANDLE semaphoreFull;
HANDLE semaphoreEmpty;
HANDLE mutexBuffer;
int buffer_size, sleepProducer, sleepConsumer, itemsToProduce;

DWORD WINAPI Producer(LPVOID);
DWORD WINAPI Consumer(LPVOID);

int main() {
    DWORD producerId, consumerId;

    std::cout << "Enter the number of objects to be produced: ";
    std::cin >> itemsToProduce;
    std::cout << "Enter the buffer size(no more than " << itemsToProduce << ") :";

    while (std::cin >> buffer_size) {
        if (buffer_size < itemsToProduce) {
            break;
        }
        else {
            std::cout << "The buffer size cannot be greater than the number of objects produced! Try again: ";
        }
    }
   /* std::cin >> buffer_size;*/
    std::cout << "Enter manufacturer timeout (ms): ";
    std::cin >> sleepProducer;
    std::cout << "Enter the timeout for the consumer(ms) : ";
    std::cin >> sleepConsumer;

    if (buffer_size > itemsToProduce) {
        std::cout << "The buffer size cannot be greater than the number of objects produced!";
        exit;
    }

    semaphoreFull = CreateSemaphore(NULL, 0, buffer_size, NULL);
    semaphoreEmpty = CreateSemaphore(NULL, buffer_size, buffer_size, NULL);
    mutexBuffer = CreateMutex(NULL, FALSE, NULL);

    HANDLE hProducer = CreateThread(NULL, 0, Producer, NULL, 0, &producerId);
    HANDLE hConsumer = CreateThread(NULL, 0, Consumer, NULL, 0, &consumerId);

    WaitForSingleObject(hProducer, INFINITE);
    WaitForSingleObject(hConsumer, INFINITE);

    CloseHandle(semaphoreFull);
    CloseHandle(semaphoreEmpty);
    CloseHandle(mutexBuffer);
    CloseHandle(hProducer);
    CloseHandle(hConsumer);

    return 0;
}

DWORD WINAPI Producer(LPVOID lpParam) {
    for (int i = 0; i < itemsToProduce; ++i) {
        Sleep(sleepProducer);

        WaitForSingleObject(semaphoreEmpty, INFINITE);

        WaitForSingleObject(mutexBuffer, INFINITE);
        buffer.push(i);
        std::cout << "Produced: " << i << std::endl;
        ReleaseMutex(mutexBuffer);

        ReleaseSemaphore(semaphoreFull, 1, NULL);
    }

    isClosed = true;

    return 0;
}

DWORD WINAPI Consumer(LPVOID lpParam) {
    while (true) {
        WaitForSingleObject(semaphoreFull, INFINITE);

        WaitForSingleObject(mutexBuffer, INFINITE);
        if (!buffer.empty()) {
            int item = buffer.front();
            buffer.pop();
            std::cout << "Consumed: " << item << std::endl;
            ReleaseMutex(mutexBuffer);

            ReleaseSemaphore(semaphoreEmpty, 1, NULL);
        }
        else if (isClosed) {
            ReleaseMutex(mutexBuffer);
            break;
        }
        else {
            ReleaseMutex(mutexBuffer);
        }

        Sleep(sleepConsumer);
    }

    return 0;
}
//
// Created by Дмитрий on 11/20/25.
//

#ifndef RUNNER_H
#define RUNNER_H

typedef struct ProcessHandle ProcessHandle;

/*
startProcess - Запучкает процесс в фоне

params:
    program - Путь к программе для запуска
    argv - Массив строк аргументов
return:
    Указатель на ProcessHandle | null(при ошибке)

ProcessHandle - идентификатор на запузенный процесс
*/
ProcessHandle* startProcess(
    const char* program,
    char* const argv[]
);

/*
waitUntilProcessEnd - Ожидание пока процесс закончится

params:
    handle - указатель на процесс
    exit_code - код возврата дочернего процесса
return:
    0 при успехе, -1 при ошибке
*/
int waitUntilProcessEnd(
    ProcessHandle* handle,
    int* exit_code
);

/*
endProcess - Завершает процесс

params:
    handle - указатель на процесс
*/
void endProcess(
    ProcessHandle* handle
);

#endif //RUNNER_H

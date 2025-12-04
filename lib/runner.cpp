// process_runner.cpp
#include "runner.h"

#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <csignal>
#endif


/*
 Структура ProcessHandle
 win:
    pi(process info) - информация о процессе(структура WinAPI)
unix:
    pid - идентификатор процесса
 */
struct ProcessHandle {
    #ifdef _WIN32
        PROCESS_INFORMATION pi;
    #else
        pid_t pid;
    #endif
};

/*
    Вспомогательная функция для Windows для запуска процесса через 
    CreateProcessA. CreateProcessA ожидает на вход одну строку для командной
    строки, а в функции winBuildCommand мы собираем название программы для
    запуска и набор аргументов в одну сроку вида:
        "program" "arg1" ... "argn"
    Начинаем с arg1, а не с arg0, т.к. arg0 == program
 */
#ifdef _WIN32
static char* winBuildCommand(const char* program, char* const argv[]) {
    std::string cmd = "\"";
    cmd += program;
    cmd += "\"";

    if (argv) {
        for (int i = 1; argv[i] != nullptr; i++) {
            cmd += " \"";
            cmd += argv[i];
            cmd += "\"";
        }
    }

    // Переводим string -> char* т.к. CreateProcessA на вход принимает char*
    char* buff = (char*)malloc(cmd.size() + 1); // +1 - нулевой символ(\0)
    if (!buff) return nullptr;
    strcpy(buff, cmd.c_str());
    return buff;
}
#endif

/*
Запуск процесса в фоне
win:
    через CreateProcessA
unix:
    через fork() + execvp()
 */
ProcessHandle *startProcess(const char *program, char *const argv[]) {
    if (!program || !argv || !argv[0]) {
        fprintf(stderr, "startProcess: Invalid or NULL argements\n");
        return nullptr;
    }

    #ifdef _WIN32
        char* cmdline = winBuildCommand(program, argv);
        if (!cmdline) {
            fprintf(stderr, "startProcess: Can't build command\n");
            return nullptr;
        }

        STARTUPINFOA si; // Информация о страте процесса
        PROCESS_INFORMATION pi; // Информация о процессе

        // Обнуляем si и pi во избежание ошибок из-за мусора
        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));

        // Дозаполняем обязательное поле STARTUPINFOA - размер структуры si
        si.cb = sizeof(si);

        // Запуск процесса
        BOOL ok = CreateProcessA(
            NULL,
            cmdline,    // lpCommandLine - строк для запуска процесса
            NULL, NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi
        );

        // cmdline - больше не нужна, поэтому очищаем память
        free(cmdline);

        if (!ok) {
            DWORD err = GetLastError();
            fprintf(stderr, "Can't complete CreateProcessA, error = %lu\n", (unsigned long)err);
            return nullptr;
        }

        ProcessHandle* handle = (ProcessHandle*)malloc(sizeof(ProcessHandle));
        if (!handle) {
            // при ошибке освобождаем память от дескрипторов pi
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return nullptr;
        }

        handle->pi = pi;
        return handle;

    #else
        // создаем процесс
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            return nullptr;
        }

        if (pid == 0) {
            // заменяет текущий процесс программой program
            execvp(program, argv);

            // Если отрабатывает код ниже => execvp не отработал
            perror("execvp");
            _exit(127);
        }

        // В родительском процессе выделяем память под ProcessHandle
        ProcessHandle* handle = (ProcessHandle*) malloc(sizeof(ProcessHandle));
        if (!handle) {
            kill(pid, SIGKILL);
            return nullptr;
        }

        handle->pid = pid;
        return handle;
    #endif
}

int waitUntilProcessEnd(ProcessHandle *handle, int *exit_code) {
    if (!handle) {
        fprintf(stderr, "waitUntilProcessEnd: argument is null\n");
        return -1;
    }

    #ifdef _WIN32
        // Ждём INFINITE пока процесс не завершится :)
        DWORD res = WaitForSingleObject(handle->pi.hProcess, INFINITE);
        if (res == WAIT_FAILED) {
            DWORD err = GetLastError();
            fprintf(stderr, "WaitForSingleObject WAIT_FAILED, error = %lu\n", (unsigned long)err);
            return -1;
        }

        DWORD code = 0;
        if (!GetExitCodeProcess(handle->pi.hProcess, &code)) {
            DWORD err = GetLastError();
            fprintf(stderr, "GetExitCodeProcess can't get exit code, error = %lu\n", (unsigned long)err);
            return -1;
        }

        if (exit_code) *exit_code = (int)code;

        // Закрываем дескрипторы
        CloseHandle(handle->pi.hProcess);
        CloseHandle(handle->pi.hThread);
        handle->pi.hProcess = NULL;
        handle->pi.hThread  = NULL;

        return 0;

    #else
        int status = 0;
        // 0 -> блочное ожидание(пока не завершится)
        pid_t res = waitpid(handle->pid, &status, 0);

        if (res < 0) {
            perror("waitpid");
            return -1;
        }

        int code = 0;
        if (WIFEXITED(status)) {
            code = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            // WIFSIGNALED(status) - возвращает сигнал например SIGKILL или SIGSEGV
            code = -WTERMSIG(status); // со знаком - обозначим что завершено сигналом
        } else {
            code = -1;
        }

        if (exit_code) {
            *exit_code = code;
        }

        // pid = -1 - процесс больше не существует
        handle->pid = -1;

        return 0;
    #endif
}

void endProcess(ProcessHandle *handle) {
    if (!handle) return;
    #ifdef _WIN32
        // На всякий случай если дескрипторы не закрылись
        if (handle->pi.hProcess) {
            CloseHandle(handle->pi.hProcess);
        }
        if (handle->pi.hThread) {
            CloseHandle(handle->pi.hThread);
        }
    #endif

    free(handle);
}

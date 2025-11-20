# ChildParentProcesses

## Задание

```
Написать на языке C или C++ библиотеку, которая 
содержит функцию для запуска программ в фоновом 
режиме, с возможностью подождать завершения 
работы дочерней программы и получить код ответа. 

Написать тестовую утилиту для проверки библиотеки. 
И библиотека и утилита должны быть кроссплатформенными."
```

## Структура проекта
```
ChildParentProcesses/
├── lib/                 # Папка библиотеки
│   ├── runner.h         # Код библиотеки
│   └── runner.cpp       # Заголовочный файл библиотеки
├── child.cpp            # Программа дочернего процесса
├── parent.cpp           # Программа родительского процесса
├── CMakeLists.txt       # Файл для сборки проекта с помощью CMake
└── README.md            # Документация
```

## Библиотека (runner.h)
**Методы**:
1. startProcess - Запучкает процесс в фоне
```
params:
    program - Путь к программе для запуска
    argv - Массив строк аргументов
return:
    Указатель на ProcessHandle | null(при ошибке)

ProcessHandle - идентификатор на запузенный процесс
```

2. waitUntilProcessEnd - Ожидание пока процесс закончится
```
params:
    handle - указатель на процесс
    exit_code - код возврата дочернего процесса
return:
    0 при успехе, -1 при ошибке
```

3. endProcess - Завершает процесс
```
params:
    handle - указатель на процесс
return:
   void
```

## Запуск
1. Сборка CMake
```bash
rm -rf build # если до этого делали сборку
mkdir build
cd build
cmake ..
cmake --build .
```

2. Запуск(Пример)
```bash
./parent ./child ./child 
```

```
# Пример вывода
[Parent] Starting CHILD PROCESS A with arg: 2
[Parent] Starting CHILD PROCESS B with arg: 4
[Parent] ALL CHILD PROCESS successfully started
[Child A] START CHILD PROCESS for 2 sec
[Child B] START CHILD PROCESS for 4 sec
[Parent] PARENT PROCESS progress 0/5
[Parent] PARENT PROCESS progress 1/5
[Child B] PROGRESS 0/4
[Child A] PROGRESS 0/2
[Parent] PARENT PROCESS progress 2/5
[Parent] PARENT PROCESS progress 3/5
[Child B] PROGRESS 1/4
[Child A] PROGRESS 1/2
[Parent] PARENT PROCESS progress 4/5
[Parent] PARENT PROCESS progress 5/5
[Parent] WAIT until children finish...
[Child B] PROGRESS 2/4
[Child A] PROGRESS 2/2
[Child A] FINISH PROCESS with code = 2
[Parent] CHILD PROCESS A finished with exit code 2
[Child B] PROGRESS 3/4
[Child B] PROGRESS 4/4
[Child B] FINISH PROCESS with code = 4
[Parent] CHILD PROCESS B finished with exit code 4
[Parent] All CHILD PROCESSES finished. Parent exiting.
```

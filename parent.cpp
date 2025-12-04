#include "lib/runner.h"
#include <bits/stdc++.h>

using namespace std;

#ifdef _WIN32
    #include <windows.h>
    void msleep(unsigned ms) { Sleep(ms); }
#else
    #include <unistd.h>
    void msleep(unsigned ms) { usleep(ms * 1000); }
    // умножаем на 1000 т.к. usleep принимает на вход МИКРОсекунды
#endif

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage example\n" << argv[0] << " " << "<child programm(FE: ./child)>" << endl;
        return 1;
    }

    vector<ProcessHandle*> children;
    children.reserve(argc - 1);

    for (int i = 1; i < argc; i++) {
        const char *child_prog = argv[i];

        // дочерний процесс будет жить i * 2 секунд
        string child_lifetime = to_string(i * 2); // дочерний процесс эивет i * 2 секунд
        char arg[child_lifetime.size() + 1];
        strcpy(arg, child_lifetime.c_str());

        char *child_argv[] = {
            const_cast<char *>(child_prog),
            arg,
            nullptr
        };

        string label(1, 'A' + i - 1);

        cout << "[Parent] Starting CHILD PROCESS " << label << " with arg: " << i * 2 << endl;
        ProcessHandle *child = startProcess(child_prog, child_argv);

        if (!child) {
            cerr << "[Parent] FAILED TO START CHILD PROCESS" << label << endl;

            // завершаем уже запущенных детей
            for (auto *h : children) {
                int tmp_code = 0;
                waitUntilProcessEnd(h, &tmp_code);
                endProcess(h);
            }
            return 1;
        }

        children.push_back(child);
    }

    cout << "[Parent] ALL CHILD PROCESS successfully started" << endl;


    for (int i = 0; i <= 5; i++) {
        msleep(350);
        cout << "[Parent] PARENT PROCESS progress " << i << "/5" << endl;
    }
    cout << "[Parent] WAIT until children finish...\n";

    for(int i = 0; i < children.size(); i++) {
        int code = 0;
        string fn_label(1, 'A' + i);

        if (waitUntilProcessEnd(children[i], &code) != 0) {
            cerr << "[Parent] ERROR while waiting CHILD PROCESS " << fn_label << endl;
            endProcess(children[i]);
            continue;
        }

        cout << "[Parent] CHILD PROCESS " << fn_label << " finished with exit code " << code << endl;

        endProcess(children[i]);
    }

        cout << "[Parent] All CHILD PROCESSES finished. Parent exiting." << endl;
        return 0;
}

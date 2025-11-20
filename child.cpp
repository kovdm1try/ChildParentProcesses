#include <bits/stdc++.h>

using namespace std;

int main(int argc, char *argv[]) {
    int time_to_work = 3;
    if (argc > 1) {
        int v = atoi(argv[1]);
        if (v > 0) time_to_work = v;
    }

    string label(1, 'A' + time_to_work / 2 - 1);
    cout << "[Child " << label << "] START CHILD PROCESS for " << time_to_work << " sec" << endl;

    for (int i = 0; i <= time_to_work; i++) {
        this_thread::sleep_for(chrono::seconds(1));
        cout << "[Child "<< label << "] PROGRESS " << i << "/" << time_to_work << endl;
    }

    cout << "[Child "<< label << "] FINISH PROCESS with code = " << time_to_work << endl;

    return time_to_work;
}

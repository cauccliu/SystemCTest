#include "sysc/kernel/sc_externs.h"
#include "sysc/kernel/sc_module.h"
#include "sysc/kernel/sc_simcontext.h"
#include <systemc>

using namespace sc_core;
using namespace std;

SC_MODULE(Stage) {
    SC_CTOR(Stage) {
        cout << sc_time_stamp() << ": Elaboration: constructor" << endl;
        SC_THREAD(thread);
    }

    ~Stage() {
        cout << sc_time_stamp() << ": Cleanup: desctructor" << endl;
    }

    void thread() {
        cout << sc_time_stamp() << ": Execution.initialization" << endl;
        int i = 0;
        while (true) {
            wait(1, SC_SEC);
            cout << sc_time_stamp() << ": Execution.simulation" << endl;
            if (++i >= 2) {
                sc_stop();
            }
        }
    }

    void before_end_of_elaboration() {
        cout << "before end of elaboration" << endl;
    }

    void end_of_elaboration() {
        cout << "end of elaboration" << endl;
    }

    void start_of_simulation() {
        cout << "start of simulation" << endl;
    }

    void end_of_simulation() {
        cout << "end of simulation" << endl;
    }
};

int sc_main(int argc, char **argv) {
    Stage stage("stage");
    sc_start();
    return 0;
}
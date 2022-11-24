#include <bits/stdc++.h>

#include "perfect_power.h"

int main(int argc, char *argv[]) {

    // Verify number of arguments for program
    if (argc != 4) {
        cout << "Program usage: " << argv[0] << " M R inputfile | M = number of Mappers, R = number of Reducers\n";
        return -1;
    }

    // Converting arguments to their type
    int numberOfMappers = atoi(argv[1]);
    int numberOfReducers = atoi(argv[2]);
    if (numberOfMappers || numberOfReducers == 0) {
        cout << "ERROR: Invalid arguments!\n";
    }
    string filename = argv[3];

    // Initialization of PerfectPower program and execution of it
    PerfectPower *perfectPower = new PerfectPower(numberOfMappers, numberOfReducers, filename);
    perfectPower->execute();
    delete perfectPower;

    return 0;
}

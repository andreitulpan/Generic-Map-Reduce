#include "reducer.h"

// Reducer Class Constructor
template <typename K, typename V>
Reducer<K, V>::Reducer(vector<unordered_map<K, list<V>>> *maps_, list<int> *reducerList_, K ID_, pthread_barrier_t *barrier_, int nrOfMappers_) {
    maps = maps_;
    reducerList = reducerList_;
    ID = ID_;
    barrier = barrier_;
    nrOfMappers = nrOfMappers_;
}

// Reducer Class Destructor
template <typename K, typename V>
Reducer<K, V>::~Reducer() {}

// Method to execute the Reducer
template <typename K, typename V>
void Reducer<K, V>::execute() {

    // Concatenate all the partial-list from
    // mappers which correspond to this reducer
    for (int i = 0; i < nrOfMappers; i++) {
        auto it = (*maps)[i].find(ID);
        reducerList->insert(reducerList->end(), (it->second).begin(), (it->second).end());
    }

    // Select only the unique elements for the final list
    reducerList->sort();
    reducerList->unique();

    // Write the result in the output file
    ofstream outputFile("out" + to_string(ID + 2) + ".txt");
    outputFile << reducerList->size();
    outputFile.close();


}

// Static method to execute the Reducer from a thread
template <typename K, typename V>
void* Reducer<K, V>::pthread_execute(void *arg) {

    // Get instance of Reducer from thread
    Reducer<K, V> *reducer = (Reducer<K, V>*)arg;

    // Wait all the Mappers to finish the work and all Reducers to start
    pthread_barrier_wait(reducer->barrier);

    // Execute the Reducer
    reducer->execute();

    // Exit from the thread
    pthread_exit(NULL);
}

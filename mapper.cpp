#include "mapper.h"

// Mapper Class Constructor
template <typename K, typename V>
Mapper<K, V>::Mapper(unordered_map<K, list<V>> *map_, priority_queue<InputFile, vector<InputFile>, CompareFile> *filesQueue_, pthread_mutex_t *mutex_, pthread_barrier_t *barrier_, int nrOfReducers_, FUNC f_) {
    map = map_;
    filesQueue = filesQueue_;
    mutex = mutex_;
    barrier = barrier_;
    nrOfReducers = nrOfReducers_;
    f = f_;
}

// Mapper Class Destructor
template <typename K, typename V>
Mapper<K, V>::~Mapper() {}

// Method to execute the Mapper
template <typename K, typename V>
void Mapper<K, V>::execute() {

    // Initialize partial-lists for Reducers
    for (int i = 0; i < nrOfReducers; i++) {
        list<V> list;
        map->insert({i, list});
    }

    // While PQ have files for processing
    while (!filesQueue->empty()) {

        // Critic zone for threads
        // (only one can get a file at a point of time)
        int r = pthread_mutex_lock(mutex);
        if (r) {
            printf("ERROR: Mutex lock failed!\n");
            exit(-1);
        }

        // Getting a file from PQ
        string filename;
        bool emptyQueue = true;
        if (!filesQueue->empty()) {
            filename = filesQueue->top().name;
            filesQueue->pop();
            emptyQueue = false;
        }

        r = pthread_mutex_unlock(mutex);
        if (r) {
            printf("Eroare la deblocarea mutex-ului");
            exit(-1);
        }
        // End of critic zone

        // If PQ is empty exit from function
        if (emptyQueue) {
            return;
        }

        // Read from input file and apply function
        ifstream file (filename);
        if (file.is_open()) {
            int inputs;
            string buffer;

            // Get number of lines
            getline(file, buffer);
            stringstream data1(buffer);
            data1 >> inputs;

            // For every element of file execute the f on it
            for (int i = 0; i < inputs; i++) {
                if (!getline (file,buffer))
                    break;

                // Get the element from the input
                stringstream data2(buffer);
                V element;
                data2 >> element;

                // Apply function on elements for every type of reducer
                for (int j = 0; j < nrOfReducers; j++) {
                    bool result = f((void *)&element, (void *)&j);

                    // If the element is valid, we put it in a partial-list
                    if (result) {
                        auto it = map->find(j);
                        it->second.push_back(element);
                    }
                }
            }

            file.close();
        } else {
            cout << "ERROR: Unable to open " << filename << "\n";
        }
    }
}

// Static method to execute the Mapper from a thread
template <typename K, typename V>
void* Mapper<K, V>::pthread_execute(void *arg) {

    // Get instance of Mapper from thread
    Mapper<K, V> *mapper = (Mapper<K, V>*)arg;

    // Execute the Mapper
    mapper->execute();

    // Wait all the Mappers to finish the work and all Reducers to start
    pthread_barrier_wait(mapper->barrier);

    // Exit from the thread
    pthread_exit(NULL);
}

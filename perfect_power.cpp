#include "perfect_power.h"

// Binary serach for checking if a number is perfect power
int isPerfectPower(long start, long end, long number, long exp) {

    long mid = (start + end) / 2;

    if (start > end) {
        return 0;
    }

    if (pow(mid, exp) == number) {
        return 1;
    } else if (pow(mid, exp) > number) {
        return isPerfectPower(start, mid - 1, number, exp);
    } else {
        return isPerfectPower(mid + 1, end, number, exp);
    }
}

// Function to check if a number is perfect power at an exponent
int f(void *test, void *exponent) {
    int number = *((int *)test);
    int exp = *((int *)exponent) + 2;

    // Exclude the 0 number
    if (number == 0) {
        return 0;
    }

    // Bordering the number betwheen two powers of 2
    double end = 0;
    long long nr = 1;
    while (nr <= (long)number) {
        nr *= 2;
        end++;
    }
    double start = end - 1;

    // Initialize start and end for binary search (2^(x/exp))
    start = floor(pow(2, start / exp));
    end = ceil(pow(2, end / 2));


    return isPerfectPower((long)start, (long)end, (long)number, (long)exp);
}

// PerfectPower Class Constructor
PerfectPower::PerfectPower(int numberOfMappers_, int numberOfReducers_, string filename_) {
    numberOfMappers = numberOfMappers_;
    numberOfReducers = numberOfReducers_;
    filename = filename_;
}

// PerfectPower Class Destructor
PerfectPower::~PerfectPower() {

    // Dealocate memory for mappers
    for (int i = 0; i < numberOfMappers; i++) {
        delete mappers[i];
    }

    // Dealocate memory for reducers
    for(int i = 0; i < numberOfReducers; i++) {
        delete reducers[i];
    }
}

// Public method of class to execute the program from main
void PerfectPower::execute() {
    init();
    getInputFiles();
    executeThreads();
}

// Getting the size for a file using fstat
long int PerfectPower::fileSize(string filename) {

    // Open the file
    int file=0;
    if((file=open(filename.c_str(),O_RDONLY)) < -1)
        return 1;

    // Geting properties with fstat
    struct stat fileStat;
    if(fstat(file,&fileStat) < 0)
        return 1;

    // Close the file
    close(file);

    // Return size of the file
    return fileStat.st_size;
}

// Open input file and get all the files in a PQ
void PerfectPower::getInputFiles() {
    ifstream file (filename);
    if (file.is_open()) {
        int numberOfFiles;
        string buffer;

        // Get number of files for input
        getline(file, buffer);
        stringstream data1(buffer);
        data1 >> numberOfFiles;

        for (int i = 0; i < numberOfFiles; i++) {
            // If file ends and number of files is bigger stop
            if (!getline (file,buffer))
                break;

            // Getting the file name
            stringstream data2(buffer);
            string inputFileName;
            data2 >> inputFileName;

            // Initialize an InputFile struct with
            // file name and size, and put it in PQ
            InputFile inputFile(inputFileName, fileSize(inputFileName));
            filesQueue.push(inputFile);

        }

        file.close();
    } else {
        cout << "ERROR: Unable to open " << filename << endl;
    }
}

void PerfectPower::init() {
    mappers.resize(numberOfMappers);
    reducers.resize(numberOfReducers);

    // Initialize maps for Mappers
    for (int i = 0; i < numberOfMappers; i++) {
        unordered_map<int, list<int>> map;
        maps.push_back(map);
    }

     // Initialize lists for Reducers
    for (int i = 0; i < numberOfReducers; i++) {
        list<int> reducerList;
        lists.push_back(reducerList);
    }
}

void PerfectPower::executeThreads() {

    // Initialize threads and variables for them
    int numberOfThreads = numberOfMappers + numberOfReducers;
    pthread_t threads[numberOfThreads];
    int id, r;
    void *status;

    // Initialize barrier for Mappers & Reducers
    pthread_barrier_t barrier;
    r = pthread_barrier_init(&barrier, NULL, numberOfThreads);
	if (r) {
		printf("ERROR: Barrier init failed!\n");
		exit(-1);
	}

    // Initialize mutex for PQ operations
    pthread_mutex_t mutex;
    r = pthread_mutex_init(&mutex, NULL);
    if (r) {
        printf("ERROR: Mutex init failed!\n");
        exit(-1);
    }

    // Create Mappers & Reducers threads
    for (id = 0; id < numberOfThreads; id++) {

        if (id < numberOfMappers) {
            // Init a Mapper, execute it in a thread and save a reference for the destructor
            Mapper<int, int> *mapper = new Mapper<int, int>(&(maps[id]), &filesQueue, &mutex, &barrier, numberOfReducers, f);
            mappers[id] = mapper;
            r = pthread_create(&threads[id], NULL, mapper->pthread_execute, (void*)mapper);
        } else {
            // Init a Reducer, execute it in a thread and save a reference for the destructor
            Reducer<int, int> *reducer = new Reducer<int, int>(&maps, &(lists[id - numberOfMappers]), id - numberOfMappers, &barrier, numberOfMappers);
            reducers[id - numberOfMappers] = reducer;
            r = pthread_create(&threads[id], NULL, reducer->pthread_execute, (void*)reducer);
        }

        if (r) {
            printf("ERROR: Thread %d create failed!\n", id);
            exit(-1);
        }
    }

    // Wait for the threads to execute
    for (id = 0; id < numberOfThreads; id++) {
        r = pthread_join(threads[id], &status);

        if (r) {
            printf("ERROR: Thread %d waiting failed!\n", id);
            exit(-1);
        }
    }

    // Destroy the Mutex
    r = pthread_mutex_destroy(&mutex);
	if (r) {
		printf("ERROR: Mutex destroy failed!\n");
		exit(-1);
	}

    // Destroy the Barrier
    r = pthread_barrier_destroy(&barrier);
    if (r) {
		printf("ERROR: Barrier destroy failed!\n");
		exit(-1);
	}
}

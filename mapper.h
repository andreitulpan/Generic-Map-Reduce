#pragma once

#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

typedef int (*FUNC) (void *, void *);

struct InputFile {
    string name;
    long int size;

    InputFile(string name, long int size) : name(name), size(size) {}
};

struct CompareFile {
    bool operator()(InputFile const &file1, InputFile const &file2) {
        return file1.size < file2.size;
    }
};

template <typename K, typename V>
class Mapper {
private:
    unordered_map<K, list<V>> *map;
    priority_queue<InputFile, vector<InputFile>, CompareFile> *filesQueue;
    pthread_mutex_t *mutex;
    pthread_barrier_t *barrier;
    FUNC f;
    int nrOfReducers;

public:
    Mapper(unordered_map<K, list<V>> *map_, priority_queue<InputFile, vector<InputFile>, CompareFile> *filesQueue_, pthread_mutex_t *mutex_, pthread_barrier_t *barrier_, int nrOfReducers_, FUNC f_);
    ~Mapper();
    void execute();
    static void* pthread_execute(void *arg);
};

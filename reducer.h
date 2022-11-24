#pragma once

#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

template <typename K, typename V>
class Reducer {
private:
    vector<unordered_map<K, list<V>>> *maps;
    list<V> *reducerList;
    K ID;
    pthread_barrier_t *barrier;
    int nrOfMappers;

public:
    Reducer(vector<unordered_map<K, list<V>>> *maps_, list<int> *reducerList_, K ID_, pthread_barrier_t *barrier_, int nrOfMappers_);
    ~Reducer();
    void execute();
    static void* pthread_execute(void *arg);
};

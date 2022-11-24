#pragma once
#include <bits/stdc++.h>

#include "mapper.cpp"
#include "reducer.cpp"

using namespace std;

class PerfectPower {
private:
    vector<Mapper<int, int>*> mappers;
    vector<Reducer<int, int>*> reducers;
    vector<unordered_map<int, list<int>>> maps;
    vector<list<int>> lists;
    int numberOfMappers;
    int numberOfReducers;
    string filename;
    priority_queue<InputFile, vector<InputFile>, CompareFile> filesQueue;

    long int fileSize(string filename);
    void init();
    void getInputFiles();
    void executeThreads();

public:
    PerfectPower(int numberOfMappers_, int numberOfReducers_, string filename_);
    ~PerfectPower();
    void execute();
};

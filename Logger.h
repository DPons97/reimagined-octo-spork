//
// Created by dPons on 3/20/19.
//

#pragma once

#ifndef OCTOSPORK_LOGGER_H
#define OCTOSPORK_LOGGER_H

#include <fstream>

using namespace std;

/**
 * Custom logger class
 */
class Logger {

private:
    string LastMessage;

    string FileName;

    fstream * LogFile;

    bool print;
public:

    Logger(bool print = false);

    Logger(const string &fileName, bool print=false);

    string getLastMessage() const;

    void writeLog(const string &ToWrite);

    ~Logger();
};


#endif //OCTOSPORK_LOGGER_H

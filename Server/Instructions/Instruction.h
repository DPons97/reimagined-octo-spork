//
// Created by dpons on 5/19/19.
//

#ifndef OCTOSPORK_INSTRUCTION_H
#define OCTOSPORK_INSTRUCTION_H

#include "../../Logger.h"

#include <vector>
#include <map>

#include <strings.h>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <thread>
#include <mutex>
#include <dirent.h>
#include <sys/stat.h>

class Instruction {

public:
    string name;

    int nodeSocket;

    int nodePort;

    Logger * log;

    // Map of current open instructions: int PID, int SOCKET
    std::map<int, int> instructions;

    // Vector of memory that is shared with this node
    vector<void*> sharedMemory;

    Instruction(const string &name, const map<int, int> &instructions, vector<void*> sharedMemory);

    Instruction(const string &name, const map<int, int> &instructions, void * sharedMemory);

    virtual void start(int socket, int port, std::vector<std::string> args);

    virtual void start(int socket, int port);

    virtual string toString();

    virtual ~Instruction();

    int getNodeSocket() const;

protected:
    static std::mutex instrLock;

    // Methods
    bool sendMessage(int instrCode, const std::vector<string> &args = std::vector<string>());

    bool getAnswerCode(string& outCode, int instrSocket);

    int startInstruction(int instrCode, std::vector<string> args = std::vector<string>());

    void disconnect(int instrPid = 0);

private:

};


#endif //OCTOSPORK_INSTRUCTION_H

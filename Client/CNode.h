//
// Created by luca collini on 3/26/19.
//

#ifndef OCTOSPORK_CNODE_H
#define OCTOSPORK_CNODE_H
#include "../Logger.h"
#include <vector>
#include <map>
#include <list>


/**
 * Generic child created during execution
 */
typedef struct {
    int pid;
    int socket;
} child;

/**
 * Default client node handler
 */
class CNode {

public:
    CNode(int portno, char * hostname, int id, int abs_x, int abs_z, int theta,
            int upNeighbour, int bottomNeighbour, int leftNeighbour, int rightNeighbour);

    void listen();

    virtual ~CNode();

protected:

private:
    Logger * log;

    int sockfd;

    char * hostname;

    int abs_x;

    int abs_z;

    list<child*> children;

    map<int, string> execNames;

    void sendMessage(int sockfd, int cod);

    void error(const char * msg);

    vector<string> split(const string& str,const string& sep);

    vector<char *> split_char(string str,string sep);

    int newSocket(int port);

    child* findChild(int pid);

    void killChild(child * toKill);

    void cleanChild(child * toKill);

    void readCodeFile();

};


#endif //OCTOSPORK_CNODE_H

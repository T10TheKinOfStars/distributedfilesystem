#ifndef DHTCONTROLLER_H
#define DHTCONTROLLER_H

#include "FileStore.h"
#include <iostream>
#include <vector>
#include <ctime>
#include "common.h"
#include "sha256.h"
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;      

class DHTController {
private:
    std::vector<NodeID> dht;
    NodeID pre;
    NodeID cur;
    NodeID succ;
    bool inited;
    std::mutex *xlock = new std::mutex();
public:
    boost::shared_ptr<FileStoreClient> getClientConn(const std::string &ip, int port);
    void serverinit(std::string ip, int port);
    void setCur(const std::string &ip, int port);
    NodeID getCur();
    DHTController();
    void setInitFlag();
    bool checkFtbInit();
    void setFingerTB(const std::vector<NodeID> &nodes);
    NodeID findPred3(const std::string &key);
    NodeID findPred4(const std::string &key);
    NodeID getSucc();
    NodeID getPred();
    NodeID RPCGetNodeSucc(NodeID node);
    NodeID RPCFindPred(NodeID node, const std::string &key);
    bool isBetween(const std::string &left, const std::string &key, const std::string &right);
    bool isBetweenE(const std::string &left, const std::string &key, const std::string &right);
    void setPred(NodeID node);
    void updateFingertb(int idx, NodeID node);
    std::vector<NodeID> getFingertb();

    void join3(const NodeID &node);
    void join4(const NodeID &node);
    void init_ftb(const NodeID& node);
    void update_others();
    void remove();
    void stabilize();
    void notify(NodeID node);
    void fixFingertb();
private:
    std::string addID(const std::string& id, int exp);
    std::string minusID(const std::string& id, int exp);
    std::string stradd(const std::string& str1, const std::string& str2);
    std::string strsub(std::string str1, const std::string& str2);
};
#endif

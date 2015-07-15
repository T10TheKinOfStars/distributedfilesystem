#ifndef DHTCONTROLLER_H
#define DHTCONTROLLER_H

#include "FileStore.h"
#include <iostream>
#include <vector>
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
        boost::shared_ptr<FileStoreClient> getClientConn(const std::string &ip, int port);
    public:
        void setCur(const std::string &ip, int port);
        NodeID getCur();
        DHTController();
        void setInitFlag();
        bool checkFtbInit();
        void setFingerTB(const std::vector<NodeID> &nodes);
        NodeID findPred(const std::string &key);
        NodeID getSucc();
        NodeID getPred();
        NodeID RPCGetNodeSucc(NodeID node);
        NodeID RPCFindPred(NodeID node, const std::string &key);
        bool isBetween(const std::string &left, const std::string &key, const std::string &right);
        bool isBetweenE(const std::string &left, const std::string &key, const std::string &right);
        void setPred(NodeID node);
        void updateFingertb(int idx, NodeID node);
        std::vector<NodeID> getFingertb();

        std::vector<RFile> pullFiles();
        void pushFiles();

        void join(NodeID node);
        void remove();
        void stabilize();
        void notify(NodeID node);
        void fixFingertb();
};
#endif

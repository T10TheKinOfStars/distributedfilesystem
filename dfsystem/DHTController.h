#ifndef DHTCONTROLLER_H
#define DHTCONTROLLER_H

#include "FileStore.h"
#include <vector>

class DHTController {
    private:
        vector<NodeID> dht(256);
        std::string ip;
        int port;
        NodeID cur;
        NodeID pre;
        NodeID succ;
    public:
        NodeID findSucc(std::string);
        NodeID findPred(std::string);
        NodeID getSucc(std::string);
        NodeID getPred(std::string);
        void setPred(NodeID node);
        void updateFingertb(int idx, NodeID node);
        vector<NodeID> getFingertb();

        vector<RFile> pullFiles();
        void pushFiles();

        void join(NodeID node);
        void remove();
        void stabilize();
        void notify(NodeID node);
        void fixFingertb();
#endif

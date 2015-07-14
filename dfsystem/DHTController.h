#ifndef DHTCONTROLLER_H
#define DHTCONTROLLER_H

#include "FileStore.h"
#include <vector>

class DHTController {
    private:
        vector<NodeID> dht;
        std::string ip;
        int port;
        NodeID cur;
        NodeID pre;
        NodeID succ;
        bool inited;
        boost::share_ptr<FileStoreClient> getClientConn(const std::string &ip, int port) const;
    public:
        DHTController();
        void setInitFlag();
        bool checkFtbInit();
        void setFingerTB(const std::vector<NodeID> &nodes);
        NodeID findSucc(std::string);
        NodeID findPred(std::string);
        NodeID getSucc();
        NodeID getPred();
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

#include "DHTController.h"

void DHTController::setPort(int vport) {
    port = vport;
}

boost::shared_ptr<FileStoreClient> DHTController::getClientConn(const std::string &ip,int port) {
    boost::shared_ptr<TSocket> socket(new TSocket(ip.c_str(), port));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport)); 

    boost::shared_ptr<FileStoreClient> client(new FileStoreClient(protocol));
    try {
        transport->open();
    } catch (...) {
        dprintf("connect to %s:%d failed\n",ip.c_str(),port);
        client = boost::shared_ptr<FileStoreClient>();
    }

    return client;
}

DHTController::DHTController() {
    dht.resize(256);
    inited = false;
}

void DHTController::setInitFlag() {
    inited = true;
}

bool DHTController::checkFtbInit() {
    return inited;
}

void DHTController::setFingerTB(const std::vector<NodeID> &nodes) {
    dht = nodes;
    pre = nodes[0];
}

NodeID DHTController::findPred(const std::string &key) {
    NodeID ret;
    if (isBetweenE(cur.id,key,succ.id))
        return cur;
    //the for loop is closest_preceding_finger func
    for (int i = 255; i >= 0; --i) {
        if (dht[i].id.empty())
            continue;
        if (isBetween(cur.id,dht[i].id,key))
            ret = RPCFindPred(dht[i],key);
    }

    return ret;
}

NodeID DHTController::RPCGetNodeSucc(NodeID node) {
    if (node.id == cur.id)
        return cur;
    std::mutex xlock;
    AutoLock autolock(xlock);
    boost::shared_ptr<FileStoreClient> client = getClientConn(node.ip,node.port);
    if (client.get() == NULL) {
        SystemException se;
        char message[128];
        snprintf(message,128,"connect to %s:%d failed\n",node.ip.c_str(), node.port);
        se.__set_message(message);
        throw se;
    }
    NodeID ret;
    client->getNodeSucc(ret);

    return ret;
}

NodeID DHTController::RPCFindPred(NodeID node, const std::string &key) {
    std::mutex xlock;
    AutoLock autolock(xlock);
    boost::shared_ptr<FileStoreClient> client = getClientConn(node.ip,node.port);
    
    if (client.get() == NULL) {
        SystemException se;
        char message[128];
        snprintf(message,128,"connect to %s:%d failed\n",node.ip.c_str(),node.port);
        se.__set_message(message);
        throw se;
    }
    NodeID ret;
    client->findPred(ret,key);
    return ret;
}

NodeID DHTController::getSucc() {
    return succ;
}

NodeID DHTController::getPred() {
    return pre;
}

void DHTController::setPred(NodeID node) {
    pre = node;
}

void DHTController::updateFingertb(int idx, NodeID node) {

}

std::vector<NodeID> DHTController::getFingertb() {
    return dht;
}

std::vector<RFile> DHTController::pullFiles() {

}

void DHTController::pushFiles() {

}

void DHTController::join(NodeID node) {

}

void DHTController::remove() {

}

void DHTController::stabilize() {

}

void DHTController::notify(NodeID node) {

}

void DHTController::fixFingertb() {

}

bool DHTController::isBetween(const std::string &left, const std::string &key, const std::string &right) {
    if (left < right) {
        return left < key && key < right;
    } else if (left > right) {
        return (left < key && key > right) ||
               (left > key && key < right);
    }
}

bool DHTController::isBetweenE(const std::string &left, const std::string &key, const std::string &right) {
    if (key == right)
        return true;
    else
        return isBetween(left,key,right);
}

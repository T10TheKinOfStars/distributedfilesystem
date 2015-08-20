#include "DHTController.h"

void DHTController::setCur(const std::string &ip, int port) {
    cur.ip = ip;
    cur.port = port;
    cur.id = sha256_ip_port_hex(ip,port);
}

NodeID DHTController::getCur() {
    return cur;
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
    succ = dht[0];
    dprintf("The Succ for %s:%d is %s:%d\n",cur.ip.c_str(),cur.port,succ.ip.c_str(),succ.port);
    for (auto x : dht) 
        std::cout<<x.id<<" ["<<x.ip<<":"<<x.port<<"]\n";
    std::cout<<"=========================================================="<<std::endl;
}

NodeID DHTController::findPred(const std::string &key) {
    dprintf("In findPred. The key is %s\n",key.c_str());
    if (isBetweenE(cur.id,key,succ.id))
        return cur;
    //the for loop is closest_preceding_finger func
    for (int i = 255; i >= 0; --i) {
        if (dht[i].id.empty())
            continue;
        if (isBetween(cur.id,dht[i].id,key))
            return RPCFindPred(dht[i],key);
    }
    //wont reach here
    return cur;
}

NodeID DHTController::RPCGetNodeSucc(NodeID node) {
    if (node.id == cur.id)
        return succ;
    std::mutex xlock;
    AutoLock autolock(xlock);
    boost::shared_ptr<FileStoreClient> client(getClientConn(node.ip,node.port));
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
    dprintf("RPCFINDPRED node: %s:%d\n",node.ip.c_str(),node.port);
    boost::shared_ptr<FileStoreClient> client(getClientConn(node.ip,node.port));
    
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
    dht[idx] = node;
    succ = dht[0];
}

std::vector<NodeID> DHTController::getFingertb() {
    return dht;
}

void DHTController::join(const NodeID& node) {
    try {
        if (node != cur) {
            init_ftb(node);
            update_others();
        }
    } catch(...) {
        SystemException se;
        se.__set_message("join failed.\n");
        throw se;
    }
}

void DHTController::init_ftb(const NodeID& node) {
    boost::shared_ptr<FileStoreClient> client(getClientConn(node.ip,node.port));
    if (client.get() == NULL) {
        SystemException se;
        char message[128];
        snprintf(message,128,"connect to %s:%d failed\n",node.ip.c_str(), node.port);
        se.__set_message(message);
        throw se;
    }
    
    for (int i = 0; i < 256; ++i) {
        std::string succid = addID(cur.id,i);
        if (i > 0 && isBetween(cur.id, succid, dht[i-1].id)) {
        //如果dht[i]的id在dht[i-1].id和node的id之间，说明dht[i]的后继和dht[i-1]是一样的
            dht[i] = dht[i-1];
        } else {
            NodeID succ;
            client->findSucc(succ,succid);
            if (isBetween(succid, cur.id, succ.id))
                //如果当前节点（实际存在于网络中的）在succid和node的后继之间的话，那么succid的后继节点应该是当前节点。故更新之
                succ = cur;
            dht[i] = succ;
        }
    }

    succ = dht[0];
    pre = RPCFindPred(node,cur.id);
}

void DHTController::update_others() {
    for (int i = 0; i < 256; ++i) {
        NodeID pred;
        std::vector<NodeID> remotedht;
        pred = findPred(minusID(cur.id,i));
        boost::shared_ptr<FileStoreClient> client(getClientConn(pred.id,pred.port));
        while (pred.id != cur.id) {
            client->getFingertable(remotedht);
            if (isBetween(pred.id,cur.id,remotedht[i].id))
                client->updateFinger(i, cur);
            else
                break;
            client = boost::shared_ptr<FileStoreClient>();
            pred = findPred(pred.id);
        }
    }
    //remote set pred
    boost::shared_ptr<FileStoreClient> client(getClientConn(succ.id,succ.port));
    client->setNodePred(cur);
}

void DHTController::remove() {
    if (cur == succ)
        //only one node in the network
        return;
    boost::shared_ptr<FileStoreClient> client(getClientConn(succ.id,succ.port));
    client->setNodePred(pre);
    //reset client
    client = boost::shared_ptr<FileStoreClient>();
    for (int i = 0; i < 256; ++i) {
        std::string id = minusID(cur.id,i);
        NodeID pred = findPred(id);

        while (pred.id != cur.id) {
            //this node need fix
            std::vector<NodeID> remotedht;

            client = getClientConn(pre.id,pre.port);
            client->getFingertable(remotedht);
            if (remotedht[i].id == cur.id) {
                client->updateFinger(i,succ);
            } else {
                client = boost::shared_ptr<FileStoreClient>();
                break;
            }
            client = boost::shared_ptr<FileStoreClient>();
            pred = findPred(pred.id);
        }
    }
    //reset curruent node
    for (int i = 0; i < 256; ++i)
        dht[i] = cur;
    pre = cur;
    succ = cur;
}

void DHTController::stabilize() {

}

void DHTController::notify(NodeID node) {

}

void DHTController::fixFingertb() {

}

bool DHTController::isBetween(const std::string &left, const std::string &key, const std::string &right) {
    if (left < right) {
        return (left < key && key < right);
    } else if (left > right) {
        return ((left < key && key > right) ||
               (left > key && key < right));
    } 
    return true;
}

bool DHTController::isBetweenE(const std::string &left, const std::string &key, const std::string &right) {
    if (key == right)
        return true;
    else if (key == left)
        return false;
    else
        return isBetween(left,key,right);
}

std::string DHTController::addID(const std::string& id, int exp) {
//十六进制加    
    double addend = pow(2,exp);
    std::string addendstr = sha256_calc_hex(std::to_string(addend));
    return stradd(id,addendstr);    
}

std::string DHTController::minusID(const std::string& id, int exp) {
//十六进制减    
    return strsub(id,sha256_calc_hex(std::to_string(pow(2,exp))));
}

#define DECTOHEX(x) ((x) >= 10?(x)-10+'a':(x)+'0')
#define HEXTODEC(x) ((x) >= 'a'?(x)-'a'+10:(x)-'0')

std::string DHTController::stradd(const std::string& str1, const std::string& str2) {
    int carry = 0;
    std::string result;
    result.reserve(64);

    for (int i = 63; i >=0; --i) {
        int x = HEXTODEC(str1[i]);
        int y = HEXTODEC(str2[i]);
        int z = x + y + carry; 
        if (z >= 16)
            carry = 1;
        else
            carry = 0;
        result[i] = DECTOHEX(z >= 16 ? z-16 : z);
    }

    return result;
}

std::string DHTController::strsub(std::string str1, const std::string& str2) {
    int borrow = 0;
    bool flag = false;
    std::string result;
    result.reserve(64);

    for (int i = 63; i >= 0; --i) {
        int x = HEXTODEC(str1[i]);
        int y = HEXTODEC(str2[i]);
        borrow = x < y || flag ? 1 : 0;
        if (borrow == 1) {
            str1[i-1] = (int)str1[i] - 1;
        }
        result[i] = DECTOHEX(x + borrow - y);
        if (str1[i-1] < '0') {
            str1[i-1] = '0';
            flag = true;
        } else {
            flag = false;
        }
    }

    return result;
}

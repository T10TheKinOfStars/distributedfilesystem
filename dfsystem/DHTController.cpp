#include "DHTController.h"

void DHTController::serverinit(const std::string ip, int port) {
    cur.__set_ip(ip);
    cur.__set_port(port);
    cur.__set_id(sha256_ip_port_hex(ip,port)); 
    for (int i = 0; i < 256; ++i) 
        dht[i] = cur;

    pre = cur;
    succ = cur;
    inited = true;
}

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

NodeID DHTController::findPred3(const std::string &key) {
    dprintf("In findPred. The key is %s\n",key.c_str());
    if (isBetweenE(cur.id,key,succ.id))
        return cur;
    //the for loop is closest_preceding_finger func
    for (int i = 255; i >= 0; --i) {
        if (isBetween(cur.id,dht[i].id,key))
            return RPCFindPred(dht[i],key);
    }
    //wont reach here
    return cur;
}

NodeID DHTController::findPred4(const std::string &key) {
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

void DHTController::join3(const NodeID& node) {
    try {
        dprintf("in join3\n");
        dprintf("check id\n");
        if (node.id != cur.id) {
            dprintf("node id != cur id\n");
            init_ftb(node);
            update_others();
        }
    } catch(...) {
        SystemException se;
        se.__set_message("join failed.\n");
        throw se;
    }
}

void DHTController::join4(const NodeID& node) {
    boost::shared_ptr<FileStoreClient> client(getClientConn(node.ip,node.port));
    pre.id.clear();
    client->findSucc(succ, cur.id);
    dht[0] = succ;
}

void DHTController::init_ftb(const NodeID& node) {
    dprintf("in init_ftb\n");
    boost::shared_ptr<FileStoreClient> client(getClientConn(node.ip,node.port));
    if (client.get() == NULL) {
        SystemException se;
        char message[128];
        snprintf(message,128,"connect to %s:%d failed\n",node.ip.c_str(), node.port);
        se.__set_message(message);
        throw se;
    }
    dprintf("start to init\n");
    for (int i = 0; i < 256; ++i) {
        std::string succid = addID(cur.id,i);
        if (i > 0 && isBetween(cur.id, succid, dht[i-1].id)) {
        //如果dht[i]的id在dht[i-1].id和node的id之间，说明dht[i]的后继和dht[i-1]是一样的
            dht[i] = dht[i-1];
        } else {
            NodeID succ;
            dprintf("before find succ\n");
            client->findSucc(succ,succid);
            dprintf("after find succ\n");
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
    dprintf("start to update others\n");
    for (int i = 0; i < 256; ++i) {
        NodeID pred;
        std::vector<NodeID> remotedht;
        pred = findPred3(minusID(cur.id,i));
        boost::shared_ptr<FileStoreClient> client(getClientConn(pred.id,pred.port));
        while (pred.id != cur.id) {
            client->getFingertable(remotedht);
            if (isBetween(pred.id,cur.id,remotedht[i].id))
                client->updateFinger(i, cur);
            else
                break;
            client = boost::shared_ptr<FileStoreClient>();
            pred = findPred3(pred.id);
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
        NodeID pred = findPred3(id);

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
            pred = findPred3(pred.id);
        }
    }
    //reset curruent node
    for (int i = 0; i < 256; ++i)
        dht[i] = cur;
    pre = cur;
    succ = cur;
}

void DHTController::stabilize() {
    NodeID succ_pred;
    boost::shared_ptr<FileStoreClient> client;
    AutoLock autolock(xlock);
    if (succ.id == cur.id) {
        succ_pred = pre;
    } else {
       client = getClientConn(succ.id,succ.port);
       client->getNodePred(succ_pred);
    }

    if (succ_pred.id == cur.id)
        return;

    if (isBetween(cur.id,succ_pred.id,succ.id)) {
        succ = succ_pred;
        dht[0] = succ;
    }

    if (succ.id != cur.id) {
        client = getClientConn(succ.id,succ.port);
        client->notify(cur);
    }
}

void DHTController::notify(NodeID node) {
    //告诉别的节点要更新fingettable了 因为有新的节点插进来了
    AutoLock autolock(xlock);
    if (pre.id.empty() || isBetween(pre.id, node.id, cur.id))
        pre = node;
}

void DHTController::fixFingertb() {
    srand(time(NULL));
    int randidx = rand() % 255 + 1;
    NodeID pred = findPred4(addID(cur.id, randidx));
    dht[randidx] = RPCGetNodeSucc(pred);
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

#define DECTOHEX(a) ((a) >= 10? (a) - 10 + 'a': (a) + '0');
#define HEXTODEC(a) ((a) >= 'a'? (a) - 'a' + 10 : (a) - '0')

std::string DHTController::addID(const std::string &id, int exp)
{
    char idstr[65];
    snprintf(idstr, 65, id.c_str());
    int carry = (1 << (exp % 4));
    for(int i = 256 / 4 - exp / 4 - 1; i >= 0; i--) {
        int k = HEXTODEC(idstr[i]);
        k += carry;
        carry = k / 16;
        idstr[i] = DECTOHEX(k % 16);
        if (carry == 0)
            break;
    }

    return idstr;
}

std::string DHTController::minusID(const std::string &id, int exp)
{
    char idstr[65];
    snprintf(idstr, 65, id.c_str());
    int carry = (1 << (exp % 4));
    for(int i = 256 / 4 - exp / 4 - 1; i >= 0; i--) {
        int k = HEXTODEC(idstr[i]);
        k -= carry;
        carry = 0;
        if (k < 0) {
            carry = 1;
            k += 16;
        }
        idstr[i] = DECTOHEX(k);
        if (carry == 0)
            break;
    }

    return idstr;
}

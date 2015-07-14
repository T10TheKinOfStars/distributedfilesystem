#include "DHTController.h"

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

NodeID DHTController::findSucc(std::string) {
    
}

NodeID DHTController::findPred() {

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

vector<NodeID> DHTController::getFingertb() {
    return dht;
}

vector<RFile> DHTController::pullFiles() {

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

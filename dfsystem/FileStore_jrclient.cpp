#include <iostream>
#include "FileStore.h"
#include <unistd.h>
#include "sha256.h"
#include <getopt.h>
#include "common.h"
#include <vector>
#include <fstream>
#include <cstdlib>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>

using namespace std;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using boost::shared_ptr;

int main(int argc, char** argv) {
    std::string ip = argv[1];
    int port = atoi(argv[2]);
    std::string op = argv[3];
    boost::shared_ptr<TSocket> socket(new TSocket(ip, port));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport)); 
    boost::shared_ptr<TMemoryBuffer> buffer(new TMemoryBuffer());
    TJSONProtocol jsonprotocol(buffer);
    FileStoreClient client(protocol);
    try {
        transport->open();

        if (op == "join") {
            try {
                NodeID node;
                node.__set_ip(ip);
                node.__set_port(port);
                node.__set_id(sha256_ip_port_hex(ip,port));
                client.join(node);
            } catch (SystemException se) {
                //format se in json format
                std::cout<<ThriftJSONString(se)<<std::endl;
                return -1;
            }
        } else if (op == "remove") {
            try {
                client.remove();
            } catch (SystemException se) {
                //format se in json format
                std::cout<<ThriftJSONString(se)<<std::endl;
                return -1;
            }
        } else {
            std::cerr<<"operation argument error"<<std::endl;
            return 0;
        }

        transport->close();
    } catch (TException &tx) {
        std::cout<<"ERROR: "<<tx.what()<<std::endl;
        return -1;
    }

    return 0;
}

#include <iostream>
#include "FileStore.h"
#include <getopt.h>
#include <cstdlib>
#include <vector>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

int main(int argc, char* argv[]) {
    if (argc < 8) {
        std::cout<<"not enough arguments"<<std::endl;
        return 0;
    }

    static struct option long_options[] = {
        {"operation" , required_argument, 0, 'o'},
        {"filename", required_argument, 0, 'f'},
        {"user", required_argument, 0, 'u'},
        {0,0,0,0}
    }
    int opt;
    int optidx;
    std::string operation;
    std::string filename;
    std::string user;
    RFile rfile;
    FileMetadata fdata;
    StatusReport status;

    boost::shared_ptr<TTransport> socket(new TSocket(argv[1], atoi(argv[2])));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport)); 
    boost::shared_ptr<TMemoryBuffer> buffer(new TMemoryBuffer());
    boost::shared_ptr<TJSONProtocol> jsonprotocol(new TJSONProtocol(buffer));

    FileStoreClient client(protocol);
    try {
        transport->open();

        while ((opt = getopt(argc-2,argv + 2, "o:f:u", long_options, &optidx)) != -1) {
            //-1 means it reaches end
            switch (opt) {
                case 'o':
                    operation = optarg;
                    break;
                case 'f':
                    filename = optarg;
                    break;
                case 'u':
                    user = optarg;
                    break;
                default:
                    std::cout<<"operation error"<<std<<endl;
                    return -1;
            }
        }

        if (operation == "read") {
            //init rfile
            data.__set_filename(filename);
            data.__set_owner(user);
            rfile.__set_meta(data);
            
            client.readFile(rfile,filename,user);
            //format rfile in json format
            //...
        } else if (operation == "write") {
            //init rfile
            data.__set_filename(filename);
            data.__set_version(0);
            data.__set_owner(user);
            
            //read data from disk
            //...
            std::ifstream ifs(filename.c_str(),ios::binary);

            client.writeFile(status,rfile);
            //format status in json format
            //...
        } else if (operation == "delete") {
            client.deleteFile(status, filename, user);
            //format status in json format
            //...
        } else if (operation == "list") {
            std::vector<RFileMetadata> datas;
            client.listOwnedFiles(datas,user);
            //format datas in json format
            //....
        } else {
            std::cerr<<"operation argument error"<<std::endl;
            return 0;
        }

        transport->close();
    } catch (TException &tx) {
        std::cout<<"ERROR: "<<tx.what()<<std::endl;
    }
}

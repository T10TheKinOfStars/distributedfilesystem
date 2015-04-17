#include <iostream>
#include "FileStore.h"
#include <unistd.h>
#include <getopt.h>
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
    int opt;
    int optidx;
    std::string operation;
    std::string filename;
    std::string user;
    RFile rfile;
    RFileMetadata data;
    StatusReport status;
    
    boost::shared_ptr<TSocket> socket(new TSocket(argv[1], atoi(argv[2])));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport)); 
    boost::shared_ptr<TMemoryBuffer> buffer(new TMemoryBuffer());
    TJSONProtocol jsonprotocol(buffer);

    FileStoreClient client(protocol);
    try {
        transport->open();

        static struct option long_options[] = {
            {"operation" , required_argument, 0, 'o'},
            {"filename", required_argument, 0, 'f'},
            {"user", required_argument, 0, 'u'},
            {0,0,0,0}
        };
        while ((opt = getopt_long(argc-2,argv + 2, "o:f:u", long_options, &optidx)) != -1) {
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
                    std::cout<<"operation error"<<std::endl;
                    return -1;
            }
        }

        if (operation == "read") {
            try {
                client.readFile(rfile,filename,user);
            } catch (SystemException se) {
                //format se in json format
                std::cout<<ThriftJSONString(se)<<std::endl;
                return -1;
            }
            //format rfile in json format
            std::cout<<ThriftJSONString(rfile)<<std::endl;
        } else if (operation == "write") {
            //init rfile
            data.__set_filename(filename);
            data.__set_version(0);
            data.__set_owner(user);
            
            //read data from disk            
            std::ifstream ifs(filename.c_str(),std::ios::binary);
            if (ifs) {
                ifs.seekg(0,ifs.end);
                int len = ifs.tellg();
                ifs.seekg(0,ifs.beg);
                char *buf = new char[len+1];
                ifs.read(buf,len);
                buf[len] = '\0';
                ifs.close();
                delete []buf;
                data.__set_contentLength(len);
                rfile.__set_content(buf);
                rfile.__set_meta(data);
            } else {
                std::cerr<<"read file error!\n";
                return -1;
            }
            try {
                client.writeFile(status,rfile);
            } catch (SystemException se) {
                //format se information in json format
                std::cout<<ThriftJSONString(se)<<std::endl;
                return -1;
            }
            //format status in json format
            std::cout<<ThriftJSONString(status)<<std::endl;
        } else if (operation == "delete") {
            try {
                client.deleteFile(status, filename, user);
            } catch (SystemException se) {
                //format se information in json format
                std::cout<<ThriftJSONString(se)<<std::endl;
                return -1;
            }
            //format status in json format
            std::cout<<ThriftJSONString(status)<<std::endl;
        } else if (operation == "list") {
            std::vector<RFileMetadata> datas;
            try {
                client.listOwnedFiles(datas,user);
            } catch (SystemException se) {
                //format se information in json format
                std::cout<<ThriftJSONString(se)<<std::endl;
                return -1;
            }
            //format datas in json format
            jsonprotocol.writeListBegin(::apache::thrift::protocol::T_STRUCT, datas.size());
            for(int i = 0; i < (signed)datas.size(); i++) {
                datas[i].write(&jsonprotocol);
            }
            jsonprotocol.writeListEnd();

            std::cout << buffer->getBufferAsString()<< std::endl;
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

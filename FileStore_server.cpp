// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "FileStore.h"
#include "md5.h"
#include <ctime>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

FileWorker fwoker;

class FileStoreHandler : virtual public FileStoreIf {
 public:
  FileStoreHandler() {
    // Your initialization goes here
  }

  void listOwnedFiles(std::vector<RFileMetadata> & _return, const UserID& user) {
    // Your implementation goes here
    printf("listOwnedFiles\n");
    if (UserFileMap.find(user) != UserFileMap.end()) {
        fwoker.getfiles(user,_return);
  }

  void writeFile(StatusReport& _return, const RFile& rFile) {
    // Your implementation goes here
    //printf("writeFile\n");
    if (fworker.writefile(rFile) != -1) {
        _return.__set_status(SUCCESSFUL);
    } else {
        _return.__set_status(FAILED);
    }
  }

  void readFile(RFile& _return, const std::string& filename, const UserID& owner) {
    // Your implementation goes here
    //printf("readFile\n");
    if (UserFileMap.find(owner) != UserFileMap.end()) {
        if (fworker.readfile(owner,filename,_return) == -1) {
            SystemException se;
            se.__set_message("read file failed");
            throw se;
        }
    }
  }

  void deleteFile(StatusReport& _return, const std::string& filename, const UserID& owner) {
    // Your implementation goes here
    //printf("deleteFile\n");
    if (UserFileMap.find(owner) == UserFileMap.end()) {
        //not this user, return
        _return.__set_status(FAILED);
    } else {
        if (fworker.deletefile(owner,filename) != -1) {
            _return.__set_status(SUCCESSFUL);
        } else {
            _return.__set_status(FAILED);
            SystemException se;
            se.__set_message("read file failed");
            throw se;
        }
    }
  }   
};

int main(int argc, char **argv) {
  int port = 9090;
  shared_ptr<FileStoreHandler> handler(new FileStoreHandler());
  shared_ptr<TProcessor> processor(new FileStoreProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  fworker.initFolder();
  server.serve();
  return 0;
}

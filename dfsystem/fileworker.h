#ifndef FILEWORKER_H
#define FILEWORKER_H

#include "FileStore.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include "sha256.h"
#include <vector>
#include <string>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;
typedef std::map<std::string, RFileMetadata> NameDataMap;

class FileWorker {    
    private:
        std::map<UserID, NameDataMap> UserFileMap;
        int readFromDisk(std::string path, int fsize, std::string &content);
        int write2Disk(std::string path, const std::string &content);
        int deleteFromDisk(std::string path);
    public:
        std::map<UserID, NameDataMap> getUserFileMap();
        void initFolder();
        int writefile(const RFile &rfile);
        int deletefile(std::string id, std::string filename);
        int readfile(std::string id, std::string filename, RFile &rfile);
        int getfiles(std::string id, std::vector<RFileMetadata> &datas);
};
#endif

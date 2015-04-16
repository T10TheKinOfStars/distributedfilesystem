#ifndef FILEWORKER_H
#define FILEWORKER_H

#include "FileStore.h"
#include "md5.h"
#include <ctime>
#include <cstdlib>
#include <vector>
#include <string>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace std;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

class FileWorker {
    public:
        typedef std::unordered_map<std::string, RFileMetadata> NameDataMap;
    private:
        std::unordered_map<UserID, NameDataMap> UserFileMap;
        int readFromDisk(string path, int fsize, string &content);
        int write2Disk(string path, const string &content);
        int deleteFromDisk(string path);
    public:
        void initFolder();
        int writefile(const RFile &rfile);
        int deletefile(string id, string filename);
        int readfile(string id, string filename, RFile &rfile);
        int getfiles(string id, vector<RFileMetadata> &files);
};
#endif

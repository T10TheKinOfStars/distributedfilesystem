#include "fileworker.h"
#include "md5.h"
#include <fstream>

void FileWorker::initFolder() {
    std::string cmd = "mkdir files";    
    if (system(cmd.c_str()) == -1) {
        SystemException se;
        se.__set_message("create folder failed");
        throw se;
    }
}

int FileWorker::readFromDisk(std::string path, int fsize, std::string &content) {
    std::ifstream ifs(path.c_str(), std::ios::binary);

    if (ifs) {   
        char *buf = new char[fsize + 1];
        ifs.read(buf,fsize);
        buf[fsize] = '\0';
        ifs.close();
    } else {
        return -1;
    }

    return 0;
}

int FileWorker::write2Disk(std::string path, const std::string &content) {
    std::ofstream ofs(path.c_str(), std::ios::binary);

    if (ofs) {
        ofs<<content;
        ofs.close();
    } else {
        return -1;
    }

    return 0;
}

int FileWorker::deleteFromDisk(std::string path) {
    std::string cmd = "rm -rf " + path;
    return system(cmd.c_str());
}

int FileWorker::writefile(const RFile &_rfile) {
    RFile rfile = _rfile;
    RFileMetadata rdata = rfile.meta;
    UserID id = rdata.owner;
    std::string filename = rdata.filename;
    std::string path = ".//files//" + id + "_" + filename;
    SystemException exception;
    if (UserFileMap.find(id) == UserFileMap.end()) {
        //this user doesn't exist
        //create a new entry                
        if (write2Disk(path,rfile.content) != -1) {
            rdata.__set_version(0);            
            rdata.__set_contentHash(md5(rfile.content));
            rdata.__set_created((Timestamp)time(NULL) * 1000);    //need to change
            rdata.__set_updated((Timestamp)time(NULL) * 1000);    //need to change
            rdata.__set_deleted(0);
            NameDataMap data;
            data[filename] = rdata;
            UserFileMap.insert(std::pair<UserID,NameDataMap>(id,data));
        } else {
            return -1;
        }
    } else {
        //this user exists
        NameDataMap files = UserFileMap[id];
        if (files.find(filename) == files.end() && files[filename].deleted != 0) {
            //this file not exists, we need create a new one            
            if (write2Disk(path,rfile.content) != -1) {
                rdata.__set_version(0);
                rdata.__set_contentHash(md5(rfile.content));
                rdata.__set_created((Timestamp)time(NULL) * 1000);    //need to change
                rdata.__set_updated((Timestamp)time(NULL) * 1000);    //need to change
                rdata.__set_deleted(0);
                files.insert({filename,rdata});
            } else {
                return -1;
            }
        } else {
            //this file exists, we need do update
            NameDataMap files = UserFileMap[id];
            if (write2Disk(path,rfile.content) != -1) {
                ++files[filename].version;
                files[filename].__set_contentLength(rfile.content.size());
                files[filename].__set_contentHash(md5(rfile.content));
                files[filename].__set_updated((Timestamp)time(NULL) * 1000);    //need to change
            } else {
                return -1;
            }
        }
    }
    return 0;
}

int FileWorker::deletefile(std::string id, std::string filename) {
    std::string path = ".//files//" + id + "_" + filename;

    if (deleteFromDisk(path) != -1) {
        NameDataMap files = UserFileMap[id];
        RFileMetadata data = files[filename];    
        data.__set_deleted((Timestamp)time(NULL) * 1000);
    } else {
        return -1;
    }

    return 0;    
}

int FileWorker::readfile(std::string id, std::string filename, RFile &rfile) {
    std::string path = ".//files//" + id + "_" + filename;
    NameDataMap files = UserFileMap[id];
    RFileMetadata data = files[filename];
    std::string content;
    if (readFromDisk(path,data.contentLength,content) != -1) {
        rfile.__set_content(content);
        rfile.__set_meta(data);
    } else {
        return -1;
    }

    return 0;
}

int FileWorker::getfiles(std::string id, std::vector<RFileMetadata> &datas) {
    NameDataMap files = UserFileMap[id];
    for (auto it = files.begin(); it != files.end(); ++it) {
        if (it->second.deleted == 0) {
            //0 is for not deleted
            datas.push_back(it->second);
        }
    }

    return 0;
}

std::map<UserID, NameDataMap> FileWorker::getUserFileMap() {
    return UserFileMap;
}

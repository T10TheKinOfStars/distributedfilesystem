#include "fileworker.h"

void FileWorker::initFolder(int port) {
    char cmd[30];
    sprintf(&cmd,"rm -rf %d | mkdir %d", port);
    //std::string cmd = "rm -rf %d | mkdir %d";    
    if (system(cmd) == -1) {
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
        content = buf;
        ifs.close();
        delete []buf;
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
    std::string path = "./files/" + id + "_" + filename;
    SystemException exception;
    
    if (UserFileMap.find(id) == UserFileMap.end()) {
        std::cout<<"this user doesn't exist"<<std::endl;
        //create a new entry                
        if (write2Disk(path,rfile.content) != -1) {
            rdata.__set_version(0);            
            rdata.__set_contentHash(sha256_calc_hex(rfile.content));
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
        //std::cout<<"this user exists"<<std::endl;
        NameDataMap files = UserFileMap[id];
        if (files.find(filename) == files.end() || files[filename].deleted != 0) {
            std::cout<<"this file not exists, we need create a new one"<<std::endl;
            if (write2Disk(path,rfile.content) != -1) {
                rdata.__set_version(0);
                rdata.__set_contentHash(sha256_calc_hex(rfile.content));
                rdata.__set_created((Timestamp)time(NULL) * 1000);    //need to change
                rdata.__set_updated((Timestamp)time(NULL) * 1000);    //need to change
                rdata.__set_deleted(0);
                UserFileMap[id].insert(std::pair<std::string, RFileMetadata>(filename,rdata));
            } else {
                return -1;
            }
        } else {
            std::cout<<"this file exists, we need do update"<<std::endl;
            NameDataMap &files = UserFileMap[id];
            if (write2Disk(path,rfile.content) != -1) {                
                ++files[filename].version;
                files[filename].__set_contentLength(rfile.content.size());
                files[filename].__set_contentHash(sha256_calc_hex(rfile.content));
                files[filename].__set_updated((Timestamp)time(NULL) * 1000);    //need to change
            } else {
                return -1;
            }
        }
    }
    return 0;
}

int FileWorker::deletefile(std::string id, std::string filename) {
    std::string path = "./files/" + id + "_" + filename;    
    if (deleteFromDisk(path) != -1) {
        NameDataMap &files = UserFileMap[id];
        files[filename].__set_deleted((Timestamp)time(NULL) * 1000);
    } else {
        return -1;
    }

    return 0;    
}

int FileWorker::readfile(std::string id, std::string filename, RFile &rfile) {
    std::string path = "./files/" + id + "_" + filename;
    NameDataMap files = UserFileMap[id];
    RFileMetadata data = files[filename];
    if (data.deleted != 0)
        return -1;
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

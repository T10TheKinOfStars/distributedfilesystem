/**
 * Autogenerated by Thrift Compiler (0.9.2)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef chord_TYPES_H
#define chord_TYPES_H

#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <thrift/cxxfunctional.h>




typedef std::string UserID;

typedef int64_t Timestamp;

class SystemException;

class RFileMetadata;

class RFile;

class NodeID;

typedef struct _SystemException__isset {
  _SystemException__isset() : message(false) {}
  bool message :1;
} _SystemException__isset;

class SystemException : public ::apache::thrift::TException {
 public:

  static const char* ascii_fingerprint; // = "66E694018C17E5B65A59AE8F55CCA3CD";
  static const uint8_t binary_fingerprint[16]; // = {0x66,0xE6,0x94,0x01,0x8C,0x17,0xE5,0xB6,0x5A,0x59,0xAE,0x8F,0x55,0xCC,0xA3,0xCD};

  SystemException(const SystemException&);
  SystemException& operator=(const SystemException&);
  SystemException() : message() {
  }

  virtual ~SystemException() throw();
  std::string message;

  _SystemException__isset __isset;

  void __set_message(const std::string& val);

  bool operator == (const SystemException & rhs) const
  {
    if (__isset.message != rhs.__isset.message)
      return false;
    else if (__isset.message && !(message == rhs.message))
      return false;
    return true;
  }
  bool operator != (const SystemException &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const SystemException & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  friend std::ostream& operator<<(std::ostream& out, const SystemException& obj);
};

void swap(SystemException &a, SystemException &b);

typedef struct _RFileMetadata__isset {
  _RFileMetadata__isset() : filename(false), created(false), updated(false), deleted(false), version(false), owner(false), contentLength(false), contentHash(false) {}
  bool filename :1;
  bool created :1;
  bool updated :1;
  bool deleted :1;
  bool version :1;
  bool owner :1;
  bool contentLength :1;
  bool contentHash :1;
} _RFileMetadata__isset;

class RFileMetadata {
 public:

  static const char* ascii_fingerprint; // = "AE509EADC6F5C652FAA107524F293698";
  static const uint8_t binary_fingerprint[16]; // = {0xAE,0x50,0x9E,0xAD,0xC6,0xF5,0xC6,0x52,0xFA,0xA1,0x07,0x52,0x4F,0x29,0x36,0x98};

  RFileMetadata(const RFileMetadata&);
  RFileMetadata& operator=(const RFileMetadata&);
  RFileMetadata() : filename(), created(0), updated(0), deleted(0), version(0), owner(), contentLength(0), contentHash() {
  }

  virtual ~RFileMetadata() throw();
  std::string filename;
  Timestamp created;
  Timestamp updated;
  Timestamp deleted;
  int32_t version;
  UserID owner;
  int32_t contentLength;
  std::string contentHash;

  _RFileMetadata__isset __isset;

  void __set_filename(const std::string& val);

  void __set_created(const Timestamp val);

  void __set_updated(const Timestamp val);

  void __set_deleted(const Timestamp val);

  void __set_version(const int32_t val);

  void __set_owner(const UserID& val);

  void __set_contentLength(const int32_t val);

  void __set_contentHash(const std::string& val);

  bool operator == (const RFileMetadata & rhs) const
  {
    if (__isset.filename != rhs.__isset.filename)
      return false;
    else if (__isset.filename && !(filename == rhs.filename))
      return false;
    if (__isset.created != rhs.__isset.created)
      return false;
    else if (__isset.created && !(created == rhs.created))
      return false;
    if (__isset.updated != rhs.__isset.updated)
      return false;
    else if (__isset.updated && !(updated == rhs.updated))
      return false;
    if (__isset.deleted != rhs.__isset.deleted)
      return false;
    else if (__isset.deleted && !(deleted == rhs.deleted))
      return false;
    if (__isset.version != rhs.__isset.version)
      return false;
    else if (__isset.version && !(version == rhs.version))
      return false;
    if (__isset.owner != rhs.__isset.owner)
      return false;
    else if (__isset.owner && !(owner == rhs.owner))
      return false;
    if (__isset.contentLength != rhs.__isset.contentLength)
      return false;
    else if (__isset.contentLength && !(contentLength == rhs.contentLength))
      return false;
    if (__isset.contentHash != rhs.__isset.contentHash)
      return false;
    else if (__isset.contentHash && !(contentHash == rhs.contentHash))
      return false;
    return true;
  }
  bool operator != (const RFileMetadata &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const RFileMetadata & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  friend std::ostream& operator<<(std::ostream& out, const RFileMetadata& obj);
};

void swap(RFileMetadata &a, RFileMetadata &b);

typedef struct _RFile__isset {
  _RFile__isset() : meta(false), content(false) {}
  bool meta :1;
  bool content :1;
} _RFile__isset;

class RFile {
 public:

  static const char* ascii_fingerprint; // = "C073435743C38E100878E0BBF30AE795";
  static const uint8_t binary_fingerprint[16]; // = {0xC0,0x73,0x43,0x57,0x43,0xC3,0x8E,0x10,0x08,0x78,0xE0,0xBB,0xF3,0x0A,0xE7,0x95};

  RFile(const RFile&);
  RFile& operator=(const RFile&);
  RFile() : content() {
  }

  virtual ~RFile() throw();
  RFileMetadata meta;
  std::string content;

  _RFile__isset __isset;

  void __set_meta(const RFileMetadata& val);

  void __set_content(const std::string& val);

  bool operator == (const RFile & rhs) const
  {
    if (__isset.meta != rhs.__isset.meta)
      return false;
    else if (__isset.meta && !(meta == rhs.meta))
      return false;
    if (__isset.content != rhs.__isset.content)
      return false;
    else if (__isset.content && !(content == rhs.content))
      return false;
    return true;
  }
  bool operator != (const RFile &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const RFile & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  friend std::ostream& operator<<(std::ostream& out, const RFile& obj);
};

void swap(RFile &a, RFile &b);

typedef struct _NodeID__isset {
  _NodeID__isset() : id(false), ip(false), port(false), count(false) {}
  bool id :1;
  bool ip :1;
  bool port :1;
  bool count :1;
} _NodeID__isset;

class NodeID {
 public:

  static const char* ascii_fingerprint; // = "6C490E00301248580B234D1B4BD756FB";
  static const uint8_t binary_fingerprint[16]; // = {0x6C,0x49,0x0E,0x00,0x30,0x12,0x48,0x58,0x0B,0x23,0x4D,0x1B,0x4B,0xD7,0x56,0xFB};

  NodeID(const NodeID&);
  NodeID& operator=(const NodeID&);
  NodeID() : id(), ip(), port(0), count(0) {
  }

  virtual ~NodeID() throw();
  std::string id;
  std::string ip;
  int32_t port;
  int32_t count;

  _NodeID__isset __isset;

  void __set_id(const std::string& val);

  void __set_ip(const std::string& val);

  void __set_port(const int32_t val);

  void __set_count(const int32_t val);

  bool operator == (const NodeID & rhs) const
  {
    if (!(id == rhs.id))
      return false;
    if (!(ip == rhs.ip))
      return false;
    if (!(port == rhs.port))
      return false;
    if (__isset.count != rhs.__isset.count)
      return false;
    else if (__isset.count && !(count == rhs.count))
      return false;
    return true;
  }
  bool operator != (const NodeID &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const NodeID & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  friend std::ostream& operator<<(std::ostream& out, const NodeID& obj);
};

void swap(NodeID &a, NodeID &b);



#endif

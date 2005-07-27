#ifndef __MUNHASHDIRPREPACK_H
#define __MUNHASHDIRPREPACK_H

#include "msg/Message.h"
#include "mds/CInode.h"
#include "include/types.h"

class MUnhashDirPrepAck : public Message {
  inodeno_t ino;
  bool assim;

  // subdir dentry names + inodes 
  map<string,CInodeDiscover*>    inodes;

 public:
  inodeno_t get_ino() { return ino; }
  map<string,CInodeDiscover*>& get_inodes() { return inodes; }

  bool did_assim() { return assim; }
  void mark_assim() { assert(!assim); assim = true; }

  MUnhashDirPrepAck() : assim(false) { }
  MUnhashDirPrepAck(inodeno_t ino) :
	Message(MSG_MDS_UNHASHDIRPREPACK),
	assim(false) {
	this->ino = ino;
  }
  ~MUnhashDirPrepAck() {
    for (map<string,CInodeDiscover*>::iterator it = inodes.begin();
         it != inodes.end();
         it++) 
      delete it->second;
  }


  virtual char *get_type_name() { return "HP"; }

  void add_inode(const string& dentry, CInodeDiscover *in) {
    inodes[dentry] = in;
  }

  void decode_payload() {
	int off = 0;
	payload.copy(off, sizeof(ino), (char*)&ino);
    off += sizeof(ino);
    
    // inodes
    int ni;
    payload.copy(off, sizeof(int), (char*)&ni);
    off += sizeof(int);
    for (int i=0; i<ni; i++) {
	  // dentry
	  string dname;
	  _decode(dname, payload, off);
	  
	  // inode
      CInodeDiscover *in = new CInodeDiscover;
      in->_decode(payload, off);
	  
	  inodes[dname] = in;
    }
  }

  virtual void encode_payload() {
	payload.append((char*)&ino, sizeof(ino));

    // inodes
    int ni = inodes.size();
    payload.append((char*)&ni, sizeof(int));
    for (map<string,CInodeDiscover*>::iterator iit = inodes.begin();
         iit != inodes.end();
         iit++) {
	  _encode(iit->first, payload);   // dentry
	  iit->second->_encode(payload);  // inode
	}
  }
};

#endif

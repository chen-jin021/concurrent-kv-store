#include "simple_kvstore.hpp"

bool SimpleKvStore::Get(const GetRequest* req, GetResponse* res) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  if(req->key.empty()){
    return false;
  }
  mtx.lock();
  if(kvstore.find(req->key) == kvstore.end()) {
    return false;
  }

  res->value = kvstore.at(req->key);
  mtx.unlock();
  return true;
}

bool SimpleKvStore::Put(const PutRequest* req, PutResponse*) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  mtx.lock(); // write lock
  kvstore[req->key] = req->value;
  mtx.unlock();
  return true;
}

bool SimpleKvStore::Append(const AppendRequest* req, AppendResponse*) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  mtx.lock(); // write lock
  kvstore[req->key] += req->value;
  mtx.unlock();
  return true;
}

bool SimpleKvStore::Delete(const DeleteRequest* req, DeleteResponse* res) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  // check if the key is in kvstore
  mtx.lock();
  if(kvstore.find(req->key) == kvstore.end()){
    return false;
  }
  // otherwise
  
  res->value = kvstore.at(req->key);
  kvstore.erase(req->key);
  mtx.unlock();
  return true;
}

bool SimpleKvStore::MultiGet(const MultiGetRequest* req,
                             MultiGetResponse* res) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  int keySize = req->keys.size(); 
  mtx.lock();
  for(int i = 0; i < keySize; i++){
    std::string curKey = req->keys[i];
    // check if valid && key is in kvstore
    if(curKey.empty() || kvstore.find(curKey) == kvstore.end()){
      return false; // not able to find the key 
    }
    res->values.push_back(kvstore.at(curKey));
  }
  mtx.unlock();
  return true;
}

bool SimpleKvStore::MultiPut(const MultiPutRequest* req, MultiPutResponse*) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  mtx.lock();
  int keySize = req->keys.size();
  int valSize = req->values.size();
  if(keySize != valSize){ // check size are the same
    return false;
  }

  for(int i = 0; i < keySize; i++){
    std::string curKey = req->keys[i];
    std::string curVal = req->values[i];
    if(curKey.empty() || curVal.empty()){ // empty key value is not correct
      return false;
    }
    kvstore[curKey] = curVal;
  }
  mtx.unlock();
  return true;
}

std::vector<std::string> SimpleKvStore::AllKeys() {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  // returns all of the keys from db
  mtx.lock();
  std::vector<std::string> ret;
  for(auto const& ele : kvstore){
    ret.push_back(ele.first);
  }
  mtx.unlock();
  return ret;
}

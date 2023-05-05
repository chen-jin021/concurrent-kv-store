#include "concurrent_kvstore.hpp"

#include <optional>

bool ConcurrentKvStore::Get(const GetRequest* req, GetResponse* res) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  store.m[store.bucket(req->key)].lock_shared(); // read lock
  std::string curKey = req->key;
  // std::shared_mutex m = store.m[store.bucket(curKey)];
  // m.lock_shared();
  // check valid
  if(curKey.empty() || store.getIfExists(this->store.bucket(curKey), curKey) == std::nullopt){
    store.m[store.bucket(curKey)].unlock_shared();
    return false; // not able to find the key 
  }

  res->value = store.getIfExists(this->store.bucket(curKey), curKey)->value;
  store.m[store.bucket(curKey)].unlock_shared();
  return true;
}

bool ConcurrentKvStore::Put(const PutRequest* req, PutResponse*) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  std::string curKey = req->key;
  std::string curValue = req->value;
  // check valid
  if(curKey.empty() || curValue.empty()){
    return false;
  }
  // write lock
  store.m[store.bucket(curKey)].lock();
  this->store.insertItem(this->store.bucket(curKey), curKey, curValue);
  store.m[store.bucket(curKey)].unlock();
  return true;
}

bool ConcurrentKvStore::Append(const AppendRequest* req, AppendResponse*) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  std::string curKey = req->key;
  std::string curVal = req->value;

  // check valid
  if(curKey.empty() || curVal.empty()){
    return false;
  }

  store.m[store.bucket(curKey)].lock();

  std::optional<DbItem> origin = store.getIfExists(store.bucket(curKey), curKey);

  if(origin != std::nullopt){
    store.insertItem(store.bucket(curKey), curKey, origin.value().value + curVal);
  } else{
    store.insertItem(store.bucket(curKey), curKey, curVal);
  }
  store.m[store.bucket(curKey)].unlock();
  return true;
}

bool ConcurrentKvStore::Delete(const DeleteRequest* req, DeleteResponse* res) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  store.m[store.bucket(req->key)].lock(); // read lock
  std::string curKey = req->key;
  // check if the key exists
  if(store.getIfExists(this->store.bucket(curKey), curKey) == std::nullopt){
    return false;
  }
  // otherwise
  std::optional<DbItem> item = store.getIfExists(this->store.bucket(curKey), curKey);
  res->value = item.value().value;
  store.removeItem(store.bucket(curKey), curKey);
  store.m[store.bucket(curKey)].unlock();
  return true;
}

bool ConcurrentKvStore::MultiGet(const MultiGetRequest* req,
                                 MultiGetResponse* res) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  int keySize = req->keys.size();

  std::vector<int> indices;
  for(int i = 0; i < (int) req->keys.size(); i++){
    if(std::find(indices.begin(), indices.end(), store.bucket(req->keys[i])) != indices.end()){
      continue;
    }
    indices.push_back(store.bucket(req->keys[i]));
  }
  // sort the indices
  std::sort(indices.begin(), indices.end());
  // lock each mutex in order
  for(int i = 0; i < (int) indices.size(); i++){
    store.m[indices[i]].lock_shared();
  }
 
  for(int i = 0; i < keySize; i++){
    std::string curKey = req->keys[i];
    // lock the read mutex for curKey
    // store.m[store.bucket(curKey)].lock_shared();
    if(curKey.empty() || store.getIfExists(this->store.bucket(curKey), curKey) == std::nullopt){
      // store.m[store.bucket(curKey)].unlock_shared();
      for(int i = 0; i < (int) indices.size(); i++){
        store.m[indices[i]].unlock_shared();
      }
      return false; // not able to find the key 
    }
    res->values.push_back(store.getIfExists(this->store.bucket(curKey), curKey)->value);
    // store.m[store.bucket(curKey)].unlock_shared();
  }

  // unlock each mutex in order
  for(int i = 0; i < (int) indices.size(); i++){
    store.m[indices[i]].unlock_shared();
  }

  return true;
}

bool ConcurrentKvStore::MultiPut(const MultiPutRequest* req,
                                 MultiPutResponse*) {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  int keySize = req->keys.size();
  int valSize = req->values.size();
  if(keySize != valSize){ // check size are the same
    return false;
  }

  std::vector<int> indices;
  for(int i = 0; i < (int) req->keys.size(); i++){
    if(std::find(indices.begin(), indices.end(), store.bucket(req->keys[i])) != indices.end()){
      continue;
    }
    indices.push_back(store.bucket(req->keys[i]));
  }
  // sort the indices
  std::sort(indices.begin(), indices.end());
  // lock each mutex in order
  for(int i = 0; i < (int) indices.size(); i++){
    store.m[indices[i]].lock();
  }
 
  // lock all mutex for req->keys in a for loop
  for(int i = 0; i < keySize; i++){
    std::string curKey = req->keys[i];
    std::string curVal = req->values[i];
    // check valid
    if(curKey.empty() || curVal.empty()){
      // unlock and returb false
      for(int i = 0; i < (int) indices.size(); i++){
        store.m[indices[i]].unlock();
      }
      return false;
    }
    // write lock
    this->store.insertItem(this->store.bucket(curKey), curKey, curVal);
  }

  // unlock each mutex in order
  for(int i = 0; i < (int) indices.size(); i++){
    store.m[indices[i]].unlock();
  }
 
  return true;
}

std::vector<std::string> ConcurrentKvStore::AllKeys() {
  // TODO (Part A, Steps 4 and 5): IMPLEMENT
  std::vector<std::string> ret;
  for(int i = 0; i < (int) store.BUCKET_COUNT; i++){
    // get all keys in the bucket
    std::list<DbItem> dbItems = store.buckets.at(i);
    for(auto const& ele : dbItems){
      ret.push_back(ele.key);
    }
  }
  return ret;
}

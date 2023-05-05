#include "synchronized_queue.hpp"

template <typename T>
size_t synchronized_queue<T>::size() {
  // TODO (Part A, Step 3): IMPLEMENT
  std::unique_lock<std::mutex> guard(this->mtx);
  return this->q.size();
}

template <typename T>
bool synchronized_queue<T>::pop(T* elt) {
  // TODO (Part A, Step 3): IMPLEMENT
  // create unique lock for condition variable
  std::unique_lock<std::mutex> guard(this->mtx);

  // wait if queue is empty
  while (!this->is_stopped && this->q.empty()) {
    this->cv.wait(guard);
  }

  // check if queue has been stopped
  if (this->is_stopped) {
    return true;
  }

  *elt = q.front(); // stores the head ele
  q.pop(); // pops it
  return false;
}

template <typename T>
void synchronized_queue<T>::push(T elt) {
  // TODO (Part A, Step 3): IMPLEMENT
  std::unique_lock<std::mutex> guard(this->mtx);
  this->q.push(elt); // push to back of queue
  // wake up pop thread
  if (!this->q.empty()) {
    this->cv.notify_all();
  }
}

template <typename T>
std::vector<T> synchronized_queue<T>::flush() {
  // TODO (Part A, Step 3): IMPLEMENT
  std::unique_lock<std::mutex> guard(this->mtx);
  std::vector<T> ret;
  while(!q.empty()) {
    ret.push_back(q.front());
    q.pop();
  }
  
  return ret;
}

template <typename T>
void synchronized_queue<T>::stop() {
  // TODO (Part A, Step 3): IMPLEMENT
  this->is_stopped = true;
  this->cv.notify_all();
}

// NOTE: DO NOT TOUCH! Why is this necessary? Because C++ is a beautiful
// language:
// https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
template class synchronized_queue<int>;
template class synchronized_queue<std::shared_ptr<ClientConn>>;

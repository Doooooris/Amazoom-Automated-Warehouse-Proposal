#ifndef LAB6_DYNAMICORDERQUEUE_H
#define LAB6_DYNAMICORDERQUEUE_H

#include "OrderQueue.h"
#include <deque>
#include <condition_variable>
#include <mutex>
/**
 * Dynamically-sized Queue Implementation
 *
 * Does not block when adding items
 */
class DynamicOrderQueue : public virtual OrderQueue {
  std::deque<Order> buff_;
  std::deque<RestockItem> buffItem_;
  std::mutex mutex_;
  std::condition_variable cv_;

 public:
  /**
   * Creates the dynamic queue
   */
  DynamicOrderQueue() :
      buff_(), buffItem_(), mutex_(), cv_(){}

  void add(const Order& order) {

    //==================================================
    // TODO: Safely add item to "queue"
    //    - safely add to end of internal queue
    //    - notify others of item availability
    //==================================================
    mutex_.lock();
    buff_.push_back(order);
    mutex_.unlock();
    cv_.notify_one();

  }
    void addItem(const RestockItem& item) {
        
        //==================================================
        // TODO: Safely add item to "queue"
        //    - safely add to end of internal queue
        //    - notify others of item availability
        //==================================================
        mutex_.lock();
        buffItem_.push_back(item);
        mutex_.unlock();
        cv_.notify_one();
        
    }
    
    bool checkEmpty(){
        return buff_.empty();
    }
    
  Order get() {

    //==================================================
    // TODO: Safely remove item from "queue"
    //    - wait until internal queue is non-empty
    //    - safely acquire item from internal queue
    //==================================================
      std::unique_lock<std::mutex> lock(mutex_);
      //buffer have data, we want to read data
      //buff_.empty() = false,
      cv_.wait(lock, [&]() { return !buff_.empty();});
      // get first item in queue
      Order out = buff_.front();
      buff_.pop_front();
      lock.unlock();
    return out;
  }
    
    RestockItem getItem() {
        
        //==================================================
        // TODO: Safely remove item from "queue"
        //    - wait until internal queue is non-empty
        //    - safely acquire item from internal queue
        //==================================================
        std::unique_lock<std::mutex> lock(mutex_);
        //buffer have data, we want to read data
        //buff_.empty() = false,
        cv_.wait(lock, [&]() { return !buffItem_.empty();});
        // get first item in queue
        RestockItem out = buffItem_.front();
        buffItem_.pop_front();
        lock.unlock();
        return out;
    }
};

#endif //LAB6_DYNAMICORDERQUEUE_H

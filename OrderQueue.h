#ifndef LAB6_QUEUE_H
#define LAB6_QUEUE_H

#include "Order.h"

/**
 * Thread-safe queue of items
 */
class OrderQueue {
 public:
  /**
   * Adds an order item to the queue
   * @param order item to add
   */
  virtual void add(const Order& order) = 0;
    virtual void addItem(const RestockItem& item) = 0;

  /**
   * Retrieve the next order item in the queue
   * @return next available item
   */
  virtual Order get() = 0;
    virtual RestockItem getItem() = 0;

    virtual bool checkEmpty() = 0;

};

#endif //LAB6_QUEUE_H

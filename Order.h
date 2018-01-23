#ifndef LAB6_ORDER_H
#define LAB6_ORDER_H

#include "Item.h"

/**
 * Basic order information containing a order id and item
 */
struct Order {
    int id;
    std::list<Item> items;
    char status;
    float weight;

  bool operator==(const Order& other) const {
      return (id == other.id);
  }

  bool operator!=(const Order& other) const {
    return !(*this == other);
  }
};

#endif //LAB6_ORDER_H

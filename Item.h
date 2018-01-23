#ifndef PROJECT_ITEM_H
#define PROJECT_ITEM_H

/**
 * Basic order information containing a order id and item
 */
struct Item {
    std::string name;
    float weight;
    int column;
    int row;
    int side;
    int shelf;
};

struct RestockItem {
    std::string name;
    float weight;
};

#endif //LAB6_ORDER_H

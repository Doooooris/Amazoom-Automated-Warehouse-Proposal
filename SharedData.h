#ifndef PROJECT_SHARED_DATA_H
#define PROJECT_SHARED_DATA_H

#include "Order.h"
#include "Item.h"
#include <vector>;
#include <string>;


struct SharedData{
    int cmd;
    std::string item_name;
    int orderNum;
    int stockNum;
    std::string OrderStatus;
    std::string lowStcokItem[8];
    int alertReady;
};

#endif //LAB6_Truck_Status_H

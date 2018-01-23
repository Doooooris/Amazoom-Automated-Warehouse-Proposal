#ifndef LAB6_MENU_H
#define LAB6_MENU_H

#include <string>
#include <vector>
#include <fstream>
#include <list>
#include <stdlib.h>
#include "SharedData.h"
#include "Order.h"
#include </Users/doooooris/Documents/Course_Material/CPEN_333/LAB/Project/Amazoom_V2/include/json.hpp>
#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/process/mutex.h>
#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/process/shared_memory.h>
using JSON = nlohmann::json;

/**
 * Menu items
 */

struct WarehouseShelf{
    int row;
    int col;
    int side;
    int shelf;
    float weight;
};

/**
 * Warehouse, containing list of items that can
 * be ordered at the webserver
 */
class Warehouse {
    std::list<Item> warehouse_;
    cpen333::process::shared_object<SharedData> sharedData_;
    cpen333::process::mutex mutexS_;
    cpen333::process::mutex mutex_;
    int nrow_;
    int ncol_;
    int nshelf_;

 public:
  Warehouse(const int nrow, const int ncol, const int nshelf):
    nrow_(nrow), ncol_(ncol), nshelf_(nshelf), sharedData_("Project_Shared_Data"),
    mutexS_("Project_SharedData_Mutex"), mutex_("Project_Warehouse_Mutex") {}

  /**
   * Loads items from a JSON file
   * @param filename file to load items from
   * @return number of new items added to the menu
   */
  void load(const std::string& filename) {
    std::ifstream fin(filename);

    size_t nadded = 0;
    if (fin.is_open()) {
      JSON jwarehouse;
      fin >> jwarehouse;
        
      for (const auto& jitem : jwarehouse) {
        Item item;
        item.name = jitem["name"];
        item.weight = (float)jitem["weight"];
        item.column = (int)jitem["column"];
        item.row = (int)jitem["row"];
        item.side = (int)jitem["side"];
        item.shelf = (int)jitem["shelf"];
          mutex_.lock();
        warehouse_.push_back(item);
          mutex_.unlock();
        }
      }
    }
    
    /**
     * Adds a items to the warehouse
     * @param item item info to add
     * @return true if added, false if already exists
     */
    bool add(const RestockItem& restockitem) {
        int col;
        int row;
        int shelf;
        int side;
        //randomly generate a location in warehouse for new item
        while(1){
            col = rand() % ncol_ + 1;
            row = rand() % (nrow_-1) + 2;
            if(col == 1)
                side = 2;//side 2 is left hand side
            else if (col == ncol_)
                side = 1;//side 1 is right hand side
            else
            {
                side = rand() % 2 + 1;
            }
            shelf = rand() % nshelf_ + 1;
            //================================================
            //Check if the total weight exceeds the max weight
            //================================================
            float weightOfShelf = 0;
            Item itemtemp;
            //search loacation
            for (std::list<Item>::iterator it=warehouse_.begin(); it != warehouse_.end(); ++it){
                itemtemp = *it;
                if(itemtemp.column == col && itemtemp.row == row && itemtemp.side == side && itemtemp.shelf && shelf){
                    weightOfShelf = weightOfShelf + itemtemp.weight;
                }
            }
            if(weightOfShelf + restockitem.weight <= 4000){
                break;
            }
        }
        
        Item newitem;
        newitem.name = restockitem.name;
        newitem.weight = restockitem.weight;
        newitem.column = col;
        newitem.row = row;
        newitem.side = side;
        newitem.shelf = shelf;
        mutex_.lock();
        unsigned long ini_length = warehouse_.size();
        // try to add element to the list
        warehouse_.push_back(newitem);
        unsigned long add_length = warehouse_.size();
        mutex_.unlock();
        if (add_length == ini_length + 1)
            return true;
        return false;
    }
    
    bool remove(Item& removeItem){
        Item item;
        for (std::list<Item>::iterator it=warehouse_.begin(); it != warehouse_.end(); ++it){
            item = *it;
            if(item.name == removeItem.name)
            {
                mutex_.lock();
                warehouse_.erase(it);
                mutex_.unlock();
                return true;
            }
        }
        return false;
    }
    
    bool IsInStock(std::string name){
        Item item;
        for (std::list<Item>::iterator it=warehouse_.begin(); it != warehouse_.end(); ++it){
            item = *it;
            if(item.name == name)
                return true;
        }
        return false;
    }
    
    int getStock(std::string name){
        
        Item item;
        int count = 0;
        for (std::list<Item>::iterator it= warehouse_.begin(); it != warehouse_.end(); ++it){
            item = *it;
            if(item.name == name)
                count ++;
        }
        return count;
    }
    
    //search warehouse with the item named 'name'
    Item search(std::string name){
        Item out;
        Item item;
        
        for (std::list<Item>::iterator it=warehouse_.begin(); it != warehouse_.end(); ++it){
            item = *it;
            if(item.name == name)
                return item;
        }
        //doesn't find item;
        return out;
    }
    
    const std::list<Item>& warehouse() const {
        return warehouse_;
    }

};

#endif //LAB6_MENU_H

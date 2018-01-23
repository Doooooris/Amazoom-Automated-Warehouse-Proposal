#ifndef LAB6_INVENTORY_TRUCK_H
#define LAB6_INVENTORY_TRUCK_H

#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/thread/thread_object.h>
#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/thread/semaphore.h>
#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/process/mutex.h>
#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/process/shared_memory.h>
#include <iostream>
#include <thread>
#include "Webserver.h"
#include "Status.h"
#include "Warehouse.h"
#include "OrderQueue.h"
#include "safe_printf.h"
#include <list>
#include <string>

/**
 * delivery truck take items from dock and deliver
 * them to customers.
 */
class InventoryTruck : public cpen333::thread::thread_object {
  OrderQueue& import_queue_;
    cpen333::thread::semaphore& docks_;
    cpen333::thread::semaphore& RobotSema_;
    std::list<RestockItem> restockItem_;
    cpen333::process::shared_object<Status> Status_;
    cpen333::process::mutex mutex_;
    
 public:
  /**
   * Create a truck
   * @param dock_queue_ queue of completed items to deliver.
   */
  InventoryTruck(OrderQueue& import_queue, cpen333::thread::semaphore& docks, cpen333::thread::semaphore& RobotSema) : import_queue_(import_queue), docks_(docks), restockItem_(), Status_("Project_Status"), mutex_("Project_Status_Mutex__"), RobotSema_(RobotSema) {}

    /**
     * Loads items from a JSON file
     * @param filename file to load items from
     * @return number of new items added to the menu
     */
    void load(const std::string& filename) {
        std::ifstream fin(filename);
        
        size_t nadded = 0;
        if (fin.is_open()) {
            JSON jrestock;
            fin >> jrestock;
            
            for (const auto& jitem : jrestock) {
                RestockItem item;
                item.name = jitem["name"];
                item.weight = (float)jitem["weight"];
                restockItem_.push_back(item);
            }
        }
    }
    
  /**
   * Main work function
   * @return 0 when complete
   */
  int main() {
      load("./data/restock_item.json");
      
      while(1){
          // inventory truck arrives every 30 seconds
          std::this_thread::sleep_for(std::chrono::seconds(35));
          // wait for docks to be free
          docks_.wait();
    
          safe_printf("Inventory truck arrives\n");
          
          RestockItem item;
      int count = 0;
          // taking item to import_queue_
          for (std::list<RestockItem>::iterator it= restockItem_.begin(); it != restockItem_.end(); ++it){
                item = *it;
                import_queue_.addItem(item);
              safe_printf("Inventory truck adding item ");
              std::cout<<item.name<<std::endl;
              count++;
            }
          std::this_thread::sleep_for(std::chrono::seconds(1));
          // notify robot take item from import queue
          mutex_.lock();
          Status_->inventoryTruck = 1;
          mutex_.unlock();
            for (int i=0; i < count; i++){
                RobotSema_.notify();
            }
        
          
          //notify central computer truck leaves and dock is avaiable now;
          docks_.notify();
            std::this_thread::sleep_for(std::chrono::seconds(2));
          safe_printf("Inventory truck leaves\n");
      }
    return 0;
  }
};

#endif //LAB6_SERVER_H

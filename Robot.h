#ifndef LAB6_CHEF_H
#define LAB6_CHEF_H

#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/thread/thread_object.h>
#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/process/mutex.h>
#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/process/shared_memory.h>
#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/thread/semaphore.h>
#include <iostream>
#include <thread>
#include "Warehouse.h"
#include "Status.h"
#include "OrderQueue.h"
#include "safe_printf.h"
#include "SharedData.h"

/**
 * The Chef grabs orders from a queue, cooks them,
 * then adds the cooked dishes to a new queue for
 * the servers to serve
 */
class Robot : public cpen333::thread::thread_object {
  OrderQueue& order_queue_;
    OrderQueue& import_queue_;
  OrderQueue& loading_queue_;
    Warehouse& warehouse_;
    cpen333::process::shared_object<Status> Status_;
    cpen333::process::mutex mutex_;
    cpen333::process::mutex mutexS_;
    int id_;
    cpen333::thread::semaphore& RobotSema_;
    cpen333::process::shared_object<SharedData> sharedData_;
    std::list<Order>& orderReceived_;
    std::list<Order>& orderReadyDel_;
    
 public:
  /**
   * Create a robot chef
   * @param id the robot's id
   * @param order_queue_ to read orders from
   * @param dock_queue_ to add finded item to
   */
  Robot(int id, OrderQueue& order_queue, OrderQueue& import_queue, OrderQueue& loading_queue, Warehouse& warehouse, cpen333::thread::semaphore& RobotSema, std::list<Order>& orderReceived, std::list<Order>& orderReadyDel) :

      id_(id), order_queue_(order_queue), import_queue_(import_queue),loading_queue_(loading_queue), warehouse_(warehouse), mutex_("Project_Status_Mutex__"),Status_("Project_Status"), RobotSema_(RobotSema) , sharedData_("Project_Shared_Data"), mutexS_("Project_SharedData_Mutex"), orderReceived_(orderReceived), orderReadyDel_(orderReadyDel) {}

  int id() {
    return id_;
  }
    

  /**
   * Main execution function
   * @return 0 if completed
   */
  int main() {

    safe_printf("Robot %d started\n", id_);
      
    /**
     * Robot has two operation method
     * First: Take order from order queue when order queue is not empty, then move order to loading dock;
     * Second: Take order from loading dock when inventory truck arrives, then move order to warehouse;
    */
      
    // First operation stops, if inventory truck arrives
      
    
      std::list<Item> items;
      Item item;
      RestockItem restockItem;
      Order order;
      
      safe_printf("Robot %d waits order\n",id_);
      RobotSema_.wait();
          while(1){
          //===================================================
          //if() inventory truck arrive, start second operation.
          int inventoryStatus = Status_->inventoryTruck;
              if(inventoryStatus == 1)//inventory truck arrives
              {    
                  while(import_queue_.checkEmpty() == true)
                  {
                      // Inventory truck arrives, robots start load restock item
                      restockItem = import_queue_.getItem();
                      //place item into warehouse_
                      bool added = warehouse_.add(restockItem);
                      if (added == true)
                          safe_printf("Robot %d adding item %c to warehouse\n", id_, *restockItem.name.c_str());
                      else
                          safe_printf("Robot %d adding item %c faild\n", id_, *restockItem.name.c_str());
                      std::this_thread::sleep_for(std::chrono::seconds(1));
                  }
                  if(import_queue_.checkEmpty() == false)
                  {
                      mutex_.lock();
                      Status_->inventoryTruck = 0;
                      mutex_.unlock();
                  }
              }
              else{
                  order = order_queue_.get();
                  items = order.items;
                  order.status = 'PROCESSING';
                  safe_printf("Robot %d proccing order %d\n",id_,order.id);
                  // process item
                  //Get the location of the item:
                  int row,col,side,shelf;
                      for(std::list<Item>::iterator it = items.begin(); it != items.end(); ++it){
                          item = *it;
                          row = item.row;
                          col = item.column;
                          side = item.side;
                          shelf = item.shelf;
                          
                          safe_printf("Robot %d taking item ", id_);
                          std::cout<<item.name ;
                          safe_printf(" from warehouse\n");
                          std::this_thread::sleep_for(std::chrono::seconds(1));
                          
                        
                          
                          //remove the item from warehouse
                          warehouse_.remove(item);
                      }
                  // add to those to dock
                  orderReadyDel_.push_back(order);
                  orderReceived_.remove(order);
                  //delete the order from orderReceived list:
                  
                  loading_queue_.add(order);
                  safe_printf("Robot %d complete order %d, order %d wait delivery truck\n", id_, order.id, order.id);
              }
          }
      
      safe_printf("Robot %d done\n", id_);

    return 0;
  }
};

#endif //LAB6_CHEF_H

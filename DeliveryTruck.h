#ifndef LAB6_SERVER_H
#define LAB6_SERVER_H

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
#include "Status.h"

/**
 * delivery truck take items from dock and deliver
 * them to customers.
 */
class DeliveryTruck : public cpen333::thread::thread_object {
  OrderQueue& loading_queue_;
    cpen333::thread::semaphore& docks_;
    cpen333::thread::semaphore& DeliveryTruckSema_;
    cpen333::process::shared_object<Status> Status_;
    cpen333::process::mutex mutex_;
    cpen333::process::mutex mutexS_;
    cpen333::process::shared_object<SharedData> sharedData_;
    std::list<Order>& orderReadyDel_;
    std::list<Order>& orderDelived_;

 public:
  /**
   * Create a truck
   * @param dock_queue_ queue of completed items to deliver.
   */
    DeliveryTruck(OrderQueue& loading_queue, cpen333::thread::semaphore& docks, cpen333::thread::semaphore& DeliveryTruckSema, std::list<Order>& orderReadyDel, std::list<Order>& orderDelived) : loading_queue_(loading_queue), docks_(docks), Status_("Project_Status"), mutex_("Project_Status_Mutex__"), DeliveryTruckSema_(DeliveryTruckSema),  sharedData_("Project_Shared_Data"), mutexS_("Project_SharedData_Mutex"), orderReadyDel_(orderReadyDel), orderDelived_(orderDelived){}

  /**
   * Main work function
   * @return 0 when complete
   */
  int main() {
      while(1){
          // delivery truck arrives every 20 seconds
          std::this_thread::sleep_for(std::chrono::seconds(20));
          // wait for docks to be free
          docks_.wait();
          // notify the central computer delivery truck arrives
          
          safe_printf("Delivery truck arrives\n");
          mutex_.lock();
          Status_->deliveryTruck = 1;
          mutex_.unlock();
          
          float total_weight = 0;
          float capacity = 5000;
          Order order ;
          
          // taking order from loading_queue_
          while(1){
              order = loading_queue_.get();
              safe_printf("Delivery truck loading order %d\n",order.id);
              if((order.weight + total_weight) <= capacity){
                  total_weight = order.weight + total_weight;
                  order.status = 'DELIVERIED';
                  orderReadyDel_.remove(order);
                  orderDelived_.push_back(order);
              }
              else{
                  safe_printf("Delivery truck reject order %d, truck full\n",order.id);
                  loading_queue_.add(order);
                  //Truck full and leave
                  break;
              }
              if(total_weight == capacity){
                  safe_printf("Delivery truck full\n");
                  break;
              }
          }
          //notify central computer truck leaves and dock is avaiable now;
          docks_.notify();
          mutex_.lock();
          Status_->deliveryTruck = 0;
          mutex_.unlock();
          safe_printf("Delivery truck leaves\n");
    }
    return 0;
  }
};

#endif //LAB6_SERVER_H

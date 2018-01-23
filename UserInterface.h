#ifndef LAB6_USER_INTERFACE_H
#define LAB6_USER_INTERFACE_H

#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/thread/thread_object.h>
#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/thread/semaphore.h>
#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/process/mutex.h>
#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/process/shared_memory.h>
#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/process/socket.h>
#include <iostream>
#include <thread>
#include <list>
#include <vector>
#include <string>

#include "SharedData.h"
#include "Order.h"
#include "Warehouse.h"

class UserInterface  : public cpen333::thread::thread_object {
    cpen333::process::shared_object<SharedData> sharedData_;
    cpen333::process::mutex mutex_;
    std::list<Order>& orderReceived;
    std::list<Order>& orderReadyDel;
    std::list<Order>& orderDelived;
    Warehouse& warehouse_;
 public:
  /**
   * Create a truck
   * @param dock_queue_ queue of completed items to deliver.
   */
    UserInterface(std::list<Order>& orderReceived, std::list<Order>& orderReadyDel, std::list<Order>& orderDelived, Warehouse& warehouse) : orderReceived(orderReceived), orderReadyDel(orderReadyDel),orderDelived(orderDelived),warehouse_(warehouse), sharedData_("Project_Shared_Data"), mutex_("Project_SharedData_Mutex"){}

    void query_order(int OrderNum){
        
        Order order;
        for (std::list<Order>::iterator it= orderReceived.begin(); it != orderReceived.end(); ++it){
            order = *it;
            if(order.id == OrderNum){
                mutex_.lock();
                sharedData_->OrderStatus = "RECEIVED";
                mutex_.unlock();
                std::cout<<"Order "<<OrderNum<<" is received"<<std::endl;
                return;
            }
        }
        for (std::list<Order>::iterator it= orderReadyDel.begin(); it != orderReadyDel.end(); ++it){
            order = *it;
            if(order.id == OrderNum){
                mutex_.lock();
                sharedData_->OrderStatus = "READY FOR DELIVERY";
                mutex_.unlock();
                std::cout<<"Order "<<OrderNum<<" is ready for delivery"<<std::endl;
                return;
            }
        }
        for (std::list<Order>::iterator it= orderDelived.begin(); it != orderDelived.end(); ++it){
            order = *it;
            if(order.id == OrderNum){
                mutex_.lock();
                sharedData_->OrderStatus = "DELIVERIED";
                mutex_.unlock();
                std::cout<<"Order "<<OrderNum<<" is deliveried"<<std::endl;
                return;
            }
        }
        mutex_.lock();
        sharedData_->OrderStatus = "NOT FOUND";
        mutex_.unlock();
        return;
    }
    
    void getStock(std::string name){
        
        Item item;
        int count = 100;
        count = warehouse_.getStock(name);
        mutex_.lock();
        sharedData_->stockNum = count;
        mutex_.unlock();
        return;
    }
    
    /*
    void get_order_name(char n){
        std::string name;
        std::cout << std::endl << "Search Item" << std::endl;
        std::cout << " Item Name : ";
        std::getline(std::cin, name);
        userInterface.write(n);
        userInterface.write(msg);
    }*/
    
    void alert(){
        std::vector<std::string> name = {"iPhone","iPad","Mac","pen","iPhone_case","backpack","tissue","watch"};
        unsigned long size = name.size();
        Item item;
        int j = 0;
        for(unsigned long i=0; i<8; i++){
            int count = 0;
            count = warehouse_.getStock(name.at(i));
            if(count <= 1){
                mutex_.lock();
                (sharedData_->lowStcokItem)[j] = name.at(i);
                mutex_.unlock();
            }
            else
            {
                mutex_.lock();
                (sharedData_->lowStcokItem)[j] = "";
                mutex_.unlock();
            }
            j++;
        }
        mutex_.lock();
        sharedData_->alertReady = 1;
        mutex_.unlock();
        return;
    }
    
  /**
   * Main work function
   * @return 0 when complete
   */
    int main() {
      /*while(1){
      cpen333::process::socket_server server(52102);
      server.open();
      //Creat empty client
      cpen333::process::socket userInterface;
      server.accept(userInterface);
      
          server.read_all();
      }*/
        int cmd = 0;
      while(1){
          mutex_.lock();
          cmd = sharedData_->cmd;
          mutex_.unlock();
          if(cmd > 0){
              if(cmd == 2){
                  std::string name;
                  while(name.size() < 2){
                      mutex_.lock();
                      name = sharedData_->item_name;
                      mutex_.unlock();
                  }
                  getStock(name);
              }
              else if(cmd == 1){
                  int orderNum = 0;
                  while(orderNum == 0){
                      mutex_.lock();
                      orderNum = sharedData_->orderNum;
                      mutex_.unlock();
                  }
                  query_order(orderNum);
              }
              else if(cmd == 3){
                  alert();
              }
          }
      }
    return 0;
  }

};
#endif //LAB6_USER_INTERFACE_H

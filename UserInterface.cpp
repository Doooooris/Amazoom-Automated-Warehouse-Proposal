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
#include <limits>
#include <string>
#include <sstream>
#include "SharedData.h"
#include "Order.h"
#include "Item.h"

static const char ORDER_STATUS = '1';
static const char STOCK_NUMBER = '2';
static const char GET_ALERTS= '3';
static const char USER_QUIT = '4';

    void do_query_order(){
        cpen333::process::mutex mutex_("Project_SharedData_Mutex");
        mutex_.unlock();
        cpen333::process::shared_object<SharedData> sharedData_("Project_Shared_Data");
        int orderNum;
        std::cout << std::endl << "Search Order" << std::endl;
        std::cout << " Order Number : ";
        std::cin >> orderNum;
        // gobble newline
        std::cin.ignore (std::numeric_limits<std::streamsize>::max(), '\n');
        
        mutex_.lock();
        sharedData_ -> cmd = 1;
        sharedData_ -> orderNum = orderNum;
        mutex_.unlock();
        
        std::string orderStatus = "";
        while(orderStatus.size() < 3){
            mutex_.lock();
            orderStatus = sharedData_->OrderStatus;
            mutex_.unlock();
        }
        std::cout<<"Order "<<orderNum<< " is "<< orderStatus<<std::endl;
        mutex_.lock();
        sharedData_ -> cmd = 0;
        sharedData_ -> orderNum = 0;
        sharedData_->OrderStatus = "0";
        mutex_.unlock();
    }
    
    void do_getStock(){
        cpen333::process::mutex mutex_("Project_SharedData_Mutex");
        mutex_.unlock();
        cpen333::process::shared_object<SharedData> sharedData_("Project_Shared_Data");
        std::string name;
        std::cout << std::endl << "Search Item" << std::endl;
        std::cout << " Item Name : ";
        std::getline(std::cin, name);
        
        mutex_.lock();
        sharedData_ -> cmd = 2;
        sharedData_ -> item_name = name;
        mutex_.unlock();
        
        int stockNum = 100;
        while(stockNum == 100){
            mutex_.lock();
            stockNum = sharedData_->stockNum;
            mutex_.unlock();
        }
        std::cout<< stockNum <<" "<< name <<" is in stock"<<std::endl;
        mutex_.lock();
        sharedData_ -> cmd = 0;
        sharedData_ -> item_name = "0";
        sharedData_ -> stockNum = 100;
        mutex_.unlock();
    }
    
    void do_alert(){
        cpen333::process::mutex mutex_("Project_SharedData_Mutex");
        cpen333::process::shared_object<SharedData> sharedData_("Project_Shared_Data");
        mutex_.lock();
        sharedData_ -> cmd = 3;
        mutex_.unlock();
        int alertReady = 0;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        while(alertReady == 0){
            mutex_.unlock();
            mutex_.lock();
            alertReady = sharedData_ -> alertReady;
            mutex_.unlock();
        }
        
        std::vector<std::string> low_stock_item;
        std::string emptystr[8] = {" "," "," "," "," "," "," "," "};
        std::cout<<"Low stock item: ";
        for(int i = 0; i < 8; i++){
            mutex_.lock();
            //low_stock_item.push_back((sharedData_ -> lowStcokItem)[i]);
            std::cout<<(sharedData_ -> lowStcokItem)[i]<<" ";
            (sharedData_ -> lowStcokItem)[i] = " ";
            mutex_.unlock();
        }
        std::cout<<";"<<std::endl;
        mutex_.lock();
        sharedData_ -> cmd = 0;
        sharedData_ -> alertReady = 0;
        mutex_.unlock();
    }
    
    // print menu options
    void print_shopping_menu() {
        
        std::cout << "==========================================" << std::endl;
        std::cout << "=                   Menu                 =" << std::endl;
        std::cout << "==========================================" << std::endl;
        std::cout << " (1) Query Order Status" << std::endl;
        std::cout << " (2) Check the Number in Stock of an Item" << std::endl;
        std::cout << " (3) Low Stock Items Alerts" << std::endl;
        std::cout << " (4) Quit" << std::endl;
        std::cout << "==========================================" << std::endl;
        std::cout << "Enter number: ";
        std::cout.flush();
    }
    
    void do_goodbye() {
        std::cout << "Goodbye Manager.\n" << std::endl;
        std::cout << std::endl;
    }

  /**
   * Main work function
   * @return 0 when complete
   */
  int main() {
      
      char cmd = 0;
      while(cmd != USER_QUIT){
          //print shopping menu
          print_shopping_menu();
          std::cin >> cmd;
          std::cin.ignore (std::numeric_limits<std::streamsize>::max(), '\n');
          
          switch(cmd){
              case ORDER_STATUS:
                  do_query_order();
                  break;
              case STOCK_NUMBER:
                  do_getStock();
                  break;
              case GET_ALERTS:
                  do_alert();
                  break;
              case USER_QUIT:
                  do_goodbye();
                  break;
              default:
                  std::cout << "Invalid command number " << cmd << std::endl << std::endl;
          }
          cpen333::pause();
      }
      
    return 0;
  }


#ifndef LAB6_CUSTOMER_H
#define LAB6_CUSTOMER_H

#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/thread/thread_object.h>
#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/thread/semaphore.h>
#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/process/mutex.h>
#include <thread>
#include <vector>
#include "safe_printf.h"
#include "SharedData.h"
static const char CLIENT_SEARCH_ITEM = '1';
static const char CLIENT_REMOVE_ITEM= '2';
static const char CLIENT_PLACE_ORDER= '3';
static const char CLIENT_QUIT = '4';
static const char ADD_TO_SHOPPINGBAG = 'Y';
static const char NOT_ADD_TO_SHOPPINGBAG = 'N';

/**
 * Cilents search, add items into shopping bag, then place orders into a order_queue, wait for them to be
 * delivered
 */
class Webserver : public cpen333::thread::thread_object {
  OrderQueue& order_queue_;
  Warehouse& warehouse_;
    std::list<Item> shopping_bag;
    int iniOrder_id;
    cpen333::process::shared_object<SharedData> sharedData_;
    cpen333::thread::semaphore delivered;
    cpen333::thread::semaphore& OrderPlaced_;
    cpen333::process::mutex mutex_;
    std::list<Order>& orderReceived_;

 public:
  /**
   * Creates a customer
   * @param warehouse warehouse for customer to order item from
   * @param queue queue to place order into
   */
  Webserver(OrderQueue& queue, Warehouse& warehouse, cpen333::thread::semaphore& OrderPlaced, std::list<Order>& orderReceived) :
      warehouse_(warehouse), order_queue_(queue),delivered(0), shopping_bag(),iniOrder_id(1),
      OrderPlaced_(OrderPlaced), sharedData_("Project_Shared_Data"), mutex_("Project_SharedData_Mutex"), orderReceived_(orderReceived) {}

  /**
   * Server customer an order
   * @param order order that is complete
   */
  void delivery(const Item& item) {

    //==================================================
    // TODO: Notify main method that order is ready
    //==================================================
      delivered.notify();
  }
    
    void do_add(Item item){
        //add the item to shopping bag
        shopping_bag.push_back(item);
        std::cout<< "item is sucessfully add to shopping bag\n";
    }
    
    // search a item in warehouse
    void do_search() {
        std::string add2Bag;
        std::string name;
        
        //First search the item in warehousre
        std::cout << std::endl << "Search for Items" << std::endl;
        std::cout << " Product Name : ";
        std::getline(std::cin, name);
        
        Item searchedItem;
        if(warehouse_.IsInStock(name) == true){
            searchedItem = warehouse_.search(name);
            char cmd = 0;
            while(cmd != ADD_TO_SHOPPINGBAG){
            std::cout << std::endl << "Item "<< name <<" is in stock" << std::endl;
            std::cout << " Add to shopping bag? : "<< std::endl;
            std::cin >> cmd;
                if(cmd == ADD_TO_SHOPPINGBAG){
                    do_add(searchedItem);
                    return;
                }
                else if(cmd == NOT_ADD_TO_SHOPPINGBAG){
                    return;
                }
            }
        }
        else //no such item
            std::cout << std::endl << "   Search \"" << name << "\" failed: "<< std::endl;
    }
    
    void do_remove(){
        std::string name;
        
        //Enter the name of item to remove
        std::cout << std::endl << "Remove Items" << std::endl;
        std::cout << " Product Name : ";
        std::getline(std::cin, name);
        
        //Search if the item is in shopping bag
        Item item;
        for (std::list<Item>::iterator it=shopping_bag.begin(); it != shopping_bag.end(); ++it){
            item = *it;
            if(item.name == name){
                shopping_bag.erase(it);
                safe_printf("Item Remove\n");
                return;
            }
        }
        
        //item not in shopping bag
        safe_printf("Remove Failed\n");
    }

    void do_place_order(){
        if(shopping_bag.empty() == 1)
        {
            std::cout<<"Empty shopping bag, place order failed\n";
            return;
        }
        
        //Generate a order has the item in shopping bag
        Order order;
        order.items = shopping_bag;
        order.id = iniOrder_id;
        iniOrder_id = iniOrder_id + 1;
        Item item;
        order.weight = 0;
        for(std::list<Item>::iterator it=shopping_bag.begin(); it != shopping_bag.end(); ++it){
            item = *it;
            order.weight = order.weight + item.weight;
        }
        order.status = 'PLACED';
        order_queue_.add(order);
        orderReceived_.push_back(order);
        OrderPlaced_.notify();
        safe_printf("Order %d is placed\n", order.id);
        shopping_bag.clear();
    }
    
    void do_goodbye() {
        std::cout << "Goodbye Amazoom.\n" << std::endl;
        std::cout << std::endl;
    }
    
    // print menu options
    void print_shopping_menu() {
        
        std::cout << "\n=========================================" << std::endl;
        std::cout << "=             Shopping Menu             =" << std::endl;
        std::cout << "=========================================" << std::endl;
        std::cout << " (1) Search Item" << std::endl;
        std::cout << " (2) Remove Item" << std::endl;
        std::cout << " (3) Place Order" << std::endl;
        std::cout << " (4) Quit" << std::endl;
        std::cout << "=========================================" << std::endl;
        std::cout << "Enter number: ";
        std::cout.flush();
        
    }

  /**
   * Main webserver function
   * @return 0 when complete
   */
  int main() {
    safe_printf("A client starts webserver\n");
      
      //Client Interface
      char cmd = 0;
      while(cmd != CLIENT_QUIT){
          //print shopping menu
          print_shopping_menu();
          std::cin >> cmd;
          std::cin.ignore (std::numeric_limits<std::streamsize>::max(), '\n');
          
          switch(cmd){
              case CLIENT_SEARCH_ITEM:
                  do_search();
                  break;
              case CLIENT_REMOVE_ITEM:
                  do_remove();
                  break;
              case CLIENT_PLACE_ORDER:
                  do_place_order();
                  break;
              case CLIENT_QUIT:
                  do_goodbye();
                  break;
              default:
                  std::cout << "Invalid command number " << cmd << std::endl << std::endl;
          }
          cpen333::pause();
      }

    // wait for items to be delivered
      unsigned long nitem = shopping_bag.size();
      for(unsigned long i = 0;i < nitem; i++)
          delivered.wait();

    // stay for some time
    std::this_thread::sleep_for(std::chrono::seconds(1));

    safe_printf("Client get the order.\n");
      
    return 0;
  }
};

#endif //LAB6_CUSTOMER_H

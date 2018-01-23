#include "Warehouse.h"
#include "Robot.h"
#include "Webserver.h"
#include "DeliveryTruck.h"
#include "InventoryTruck.h"
#include "SingleOrderQueue.h"
#include "CircularOrderQueue.h"
#include "DynamicOrderQueue.h"
#include "Status.h"
#include "SharedData.h"
#include "UserInterface.h"

#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/thread/semaphore.h>
#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/process/shared_memory.h>
#include "/Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/process/pipe.h"
#include "/Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/process/unlinker.h"
#include "/Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/process/subprocess.h"
#include </Users/doooooris/Documents/Course_Material/CPEN_333/library/include/cpen333/process/socket.h>

#include <vector>
#include <list>
#include <limits>
#include <stdio.h>
#include <string.h>
/**
 * Main function to run the restaurant
 * @return#include <vector>
 */
int main() {
    cpen333::thread::semaphore docks(2);
    cpen333::thread::semaphore RobotSema(0);
    cpen333::thread::semaphore DeliveryTruckSema(0);
    cpen333::thread::semaphore InventoryTruckSema(0);
    cpen333::process::shared_object<Status> Status("Project_Status");
    cpen333::process::mutex mutexS("Project_SharedData_Mutex");
    cpen333::process::shared_object<SharedData> sharedData("Project_Shared_Data");
    cpen333::process::mutex mutexW("Project_Warehouse_Mutex");
    cpen333::process::mutex mutex("Project_Status_Mutex__");
    mutex.unlock();
    mutex.lock();
    Status -> deliveryTruck = 0;
    Status -> inventoryTruck = 0;
    Status -> orderPlaced = 0;
    mutex.unlock();
    mutexS.unlock();
    mutexS.lock();
    sharedData -> cmd = 0;
    sharedData -> orderNum = 0;
    sharedData -> stockNum = 100;
    
    mutexS.unlock();
    
    // create a shared pipe of size 1024 bytes, in "write" mode
    //cpen333::process::pipe pipe("ORDER_PIPE", cpen333::process::pipe::READ, 1024);
    //cpen333::process::unlinker<decltype(pipe)> unlinker(pipe);
    
    
  Warehouse warehouse(5,8,6);
  warehouse.load("./data/stock_item.json");
    Webserver* webserver;

  std::vector<Robot*> robots;
    DeliveryTruck* deliveryTruck;
    InventoryTruck* inventoryTruck;
    UserInterface* userInterface;
    

    std::list<Order> orderReceived;
    std::list<Order> orderReadyDel;
    std::list<Order> orderDelived;
    
    DynamicOrderQueue order_queue;
    DynamicOrderQueue loading_queue;
    DynamicOrderQueue import_queue;

    for (int i=0; i<4; ++i) {
        robots.push_back(new Robot(i, order_queue, import_queue, loading_queue, warehouse, RobotSema,orderReceived,orderReadyDel));
    }
    inventoryTruck = new InventoryTruck(import_queue,docks, RobotSema);
    deliveryTruck = new DeliveryTruck(loading_queue, docks, DeliveryTruckSema, orderReadyDel, orderDelived);
    webserver = new Webserver(order_queue, warehouse, RobotSema, orderReceived);
    userInterface = new UserInterface(orderReceived, orderReadyDel, orderDelived, warehouse);
   
    //first start webserver
    for (auto& robot : robots) {
        robot->start();
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    webserver->start();
    inventoryTruck ->start();
    deliveryTruck -> start();
    userInterface -> start();
    
    webserver->join();
    // wait for all customers to leave
    for (auto& robot : robots) {
        robot->join();
    }
    inventoryTruck->join();
    deliveryTruck->join();
    userInterface -> join();
    
  return 0;
}

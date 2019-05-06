/*
 * Name: Jeffrey Strahm, Joshua Cash, Hannah Reinbolt
 * Class: Operating Systems
 * Assignment Shell Emulator
 * Compile with: g++ -pthread monitor.cpp -o taskMonitor
 */

#include <iostream>
#include <chrono>
#include <unistd.h>
#include "taskMonitor.h"

// create a mutlitask thread that just checks for the quit command
void static check(bool &stop) {
  std::string input;
  while(true) {
    std::cin >> input;
    if (input[0] == 'q' || input[0] == 'Q') {
      stop = true;
      break;
    }
  }
}

int main(int argc, char *argv[])
{
  // print out to explain the program
  std::cout << "welcome to the task monitor! To start, enter anything!" << std::endl;
  std::cout << "If you wish to exit the program, simply enter the letter q after it starts." << std::endl;
  // make sure the user wants it to start
  std::string junk;
  std::cin >> junk;
  //std::cout << argc << std::endl;
  Display::TaskMonitor monitor;
  int size = -1;
  int hight = -1;
  // track if it needs to stop
  bool stop = false;
  // checks for the arguments -set-size=## and -set-hight=##
  if (argc > 1) {
    std::string argument;
    // iterates over the arguments
    for (int i = 1; i < argc; i++) {
      //std::cout << argv[i] << "|" << std::endl;
      argument = argv[i];
      // make sure the argument is of a realistic size.
      if (argument.size() < 11) {
        // tell them what they did wrong.
        std::cout << "improper command formating. Commands are \'-set-size=#\' and \'-set-hight=#\'" << std::endl;
        break;
      }
      // make sure they are designating a -
      if (argument[0] != '-') {
        // tell them what they did wrong.
        std::cout << "improper format, arguments must start with -" << std::endl;
        break;
      }
      // make sure they are using a existing prefix
      if (argument.substr(1, 4).compare("set-") != 0) {
        // tell them what they did wrong.
        std::cout << "only \"set-\" commands are supported" << std::endl;
        break;
      }
      // make sure they are setting a existing setting.
      if (argument.substr(5, 5).compare("size=") == 0) {
        //std::cout << "set1 - " << stoi(argument.substr(10)) << "|" << std::endl;
        // set it.
        size = stoi(argument.substr(10));
      }
      // make sure they are setting a existing setting.
      if (argument.substr(5, 6).compare("hight=") == 0) {
        //std::cout << "set2 - " << stoi(argument.substr(11)) << "|"  << std::endl;
        // set it.
        hight = stoi(argument.substr(11));
      }
    }
  }
  //usleep(5000000);
  // set the size if anything was enterd.
  if (size != -1 || hight != -1) {
    monitor.changeSize(size, hight);
  }
  // start the quitting thread.
  std::thread (check, std::ref(stop)).detach();
  // set up the computer and enter in fake processes.
  monitor.setComp(100);
  monitor.addProcess("TEST1"      , 1, 0, 10, 100);
  monitor.addProcess("TEST2"      , 2, 1, 0, 59);
  monitor.addProcess("TESTING3"   , 3, 1, 2, 120);
  monitor.addProcess("TESTING4"   , 4, 1, 6, 1800);
  monitor.addProcess("TESTING5"   , 5, 2, 0, 18000);
  monitor.addProcess("TESTING678" , 6, 2, 0, 2);
  monitor.addProcess("TESTING700" , 7, 1, 4, 1);
  monitor.addProcess("TESTING800" , 7, 2, 4, 3600);
  monitor.addProcess("TESTING9001", 8, 1, 4, 600);
  // print the stuff.
  monitor.print();
  // continue printing the stuff after short breaks until it is told to stop.
  while (true) {
    if (stop) {
      break;
    }
    usleep(500000);
    monitor.print();
  }

  // safety return
  return 0;
}
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

void error(const char *msg)
{
  std::cout << msg << std::endl;
  // exit(1);
}

// create a mutlitask thread that just checks for the quit command
void static check(bool &stop)
{
  std::string input;
  while(true)
  {
    std::cin >> input;
    if (input[0] == 'q' || input[0] == 'Q')
    {
      stop = true;
      break;
    }
  }
}

void static talker(int portno, bool &stop, Display::TaskMonitor &monitor)
{
  int sockfd, newsockfd;
  socklen_t clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
          sizeof(serv_addr)) < 0) 
          error("ERROR on binding");
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
  if (newsockfd < 0)
  {
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    error("ERROR on accept");
  }
  while(!stop)
  {
    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
    std::cout << "\n\n\n\n\n\nread message\n\n\n\n\n\n\n\n\nread message" << std::endl;
    if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n",buffer);
    if (buffer[0] == 'n')
    {
      // store the attributes of the new process.
      std::string name = "";
      int id = 0;
      int threadId = 0;
      int memory = 0;
      ull time = 0;
      // store the location that is being processed, and the current input.
      int loc = 1;
      int num = 0;
      // store which input we are on.
      while(buffer[loc] != '|')
      {
        loc++;
        if (buffer[loc] == '-')
        {
          num++;
          continue;
        }
        if (num == 0)
        {
          id *= 10;
          id += buffer[loc] - '0';
        }
        else if (num == 1)
        {
          name += buffer[loc];
        }
        else if (num == 2)
        {
          threadId *= 10;
          threadId += buffer[loc] - '0';
        }
        else if (num == 3)
        {
          memory *= 10;
          memory += buffer[loc] - '0';
        }
        else if (num == 4)
        {
          time *= 10;
          time += buffer[loc] - '0';
        }
        else
        {
          break;
        }
      }
      // add the process to the to the task manager
      monitor.addProcess(name, id, threadId, memory, time);
      n = write(newsockfd,"I got your message",18);
    }
    else if (buffer[0] == 'u')
    {
      // store the attributes of the new process.
      int id = 0;
      int memory = 0;
      ull time = 0;
      // store the location that is being processed, and the current input.
      int loc = 1;
      int num = 0;
      // store which input we are on.
      while(buffer[loc] != '|')
      {
        loc++;
        if (buffer[loc] == '-')
        {
          num++;
          continue;
        }
        if (num == 0)
        {
          id *= 10;
          id += buffer[loc] - '0';
        }
        else if (num == 1)
        {
          memory *= 10;
          memory += buffer[loc] - '0';
        }
        else if (num == 2)
        {
          time *= 10;
          time += buffer[loc] - '0';
        }
        else
        {
          break;
        }
      }
      // updates the process
      monitor.updateProcess(id, memory, time);
      n = write(newsockfd,"I got your message",18);
    }
    else if (buffer[0] == 'q')
    {
      close(newsockfd);
      close(sockfd);
      n = write(newsockfd,"quitting....",18);
      return;
    }
    else
    {
      n = write(newsockfd,"Improper message",18);
    }
  }
  if (n < 0) error("ERROR writing to socket");
  close(newsockfd);
  close(sockfd);
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
  if (argc > 1)
  {
    std::string argument;
    // iterates over the arguments
    for (int i = 1; i < argc; i++)
    {
      //std::cout << argv[i] << "|" << std::endl;
      argument = argv[i];
      // make sure the argument is of a realistic size.
      if (argument.size() < 11)
      {
        // tell them what they did wrong.
        std::cout << "improper command formating. Commands are \'-set-size=#\' and \'-set-hight=#\'" << std::endl;
        break;
      }
      // make sure they are designating a -
      if (argument[0] != '-')
      {
        // tell them what they did wrong.
        std::cout << "improper format, arguments must start with -" << std::endl;
        break;
      }
      // make sure they are using a existing prefix
      if (argument.substr(1, 4).compare("set-") != 0)
      {
        // tell them what they did wrong.
        std::cout << "only \"set-\" commands are supported" << std::endl;
        break;
      }
      // make sure they are setting a existing setting.
      if (argument.substr(5, 5).compare("size=") == 0)
      {
        //std::cout << "set1 - " << stoi(argument.substr(10)) << "|" << std::endl;
        // set it.
        size = stoi(argument.substr(10));
      }
      // make sure they are setting a existing setting.
      if (argument.substr(5, 6).compare("hight=") == 0)
      {
        //std::cout << "set2 - " << stoi(argument.substr(11)) << "|"  << std::endl;
        // set it.
        hight = stoi(argument.substr(11));
      }
    }
  }
  //usleep(5000000);
  // set the size if anything was enterd.
  if (size != -1 || hight != -1)
  {
    monitor.changeSize(size, hight);
  }
  // start the quitting thread.
  std::thread (check, std::ref(stop)).detach();
  std::thread (talker, 41717, std::ref(stop), std::ref(monitor)).detach();
  monitor.setComp(500);
  // set up the computer and enter in fake processes.
  //monitor.setComp(100);
  //monitor.addProcess("TEST1"      , 1, 0, 10, 100);
  //monitor.addProcess("TEST2"      , 2, 1, 0, 59);
  //monitor.addProcess("TESTING3"   , 3, 1, 2, 120);
  //monitor.addProcess("TESTING4"   , 4, 1, 6, 1800);
  //monitor.addProcess("TESTING5"   , 5, 2, 0, 18000);
  //monitor.addProcess("TESTING678" , 6, 2, 0, 2);
  //monitor.addProcess("TESTING700" , 7, 1, 4, 1);
  //monitor.addProcess("TESTING800" , 7, 2, 4, 3600);
  //monitor.addProcess("TESTING9001", 8, 1, 4, 600);
  // print the stuff.
  monitor.print();
  // continue printing the stuff after short breaks until it is told to stop.
  while (true)
  {
    if (stop)
    {
      break;
    }
    usleep(500000);
    monitor.print();
  }

  // safety return
  return 0;
}
#ifndef COMPUTER_H
#define COMPUTER_H
#include <vector>
#include <iostream>
#include <fstream>
#include "node.h"
#include <limits>
#include <sstream>
#include <map>
#include <string>
#ifndef ull
#define ull unsigned long long
#endif
#include "thread.h"
#include <queue>
#include <stdio.h> 
#include <fstream>
#include <unistd.h> 

namespace Shell
{
  // struct for easy task storage
  struct process {
    std::string name;
    int id;
    int threadId;
    int memory;
    // int cpu;
    ull time;
  };

  const std::vector<std::string> CMDS = 
  {
    "ls",
    "touch",
    "pwd",
    "cd",
    "mkdir",
    "rm",
    "rmdir",
    "chmod",
    "chown",
    "groupadd",
    "usermod",
    "userdel",
    "useradd",
    "groups",
    "chgrp",
    "whoami",
    "switchto",
    "logout",
    "exit"
  };

  // Computer class 
  // Represents the OS who controls the file System.
  class Computer
  {
    // Private vars
    private:
      // The root File
      Node* rootFile;
      // The current Directory the System is looking at
      Node* curDir;
      // A list of users.
      std::map<std::string, User*> users;
      // The current logged in user.
      User* curUser;
      // Reference to the root (makes things easier for me.)
      User* root;
      // The name of this computer
      std::string computerName;
      /**
       * set of the groups aviable
       */
      std::set<std::string> groups;
      /**
       * map of current threads
       */
      std::map<ull, Thread*> threads;
      /**
       * used to track the processes that need to be updated/created.
       */
      std::queue<process> processesToUpdate;

      bool running;

    // Public functions
    public:

      // The constructor taking in the computer name
      Computer(std::string name)
      {
        // set the name and call the default constructor
        computerName = name;
        Computer();
      }

      // Deconstructor, make sure we don't have memory leaks :)
      ~Computer()
      {
        // Users are a pair so second is the actual user.
        // Delete all users.
        for(auto userPair : users)
          delete userPair.second;

        for(auto threadPair : threads)
          delete threadPair.second;
        // Delete the root : let its deconstructor handle deleting
        // the rest of the file system.
        delete rootFile;
        // Make sure we clear the pointers to prevent some acedental
        // pointer to mem we don't own.
        users.clear();
        rootFile = nullptr;
        curDir = nullptr;
        curUser = nullptr;
        root = nullptr;
      }

      // Main constructor - does the heavy lifting
      Computer()
      {
        running = true;
        // No user to start off with, need to login.
        curUser = nullptr;
        // Create the root of the file system.
        rootFile = new Node("", true, nullptr, 0, "root", "root");
        // Set the root's parent to itself - makes it auto handle ../ on root. 
        // simple hack to make my life easier down the line.
        rootFile->parent = rootFile;
        // Set the default perms for the root
        rootFile->perms = std::array<int, 3>{7,7,7};
        // Make the root user.
        root = new User("root", true, true, "toor");
        // set the computer name.
        computerName = "computer";
        // move the current location to the root - this will change depending
        // on who logs in
        curDir = rootFile;
        // Add the root to the list of users.
        users.emplace(root->Username(),root);
        // make a new user without perms
        users.emplace("user", new User("user", false, true, "pie"));

        // create home and root directory
        rootFile->AddChild(root, new Node("root", true, rootFile));
        rootFile->AddChild(root, new Node("home", true, rootFile));
        // make the user's directory
        curDir = rootFile->children["home"];
        curDir->AddChild(root, new Node("user", true, curDir, 1, "user", "user"));
        curDir = rootFile->children["home"]->children["user"];

        groups.insert("user");
        groups.insert("users");
        groups.insert("root");
        groups.insert("wheel");
        root->AddToGroup("wheel");
        Thread* thread = new Thread(10000000, Thread::fifo);
        threads.emplace(thread->ID(), thread);
        
      }

      // Running the computer. Handles all operations from here.
      void run()
      {
        // login
        login();
        // Start the console.
        running = true;
        std::thread t(&Computer::client, std::ref(*this));
        console();
        running = false;
        t.join();
      }
      
      void threadUpdate()
      {
        while(running)
        {
          for(auto threadObj : threads)
          {
            auto l = threadObj.second->Update(1);
            for(auto item : l)
              killTask(item);
            const Task *t = threadObj.second->GetRunningTask();
            if (t != nullptr)
              updateTask(t->ID(), t->MemoryUsage(), t->TimeRemaining());
          }
          sleep(1);
        }
      }
      bool Running() const { return running; }
    // Private functions
    private:
    // handles the client side server
    // @input running: should be a pointer to the running variable of the computer.
    // @input conected: should be a pointer to the connected variable of the computer.
    // @input tasks: should be a pointer to the proccesses vector.
    // @input update: should be a pointer to the update queue.
    void client()
    {
      //std::ofstream writing;
      std::ofstream outfile ("monitor/share.txt",std::ofstream::app);
      std::string input;
      char buffer[256];

      input = "c\n";
      for (unsigned int i = 0; i < input.size(); i++)
      {
        buffer[i] = input[i];
      }
      outfile.write(buffer, input.size());

      while (running) 
      {
        //writing.open("share.txt");
        if (!outfile.is_open())
        {
          std::cout << "HUSTON WE HAVE A PROBLEM!" << std::endl;
        }
        if (processesToUpdate.size() > 0)
        {
          process up = processesToUpdate.front();
          processesToUpdate.pop();
          if (up.threadId == -1)
          {
            input = "u" + std::to_string(up.id) + "-" +
                    std::to_string(up.memory) + "-" +
                    std::to_string((up.time > 999999) ? 999999 : up.time) + "|\n";
          }
          else
          {
            input = "n" + std::to_string(up.id) + "-" + up.name + "-" +
                    std::to_string(up.threadId) + "-" + std::to_string(up.memory)
                    + "-" + std::to_string((up.time > 999999) ? 999999 : up.time)
                    + "|\n";
          }
          for (unsigned int i = 0; i < input.size(); i++)
          {
            buffer[i] = input[i];
          }

          outfile.write(buffer, input.size());
          outfile.flush();
          //std::cout << buffer << "----" << std::endl;
        }
        //writing.close();
      }

      input = "q\n";
      for (unsigned int i = 0; i < input.size(); i++)
      {
        buffer[i] = input[i];
      }
      outfile.write(buffer, input.size());

      outfile.close();

      return;
    }


      // Login function. returns true if user and password is valid
      // false otherwise
      void login()
      {
        // vars to capture user info
        std::string name, pass;
        // if logged in or not
        bool loggedIn = false;
        curUser = nullptr;
        // Login loop
        do
        {
          // prompt
          std::cout << "Login: ";
          std::cin >> name;
          // Search for the user
          auto found = users.find(name);
          if(found != users.end() && !users[name]->HasPassword())
          {
            curUser = found->second;
            break;
          }
          std::cout << "Password: ";
          std::cin >> pass;
          // Validate that this is a user
          // IF we find the user, verified the pass is right
          if(found != users.end() && users[name]->VerifyPassword(pass))
          {
            // set current user of computer 
            curUser = found->second;
          }
          // if found
          loggedIn = curUser != nullptr;
          // if not logged in, report and retry
          if(!loggedIn)          
            std::cout << "Login incorrect" << std::endl;
        } while (!loggedIn);

        // sets the user in their home directory
        // Make sure we clear the buffer out so getline works
        std::cin.clear(); 
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        commands("cd", std::vector<std::string>());
        return; 
      }

      // Console handles taking in input and outputting correct response.
      // is what emulates running the shell.
      void console()
      {
        // control var for machine
        bool looping = true;
        // storages variables
        std::string dir;
        std::string input;
        // While we haven't quit
        while(looping)
        {
          // Gets current working directory
          dir = pwd();
          //looks for user's name in wd
          int pos = dir.find(curUser->Username());
          // if found, use ~ insead the complete path.
          if(pos > -1)
            dir = "~" + dir.substr(pos + curUser->Username().length());
          // output the status line. 
          std::cout << "\033[32m" << curUser->Username() << "@" << computerName <<"\033[0m:\033[34m" << dir
               << (curUser == root ? "\033[0m#" : "\033[0m$") << " ";
          // Get input from the user.
          std::getline(std::cin, input);
          // Parse it and handle it.
          looping = parser(input);
        }
      }

      // Parses input. returns true if the console should continue.
      bool parser(std::string input)
      {
        // storage vars for easier manipulation of data.
        std::stringstream stream(input);
        std::string command;
        std::vector<std::string> args;
        // Using a string stream to quickly break up input on spaces.
        stream >> command;
        // get the rest of the input
        std::string temp;
        // store it as an argument
        while(stream >> temp)
        {
          args.push_back(temp);
        }
        // return the results of commands func
        return commands(command, args);
      }

      // attempts to run a command using the command and the args for it.
      // returns true if console should keep running.
      bool commands(std::string command, std::vector<std::string> args)
      {
        // Handles ls command
        if(command == "")
        {
          return true;
        }
        else if(command == "ls")
        {
          // if command has args
          if(args.size() > 0)
          {
            // if it has only the correct arg.
            if(args.size() == 1 && args[0] == "-l") 
            {
              // display output
              for(auto childPair :curDir->Children())
              {
                Node* child = childPair.second;
                if(Node::HasPermissions(curUser, child, Read))
                {
                  std::cout << child->PermsStr() << " " << child->NumDirs()
                            << " " << child->User() << " " << child->Group() << " "
                            << child->Size() << " " << child->TimeStr() << " "
                            // Adds blue color if it is a dir
                            << (child->isDir ? "\033[34m" : (Node::HasPermissions(curUser, child, Execute) ? "\033[32m" : ""))
                            << child->name << "\033[0m" << std::endl;
                }
              }
            }
            else
            {
              // otherwise invalid useage
              std::cout << "Invalid use - For help use: help ls\n";
            }
            
          }
          // no args
          else
          {
            // display simple output
            for(auto child : curDir->Children())
            {
              if(Node::HasPermissions(curUser, child.second, Read))
              {
                std::cout << (child.second->IsDir() ? "\033[34m" : (Node::HasPermissions(curUser, child.second, Execute) ? "\033[32m" : "")) 
                          << child.first << "\033[0m ";
              }
            }
            // adds a new line at the end only if there was something to print
            if(curDir->Children().size() > 0)
              std::cout << std::endl;
          }
          
        }
        else if(command == "logout")
        {
          login();
        }
        // Handles pwd command
        else if(command == "pwd")
        {
          // outputs the current directory
          std::cout << pwd() << std::endl;
        }
        // Handles the exit command
        else if(command == "exit")
        {
          // return false to signal ending
          return false;
        }
        // Handles mkdir command
        else if(command == "mkdir")
        {
          // if there are no arguments
          if(args.size() == 0)
          {
            // error
            std::cout << "Invalid use - For help use: help mkdir\n";
          }
          // else 
          else
          {
            // iterate over arguments
            for(std::string arg : args)
            {
              // Attempt to add new directory if fails output such a message.
              if(Node::HasPermissions(curUser, curDir, Write))
              {
                if(!curDir->AddChild(curUser, new Node(arg, true, curDir)))
                {
                  std::cout << "mkdir: cannot create directory '" << arg << "': File exits\n"; 
                }
              }
              else
              {
                std::cout << "mkdir: cannot create directory '" << arg << "' Permission Denied!\n";
              }
              
            }
          }
        }
        // Handles touch command
        else if(command == "touch")
        {
          // if there are no args
          if(args.size() == 0)
          {
            // error
            std::cout << "touch: Invalid use - For help use: help touch\n";
          }
          // otherwise attempt do it
          else
          {
            // iterate over args
            for(std::string arg : args)
            {
              // try to add and if that fails, update the current timestamp
              if(curDir->children.find(arg) == curDir->children.end())
              {
                if(Node::HasPermissions(curUser, curDir, Write))
                  curDir->AddChild(curUser, new Node(arg, false, curDir, 1, curUser->Username(), curUser->Username()));
                else
                  std::cout << "touch: Cannot create '" << arg << "' Permission Denied!\n";
              }
              else
              {
                if(Node::HasPermissions(curUser, curDir->children[arg], Write))
                  curDir->children[arg]->UpdateTimeStamp();
                else
                  std::cout << "touch: Cannot update '" << arg << "' Permission Denied!\n";
              }
            }
          }
        }
        // Handles cd command
        else if (command == "cd")
        {
          // if no args
          if(args.size() == 0)
          {
            // if user is root 
            if(curUser == root)
            {
              // set current directory to root's home
              curDir = rootFile->children["root"];
            }
            // else set current directory to the user's home
            else curDir = 
              rootFile->children["home"]->children[curUser->Username()];
            // if their directory doesn't exist anymore, put them at the root.
            if(curDir == nullptr || !Node::HasPermissions(curUser, curDir, Execute))
              curDir = rootFile;
          }
          // if there is args and it is more than one
          else if(args.size() > 1)
          {
            // error
            std::cout << "cd: too many arguments - For help use: help cd\n";
          }
          // else if only one arg
          else
          {
            // attempt to find the file.
            Node* file = findFile(args[0]);
            // if file exists and is not a directory error
            if(file != nullptr)
              if(!file->isDir)
                std::cout << "cd: " << args[0] << " Not a directory\n";
              else
              {
                if(Node::HasPermissions(curUser, file, Execute))
                  // else set curDir to it
                  curDir = file;
                else
                  std::cout << "cd: Permission Denied!\n";
              }
            // else file isn't real
            else std::cout << "cd: Directory does not exist\n";
          }
        }
        // Handles rm command
        else if (command == "rm")
        {
          // if no args
          if(args.size() == 0)
          {
            // error
            std::cout << "rm: Invalid use - For help use: help rm\n";
          }
          // else if there are args
          else
          {
            // iterate over them
            for(auto arg : args)
            {
              // try to find the arg
              Node* file = findFile(arg);
              // if it doesn't exist, error
              if(file == nullptr)
              {
                std::cout << "rm: File '" << arg << "' not found\n";
              }
              else if(!Node::HasPermissions(curUser, file, Write))
              {
                std::cout << "rm: cannot remove '" << arg << "': Permission Denied!'n";
              }
              // if file is a directory
              else if(file->isDir)
              {
                // error
                std::cout << "rm: cannot remove '" << arg 
                          << "': Is a directory\n";
              }
              // else is valid so delete
              else
              {
                if(file->parent->DeleteChild(curUser, file))
                  delete file;
              }
            }
          }
        }
        // Handles rmdir command
        else if (command == "rmdir")
        {
          // if there are no args
          if(args.size() == 0)
          {
            // error
            std::cout << "rm: Invalid use - For help use: help rmdir\n";
          }
          // has args
          else
          {
            // iterate over args
            for(auto arg : args)
            {
              //find file
              Node* file = findFile(arg);
              // if not found
              if(file == nullptr)
              {
                // error
                std::cout << "rm: File '" << arg << "' not found\n";
              }
              else if(!Node::HasPermissions(curUser, file, Write))
              {
                std::cout << "rm: failed to remove '" << arg << "': Permission Denied!\n";
              }
              // if file is not a directory
              else if(!file->isDir)
              {
                // error
                std::cout << "rm: failed to remove '" << arg 
                          << "': Not a directory\n";
              }
              // if there is stuff in the file,
              else if(file->children.size() > 0)
              {
                // error
                std::cout << "rm: failed to remove '" << file->Name() 
                          << "': Directory not empty\n";
              }
              // else delete files
              else
              {
                // if we try to delete this directory, move back a directory
                if(file == curDir)
                  curDir = file->parent;
                // delete the file if it isn't the root.
                if(file != rootFile)
                {
                  file->parent->DeleteChild(root, file);
                  delete file;
                }
                // else error
                else std::cout << "rm: Permission Denied\n";
              }
            }
          }
        }
        // Handles chmod command
        else if(command == "chmod")
        {
          // int for conversion
          int permInt;
          // if there are more than 2 args
          if(args.size() > 2)
          {
            // error
            std::cout << 
              "chmod: Too many arguments - For help use: help chmod\n";
          }
          // if args is less 2 
          else if(args.size() < 2)
          {
            // error
            std::cout << 
              "chmod: Not enough arguments - For help use: help chmod\n";
          }
          else
          {
            try
            {
              // convert the number to an int
              permInt = std::stoi(args[0]);
            }
            catch(const std::invalid_argument& e)
            {
              permInt = -1;
            }
            // make sure it is a valid 
            if(permInt > 777 || permInt < 0)
            {
              // error
              std::cout <<
                "chmod: Invalid permission number\n";
            }
            else 
            {
              // iterate over args
              for(auto arg : args)
              {
                // if is the first ignore
                if(arg == args[0]) continue;
                // try to find file
                Node* file = findFile(arg);
                // if file doesn't exist
                if(file == nullptr)
                {
                  // error 
                  std::cout << "chmod: File '" << arg << "' does not exist\n";
                }
                else if(!Node::HasPermissions(curUser, file, Write))
                {
                  std::cout << "chmod: File '" << arg << "' Permission Denied\n";
                }
                else if(rootFile == file)
                {
                  std::cout << "chmod: cannot modify permissions of the root\n";
                }
                // else
                else
                {
                  // break up the digit
                  file->perms = 
                    {
                      permInt / 100, 
                      permInt / 10 % 10, 
                      permInt % 10
                    };
                }
              }
            }
          }
        }
        // Handles groupadd command
        else if (command == "groupadd")
        {
          // wrong num args
          if(args.size() < 1)
            std::cout << "groupadd: Invalid number of arguments\n" << std::endl;
          // go over all args
          for(std::string arg : args)
          {
            // if not valid
            if(groups.find(arg) == groups.end())
              // complain
              std::cout << "groupadd: group '" << arg << "' already exists\n";
            else
              // else create
              groups.insert(arg);
          }
        }
        // Handles usermod command
        else if (command == "usermod")
        {
          if(args.size() < 3)
          {
            std::cout << "usermod: Invalid number of arguments\n";
          }
          else
          {
            // check for flags
            if(args[0] != "-a")
              std::cout << "usermod: Invalid flag '" << args[0] << "'\n";
            else if(args[1] != "-G")
              std::cout << "usermod: Invalid flag '" << args[1] << "'\n";
            else
            {
              int count = 0;
              for(std::string arg : args)
              {
                // hacky way to ignore the first 2 args
                if(count < 2)
                {
                  count++;
                  continue;
                }

                if(groups.find(arg) == groups.end())
                {
                  std::cout << "usermod: Invalid group '" << arg << "'\n";
                }
                else if(curUser->IsInGroup(arg))
                {
                  std::cout << "usermod: already in group '" << arg << "'\n";
                } 
                else
                {
                  // if valid group that we aren't in, add it to our groups
                  curUser->AddToGroup(arg);
                }
              }
            }
          }
        }
        // Handles useradd command
        else if (command == "useradd")
        {
          User* usr;
          //TODO: make this work so you can add multiple users at once
          if(args.size() < 1)
          {
            std::cout << "useradd: Invalid number of arguments\n"; 
          }
          // if didn't find user...
          else if(users.find(args[args.size() - 1]) != users.end())
          {
            std::cout << "useradd: User '" << args[args.size() - 1] << "' already exists\n";
          }
          // if more than one arg means adding group as well
          else if(args.size() > 1)
          {
            if(args.size() != 3)
            {
              std::cout << "useradd: invalid number of arguments\n";
            }
            // check to make sure we have our flag
            else if(args[0] != "-G")
            {
              std::cout << "useradd: invalid flag '" << args[0] << "'\n";
            }
            else 
            {
              // add user
              usr = AddUser(args[2], "");
              // get groups to add to user
              std::string groupsToAdd = args[1];
              // change to space delimited list 
              for(uint k = 0; k < groupsToAdd.size(); k++)
              {
                if(groupsToAdd[k] == ',')
                  groupsToAdd[k] = ' ';
              }
              // insert into a string stream for extracting
              std::stringstream groupsAdding(groupsToAdd);
              // iterate over all the groups
              while(groupsAdding >> groupsToAdd)
              {
                // if not valid complain
                if(groups.find(groupsToAdd) == groups.end())
                {
                  std::cout << "useradd: invalid group '" << groupsToAdd << "'\n";
                }
                else
                {
                  // add group to user
                  usr->AddToGroup(groupsToAdd);
                }
              }
            }
          }
          else
          {
            // else add user
            AddUser(args[0], "");
          }
          
        }
        // Handles userdel command
        else if (command == "userdel")
        {
          // Invalidate
          if(args.size() < 1)
          {
            std::cout << "userdel: Invalid number of arguments\n";
          }
          else if(args.size() > 1)
          {
            if(args.size() != 3)
            {
              std::cout << "userdel: Invalid number of arguments\n";
            }
            else if(args[0] != "-G")
            {
              std::cout << "userdel: Invalid flag '" << args[0] << "'\n";
            }
            else if(groups.find(args[1]) == groups.end())
            {
              std::cout << "userdel: Invalid group '" << args[1] << "'\n";
            }
            else if(users.find(args[2]) == users.end())
            {
              std::cout << "userdel: User '" << args[2] << "' doesn't exist\n";
            }
            else if(!users[args[2]]->IsInGroup(args[1]))
            {
              std::cout << "userdel: User '" << args[2] << "' isn't in group '" << args[1] << "'\n";
            }
            // all good to remove
            else
            {
              users[args[2]]->RemoveFromGroup(args[1]);
            }
          }
          else if(users.find(args[0]) == users.end())
          {
            std::cout << "userdel: User '" << args[0] << "' doesn't exist\n";
          }
          else if(args[0] == root->Username())
          {
            std::cout << "userdel: Cannot delete root, Permission denied\n";
          }
          else if(args[0] == curUser->Username())
          {
            std::cout << "userdel: Cannot delete the current user\n";
          }
          else
          {
            // all good to remove
            User* usr = users[args[0]];
            users.erase(usr->Username());
            delete usr;
          }
        }
        // Handles groups command
        else if (command == "groups")
        {
          if(args.size() > 1)
          {
            std::cout << "groups: Invalid number of arguments\n";
          }
          else if(args.size() == 0)
          {
            std::cout << curUser->Username() << ": ";
            for(std::string group : curUser->Groups())
              std::cout << group << " ";
            std::cout << std::endl;
          }
          else if(users.find(args[0]) == users.end())
          {
            std::cout << "groups: User '" << args[0] << "' doesn't exist\n";
          }
          else
          {
            std::cout << args[0] << ": ";
            // iterate over all groups in user
            for(std::string group : users[args[0]]->Groups())
            {
              // print them out
              std::cout << group << " ";
            }
            std::cout << std::endl;
          }
          
        }
        // handles chown command
        else if (command == "chown")
        {
          if(args.size() != 2)
          {
            std::cout << "chown: invalid number of arguments\n";
          }
          else
          {
            // find file
            Node* file = findFile(args[1]);
            if(file == nullptr)
            {
              std::cout << "chown: error file '" << args[1] << "' doesn't exist\n";
            }
            else
            {
              // check for "user:group" format
              int pos = args[0].find(':');
              // if found it then parse it
              if(pos >= 0)
              {
                std::string owner = args[0].substr(0, pos + 1);
                std::string group = args[0].substr(pos);
                // invalidate
                if(users.find(owner) == users.end())
                {
                  std::cout << "chown: invalid user '" << args[0] << "'\n";
                }
                else if(groups.find(group) == groups.end())
                {
                  std::cout << "chown: invalid group '" << args[0] << "'\n";
                }
                else if(!Node::HasPermissions(curUser, file, Write))
                {
                  std::cout << "chown: permission denied\n";
                }
                else
                {
                  // good to go
                  file->group = group;
                  file->user = owner; 
                }
              }
              // else just user
              else
              {
                // invalidate
                if(users.find(args[0]) == users.end())
                {
                  std::cout << "chown: invalid user '" << args[0] << "'\n";
                }
                else if(!Node::HasPermissions(curUser, file, Write))
                {
                  std::cout << "chown: permission denied\n";
                }
                else
                  // good to go
                  file->user = args[0];
              }
            }
          }
        }
        // handles chgrp command
        else if (command == "chgrp")
        {
          if(args.size() != 2)
          {
            std::cout << "chgrp: invalid amount of arguments\n";
          }
          else
          {
            // find file
            Node* file = findFile(args[1]);
            if(file == nullptr)
              std::cout << "chgrp: error file '" << args[1] << "' doesn't exist\n";
            else if(groups.find(args[0]) == groups.end())
              std::cout << "chgrp: invalid group '" << args[0] << "'\n";
            else if(!Node::HasPermissions(curUser, file, Write))
              std::cout << "chgrp: permission denied\n";
            // set group
            else
              file->group = args[0];
          }
        }
        // handles whoami command
        else if (command == "whoami")
        {
          if(args.size() > 0)
          {
            std::cout << "whoami: extra operand '" << args[0] << "'\n";
          }
          else
          {
            // simple print who this user is
            std::cout << curUser->Username() << std::endl;
          }
        }
        // handles switchto command
        else if (command == "switchto")
        {
          // switches to another user
          if(args.size() != 1)
          {
            std::cout << "switchto: invalid number of arguments\n";
          }
          else
          {
            auto found = users.find(args[0]);
            if(found == users.end())
            {
              std::cout << "switchto: invalid user '" << args[0] << "'\n";
            }
            else
            {
              auto user = users[args[0]];
              if(user == curUser)
              {
                std::cout << "switchto: already on user '" << args[0] << "'\n";
              }
              // if they have a password then validate it
              else if(user->HasPassword())
              {
                std::string password;
                std::cout << "Password: ";
                std::cin >> password;
                std::cin.clear(); 
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if(!user->VerifyPassword(password))
                {
                  std::cout << "Invalid Password!\n";
                  std::cout << "Authetication Failed\n";
                }
                else
                {
                  curUser = user;
                  commands("cd", std::vector<std::string>());
                }
                
              }
              else
              {
                curUser = user;
                commands("cd", std::vector<std::string>());
              }
            }
            
          }
          
        }

        else if(command == "load")
        {
          if(args.size() < 1)
          {
            std::cout << "load: missing argument" << std::endl;
          }
          else if(args.size() > 1)
          {
            std::cout << "load: too many arguments" << std::endl;
          }
          else
          {
            // hold results by string
            std::string buffer = "";
            // check if args[0] is a valid file
            std::ifstream file (args[0]);

            // check if the file is open
            if(!file.is_open())
            {
              std::cout << "load: error opening file" << std::endl;
            }
            else
            {
              int threadCount = 0;
              file >> threadCount;
              std::map<ull, ull> IDMapping;
              // load in the thread config
              for(int i = 0; i < threadCount; i++)
              {
                int type;
                ull id, mem;

                file >> id >> type >> mem;
                auto threadIt = threads.find(id);
                if(threadIt == threads.end())
                {
                  Thread* t = new Thread(mem, Thread::IntToType(type));
                  IDMapping.emplace(id, t->ID());
                  threads.emplace(t->ID(), t);
                }
                else
                  IDMapping.emplace(id, id);
                
              }
              
              int taskCount = 0;
              // add task
              file >> taskCount;
              for(int i = 0; i < taskCount; i++)
              {
                std::string taskName;
                ull taskTime, taskMem, threadID;
                file >> taskName >> taskTime >> taskMem >> threadID;
                auto threadIDIt = IDMapping.find(threadID);
                if(threadIDIt == IDMapping.end())
                  threadID = 0;
                else
                  threadID = threadIDIt->second;
                
                // add new task
                auto threadmother = threads.find(threadID);
                Task* t = new Task(taskName, taskTime, taskMem); 
                if(!threadmother->second->AddTask(t))
                  delete t;
                else
                  newTask(t->Name(), t->ID(), threadID, t->MemoryUsage(), t->TimeRemaining());
              }
              
            }
          }
          
        }
        // Handle help command
        else if(command == "thread")
        {
          if(args.size() < 1)
          {
            std::cout << " ID |           type         | num tasks | tasks waiting | tasks finished | running |" << std::endl;
            for(auto threadPair : threads)
            {
              std::string type = Thread::TypeName[threadPair.second->Type()];
              std::string spaces = "";
              int count = Thread::TypeName[2].size() - type.size();
              for(int i = 0; i < count; i++)
                spaces += " ";
              std::cout 
                << " " << threadPair.second->ID() << "    " << type << spaces << "     "
                << threadPair.second->NumTasks() << "           " << threadPair.second->NumWaitingTasks() 
                << "               " << threadPair.second->NumFinishedTasks() 
                << "              " 
                << (threadPair.second->GetRunningTask() == nullptr ? "no" : "yes") << std::endl; 
            }
          }
          else
          {
            if(args[0] == "list")
            {
              std::cout << " ID |           type         | num tasks | tasks waiting | tasks finished | running |" << std::endl;
              for(auto threadPair : threads)
              {
                std::string type = Thread::TypeName[threadPair.second->Type()];
                std::string spaces = "";
                int count = Thread::TypeName[2].size() - type.size();
                for(int i = 0; i < count; i++)
                  spaces += " ";
                std::cout 
                  << " " << threadPair.second->ID() << "    " << type << spaces << "     "
                  << threadPair.second->NumTasks() << "           " << threadPair.second->NumWaitingTasks() 
                  << "               " << threadPair.second->NumFinishedTasks() 
                  << "              " 
                  << (threadPair.second->GetRunningTask() == nullptr ? "no" : "yes") << std::endl; 
              }
            }
            else if(args.size() < 2)
            {
              std::cout << "thread: missing argument" << std::endl;
            }
            else
            {
              if(args[0] == "create" || args[0] == "c")
              {
              
                try
                {
                  Thread::ScheduleType type = static_cast<Thread::ScheduleType>(std::stoi(args[1]));
                  if(type < Thread::fifo || type > Thread::shortestprocess)
                    throw std::exception();
                  Thread* thread = new Thread(1000000, type);
                  threads.emplace(thread->ID(), thread);
                }
                catch(const std::exception)
                {
                  std::cout << "thread: invalid schedule type '" << args[1] << "'\n";
                }
                
              }
              else if(args[0] == "delete" || args[0] == "d")
              {
                try
                {
                  int id = std::stoi(args[1]);
                  
                  if(threads.find(id) == threads.end())
                    throw std::exception();
                  if(id == 0)
                  {
                    std::cout << "thread: cannot delete primary thread\n";
                  }
                  else
                  {
                    delete threads[id];
                    threads.erase(id);
                  }
                }
                catch(const std::exception)
                {
                  std::cout << "thread: invalid Thread ID '" << args[1] << "'\n";
                }
                
              }
              else
              {
                std::cout << "thread: invalid argument '" << args[0] << "'\n";
              }
              
            }
            
          }
          
        }
        else if(command == "help")
        {
          if(args.size() == 0)
          {
            std::cout << "Usage: help cmd : prints help for a given command\n";
            std::cout << "Usage: help -a : Prints help for all commands\n";
            std::cout << "Usage: <executable> [thread id] : will execute a file on a given thread id, default is thread 0 if none given\n";
          }
          else if(args[0] == "-a")
          {
            // print all commands help pages
            for(std::string arg : CMDS)
            {
              std::vector<std::string> args;
              args.push_back(arg);
              commands("help", args);
            }
          } 
          else if(args[0] == "ls")
          {
            std::cout << "Usage: ls : prints files in directory\n";
            std::cout << "Usage: ls -l : prints files in directory"
                      << " with extra information\n";
          }
          else if(args[0] == "pwd")
          {
            std::cout << "Usage: pwd : prints working file directory\n"; 
          }
          else if(args[0] == "exit")
          {
            std::cout << "Usage: exit : exits console\n";
          }
          else if(args[0] == "mkdir")
          {
            std::cout << "Usage: mkdir dir[ dir]... : makes the directories listed\n";
          }
          else if(args[0] == "touch")
          {
            std::cout << "Usage: touch file[ dir]... :will make a file if "
                      << "doesn't exists \n";
          }
          else if(args[0] == "cd")
          {
            std::cout << "Usage: cd dir : changes current working directory\n";
          }
          else if(args[0] == "rm")
          {
            std::cout << "Usage: rm file[ file]... : removes the files listed\n";
          }
          else if(args[0] == "rmdir")
          {
            std::cout << "Usage: rmdir dir[ dir]... : removes the directories "
                      << "listed\n";
          }
          else if(args[0] == "chmod")
          {
            std::cout << "Usage: chmod ### file/dir[ file/dir]... : changes permissions"
                      << " of files/directories listed\n";
          }
          else if(args[0] == "logout")
          {
            std::cout << "Usage: logout : logs out of current user\n";
          }
          else if(args[0] == "chown")
          {
            std::cout << "Usage: chown user[:group] file : changes the owner and/or group of a file\n";
          }
          else if(args[0] == "groupadd")
          {
            std::cout << "Usage: groupadd group : creates a group\n";
          }
          else if(args[0] == "usermod")
          {
            std::cout << "Usage: usermod -a -G group : a user to a group\n";
          }
          else if(args[0] == "userdel")
          {
            std::cout << "Usage: userdel user : removes a user\n";
            std::cout << "Usage: userdel -G group user : removes a user from the group";
          }
          else if(args[0] == "useradd")
          {
            std::cout << "Usage: useradd user : creates a new users\n";
            std::cout << "Usage: useradd -G group[,group] user : creates a user and adds them to the groups\n";
          }
          else if(args[0] == "groups")
          {
            std::cout << "Usage: groups user : displays all the groups a user is in\n";
          }
          else if(args[0] == "chgrp")
          {
            std::cout << "Usage: chgrp group file : changes the group of a file\n";
          }
          else if(args[0] == "whoami")
          {
            std::cout << "Usage: whoami : prints the name of the current user\n";
          }
          else if(args[0] == "switchto")
          {
            std::cout << "Usage: switchto user : attempts to switch users, users with passwords will be prompted\n";
          }
          else if(args[0] == "thread")
          {
            std::cout << "Usage: thread [list] : lists all current threads available to the computer\n";
            std::cout << "Usage: thread c[reate] <type id> : creates a thread with given scheduling type\n";
            std::cout << "Usage: thread d[elete] <thread id> : killed the thread with the given id\n";
          }
          else 
          {
            std::cout << "help: command '" << args[0] << "' doesn't exist\n";
          }
        }
        // else handle no command found
        else
        {
          // check to see if we are trying to run an executable
          Node* file = findFile(command);
          if(file == nullptr)
            std::cout << "Command '" << command << "' not found.\n";
          else if(file->IsDir())
            std::cout << file->Name() << " is a folder\n";
          else if(!Node::HasPermissions(curUser, file, Execute))
            std::cout << command << ": Permission Denied\n";
          else
          {
            if(args.size() < 1)
            {
              Task* t = new Task(file->GetTask());
              threads[0]->AddTask(t);
              newTask(t->Name(), t->ID(), 0, t->MemoryUsage(), t->TimeRemaining());
              std::cout << file->Name() << " executed\n";
            }
            else
            {
              try
              {
                ull id = stoi(args[0]);
                auto itemIt = threads.find(id);
                if(itemIt == threads.end())
                {
                  std::cout << command << ": invalid thread ID '" << args[0] << "'\n";
                }
                else
                {
                  Task* t = new Task(file->GetTask());
                  itemIt->second->AddTask(t);
                  newTask(t->Name(), t->ID(), id, t->MemoryUsage(), t->TimeRemaining());
                  std::cout << file->Name() << " executed\n";
                }
              }
              catch(const std::exception& e)
              {
                std::cout << command << ": invalid thread ID '" << args[0] << "'\n";
              }
            }
          }
        }
        return true;
      }

      // returns the current working directory
      std::string pwd()
      {
        std::string dir;
        // make a new tracker
        Node* traverse = curDir;
        // check to see if we are on the root.
        if(traverse == rootFile)
        {
          dir = "/";
        }
        // if we are not, work backwards
        while(traverse != rootFile)
        {
          if(traverse->IsDir())
            dir = "/" + traverse->Name() + dir; 

          traverse = traverse->Parent();
        }
        // return directory 
        return dir;
      }

      // Finds the file or not, takes a path and returns a pointer
      // pointer is null if it wasn't found
      Node* findFile(std::string path)
      {
        Node* next = curDir;
        // stream for parsing
        std::stringstream pathStream(path);
        bool succeed = true;
        // while there exists stuff to parse
        while(pathStream.peek() != std::char_traits<char>::eof())
        {
          std::string dir;
          // delimited on /
          getline(pathStream, dir, '/');
          // check if we are looking at the root of the directory
          if(dir == "" && pathStream.peek() == std::char_traits<char>::eof())
          {
            next = rootFile;
          }
          else
          {
            // if find .. then go to parent
            if(dir == "..")
            {
              next = next->parent;
            }
            // if . then stay
            else if(dir == ".")
            {
              // Nothing needed to do.
            }
            // else look for child.
            else
            {
              auto found = next->children.find(dir);
              succeed = found != next->children.end();
              // if looking and didn't find stop
              if(!succeed)
                break;
              else next = next->children[dir];
            }
          }
        }
        // return if we found it or no
        return succeed ? next : nullptr;
      }

      // adds another user with the given name and the group
      // returns the pointer to the new user nullptr if already exists
      User* AddUser(std::string name, std::string group)
      {
        // make sure there isn't a user currently
        if(users.find(name) != users.end())
          return nullptr;
        // add the new user
        users.emplace(name, new User(name, false, false, ""));
        // make their home directory
        rootFile->children["home"]->AddChild(root, new Node(name, true, rootFile->children["home"], 1, name, name));
        // add them to the group if it exists
        if(groups.find(group) != groups.end())
          users[name]->AddToGroup(group);
        // return new user pointer
        return users[name];
      }

      /**
       * Use this to update a task
       */
      void updateTask(int id, int memory, ull time)
      {
        process newProcess;
        newProcess.id = id;
        newProcess.threadId = -1;
        newProcess.memory = (time == 0) ? 0 : memory;
        // newProcess.cpu = cpu;
        newProcess.time = time;
        processesToUpdate.push(newProcess);
      }

      /**
       * use this to kill a task
       */
      void killTask(int id)
      {
        process newProcess;
        newProcess.id = id;
        newProcess.threadId = -1;
        newProcess.memory = 0;
        // newProcess.cpu = cpu;
        newProcess.time = 0;
        processesToUpdate.push(newProcess);
      }

      /**
       * Use this to add a task
       */
      void newTask(std::string name, int id, int threadId, int memory, ull time)
      {
        // stop, the process is done
        if (time == 0) {
          return;
        }

        process newProcess;
        newProcess.name = name;
        newProcess.id = id;
        newProcess.threadId = threadId;
        newProcess.memory = memory;
        // newProcess.cpu = cpu;
        newProcess.time = time;
        processesToUpdate.push(newProcess);

        return;
      }
  };
}
#endif

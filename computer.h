#include <vector>
#include <iostream>
#include "node.h"
#include <limits>
#include <sstream>
#include <map>
#include <string>

#ifndef COMPUTER_H
#define COMPUTER_H
namespace Shell
{
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
      std::set<std::string> groups;


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
        for(auto user : users)
          delete user.second;
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
        // No user to start off with, need to login.
        curUser = nullptr;
        // Create the root of the file system.
        rootFile = new Node("", true, nullptr, 0, "root", "root");
        // Set the root's parent to itself - makes it auto handle ../ on root. 
        // simple hack to make my life easier down the line.
        rootFile->parent = rootFile;
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
      }

      // Running the computer. Handles all operations from here.
      void run()
      {
        // login
        login();
        // Start the console.
        console();
      }
      
    // Private functions
    private:
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
          std::cout << curUser->Username() << "@" << computerName <<":" << dir
               << (curUser == root ? "#" : "$") << " ";
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
                if(Node::HasPermissions(curUser, child, Permission::Read))
                {
                  std::cout << child->PermsStr() << " " << child->NumDirs()
                            << " " << child->User() << " " << child->Group() << " "
                            << child->Size() << " " << child->TimeStr() << " "
                            // Adds blue color if it is a dir
                            << (child->isDir ? "\033[34m" : (Node::HasPermissions(curUser, child, Permission::Execute) ? "\033[32m" : ""))
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
              if(Node::HasPermissions(curUser, child.second, Permission::Read))
              {
                std::cout << (child.second->IsDir() ? "\033[34m" : (Node::HasPermissions(curUser, child.second, Permission::Execute) ? "\033[32m" : "")) 
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
            // iterate over arguements
            for(std::string arg : args)
            {
              // Attempt to add new directory if fails output such a message.
              if(Node::HasPermissions(curUser, curDir, Permission::Write))
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
          else if(!Node::HasPermissions(curUser, curDir, Permission::Write))
          {
            for(std::string arg : args)
            {
              std::cout << "touch: Cannot create '" << arg << "' Permission Denied!\n";
            }
          }
          else
          {
            // iterate over args
            for(std::string arg : args)
            {
              // try to add and if that fails, update the current timestamp
              if(!curDir->AddChild(curUser, new Node(arg, false, curDir, 1, curUser->Username(), curUser->Username())))
              {
                  curDir->children[arg]->UpdateTimeStamp();
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
            if(curDir == nullptr)
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
                if(Node::HasPermissions(curUser, file, Permission::Read))
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
              else if(!Node::HasPermissions(curUser, file,
               Permission::Write))
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
                  file->parent->DeleteChild(curUser, file);
                  delete file;
                }
                // else error
                else std::cout << "rm: Permission Denied\n";
              }
            }
          }
        }
        // Handle chmod command
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
                else if(file->User() != curUser->Username() && curUser != root)
                {
                  std::cout << "chmod: File '" << arg << "' Permission Denied\n";
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
        else if (command == "groupadd")
        {
          if(args.size() < 1)
            std::cout << "groupadd: Invalid number of arguments\n" << std::endl;
          for(std::string arg : args)
          {
            if(groups.find(arg) == groups.end())
              std::cout << "groupadd: group '" << arg << "' already exists\n";
            else
              groups.insert(arg);
          }
        }
        else if (command == "usermod")
        {
          if(args.size() < 3)
          {
            std::cout << "usermod: Invalid number of arguments\n";
          }
          else
          {
            if(args[0] != "-a")
              std::cout << "usermod: Invalid flag '" << args[0] << "'\n";
            else if(args[1] != "-G")
              std::cout << "usermod: Invalid flag '" << args[1] << "'\n";
            else
            {
              int count = 0;
              for(std::string arg : args)
              {
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
                  curUser->AddToGroup(arg);
                }
                
              }
            }
            
          }
          
        }
        else if (command == "useradd")
        {
          User* usr;
          if(args.size() < 1)
          {
            std::cout << "useradd: Invalid number of arguments\n"; 
          }
          else if(users.find(args[args.size() - 1]) != users.end())
          {
            std::cout << "useradd: User '" << args[args.size() - 1] << "' already exists\n";
          }
          else if(args.size() > 1)
          {
            if(args.size() != 3)
            {
              std::cout << "useradd: invalid number of arguments\n";
            }
            else if(args[0] != "-G")
            {
              std::cout << "useradd: invalid flag '" << args[0] << "'\n";
            }
            else 
            {
              usr = AddUser(args[2], "");
              std::string groupsToAdd = args[1];
              for(uint k = 0; k < groupsToAdd.size(); k++)
              {
                if(groupsToAdd[k] == ',')
                  groupsToAdd[k] = ' ';
              }
              std::stringstream groupsAdding(groupsToAdd);
              while(groupsAdding >> groupsToAdd)
              {
                if(groups.find(groupsToAdd) == groups.end())
                {
                  std::cout << "useradd: invalid group '" << groupsToAdd << "'\n";
                }
                else
                {
                  usr->AddToGroup(groupsToAdd);
                }
              }
            }
          }
          else
          {
            AddUser(args[0], "");
          }
          
        }
        else if (command == "userdel")
        {
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
            User* usr = users[args[0]];
            users.erase(usr->Username());
            delete usr;
          }
        }
        else if (command == "groups")
        {
          if(args.size() != 1)
          {
            std::cout << "groups: Invalid number of arguments\n";
          }
          else if(users.find(args[0]) == users.end())
          {
            std::cout << "groups: User '" << args[0] << "' doesn't exist\n";
          }
          else
          {
            std::cout << args[0] << ": ";
            for(std::string group : users[args[0]]->Groups())
            {
              std::cout << group << " ";
            }
            std::cout << std::endl;
          }
          
        }
        else if (command == "chown")
        {
          if(args.size() != 2)
          {
            std::cout << "chown: Invalid number of arguments\n";
          }
          else
          {
            Node* file = findFile(args[1]);
            if(file == nullptr)
            {
              std::cout << "chown: Error File '" << args[1] << "' doesn't exist\n";
            }
            else
            {
              int pos = args[0].find(':');
              if(pos >= 0)
              {
                std::string owner = args[0].substr(0, pos + 1);
                std::string group = args[0].substr(pos);
                if(users.find(owner) == users.end())
                {
                  std::cout << "chown: invalid user '" << args[0] << "'\n";
                }
                else if(groups.find(group) == groups.end())
                {
                  std::cout << "chown: invalid group '" << args[0] << "'\n";
                }
                file->group = group;
                file->user = owner; 
              }
              else
              {
                file->user = args[0];
              }
            }
          }
        }
        else if (command == "chgrp")
        {
          if(args.size() != 2)
          {
            std::cout << "chgrp: Invalid amount of arguments\n";
          }
          else
          {
            Node* file = findFile(args[1]);
            if(file == nullptr)
              std::cout << "chgrp: error file '" << args[1] << "' doesn't exist\n";
            if(groups.find(args[0]) == groups.end())
              std::cout << "chgrp: invalid group '" << args[0] << "'\n";
            else
              file->group = args[0];
          }
        }
        else if (command == "whoami")
        {
          if(args.size() > 0)
          {
            std::cout << "whoami: extra operand '" << args[0] << "'\n";
          }
          else
          {
            std::cout << curUser->Username() << std::endl;
          }
        }
        else if (command == "switchto")
        {
          if(args.size() != 1)
          {
            std::cout << "switchto: Invalid number of arguments\n";
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
        // Handle help command
        else if(command == "help")
        {
          if(args.size() == 0)
          {
            std::cout << "Usage: help cmd : prints help for a given command\n";
            std::cout << "Usage: help -a : Prints help for all commands\n";
          }
          else if(args[0] == "-a")
          {
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
            std::cout << "Usage: switchto user : attempts to switch users, users with passwords will be prompted";
          }
          else 
          {
            std::cout << "help: command '" << args[0] << "' doesn't exist\n";
          }
        }
        // else handle no command found
        else
        {
          Node* file = findFile(command);
          if(file == nullptr)
            std::cout << "Command '" << command << "' not found.\n";
          else if(file->IsDir())
            std::cout << file->Name() << " is a folder\n";
          else if(!Node::HasPermissions(curUser, file, Permission::Execute))
            std::cout << command << ": Permission Denied\n";
          else
            std::cout << file->Name() << " executed\n";
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

      User* AddUser(std::string name, std::string group)
      {
        if(users.find(name) != users.end())
          return nullptr;
        users.emplace(name, new User(name, false, false, ""));
        rootFile->children["home"]->AddChild(root, new Node(name, true, rootFile->children["home"], 1, name, name));
        if(groups.find(group) != groups.end())
          users[name]->AddToGroup(group);
        return users[name];
      }  
  };
}
#endif

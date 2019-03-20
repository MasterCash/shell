/*
 * Name: Cash
 * Class: Operating Systems
 * Assignment: Shell Emulator
 */

#include <map>
#include <string>
#include <ctime>
#include <array>
#include <chrono>
#include <algorithm>
#include "user.h"

#ifndef NODE_H
#define NODE_H
namespace Shell
{
  // share with computer
  class Computer;
  // constants because I am lazy 
  const std::array<int, 3> DEFAULT_PERM = {6,6,4};
  const std::array<std::string, 12> MONTHS = 
  {
    "Jan", "Feb", "Mar", "Apr", "May", 
    "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };

  enum Permission { Read, Write, Execute };

  // Node class representing a file
  // similar stucture to a doubly linked list
  // NOTE: this contains some functions and parameters that were
  // to be a additional functionality incase we continue
  // using this assignment.
  class Node
  {
    private:
      // a link to it's parent
      Node* parent;
      // list of children
      std::map<std::string, Node*> children;
      // the name of file
      std::string name;
      // if file is a directory
      bool isDir;
      // user owner
      std::string user; 
      // user group
      std::string group;
      // size of file
      int size;
      // time stamp
      tm timeStamp;
      // permissions
      std::array<int, 3> perms;
      // friends with a computer
      friend Computer;
    public:
      // Constructors 
      Node(std::string n, bool dir, Node* p, int s, std::string u, std::string g)
      {
        auto time = std::chrono::system_clock::now(); 
        std::time_t timet = std::chrono::system_clock::to_time_t(time);
        timeStamp = *std::localtime(&timet);
        name = n;
        parent = p;
        children = std::map<std::string, Node*>();
        isDir = dir;
        user = u;
        group = g;
        size = s;
        perms = DEFAULT_PERM;
      }
      Node(std::string n, bool dir, Node* p) : Node(n, dir, p, 1, p->user, p->group) { }
      // deconstructor
      ~Node()
      {
        parent = nullptr;
        for(auto node : children)
        {
          delete node.second;
        }
      }
      // updates the timestamp of the node
      void UpdateTimeStamp()
      {
        auto time = std::chrono::system_clock::now(); 
        std::time_t timet= std::chrono::system_clock::to_time_t(time);
        timeStamp = *std::localtime(&timet); 
      }

      // Getters
      // not used really oh well
      Node* Parent() const { return parent; }
      std::map<std::string, Node*> Children() const { return children; }
      std::string Name() const { return name; }
      bool IsDir() const { return isDir; }
      std::string User() const { return user; }
      std::string Group() const { return group; }
      int Size() const { return size; }
      tm TimeStamp() const { return timeStamp; }
      std::array<int, 3> Perms() const { return perms; }

      // gets the string of the permission 
      std::string PermsStr() const 
      {
        std::string str = "";

        str += isDir ? "d" : "-";
        for(auto perm : perms)
        {
          str += perm / 4 > 0 ? "r" : "-";
          str += perm % 4 / 2 > 0 ? "w" : "-";
          str += perm % 4 % 2 == 1 ? "x" : "-";
        }
        return str;
      }
      // convert time object to time
      std::string TimeStr() const
      {
        return MONTHS[timeStamp.tm_mon] + "  " + 
        std::to_string(timeStamp.tm_mday) + " " + 
        std::to_string(timeStamp.tm_hour) + ":" + 
        std::to_string(timeStamp.tm_min);
      }
      // counts the number of directories
      int NumDirs() const
      {
        int count = 0;
        if(!isDir) return 1;
        for(auto child : children)
        {
          if(child.second->isDir)
            count++;
        }
        // adds two for some reason ask linux why.
        return count + 2; 
      }
    
      // Add Child
      bool AddChild(const Shell::User* user, Node* child)
      {
        bool succeed = false;
        child->parent = this;
        if(isDir)
        {
          succeed = children.emplace(child->name, child).second;
        }
        
        if(!succeed)
          delete child;
        return succeed;
      }
    // private funcs
    private:
      // deletes a child returns true if succeeded
      bool DeleteChild(const Shell::User* user, Node* child)
      {
        bool succeed = false;
        auto found = 
          children.find(child->name);
        if(HasPermissions(user, child, Write))
        {
          if(found != children.end() && found->second == child)
          {
            if(child->isDir)
            {
              succeed = child->DeleteChildren(user);
              if(succeed)
                children.erase(found);
            }
            else
            {
              children.erase(found);
              succeed = true;
            }
          }
        }
        else
        {
          std::cout << "Cannot delete this file: " << child->name << " Permission Denied!" << std::endl;
        }
        
        return succeed;
      }

      // Deletes all children of a file.
      bool DeleteChildren(const Shell::User* user)
      {
        bool succeed = true;
        if(HasPermissions(user, this, Write))
        {
          for(auto node : children)
          {
            if(HasPermissions(user, node.second, Write))
            {
              bool worked = true;
              if(node.second->isDir)
                worked = node.second->DeleteChildren(user);
              if(worked)
                delete node.second;
              else
                succeed = false;
            }
            else
            {
              std::cout << "Cannot remove this file: " << node.second->name << " Permission Denied!" << std::endl;
              succeed = false;
            }
          }
          if(succeed)
            children.empty();
        }
        else
        {
          std::cout << "Cannot remove this file: " << this->name << " Permission Denied!" << std::endl;
          succeed = false;
        } 
        return succeed;
      }

      // moves files not actually implemented
      bool MoveTo(const Shell::User* user, Node* p)
      {
        bool succeed;
        
        succeed = parent->DeleteChild(user, this);

        if(succeed)
        {
          parent = p;
          succeed = parent->AddChild(user, this);
        }

        return succeed;
      }
      // public functions
      public:
        // compares two nodes.
        static bool Compare(const Node * const a, const Node * const b)
        {
          return a->name < b->name;
        }
        static int GetPermissions(const Shell::User* user, const Node* node)
        {
          int perms = node->perms[2];
          if(user->IsInGroup(node->group))
            perms = perms | node->perms[1];
          if(user->Username() == node->User())
            perms = perms | node->perms[0];
          return perms;
        }
        static bool HasPermissions(const Shell::User* user, const Node* node, Permission perm)
        {
          int perms = GetPermissions(user, node);
          bool hasPerm;
          switch(perm)
          {
            case Permission::Read:
            {
              hasPerm = perms / 4 > 0;
              break;
            }
            case Permission::Write:
            {
              hasPerm = perms % 4 / 2 > 0;
              break;
            }
            case Permission::Execute:
            {
              hasPerm = perms % 4 % 2 == 1;
              break;
            }
            default:
            {
              hasPerm = false;
              break;
            }
          }
          return user->Username() == "root" ? true : hasPerm;
        }
  };

}
#endif

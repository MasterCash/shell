
#include <string>
#include <set>


#ifndef USER_H
#define USER_H

namespace Shell
{
  // basic user class built for future use. 
  // Mostly use to store a name and password atm
  class User
  {
    private:
      std::string uname;
      std::set<std::string> groups;
      bool isAdmin;
      bool hasPassword;
      std::string pword;
    public:
      // Constructor
      User(std::string name, bool admin, bool hasPass, std::string pass) 
      {
        uname = name;
        hasPassword = hasPass;
        groups.insert(name);
        groups.insert("users");
        isAdmin = admin;
        pword = pass;
      }
      // getters
      std::string Username() const { return uname; }
      bool IsInGroup(std::string group) const { return groups.find(group) != groups.end(); }
      void AddToGroup(std::string group) { groups.insert(group); }
      void RemoveFromGroup(std::string group) { groups.erase(group); } 
      bool HasPassword() const { return hasPassword; }
      bool IsAdmin() const { return isAdmin; }
      std::set<std::string> Groups() const { return groups; }
      // verify password bc not gonna expose password
      bool VerifyPassword(std::string pass) const { return pword == pass || !hasPassword; }
  
  };
} // 

#endif
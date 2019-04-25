#ifndef THREAD_H
#define THREAD_H
#include <map>
#include <list>
#include "task.h"
#ifndef ull
#define ull unsigned long long
#endif

class Thread
{
  private:
    static ull ID_COUNT;
    std::map<ull, Task*> tasks;
    std::map<ull, Task*> history;
    ull memory;
    ull freeMemory;
    ull id;

  public:
    Thread(ull mem) : memory(mem), freeMemory(mem), id(ID_COUNT++) { }
    ~Thread()
    {
      for(auto taskPair : tasks)
        delete taskPair.second;

      for(auto taskPair : history)
        delete taskPair.second;

      tasks.empty();
      history.empty();
    }
    
    ull TotalMemory() const { return memory; }
    ull FreeMemory() const { return freeMemory; }
    bool HasRunningTask(ull id) const { return tasks.find(id) == tasks.end(); }
    bool HasCompletedTask(ull id) const { return history.find(id) == history.end(); }

    bool AddTask(Task* task) 
    { 
      bool added = false;
      if(task->MemoryUsage() <= freeMemory)
      {
        added = tasks.emplace(task->ID(), task).second;
        if(added)
          freeMemory -= task->MemoryUsage();
      }
      return added; 
    }

    bool KillTask(ull id)
    {
      bool killed = false;
      auto taskIt = tasks.find(id);
      if(taskIt != tasks.end())
      {
        killed = true;
        taskIt->second->Kill();
        history.emplace((*taskIt));
        tasks.erase(taskIt->first);
      }
      return killed;
    }

    std::list<ull> Update(ull time)
    {
      std::list<ull> completedIDs;
      for(auto taskPair : tasks)
      {
        taskPair.second->Run(time);
        if(taskPair.second->Status == Task::DONE)
        {
          completedIDs.push_back(taskPair.first);
          history.emplace(taskPair);
          freeMemory += taskPair.second->MemoryUsage();
        }
      }

      for(ull id : completedIDs)
        tasks.erase(id);
      return completedIDs;
    }
};

unsigned long long Thread::ID_COUNT = 0;
#endif
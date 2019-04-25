#ifndef THREAD_H
#define THREAD_H
#include <map>
#include <list>
#ifndef ull
#define ull unsigned long long
#endif
#include "task.h"

/**
 * @brief class to represent a thread for the shell program 
 *
 */
class Thread
{
  private:
    /**
     * Counter to make sure each Thread has a unique Identifier
     */
    static ull ID_COUNT;
    /**
     * dictionary of running tasks
     */
    std::map<ull, Task*> tasks;
    /**
     * dictionary of finished tasks
     */
    std::map<ull, Task*> history;
    /**
     * total memory this thread has access to
     */
    ull memory;
    /**
     * current free amount of memory
     */
    ull freeMemory;
    /**
     * id of this thread
     */
    ull id;

  public:
    /**
     * Initialize a Thread 
     * @param  mem  memory this thread starts with
     */
    Thread(ull mem) : memory(mem), freeMemory(mem), id(ID_COUNT++) { }
    /**
     * Deconstructs the Thread
     * handles its dictionary of pointers
     */
    ~Thread()
    {
      for(auto taskPair : tasks)
        delete taskPair.second;

      for(auto taskPair : history)
        delete taskPair.second;

      tasks.empty();
      history.empty();
    }
    /** returns the total Memory of the thread */ 
    ull TotalMemory() const { return memory; }
    /** returns the free Memory of the thread */
    ull FreeMemory() const { return freeMemory; }
    /** returns if the thread has a running task with the given id */
    bool HasRunningTask(ull id) const { return tasks.find(id) == tasks.end(); }
    /** returns if the thread has a finished task with the given id */
    bool HasCompletedTask(ull id) const { return history.find(id) == history.end(); }
    /** 
     * Gets a running task from the thread
     * @param  iD  id of the task you want
     * @return returns the task if there is one
     */
    const Task* const GetRunningTask(ull iD) const
    {
      Task* task;
      auto taskIt = tasks.find(iD);
      if(taskIt != tasks.end())
        task = taskIt->second;
      else 
        task = nullptr;

      return task;
    }
    /** 
     * Gets a finished task from the thread
     * @param  iD  id of the task you want
     * @return returns the task if there is one
     */
    const Task* const GetFinishedTask(ull iD) const
    {
      Task* task;
      auto taskIt = tasks.find(iD);
      if(taskIt != tasks.end())
        task = taskIt->second;
      else 
        task = nullptr;
      return task;
    }
    /**
     * Adds a task to the thread to start running
     * @param  task  A pointer to a task to be added
     * @return  true if added to thread running task false otherwise
     * 
     * if added caller cleanup unnecessary else caller in charge of 
     * handing pointer if not added.
     */
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

    /**
     * Kill a task if not already finished
     * @param  _id  id of task to kill
     * @return returns if it killed task
     */
    bool KillTask(ull _id)
    {
      bool killed = false;
      auto taskIt = tasks.find(_id);
      if(taskIt != tasks.end())
      {
        killed = true;
        taskIt->second->Kill();
        history.emplace((*taskIt));
        tasks.erase(taskIt->first);
      }
      return killed;
    }

    /**
     * Update time since last call
     * @param  time  time since last called
     * @return list of finished tasked
     * 
     * the update funtion of a thread, Should be called by 
     * the "Computer" with time since last call.
     */
    std::list<ull> Update(ull time)
    {
      std::list<ull> completedIDs;
      for(auto taskPair : tasks)
      {
        taskPair.second->Run(time);
        if(taskPair.second->Status() == Task::DONE)
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
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
  public:
    enum ScheduleType {fifo, roundrobin2, shortestprocess};
  private:
    ScheduleType type;
    /**
     * Counter to make sure each Thread has a unique Identifier
     */
    static ull ID_COUNT;
    /**
     * dictionary of waiting tasks
     */
    std::map<ull, Task*> tasks;
    /**
     * list of IDS in order of arrival waiting to be processed.
     */
    std::list<ull> waitingIDs;
    /**
     * current task running
     */
    Task* runningTask;
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
    Thread(ull mem, ScheduleType t) : type(t), memory(mem), freeMemory(mem), id(ID_COUNT++) { runningTask = nullptr; }
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

      if(runningTask != nullptr)
        delete runningTask;

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
     * Gets a waiting task from the thread
     * @param  iD  id of the task you want
     * @return  returns the task if there is one
     */
    const Task* const GetWaitingTask(ull iD) const
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
     * Gets the current running task from the thread
     * @return  returns the task if there is one that is running.
     */
    const Task* const GetRunningTask() const
    {
      return runningTask;
    }
    /** 
     * Gets a finished task from the thread
     * @param  iD  id of the task you want
     * @return  returns the task if there is one
     */
    const Task* const GetFinishedTask(ull iD) const
    {
      Task* task;
      auto taskIt = history.find(iD);
      if(taskIt != history.end())
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
        {
          freeMemory -= task->MemoryUsage();
          waitingIDs.push_back(task->ID());
        }
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
      if(_id == runningTask->ID())
      {
        killed = true;
        runningTask->Kill();
        history.emplace(_id, runningTask);
        runningTask = nullptr;
      }
      else if(taskIt != tasks.end())
      {
        killed = true;
        taskIt->second->Kill();
        history.emplace((*taskIt));
        tasks.erase(taskIt->first);
        waitingIDs.remove(_id);
      }
      return killed;
    }

    /**
     * Update time since last call
     * @param  time  time since last called
     * @return list of finished tasked
     * 
     * the update function of a thread, Should be called by 
     * the "Computer" with time since last call.
     */
    std::list<ull> Update(ull time)
    {
      std::list<ull> completedIDs;

      if(runningTask == nullptr)
      {
        if(type == fifo || type == roundrobin2)
        {
          runningTask = tasks[waitingIDs.front()];
          waitingIDs.pop_front();
          tasks.erase(runningTask->ID());
        }
        else if(type == shortestprocess)
        {
          ull _id = waitingIDs.front();
          for (auto id : waitingIDs)
          {
            if(tasks[_id]->ProcessTime() > tasks[id]->ProcessTime())
              _id = id;
          }
          runningTask = tasks[_id];
          tasks.erase(_id);
          waitingIDs.remove(_id);
        }
      }
      switch (type)
      {
      case fifo:
        while(time > 0)
        {
          ull left = runningTask->TimeRemaining();
          runningTask->Run(time);
          time -= left;
          if(runningTask->Status() == Task::done)
          {
            completedIDs.push_back(runningTask->ID());
            history.emplace(runningTask->ID(), runningTask);
            runningTask = tasks[waitingIDs.front()];
            tasks.erase(waitingIDs.front());
            waitingIDs.pop_front();
          }
        }
        break;
      case roundrobin2: 
        while (time > 0)
        {
          runningTask->Run(2);
          if(runningTask->Status() == Task::done)
          {
            completedIDs.push_back(runningTask->ID());
            history.emplace(runningTask->ID(), runningTask);
            runningTask = tasks[waitingIDs.front()];
            tasks.erase(waitingIDs.front());
            waitingIDs.pop_front();
          }
          else
          {
            tasks.emplace(runningTask->ID(), runningTask);
            waitingIDs.push_back(runningTask->ID());
            runningTask = tasks[waitingIDs.front()];
            tasks.erase(waitingIDs.front());
            waitingIDs.pop_front();
          }
          time -= 2;
        }
        break;
      case shortestprocess: 
        while(time > 0)
        {
          ull _id;
          ull left = runningTask->TimeRemaining();
          runningTask->Run(time);
          time -= left;
          if(runningTask->Status() == Task::done)
          {
            completedIDs.push_back(runningTask->ID());
            history.emplace(runningTask->ID(), runningTask);

            _id = waitingIDs.front();
            for (auto id : waitingIDs)
            {
              if(tasks[_id]->ProcessTime() > tasks[id]->ProcessTime())
                _id = id;
            }
            runningTask = tasks[_id];
            tasks.erase(_id);
            waitingIDs.remove(_id);
          }
        }
        break;
      }
      return completedIDs;
    }
};

unsigned long long Thread::ID_COUNT = 0;
#endif
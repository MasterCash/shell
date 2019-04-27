#ifndef TASK_H
#define TASK_H
#include <list>
#ifndef ull
#define ull unsigned long long
#endif


class Task
{
  public:
    enum Status {done, working, error};

  private:
    static unsigned long long ID_COUNTER;
    unsigned long long id;
    unsigned long long processTime;
    unsigned long long timeRemaining;
    unsigned int memoryUsage;
    Status status;

  public:
    unsigned long long ID() const { return id; }
    unsigned long long ProcessTime() const { return processTime; }
    unsigned long long TimeRemaining() const { return timeRemaining; }
    unsigned int MemoryUsage() const { return memoryUsage; }
    Status Status() const { return status; }

    void Kill() { status = error; }

    void Run(const unsigned long long time)
    {
      if(status == working)
      {
        timeRemaining -= time;
        if(timeRemaining <= 0)
        {
          timeRemaining = 0;
          status = done;
        }
      }
    }

    Task(unsigned long long pTime, unsigned int memUsage)
    {
      id = ID_COUNTER++;
      processTime = pTime;
      memoryUsage = memUsage;
      timeRemaining = memoryUsage;
      status = working;
    }
};

unsigned long long Task::ID_COUNTER = 0;
#endif
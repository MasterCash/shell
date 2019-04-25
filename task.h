#ifndef TASK_H
#define TASK_H
#include <list>
#ifndef ull
#define ull unsigned long long
#endif


class Task
{
  public:
    enum STATUS {DONE, WORKING, ERROR};

  private:
    static unsigned long long ID_COUNTER;
    unsigned long long id;
    unsigned long long processTime;
    unsigned long long timeRemaining;
    unsigned int memoryUsage;
    STATUS status;

  public:
    unsigned long long ID() const { return id; }
    unsigned long long ProcessTime() const { return processTime; }
    unsigned long long TimeRemaining() const { return timeRemaining; }
    unsigned int MemoryUsage() const { return memoryUsage; }
    STATUS Status() const { return status; }

    void Kill() { status = ERROR; }

    void Run(const unsigned long long time)
    {
      if(status == WORKING)
      {
        timeRemaining -= time;
        if(timeRemaining <= 0)
        {
          timeRemaining = 0;
          status = DONE;
        }
      }
    }

    Task(unsigned long long pTime, unsigned int memUsage)
    {
      id = ID_COUNTER++;
      processTime = pTime;
      memoryUsage = memUsage;
      timeRemaining = memoryUsage;
      status = WORKING;
    }
};

unsigned long long Task::ID_COUNTER = 0;
#endif
#ifndef TASK_H
#define TASK_H
#include <list>
#ifndef ull
#define ull unsigned long long
#endif


/**
 * Representation of a Job/Process/Task on a Thread 
 */
class Task
{
  public:
    /**
     * Status enum: represents the current status of the task
     */
    enum Status {done, running, waiting, error};

  private:
    /**
     * running counter of ID
     */
    static ull ID_COUNTER;
    /**
     * Unqiue ID of this process.
     */
    ull id;
    /**
     * The time it takes for this process to run.
     */
    ull processTime;
    /** 
     * Time remaining for a process.
     */
    ull timeRemaining;
    /**
     * Memory this process will need to run.
     */
    ull memoryUsage;
    /**
     * Status of this Process.
     */
    Status status;

  public:
    /**
     * Gets the ID of this process
     * @return  the ID of this Process
     */
    ull ID() const { return id; }
    /**
     * gets the process time needed for this process
     * @return  The total process Time needed.
     */
    ull ProcessTime() const { return processTime; }
    /**
     * Gets the time remaining till finished processing.
     * @return time left till completion 
     */
    ull TimeRemaining() const { return timeRemaining; }
    /**
     * Get the amount of memory this process needs.
     * @return  Amount of memory needed.
     */
    ull MemoryUsage() const { return memoryUsage; }
    /**
     * Get the current status of this process.
     * @return  The current status.
     */
    Status Status() const { return status; }

    /**
     * @brief  Blocks the current running process
     * 
     * Sets the status of this process to waiting.
     */
    void Block() {status = waiting; }
    /**
     * @brief  Kills the current running process
     * if process is not done, sets status to error 
     * indicating did not finish. 
     */
    void Kill() { status = status == done ? done : error; }
    /**
     * @brief  Runs a task for specified time
     * @param  time  The time to run the process for
     * if process is waiting changes status to running and runs 
     * otherwise if running runs for specified amount of time.
     * if process finished status is updated to done.
     */
    void Run(const ull time)
    {
      if(status == waiting)
        status = running;
      if(status == running)
      {
        timeRemaining -= time;
        if(timeRemaining <= 0)
        {
          timeRemaining = 0;
          status = done;
        }
      }
    }
    /**
     * @brief initializes a Task
     * @param  pTime  Time it takes to run process
     * @param  memUsage  Memory needed to run this process.
     * 
     * each process will have its own unique id set up on creation
     * and is intialized in waiting status.
     */
    Task(unsigned long long pTime, unsigned int memUsage)
    {
      id = ID_COUNTER++;
      processTime = pTime;
      memoryUsage = memUsage;
      timeRemaining = memoryUsage;
      status = waiting;
    }
};

/**
 * initialization of static counter
 */
unsigned long long Task::ID_COUNTER = 0;
#endif
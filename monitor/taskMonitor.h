#include <vector>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#ifndef ull
#define ull unsigned long long
#endif
#include <thread>
#include <pthread.h>
#include <cmath>
#include <fstream>
#include <bits/stdc++.h>

#ifndef TASKMONITOR_H
#define TASKMONITOR_H

namespace Display
{

  // create a struct to store 
  struct process {
    std::string name;
    int id;
    unsigned int threadId;
    unsigned int memory;
    // int cpu;
    ull time;
  };

  // Computer class 
  // Represents the OS who controls the file System.
  class TaskMonitor
  {
    // Private vars
    private:
      // stores the running process that are being displated.
      std::vector<process> running;
      // tracks weither or not the process is printing
      bool printing;
      // track the machine info.
      unsigned int memory;
      // int cpu;
      // stores the size of the total display.
      unsigned int screenSize;
      unsigned int screenHight;
      // stores the size of the sub-parts of the display.
      unsigned int memorySize;
      // int cpuSize;
      unsigned int idSize;
      unsigned int threadIdSize;
      unsigned int nameSize;
      // int cpuUSize;
      unsigned int memoryUSize;
      unsigned int timeSize;
      // stores the minimum size each part can be.
      const unsigned int MINSIZE = 50;
      const unsigned int MINHIGHT = 3;
      const unsigned int MINMEMORYSIZE = 6;
      // const int MINCPUSIZE = 5;
      const unsigned int MINIDSIZE = 3;
      const unsigned int MINTHREADIDSIZE = 3;
      const unsigned int MINNAMESIZE = 12;
      // const int MINCPUUSIZE = 5;
      const unsigned int MINMEMORYUSIZE = 6;
      const unsigned int MINTIMESIZE = 9;

    // Public functions
    public:
      // Main constructor
      TaskMonitor()
      {
        // set all the default values
        printing = false;
        memory = 0;
        // cpu = 0;
        screenSize = MINSIZE;
        // higher base hight for simplicity sake.
        screenHight = MINHIGHT + 20;
        memorySize = MINMEMORYSIZE;
        // cpuSize = MINCPUSIZE;
        idSize = MINIDSIZE;
        threadIdSize = MINTHREADIDSIZE;
        nameSize = MINNAMESIZE;
        // cpuUSize = MINCPUUSIZE;
        memoryUSize = MINMEMORYUSIZE;
        timeSize = MINTIMESIZE;
      }

      // argumented constructor
      TaskMonitor(unsigned int mem, unsigned int size, unsigned int hight)
      {
        // set the memory and cpu
        memory = mem;
        // cpu = cp;
        // validate the size and hight. Set them with the validated results.
        if (size < MINSIZE)
        {
          std::cout << "WARNING! Minimum size is " << MINSIZE << ". Setting to minimum" << std::endl;
          size = MINSIZE;
        }
        screenSize = size;
        if (hight < MINHIGHT)
        {
          std::cout << "WARNING! Minimum hight is " << MINHIGHT << ". Setting to minimum" << std::endl;
          hight = MINHIGHT;
        }
        // set up the size of the monitor.
        screenSize = size;
        screenHight = hight;
        // set up the size of the parts.
        memorySize = MINMEMORYSIZE;
        // cpuSize = MINCPUSIZE;
        idSize = MINIDSIZE;
        threadIdSize = MINTHREADIDSIZE;
        nameSize = MINNAMESIZE;
        // cpuUSize = MINCPUUSIZE;
        memoryUSize = MINMEMORYUSIZE;
        timeSize = MINTIMESIZE;
        // increase the size of the parts as much as possible, and evenly.
        for (unsigned int i = 0; i < screenSize - MINSIZE; i++)
        {
          if (i % 7 == 0)
          {
            idSize++;
          } else if (i % 7 == 1)
          {
            threadIdSize++;
          } else if (i % 7 == 2)
          {
            nameSize++;
          } else if (i % 7 == 3)
          {
            nameSize++;
          } else if (i % 7 == 4)
          {
            memoryUSize++;
          } else if (i % 7 == 5)
          {
            timeSize++;
          } else if (i % 7 == 6)
          {
            memorySize++;
          }
        }
      }

      // prints out the task manager.
      void print()
      {
        printing = true;
        // count the used cpu and memory.
        // int usedCPU = 0;
        unsigned int usedMem = 0;
        for (auto it = std::begin(running); it!=std::end(running); ++it)
        {
          usedMem += (*it).memory;
          // usedCPU += (*it).cpu;
        }
        // helper int for printing buffering.
        unsigned int remaining;
        // helper string for printing.
        std::string conv;
        // store the entire print out.
        std::stringstream out;
        // clear the screen.
        out << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
        //out << "00000000011111111112222222222333333333344444444445555555555666666666677777777778\n";
        //out << "12345678901234567890123456789012345678901234567890123456789012345678901234567890\n";
        // make a empty process for dealing with empty spots.
        process empty;
        empty.id = -1;
        // empty.cpu = 0;
        empty.memory = 0;
        empty.name = "";
        empty.threadId = 0;
        empty.time = 0;
        // iterate over the screen hight.
        for (unsigned int i = 0; i < screenHight; i++)
        {
          // print out starting | with a new line.
          out << "\n|";
          // print out header.
          if (i == 0)
          {
            // print out CPU header.
            /*
            remaining = cpuSize - MINCPUSIZE;
            for (int w = 0; w < (remaining / 2) + (remaining % 2); w++)
            {
              out << " ";
            }
            out << " CPU ";
            remaining = remaining / 2;
            for (int w = 0; w < remaining; w++)
            {
              out << " ";
            }
            // print divider
            out << "|";*/
            // print out memory header
            remaining = memorySize - MINMEMORYSIZE;
            for (unsigned int w = 0; w < (remaining / 2) + (remaining % 2); w++)
            {
              out << " ";
            }
            out << "MEMORY";
            remaining = (memorySize - MINMEMORYSIZE) / 2;
            for (unsigned int w = 0; w < remaining; w++)
            {
              out << " ";
            }
            // print section divider
            out << "|###|";
            // print out the id of the process.
            remaining = idSize - MINIDSIZE;
            for (unsigned int w = 0; w < (remaining / 2); w++)
            {
              out << " ";
            }
            out << " id";
            remaining -= remaining / 2;
            for (unsigned int w = 0; w < remaining; w++)
            {
              out << " ";
            }
            // print out divider
            out << "|";
            // print out the thread id
            remaining = threadIdSize - MINTHREADIDSIZE;
            for (unsigned int w = 0; w < (remaining / 2) + (remaining % 2); w++)
            {
              out << " ";
            }
            out << "Tid";
            remaining = remaining / 2;
            for (unsigned int w = 0; w < remaining; w++)
            {
              out << " ";
            }
            // print out divider
            out << "|";
            // print out name header.
            remaining = nameSize - MINNAMESIZE;
            for (unsigned int w = 0; w < (remaining / 2) + (remaining % 2); w++)
            {
              out << " ";
            }
            out << "    name    ";
            remaining = (nameSize - MINNAMESIZE) / 2;
            for (unsigned int w = 0; w < remaining; w++)
            {
              out << " ";
            }
            // print out divider
            out << "|";
            // print out cpu usage header.
            /*remaining = cpuUSize - MINCPUUSIZE;
            for (int w = 0; w < (remaining / 2) + (remaining % 2); w++)
            {
              out << " ";
            }
            out << " CPU ";
            remaining = (cpuUSize - MINCPUUSIZE) / 2;
            for (int w = 0; w < remaining; w++)
            {
              out << " ";
            }
            // print out divider
            out << "|";*/
            // print out memory header
            remaining = memoryUSize - MINMEMORYUSIZE;
            for (unsigned int w = 0; w < (remaining / 2) + (remaining % 2); w++)
            {
              out << " ";
            }
            out << "MEMORY";
            remaining = (memoryUSize - MINMEMORYUSIZE) / 2;
            for (unsigned int w = 0; w < remaining; w++)
            {
              out << " ";
            }
            // print out divider
            out << "|";
            // print out time left header
            remaining = timeSize - MINTIMESIZE;
            for (unsigned int w = 0; w < (remaining / 2) + (remaining % 2); w++)
            {
              out << " ";
            }
            out << "time left";
            remaining = (timeSize - MINTIMESIZE) / 2;
            for (unsigned int w = 0; w < remaining; w++)
            {
              out << " ";
            }
          }
          else
          {
            // decides the process to display, aka a empty if there is none.
            process display;
            if (i <= running.size())
            {
              display = running[i - 1];
            }
            else
            {
              display = empty;
            }/*
            for (int w = 0; w < cpuSize; w++)
            {
              if (usedCPU != 0 && usedCPU >= round(static_cast<double>(cpu) / (screenHight - 1) * (screenHight - i)))
              {
                out << "#";
              }
              else
              {
                out << " ";
              }
            }
            out << "|";*/
            // print out the memory bar if that precentage of memory is being used.
            for (unsigned int w = 0; w < memorySize; w++)
            {
              if (usedMem != 0 && usedMem >= round(static_cast<double>(memory) / (screenHight - 1) * (screenHight - i)))
              {
                out << "#";
              }
              else
              {
                out << " ";
              }
            }
            // print section divider
            out << "|###|";
            // print out the id number of the process
            conv = ((display.id == -1) ? "-" : std::to_string(display.id));
            for (unsigned int w = conv.size(); w < idSize; w++)
            {
              out << ((display.id == -1) ? "-" : "0");
            }
            out << conv;
            // print out divider
            out << "|";
            // print out the thread id of the process.
            conv = ((display.id == -1) ? "-" : std::to_string(display.threadId));
            for (unsigned int w = conv.size(); w < threadIdSize; w++)
            {
              out << ((display.id == -1) ? "-" : "0");
            }
            out << conv;
            // print out divider
            out << "|";
            // print out the name of the process.
            if (display.name.size() > nameSize - 2)
            {
              out << " ";
              for (unsigned int w = 0; w < nameSize - 3; w++)
              {
                out << display.name[w];
              }
              out << "..";
            } else {
              for (unsigned int w = display.name.size(); w < nameSize - 1; w++)
              {
                out << " ";
              }
              out << display.name << " ";
            }
            // print out divider
            out << "|";/*
            double precent = (static_cast<double>(display.cpu) / cpu * 100);
            conv = std::to_string(static_cast<int>(precent));
            remaining = cpuUSize - conv.size() - 1;
            for (int w = 0; w < (remaining / 2) + (remaining % 2); w++)
            {
              out << " ";
            }
            out << conv << "%";
            remaining = remaining / 2;
            for (int w = 0; w < remaining; w++)
            {
              out << " ";
            }
            out << "|";*/
            // print out the precentage of memory this process is using.
            double precent = (static_cast<double>(display.memory) / memory * 100);
            conv = std::to_string(static_cast<int>(precent));
            remaining = memoryUSize - conv.size() - 1;
            for (unsigned int w = 0; w < (remaining / 2) + (remaining % 2); w++)
            {
              out << " ";
            }
            out << conv << "%";
            remaining = remaining / 2;
            for (unsigned int w = 0; w < remaining; w++)
            {
              out << " ";
            }
            // print out divider
            out << "|";
            // print out the time remaining of the process
            std::string timeMin = std::to_string(display.time / 60);
            std::string timeSec = std::to_string(display.time % 60);
            if (timeSec.size() == 1)
            {
              timeSec = "0" + timeSec;
            }
            if (timeMin.size() == 1)
            {
              timeMin = "0" + timeMin;
            }
            if (timeMin.size() > 4)
            {
              timeMin = "!!!!";
            }
            conv = timeMin + ":" + timeSec;
            remaining = timeSize - 1;
            for (unsigned int w = 0; w < (remaining / 2) + (remaining % 2) - timeMin.size(); w++)
            {
              out << " ";
            }
            out << conv;
            remaining = (remaining / 2);
            for (unsigned int w = 0; w < remaining - timeSec.size(); w++)
            {
              out << " ";
            }
          }
          out << "|";
        }
        printing = false;
        std::cout << out.str() << std::endl;

        return;
      }

      void addProcess(std::string name, int id, unsigned int threadId, unsigned int memory, ull time)
      {
        // stop, the process is done
        if (time == 0) {
          return;
        }

        for (auto it = std::begin(running); it!=std::end(running); ++it)
        {
          if ((*it).id == id)
          {
            std::cout << "process already exists: " << id << std::endl;

            return;
          }
        }

        process newProcess;
        newProcess.name = name;
        newProcess.id = id;
        newProcess.threadId = threadId;
        newProcess.memory = memory;
        // newProcess.cpu = cpu;
        newProcess.time = time;
        // std::cout << "ID: " << id << "-Name: " << name << "-Mem: " << memory
        //           << "-TID: " << threadId << "-Time: " << time << std::endl;
        running.push_back(newProcess);
        std::sort(running.begin(), running.end(), compareProcess);

        return;
      }

      void updateProcess(int id, unsigned int memory, ull time)
      {
        while (printing)
        {
          continue;
        }
        for (auto it = std::begin(running); it!=std::end(running); ++it)
        {
          if ((*it).id == id)
          {
            if (time <= 0)
            {
              running.erase(it);
              break;
            }
            (*it).memory = memory;
            // (*it).cpu = cpu;
            (*it).time = time;
            break;
          }
        }

        return;
      }

      void changeSize(int width, int hight)
      {
        if (width > 0 && static_cast<unsigned int>(width) < MINSIZE)
        {
          std::cout << "WARNING! Minimum size is " << MINSIZE << ". Setting to minimum" << std::endl;
          width = MINSIZE;
        }
        if (hight > 0 && static_cast<unsigned int>(hight) < MINHIGHT)
        {
          std::cout << "WARNING! Minimum hight is " << MINHIGHT << ". Setting to minimum" << std::endl;
          hight = MINHIGHT;
        }
        //std::cout << width << "|" << std::endl;
        screenSize = width == -1 ? screenSize : width;
        screenHight = hight == -1 ? screenHight : hight;
        memorySize = MINMEMORYSIZE;
        // cpuSize = MINCPUSIZE;
        idSize = MINIDSIZE;
        threadIdSize = MINTHREADIDSIZE;
        nameSize = MINNAMESIZE;
        // cpuUSize = MINCPUUSIZE;
        memoryUSize = MINMEMORYUSIZE;
        timeSize = MINTIMESIZE;
        for (unsigned int i = 0; i < screenSize - MINSIZE; i++)
        {
          if (i % 7 == 0)
          {
            idSize++;
          } else if (i % 7 == 1)
          {
            threadIdSize++;
          } else if (i % 7 == 2)
          {
            nameSize++;
          } else if (i % 7 == 3)
          {
            nameSize++;
          } else if (i % 7 == 4)
          {
            memoryUSize++;
          } else if (i % 7 == 5)
          {
            timeSize++;
          } else if (i % 7 == 6)
          {
            memorySize++;
          }
        }
        //std::cout << memorySize << "|" << cpuSize << "|" << idSize << "|" << threadIdSize << "|"
        //          << nameSize << "|" << cpuUSize << "|" << memoryUSize << "|" << timeSize << std::endl;
      }

      void setComp(unsigned int mem)
      {
        // cpu = cp;
        memory = mem;
      }

      void clear()
      {
        running.clear();
      }

      static bool compareProcess(process a, process b)
      {
        return a.id < b.id;
      }

      bool isPrinting() { return printing; }
    // Private functions
    private:
  };
}
#endif

/*
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
|Memory| CPU |#| id|Tid|    name    | CPU |Memory|time left|
|------|-----|#|001|001| process n..|  0% |  0%  |  01:00  |
|------|-----|#|002|001| windows w..|  0% |  0%  |  01:00  |
|------|-----|#|003|002| new proce..|  0% |  0%  |  01:00  |
|------|-----|#|004|003|    unknown |  0% |  0%  |  01:00  |
|######|-----|#|005|003|  virus.exe |  0% |  0%  |  01:00  |
|######|-----|#|006|001|      avast |  0% |  0%  |  01:00  |
|######|-----|#|007|002| windows d..|  0% |  0%  |  01:00  |
|######|-----|#|008|001|    notepad |  0% |  0%  |  01:00  |
|######|-----|#|009|001|     chrome |  0% |  0%  |  01:00  |
|######|-----|#|010|003|    firefox | 43% | 82%  |  01:00  |
|######|-----|#|011|003| int. expl..|  0% |  0%  |  01:00  |
|######|-----|#|---|---|            |  -  |  --  |  --:--  |
|######|-----|#|---|---|            |  -  |  --  |  --:--  |
|######|#####|#|---|---|            |  -  |  --  |  --:--  |
|######|#####|#|---|---|            |  -  |  --  |  --:--  |
|######|#####|#|---|---|            |  -  |  --  |  --:--  |
|######|#####|#|---|---|            |  -  |  --  |  --:--  |
|######|#####|#|---|---|            |  -  |  --  |  --:--  |
|######|#####|#|---|---|            |  -  |  --  |  --:--  |
|######|#####|#|---|---|            |  -  |  --  |  --:--  |
|######|#####|#|---|---|            |  -  |  --  |  --:--  |
|######|#####|#|---|---|            |  -  |  --  |  --:--  |
|######|#####|#|---|---|            |  -  |  --  |  --:--  |
*/
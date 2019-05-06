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

#ifndef TASKMONITOR_H
#define TASKMONITOR_H

namespace Dispay
{
  // create a struct to store 
  struct process {
    std::string name;
    int id;
    int threadId;
    int memory;
    int cpu;
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
      // track the machine info.
      int memory;
      int cpu;
      // stores the size of the total display.
      int screenSize;
      int screenHight;
      // stores the size of the sub-parts of the display.
      int memorySize;
      int cpuSize;
      int idSize;
      int threadIdSize;
      int nameSize;
      int cpuUSize;
      int memoryUSize;
      int timeSize;
      // stores the minimum size each part can be.
      const int MINSIZE = 60;
      const int MINHIGHT = 3;
      const int MINMEMORYSIZE = 6;
      const int MINCPUSIZE = 5;
      const int MINIDSIZE = 3;
      const int MINTHREADIDSIZE = 3;
      const int MINNAMESIZE = 12;
      const int MINCPUUSIZE = 5;
      const int MINMEMORYUSIZE = 6;
      const int MINTIMESIZE = 9;

    // Public functions
    public:
      // Main constructor
      TaskMonitor()
      {
        // set all the default values
        memory = 0;
        cpu = 0;
        screenSize = MINSIZE;
        // higher base hight for simplicity sake.
        screenHight = MINHIGHT + 20;
        memorySize = MINMEMORYSIZE;
        cpuSize = MINCPUSIZE;
        idSize = MINIDSIZE;
        threadIdSize = MINTHREADIDSIZE;
        nameSize = MINNAMESIZE;
        cpuUSize = MINCPUUSIZE;
        memoryUSize = MINMEMORYUSIZE;
        timeSize = MINTIMESIZE;
      }

      // argumented constructor
      TaskMonitor(int mem, int cp, int size, int hight)
      {
        // set the memory and cpu
        memory = mem;
        cpu = cp;
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
        cpuSize = MINCPUSIZE;
        idSize = MINIDSIZE;
        threadIdSize = MINTHREADIDSIZE;
        nameSize = MINNAMESIZE;
        cpuUSize = MINCPUUSIZE;
        memoryUSize = MINMEMORYUSIZE;
        timeSize = MINTIMESIZE;
        // increase the size of the parts as much as possible, and evenly.
        for (int i = 0; i < screenSize - MINSIZE; i++) {
          if (i % 9 == 0) {
            cpuSize++;
          } else if (i % 9 == 1) {
            idSize++;
          } else if (i % 9 == 2) {
            threadIdSize++;
          } else if (i % 9 == 3) {
            nameSize++;
          } else if (i % 9 == 4) {
            nameSize++;
          } else if (i % 9 == 5) {
            cpuUSize++;
          } else if (i % 9 == 6) {
            memoryUSize++;
          } else if (i % 9 == 7) {
            timeSize++;
          } else if (i % 9 == 8) {
            memorySize++;
          }
        }
      }

      // prints out the task manager.
      void print()
      {
        // count the used cpu and memory.
        int usedCPU = 0;
        int usedMem = 0;
        for (auto it = std::begin(running); it!=std::end(running); ++it) {
          usedMem += (*it).memory;
          usedCPU += (*it).cpu;
        }
        // helper int for printing buffering.
        int remaining;
        // helper string for printing.
        std::string conv;
        // store the entire print out.
        std::stringstream out;
        // clear the screen.
        out << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
        //out << "00000000011111111112222222222333333333344444444445555555555666666666677777777778\n";
        //out << "12345678901234567890123456789012345678901234567890123456789012345678901234567890\n";
        // make a empty process for dealing with empty spots.
        process empty;
        empty.id = -1;
        empty.cpu = 0;
        empty.memory = 0;
        empty.name = "";
        empty.threadId = 0;
        empty.time = 0;
        // iterate over the screen hight.
        for (int i = 0; i < screenHight; i++)
        {
          // print out starting | with a new line.
          out << "\n|";
          // print out header.
          if (i == 0)
          {
            // print out CPU header.
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
            out << "|";
            // print out memory header
            remaining = memorySize - MINMEMORYSIZE;
            for (int w = 0; w < (remaining / 2) + (remaining % 2); w++)
            {
              out << " ";
            }
            out << "MEMORY";
            remaining = (memorySize - MINMEMORYSIZE) / 2;
            for (int w = 0; w < remaining; w++)
            {
              out << " ";
            }
            // print section divider
            out << "|#|";
            // print out the id of the process.
            remaining = idSize - MINIDSIZE;
            for (int w = 0; w < (remaining / 2); w++)
            {
              out << " ";
            }
            out << " id";
            remaining -= remaining / 2;
            for (int w = 0; w < remaining; w++)
            {
              out << " ";
            }
            // print out divider
            out << "|";
            // print out the thread id
            remaining = threadIdSize - MINTHREADIDSIZE;
            for (int w = 0; w < (remaining / 2) + (remaining % 2); w++)
            {
              out << " ";
            }
            out << "Tid";
            remaining = remaining / 2;
            for (int w = 0; w < remaining; w++)
            {
              out << " ";
            }
            // print out divider
            out << "|";
            // print out name header.
            remaining = nameSize - MINNAMESIZE;
            for (int w = 0; w < (remaining / 2) + (remaining % 2); w++)
            {
              out << " ";
            }
            out << "    name    ";
            remaining = (nameSize - MINNAMESIZE) / 2;
            for (int w = 0; w < remaining; w++)
            {
              out << " ";
            }
            // print out divider
            out << "|";
            // print out cpu usage header.
            remaining = cpuUSize - MINCPUUSIZE;
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
            out << "|";
            // print out memory header
            remaining = memoryUSize - MINMEMORYUSIZE;
            for (int w = 0; w < (remaining / 2) + (remaining % 2); w++)
            {
              out << " ";
            }
            out << "MEMORY";
            remaining = (memoryUSize - MINMEMORYUSIZE) / 2;
            for (int w = 0; w < remaining; w++)
            {
              out << " ";
            }
            // print out divider
            out << "|";
            // print out time left header
            remaining = timeSize - MINTIMESIZE;
            for (int w = 0; w < (remaining / 2) + (remaining % 2); w++)
            {
              out << " ";
            }
            out << "time left";
            remaining = (timeSize - MINTIMESIZE) / 2;
            for (int w = 0; w < remaining; w++)
            {
              out << " ";
            }
          }
          else
          {
            process display;
            if (i <= running.size())
            {
              display = running[i - 1];
            }
            else
            {
              display = empty;
            }
            for (int w = 0; w < cpuSize; w++) {
              if (usedCPU >= (static_cast<double>(cpu) / (screenHight - 1) * (screenHight - i)))
              {
                out << "#";
              }
              else
              {
                out << " ";
              }
            }
            out << "|";
            for (int w = 0; w < memorySize; w++)
            {
              if (usedMem >= (static_cast<double>(memory) / (screenHight - 1) * (screenHight - i)))
              {
                out << "#";
              }
              else
              {
                out << " ";
              }
            }
            out << "|#|";
            conv = ((display.id == -1) ? "-" : std::to_string(display.id));
            for (int w = conv.size(); w < idSize; w++)
            {
              out << ((display.id == -1) ? "-" : "0");
            }
            out << conv;
            out << "|";
            conv = ((display.id == -1) ? "-" : std::to_string(display.threadId));
            for (int w = conv.size(); w < threadIdSize; w++)
            {
              out << ((display.id == -1) ? "-" : "0");
            }
            out << conv;
            out << "|";
            if (display.name.size() > nameSize - 2)
            {
              out << " ";
              for (int w = 0; w < nameSize - 3; w++)
              {
                out << display.name[w];
              }
              out << "..";
            } else {
              for (int w = display.name.size(); w < nameSize - 1; w++)
              {
                out << " ";
              }
              out << display.name << " ";
            }
            out << "|";
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
            out << "|";
            precent = (static_cast<double>(display.memory) / memory * 100);
            conv = std::to_string(static_cast<int>(precent));
            remaining = memoryUSize - conv.size() - 1;
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
            out << "|";
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
            for (int w = 0; w < (remaining / 2) + (remaining % 2) - timeMin.size(); w++)
            {
              out << " ";
            }
            out << conv;
            remaining = (remaining / 2);
            for (int w = 0; w < remaining - timeSec.size(); w++)
            {
              out << " ";
            }
          }
          out << "|";
        }
        std::cout << out.str() << std::endl;
      }

      void addProcess(std::string name, int id, int threadId, int memory, int cpu, ull time) {
        for (auto it = std::begin(running); it!=std::end(running); ++it) {
          if ((*it).id == id) {
            std::cout << "process already exists";

            return;
          }
        }

        process newProcess;
        newProcess.name = name;
        newProcess.id = id;
        newProcess.threadId = threadId;
        newProcess.memory = memory;
        newProcess.cpu = cpu;
        newProcess.time = time;
        running.push_back(newProcess);

        return;
      }

      void updateProcess(int id, int memory, int cpu, ull time) {
        for (auto it = std::begin(running); it!=std::end(running); ++it) {
          if ((*it).id == id) {
            if (time <= 0) {
              running.erase(it);
              break;
            }
            (*it).memory = memory;
            (*it).cpu = cpu;
            (*it).time = time;
            break;
          }
        }

        return;
      }

      void changeSize(int width, int hight) {
        if (width < MINSIZE && width > 0)
        {
          std::cout << "WARNING! Minimum size is " << MINSIZE << ". Setting to minimum" << std::endl;
          width = MINSIZE;
        }
        screenSize = width;
        if (hight < MINHIGHT && hight > 0)
        {
          std::cout << "WARNING! Minimum hight is " << MINHIGHT << ". Setting to minimum" << std::endl;
          hight = MINHIGHT;
        }
        //std::cout << width << "|" << std::endl;
        screenSize = width == -1 ? screenSize : width;
        screenHight = hight == -1 ? screenHight : hight;
        memorySize = MINMEMORYSIZE;
        cpuSize = MINCPUSIZE;
        idSize = MINIDSIZE;
        threadIdSize = MINTHREADIDSIZE;
        nameSize = MINNAMESIZE;
        cpuUSize = MINCPUUSIZE;
        memoryUSize = MINMEMORYUSIZE;
        timeSize = MINTIMESIZE;
        for (int i = 0; i < screenSize - MINSIZE; i++) {
          if (i % 9 == 0) {
            cpuSize++;
          } else if (i % 9 == 1) {
            idSize++;
          } else if (i % 9 == 2) {
            threadIdSize++;
          } else if (i % 9 == 3) {
            nameSize++;
          } else if (i % 9 == 4) {
            nameSize++;
          } else if (i % 9 == 5) {
            cpuUSize++;
          } else if (i % 9 == 6) {
            memoryUSize++;
          } else if (i % 9 == 7) {
            timeSize++;
          } else if (i % 9 == 8) {
            memorySize++;
          }
        }
        //std::cout << memorySize << "|" << cpuSize << "|" << idSize << "|" << threadIdSize << "|"
        //          << nameSize << "|" << cpuUSize << "|" << memoryUSize << "|" << timeSize << std::endl;
      }

      void setComp(int cp, int mem) {
        cpu = cp;
        memory = mem;
      }
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
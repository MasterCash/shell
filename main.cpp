/*
 * Name: Josh Cash, Jeffrey Strahm, Hannah Reinbolt
 * Class: Operating Systems
 * Assignment Shell Emulator
 */

#include <iostream>
#include <thread>
#include "computer.h"
#include "node.h"

int main()
{
  // Make a computer
  Shell::Computer c;
  // run it.
  std::thread t(&Shell::Computer::threadUpdate, std::ref(c));
  c.run();
  t.join(); 

  return 0;
}
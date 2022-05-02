#include <iostream>
#include <iomanip>
#include "Simulator.hpp"

int main(int argc,char* argv[])
{
   auto s = std::make_shared<Simulator>();
   s->read(argv[1]);
   s->run(atof(argv[2]));  
   return 0;
}

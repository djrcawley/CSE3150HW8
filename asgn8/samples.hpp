#ifndef __SAMPLES_H
#define __SAMPLES_H

#include <list>
#include <string>
#include <iostream>

class Samples {
   std::string       _name;
   std::list<double> _data;
public:
   Samples(const std::string& n) : _name(n)  {}
   void addSample(double d) { _data.push_back(d);}
   void report(std::ostream& os);
};

#endif

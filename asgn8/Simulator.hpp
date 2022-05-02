#ifndef __SIMULATOR_H
#define __SIMULATOR_H

#include <iostream>
#include <istream>
#include <map>
#include <list>
#include <functional>
#include <queue>

#include "event.hpp"
#include "component.hpp"
namespace Generator {
   class Generator;
}

class Simulator {
   int _lastEvt;
   int _lastCli;
   double  _now;
   double  _horizon;
   // Type for a builder function
   typedef std::function<void(std::istream&,const std::string& name)> buildFun;   
   std::map<std::string,buildFun> _builders; // map of builders (poly-construction)   
   std::map<std::string,Component*> _cps;    // map of components
   std::list<Generator::Generator*> _gens;    // all the generators.
   // Simulation data structures
   std::priority_queue<Event::Ptr,std::vector<Event::Ptr>,CompareEvent> _queue;
   void addComponent(const std::string& key,Component* c);
   bool hasComponent(const std::string& key) const;
   Component* getComponent(const std::string& key);
public:
   Simulator();
   ~Simulator();
   int newEvent() { return _lastEvt++;}
   Client::Ptr makeClient();
   double now() const { return _now;}
   double horizon() const { return _horizon;}
   void schedule(Event::Ptr evt);
   void read(const char* fn);      
   void run(double h);
   friend std::ostream& operator<<(std::ostream& os,const Simulator& s);
};

#endif

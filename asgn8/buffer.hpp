#ifndef __BUFFER_H
#define __BUFFER_H

#include <queue>
#include <iostream>
#include "component.hpp"
#include "event.hpp"
#include "client.hpp"
#include "samples.hpp"

class Buffer: public Component {
protected:
   Component* _target;
   void addTarget(Component* c2);
public:
   Buffer(const std::string& n) : Component(n) {  _target = nullptr;}
   virtual ~Buffer() {}
   Component* getTarget() { return _target;}
   void produce(Simulator* s,Client::Ptr client,double at);
   virtual void enter(Client::Ptr e,Simulator* sim) = 0;
   virtual Client::Ptr leave(Simulator* sim) = 0;
};

class FIFO : public Buffer {
   std::queue<Client::Ptr> _buf;
   Samples                  _ql;
   Samples                  _wt;
protected:
   std::ostream& print(std::ostream& os)  const;
public:
   FIFO(const std::string& n) : Buffer(n),_ql(n + "-Queue Len"),_wt(n+ "-wait Time")  {}
   ~FIFO() {}
   void enter(Client::Ptr e,Simulator* sim);
   Client::Ptr leave(Simulator* sim);      
   void showStats(std::ostream& os);
};
  
#endif

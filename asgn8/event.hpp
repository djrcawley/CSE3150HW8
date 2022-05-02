#ifndef __EVENT_H
#define __EVENT_H

#include <iostream>
#include <memory>
#include "client.hpp"

class Simulator;
class Component;
class Server;
class Buffer;

namespace Generator {
   class Generator;
};

class Event {
protected:
   const int _id;
   double    _at;
   virtual std::ostream& print(std::ostream& os) const;
public:
   typedef std::shared_ptr<Event> Ptr;
   Event(int id) : _id(id) {}
   Event(int id,double t) : _id(id),_at(t) {}
   virtual ~Event() {}
   double when() const { return _at;}
   virtual void simulate(Simulator* sim) = 0;
   friend std::ostream& operator<<(std::ostream& os,const Event& evt) { return evt.print(os);}
};

struct CompareEvent {
   bool operator() (Event::Ptr a,Event::Ptr b) {
      return a->when() > b->when();
   }
};

// ================================================================================
// Generator events
class StartEvt : public Event {
   Generator::Generator* _g;
   std::ostream& print(std::ostream& os) const;
public:
   StartEvt(int id,double t,Generator::Generator* g) : Event(id,t),_g(g) {}
   void simulate(Simulator* sim);
};

// ================================================================================
// Buffer events
class ArrivalEvt :public Event {
   Buffer* _in;
   Client::Ptr  _client;
   std::ostream& print(std::ostream& os) const;
public:
   ArrivalEvt(int id,Client::Ptr c,double t,Buffer* in)
      : Event(id,t),_in(in),_client(c) {}
   void simulate(Simulator* sim);
};

// ================================================================================
// Server events

class BeginEvt : public Event {
   Server*              _server;
   Buffer*         _in;
   Client::Ptr _cli;
   std::ostream& print(std::ostream& os) const;
public:
   BeginEvt(int id,double t,Buffer* src,Server* s)
      : Event(id,t),_in(src),_server(s) {}
   void simulate(Simulator* sim);
};

class EndEvt : public Event {
   Server* _server;
   Client::Ptr _cli;   
   std::ostream& print(std::ostream& os) const;
public:
   EndEvt(int id,double t,Server* s,Client::Ptr cl)
      : Event(id,t),_server(s),_cli(cl) {}
   void simulate(Simulator* sim);   
};

class FinishEvt : public Event {
   Client::Ptr _cli;
   std::ostream& print(std::ostream& os) const;
public:
   FinishEvt(int id,double t,Client::Ptr cl)
      : Event(id,t),_cli(cl) {}
   ~FinishEvt();
   void simulate(Simulator* sim);   
};

#endif

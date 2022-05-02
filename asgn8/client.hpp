#ifndef __CLIENT_H
#define __CLIENT_H

#include <iostream>
#include <memory>

class Component;

class Client {
   int _id;
protected:
   Component*      _at;
   double     _arrival;
public:
   typedef std::shared_ptr<Client> Ptr;
   Client(int id) : _id(id),_at(nullptr),_arrival(0) {}
   void arrive(Component* c,double now) { _at = c;_arrival = now;}
   double getArrival() const   { return _arrival;}
   double leave()              { _at = nullptr;return _arrival;}
   friend std::ostream& operator<<(std::ostream& os,const Client& c) {
      return os << "Client(" << c._id << ")";
   }
};

#endif

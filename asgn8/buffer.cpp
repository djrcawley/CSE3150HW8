#include "buffer.hpp"
#include <assert.h>
#include "Simulator.hpp"

void Buffer::addTarget(Component* c2)
{
   _target = c2;
   c2->setFeed(this);
}
void Buffer::produce(Simulator* s,Client::Ptr client,double at)
{
   s->schedule(Event::Ptr(new ArrivalEvt(s->newEvent(),client,at,this)));
}
std::ostream& FIFO::print(std::ostream& os)  const
{
   assert(_target != nullptr);
   return os <<  _name << "=FIFO(target = " << _target->getName() << ")";
}

void FIFO::enter(Client::Ptr e,Simulator* sim)
{
   //std::cout << "->FIFO(" << getName() << ',' << _buf.size() << ')' << std::endl;
   _buf.push(e);
   _ql.addSample(_buf.size());
}

Client::Ptr FIFO::leave(Simulator* sim)
{
   //std::cout << "<-FIFO(" << getName() << ',' << _buf.size() << ')' << std::endl;
   if (_buf.empty()) 
      return Client::Ptr();
   _ql.addSample(_buf.size());
   Client::Ptr e = _buf.front();
   _buf.pop();
   double waiting = sim->now() - e->leave();
   _wt.addSample(waiting);
   // std::cout << *e << " waiting in  queue: " << this
   //           << " for: " << waiting
   //           << std::endl;
   return e;
}

void FIFO::showStats(std::ostream& os)
{
   _ql.report(os);
   os << std::endl;
   _wt.report(os);
   os << std::endl;
}



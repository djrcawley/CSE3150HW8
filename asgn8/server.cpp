#include "server.hpp"
#include "Simulator.hpp"
#include "buffer.hpp"
#include <assert.h>

Server::Server(const std::string& n)
   : Component(n),_st(n + "-service time")
{
   _target = nullptr;   
}

void Server::showStats(std::ostream& os)
{
   _st.report(os);
   os << std::endl;
}

void Server::startClient(Client::Ptr cli)
{
   _on = cli;
}

void Server::finishClient(double now)
{
   _st.addSample(now - _on->getArrival());
   _on.reset();
}

void Server::produce(Simulator* s,Client::Ptr client,double at)
{
   assert(at == s->now());
   if (!busy())
      s->schedule(Event::Ptr(new BeginEvt(s->newEvent(),at,_feed,this)));   
}

ServerExponential::ServerExponential(const std::string& n,double r)
   : Server(n),
     _rate(r),
     _rd(),_gen(_rd()),_distr(r)
{}   

std::ostream& ServerExponential::print(std::ostream& os)  const
{
   return os << _name <<   "=ServerExp(" << _rate << ','
             << "target=" << _target->getName() << ")";
}

double ServerExponential::sampleServiceTime() 
{
   return _distr(_gen);
}

ServerNormal::ServerNormal(const std::string& n,double m,double d)
   : Server(n),
     _mean(m),
     _deviation(d),
     _rd(),_gen(_rd()),_distr(_mean,_deviation)
{
}   

std::ostream& ServerNormal::print(std::ostream& os)  const
{
   return os << _name <<   "=ServerNormal(" << _mean << ',' << _deviation
             << ',' << "target=" << _target->getName() << ")";
}

double ServerNormal::sampleServiceTime() 
{
   return _distr(_gen);
}

ServerConstant::ServerConstant(const std::string& n,double d)
   : Server(n),_dur(d)
{}   

double ServerConstant::sampleServiceTime() 
{
   return _dur;
}

std::ostream& ServerConstant::print(std::ostream& os)  const
{
   return os << _name <<   "=ServerCst(" << _dur << ','
             << "target=" << _target->getName() << ")";
}

void ServerExit::produce(Simulator* s,Client::Ptr client,double at)
{
   assert(at == s->now());
   s->schedule(Event::Ptr(new FinishEvt(s->newEvent(),at,client)));
}

void ServerExit::showStats(std::ostream& os)
{}

std::ostream& Dispatcher::print(std::ostream& os)  const
{
   os << _name << "=Dispatch([";
   for(auto p : _rules) 
      os << '(' << p.first->getName() << ',' << p.second << ')' << ',';   
   return os << '\b' << "])";
}

Dispatcher::Dispatcher(const std::string& n)
   : Server(n),
     _rd(),
     _gen(_rd()),
     _dist(0.0,1.0)
{}

void Dispatcher::addRule(Component* c,double prob)
{
   _rules.push_back(std::pair<Component*,double>(c,prob));
}

void Dispatcher::produce(Simulator* sim,Client::Ptr client,double at)
{
   double v = _dist(_gen);
   double l = 0.0;
   Component* chosen = nullptr;
   for(auto p : _rules) {
      const double u = l + p.second;
      if (l <= v && v < u) {
         chosen = p.first;
         break;
      }
      l = u;
   }
   assert(chosen != nullptr);
   // std::cout << "Dispatcher(" << getName() << ',' << " to "
   //           << chosen->getName() << '[' << v << ']' << ')' << std::endl;
   chosen->produce(sim,client,at);
}

void Dispatcher::showStats(std::ostream& os)
{}

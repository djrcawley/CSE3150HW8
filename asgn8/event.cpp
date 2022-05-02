#include "event.hpp"
#include "generator.hpp"
#include "server.hpp"
#include "Simulator.hpp"
#include "buffer.hpp"

using namespace std;

std::ostream& Event::print(std::ostream& os) const
{
   return os << "Evt(" << _id << ",at=" << _at << ')';
}

void StartEvt::simulate(Simulator* sim)
{
   _g->produce(sim,sim->makeClient(),sim->now());
}

std::ostream& StartEvt::print(std::ostream& os) const
{
   return os << "StartEvt(" << _id << ",at=" << _at << ",gen = " << *_g << ')';
}

void ArrivalEvt::simulate(Simulator* sim)
{
   _in->enter(_client,sim);
   _client->arrive(_in,sim->now());
   _in->getTarget()->produce(sim,nullptr,sim->now());
}

std::ostream& ArrivalEvt::print(std::ostream& os) const
{
   os << "ArrivalEvt(" << _id << ",at=" << _at << ",cli = ";
   if (_client.use_count() > 0)
      os << *_client;
   else os << "null";
   return os << ')';
}

// Server Events
void BeginEvt::simulate(Simulator* sim)
{
   _cli = _in->leave(sim);
   if (_cli.use_count() > 0) {
      _cli->arrive(_server,sim->now());
      _server->startClient(_cli);
      double sst = sim->now() + _server->sampleServiceTime();
      sim->schedule(Event::Ptr(new EndEvt(sim->newEvent(),sst,_server,_cli)));
   }
}

void EndEvt::simulate(Simulator* sim)
{
   _server->finishClient(sim->now());
   _server->produce(sim,nullptr,sim->now());// no longer busy. Produce a consume event.
   double waiting = sim->now() - _cli->leave();
   // cout << *_cli << " waiting on server: " << _server
   //      << " for: " << waiting << endl;
   _server->getTarget()->produce(sim,_cli,sim->now());
}

std::ostream& BeginEvt::print(std::ostream& os) const
{
   os << "BeginEvt(" << _id << ",at=" << _at << ",cli = ";
   if (_cli.use_count() > 0)
      return os << *_cli << ')';
   else return os << "null)";
}

std::ostream& EndEvt::print(std::ostream& os) const
{
   os << "EndEvt(" << _id << ",at=" << _at << ",cli = ";
   if (_cli.use_count() > 0)
      os << *_cli;
   else os << "null";
   return os << ')';
}

FinishEvt::~FinishEvt()
{}

void FinishEvt::simulate(Simulator* sim)
{
   //cout << *_cli << " leaving the simulation at: " << when() << endl;
}

std::ostream& FinishEvt::print(std::ostream& os) const
{
   os << "FinishEvt(" << _id << ",at=" << _at << ",cli = ";
   if (_cli.use_count() > 0)
      os << *_cli;
   else os << "null";
   return os << ')';
}

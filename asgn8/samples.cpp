#include "samples.hpp"

void Samples::report(std::ostream& os)
{
   auto nb = _data.size();
   double ttl = 0;
   for(auto v : _data)
      ttl += v;
   os << _name << '(' << nb << ')' << " : " << ttl/nb; 
}

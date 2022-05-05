#include "Proximity.h"

Proximity::Proximity()
{
  rssis = new DecisionQueue(40);
  gradients = new DecisionQueue(10);
  inferrer = new Inferrer();
}
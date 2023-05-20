#include "Object.h"
#include "AutoSignal.h"

Object::~Object()
{
	for (IAutoSignal *c : connections_)
	{
		c->on_object_deleted(this);
	}
}

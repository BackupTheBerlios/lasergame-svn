#ifndef MODEL_BASE_H_INCLUDED
#define MODEL_BASE_H_INCLUDED 1

#include "number/Speed.h"

class ModelBase
{
	public:
		virtual num::Speed calcSpeed(const num::Speed & in_old, const num::Time & in_dt) const = 0;
		

};

#endif // ESTER_MODEL_INCLUDED


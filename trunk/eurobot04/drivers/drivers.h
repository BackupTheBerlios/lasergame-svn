// $Id$
// Copyright (C) 2004, Zbynek Winkler

///@file
#ifndef DRIVER_H_INCLUDED
#define DRIVER_H_INCLUDED 1

#include "util/msg.h"
#include "util/util.h"

class Field;

namespace drivers {

	class Driver : public msg::Runnable
	{
		public:
			class Params
			{
				public:
					msg::Channel* p;
					const int     myID;
					const int     robotID;
					msg::Channel* done;
					Field&        field;

					Params(msg::Channel* in_p, int in_id, int in_robotID, msg::Channel* in_done, Field& in_field) 
						: p(in_p), myID(in_id), robotID(in_robotID), done(in_done), field(in_field) {}
			};

		protected:
			Params m;

			Driver(const Params& in_params) : m(in_params) {} 
	};
	
	typedef msg::Runnable* (*pFac)(Driver::Params& in_params);
	
	pFac getFactory(const char* in_name);
}

#endif // DRIVER_H_INCLUDED


#ifndef CONTACT_DATA_H_INCLUDED
#define CONTACT_DATA_H_INCLUDED

/** @file
*         Structure encapsulating contact behaviour of appropriate Geom
*         
* @author Jaroslav Sladek
*
* $Id$
*/

struct ContactData
{
	ContactData() : mu(10000) {}
	ContactData(double in_mu) : mu(in_mu) {}
	double mu;
};

#endif

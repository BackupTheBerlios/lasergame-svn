#ifndef ODOFILTER_H_INCLUDED
#define ODOFILTER_H_INCLUDED 1

/**
 * @file
 * Collection of odometry filters for MCL
 *
 * @author Zbynek Winkler (c) 2003, 2004 <zw at robotika cz>
 * 
 * $Id$
 */

#include "number/dist.h"
#include "number/angle.h"

/// Basic (most simple) implementation of odometry
struct OdoBasic //{{{1
{
	num::Dist m_l;       ///< Distance traveled by the left wheel in the current step
	num::Dist m_r;       ///< Distance traveled by the right wheel in the current step
	const num::Dist D;   ///< Length of the wheel axis
	const double ENCERR; ///< Expected encoder error
  /////////////////////////////////////////////////////////
	/// Initializes the length of wheel axis and encoder error
	OdoBasic(const num::Dist &in_R, const double & in_ENCERR=0)
		: D(in_R*2), ENCERR(in_ENCERR)
	{	}
	/// Prepare values for the current frame
	void init(const num::Dist & in_left, const num::Dist & in_right)
	{
		m_l = in_left;
		m_r = in_right;
	}
	/// Move the suplied sample according to stored values
	void apply(num::Pose* in_pos, num::Rnd & in_rnd)
	{
		double noise_l = 1 + ENCERR * in_rnd();
		double noise_r = 1 + ENCERR * in_rnd();
		num::Dist l = m_l * noise_l;
		num::Dist r = m_r * noise_r;

		num::Dist f = (r + l) / 2;
		num::Angle a = num::Rad((r - l) / D);

		in_pos->heading() += a;
		in_pos->heading().normalize();
		in_pos->x() += f * in_pos->heading().cos();
		in_pos->y() += f * in_pos->heading().sin();
	}
};

///In comparison with OdoBasic it uses the df and da values (is more effective)
class OdoNextGen //{{{1
{
	num::Dist m_df;           ///< Distance traveled forward by the robot in the current step
	num::Dist m_ds;           ///< Distance traveled to the side by the robot in the current step
	num::Angle m_da;          ///< Change in orientation in the current step
	const double AERR;   ///< Expected error in the change of the orientation
	const double FERR;   ///< Expected error in the distance traveled
	const double DAERR;  ///< Expected error in distance when changing orientation (meter/rad)
public:
	/// Init the params
	OdoNextGen(const double& in_ae, const double& in_fe, const double& in_dae)//{{{2
		: AERR(in_ae), FERR(in_fe), DAERR(in_dae) 
	{
	}
	/// Store the df and da values
	void init(const num::Dist& a_df, const num::Dist& a_ds, const num::Angle& a_da) //{{{2
	{
		m_df = a_df;
		m_ds = a_ds;
		m_da = a_da;
	}
	/// Move the supplied position by df and da taking into account the expected errors
	void apply(num::Pose * in_pos, num::Rnd & in_rnd) //{{{2
	{
		// calculate the error in position depending on the error in turning
		num::Meter df = (m_da/num::Rad(1) * DAERR) * in_rnd();
		num::Meter ds = (m_da/num::Rad(1) * DAERR) * in_rnd();
		
		in_pos->advanceBy(m_df + m_df*FERR*in_rnd() + df);
		in_pos->slideBy(  m_ds + m_ds*FERR*in_rnd() + ds);
		in_pos->turnBy(   m_da + m_da*AERR*in_rnd());
	}//}}}
};
//}}}

#endif // ODOFILTER_H_INCLUDED


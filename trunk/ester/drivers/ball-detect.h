#ifndef DRIVERS_BALL_DETECT_H_INCLUDED
#define DRIVERS_BALL_DETECT_H_INCLUDED 1

#include "drivers.h"
#include "number/pose.h" // TODO: split pose.h to point.h & pose.h

namespace drivers {

class BallDetect : public Driver
{
	public:
		BallDetect(const Params& in_params) : Driver(in_params) {}
		virtual void main()
		{
			using namespace msg;
			Subs<num::Pose>        pose(m.p, "pose");
			Subs<num::Point> ballOffset(m.p, "ball-offset");
			Subs<int>              done(m.done);

			done.value = m.myID;
			done.publish();

			while (true)
			{
				waitFor(pose);
				ballOffset.value = calc(m.field, pose.value);
				ballOffset.publish();

				done.value = m.myID;
				done.publish();
			}
		}
		
		num::Point calc(Field& in_field, const num::Pose & in_pose) //{{{1
		{
			using namespace num;
			thread::Locker l(in_field);
			Balls& in_balls(in_field.m_balls);

			Balls::const_iterator iter;
			Angle min = Deg(180);
			Point minOff;
		
			//cout << "upCamera()" << endl;
			for (iter = in_balls.begin(); iter != in_balls.end(); ++iter)
			{
				Point o = in_pose.offsetTo(*iter);
				Dist d = o;
				Angle a = o;
				if (a < Angle()) a *= -1;
				//cout << o.n() << " --- " << (*iter).n() << endl;
				//cout << setw(5) << a.rad() << " (" << (*iter).n() << ")" << endl;
				if (d > Milim(100))
				{
					if (a.lt(min, Deg(3)))
					{
						//cout << "### min:" << min.rad() << " -> " << a.rad() << ", " << d.m() << endl;
						min = a;
						minOff = o;
					}
					else if (a.lt(min+Deg(3)) && d < Dist(minOff))
					{
						//cout << "*** dist: " << Dist(minOff).m() << " -> " << d.m() << endl;
						//cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
						min = a;
						minOff = o;
					}
				}
			}
			return min < Deg(22) ? minOff : Point(Milim(-1), Dist());
			//cout << Angle(m_ballPos.m_off).deg() << endl << endl;
		}
				
};

}

#endif // DRIVERS_BALL_DETECT_H_INCLUDED


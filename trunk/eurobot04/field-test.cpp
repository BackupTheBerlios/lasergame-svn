#include "field.h"
#include "util/unit-test.h"

using namespace num;

namespace {

AUTOTEST(eatBall) //{{{1
{
	Field field;
	field.tryEatBall(Pose(), Milim(5000));
}
//}}}1
}


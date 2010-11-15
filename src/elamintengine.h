//ELAM integer engine header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_INTENGINE_H
#define ELAM_INTENGINE_H

#include "elamengine.h"

namespace ELAM {

///integer math enabled engine
class IntEngine:public Engine
{
	public:
		///instantiates a pre-configured engine
		IntEngine();
		
		///configures any engine to support basic int math
		static void configureIntEngine(Engine&);
		
		///returns the default priority of the int literal parser
		static int intParserPrio();
};

};

#endif

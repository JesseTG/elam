//ELAM string engine header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_STRENGINE_H
#define ELAM_STRENGINE_H

#include "elamengine.h"

namespace ELAM {

///integer math enabled engine
class StringEngine:public Engine
{
	public:
		///instantiates a pre-configured engine
		StringEngine();
		
		///configures any engine to support basic int math
		static void configureStringEngine(Engine&);
		
		///returns the default priority of the int literal parser
		static int stringParserPrio();
};

};

#endif

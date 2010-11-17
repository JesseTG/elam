//ELAM integer engine header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_BOOLENGINE_H
#define ELAM_BOOLENGINE_H

#include "elamengine.h"

namespace ELAM {

/**A boolean and logic math enabled engine.

This engine type plays nicely with ELAM::IntEngine
*/
class BoolEngine:public Engine
{
	public:
		///instantiates a pre-configured engine
		BoolEngine();
		
		///configures any engine to support basic boolean math
		static void configureBoolEngine(Engine&);
		
		///configures any engine to support logic functions
		static void configureLogicEngine(Engine&);
};

};

#endif

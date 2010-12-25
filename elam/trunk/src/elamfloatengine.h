//ELAM floating point engine header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_FLOATENGINE_H
#define ELAM_FLOATENGINE_H

#include "elamengine.h"

namespace ELAM {

/**an engine that has floating point (double) numbers implemented

This engine type plays nice with the IntEngine functions by supplemening its operators to be able to mix float and int in operations.

Per default only the float versions are pre-installed. Use IntEngine::configureIntEngine or IntFloatEngine to have both.
*/
class FloatEngine:public Engine
{
	public:
		///instantiates a pre-initialized engine
		FloatEngine();
		
		///configures any engine to support float
		static void configureFloatEngine(Engine&);
		
		///returns the high priority for float literal parsers, this applies to floats that are definitely not int
		static int floatHighParserPrio();
		///returns the low priority for float literal parsers, this applies to floats that would be interpreted as int if an int engine is present
		static int floatLowParserPrio();
};

///an engine with integer and float math pre-installed, see IntEngine and FloatEngine
class IntFloatEngine:public Engine
{
	public:
		///instantiates a pre-installed engine
		IntFloatEngine();
};

//end of namespace
};

#endif

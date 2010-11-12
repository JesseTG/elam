//ELAM integer engine header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_INTENGINE_H
#define ELAM_INTENGINE_H

#include "elamengine.h"

class ELAMIntEngine:public ELAMEngine
{
	public:
		ELAMIntEngine();
		
		static void configureIntEngine(ELAMEngine&);
};

#endif

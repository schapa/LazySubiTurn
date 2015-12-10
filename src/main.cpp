//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "api.h"

int main(int argc, char* argv[]) {

	initGpio();
	initInterrupts();

	while (true) {
		__WFI();
	}
}


// ----------------------------------------------------------------------------

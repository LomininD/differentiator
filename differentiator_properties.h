#ifndef DIFFERENTIATOR_PROPERTIES_H
#define DIFFERENTIATOR_PROPERTIES_H

// Turn off in release version:
#define HTML_DEBUG_MODE  // FIXME - solve define problem
#define TREE_VERIFICATION
//#define ADVERTISEMENTS

enum diff_ops
{
	unknown_op,
	sum,
	sub
};

#include "debug_lib/debug.h"
#include "tree_properties.h"

#define SAVE_FILE_NAME "formula.txt" // TODO - remake as command line argument

#endif
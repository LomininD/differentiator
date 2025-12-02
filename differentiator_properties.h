#ifndef DIFFERENTIATOR_PROPERTIES_H
#define DIFFERENTIATOR_PROPERTIES_H

// Turn off in release version:
#define HTML_DEBUG_MODE  // FIXME - solve define problem
#define TREE_VERIFICATION
//#define ADVERTISEMENTS

// TODO - move to other file
enum diff_ops // decode_operation_type_enum | parse_node_data | enum
{
	UNKNOWN_OP = -1,
	ADD,
	SUB,
	MUL,
	DIV,
	SIN,
	COS,
	POW,
	LN,

	op_count
};

#include "debug_lib/debug.h"
#include "tree_properties.h"

#define FORMULA_FILE_NAME "formula.txt" // TODO - remake as command line argument

#endif
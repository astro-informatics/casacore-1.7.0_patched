//# MSStateGram.cc: Grammar for field expressions
//# Copyright (C) 1998,1999,2001,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: MSStateGram.cc 21130 2011-10-18 07:39:05Z gervandiepen $

// MSStateGram; grammar for field command lines

// This file includes the output files of bison and flex for
// parsing command lines operating on lattices.
// This is a preliminary version; eventually it has to be incorporated
// in the AIPS++ command language.

#include <tables/Tables/ExprNode.h>
#include <tables/Tables/ExprNodeSet.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <ms/MeasurementSets/MSStateColumns.h>
#include <ms/MeasurementSets/MSStateGram.h>
#include <ms/MeasurementSets/MSStateParse.h>
#include <ms/MeasurementSets/MSStateIndex.h>
#include <ms/MeasurementSets/MSSelectionError.h>

#include <tables/Tables/TableParse.h>       // routines used by bison actions
#include <tables/Tables/TableError.h>

//# stdlib.h is needed for bison 1.28 and needs to be included here
//# (before the flex/bison files).
#include <casa/stdlib.h>
#include "MSStateGram.ycc"                  // flex output
#include "MSStateGram.lcc"                  // bison output

// Define the yywrap function for flex.
int MSStateGramwrap()
{
    return 1;
}

namespace casa { //# NAMESPACE CASA - BEGIN

//# Declare a file global pointer to a char* for the input string.
static const char*           strpMSStateGram = 0;
static Int                   posMSStateGram = 0;
  // MSStateGramwrap out of namespace

//# Parse the command.
//# Do a yyrestart(yyin) first to make the flex scanner reentrant.
int msStateGramParseCommand (const MeasurementSet* ms, const String& command) 
{
  try 
    {
      Int ret;
      MSStateGramrestart (MSStateGramin);
      yy_start = 1;
      strpMSStateGram = command.chars();     // get pointer to command string
      posMSStateGram  = 0;                   // initialize string position
      MSStateParse parser(ms);               // setup measurement set
      MSStateParse::thisMSSIParser = &parser; // The global pointer to the parser
      MSStateParse::thisMSSIParser->reset();
      //      fieldError.reset();
      ret=MSStateGramparse();                // parse command string
      return ret;
    }
  catch (MSSelectionStateError &x)
    {
      String newMesgs;
      newMesgs = constructMessage(msStateGramPosition(), command);
      x.addMessage(newMesgs);
      throw;
    }
}

  int msStateGramParseCommand (const MeasurementSet* ms, const String& command, Vector<Int>& selectedIDs)
			       
{
  try 
    {
      Int ret;
      MSStateGramrestart (MSStateGramin);
      yy_start = 1;
      strpMSStateGram = command.chars();     // get pointer to command string
      posMSStateGram  = 0;                   // initialize string position
      MSStateParse parser(ms);               // setup measurement set
      MSStateParse::thisMSSIParser = &parser; // The global pointer to the parser
      parser.reset();
      ret=MSStateGramparse();                // parse command string
      
      selectedIDs=parser.selectedIDs();
      return ret;
    }
  catch (MSSelectionStateError &x)
    {
      String newMesgs;
      newMesgs = constructMessage(msStateGramPosition(), command);
      x.addMessage(newMesgs);
      throw;
    }
}

//# Give the table expression node
const TableExprNode* msStateGramParseNode()
{
  return MSStateParse::node();
}
void msStateGramParseDeleteNode() {MSStateParse::cleanup();}
//# Give the string position.
Int& msStateGramPosition()
{
    return posMSStateGram;
}

//# Get the next input characters for flex.
int msStateGramInput (char* buf, int max_size)
{
    int nr=0;
    while (*strpMSStateGram != 0) {
	if (nr >= max_size) {
	    break;                         // get max. max_size char.
	}
	buf[nr++] = *strpMSStateGram++;
    }
    return nr;
}

void MSStateGramerror (const char*)
{
    throw (MSSelectionStateParseError ("State Expression: Parse error at or near '" +
		      String(MSStateGramtext) + "'"));
}

} //# NAMESPACE CASA - END

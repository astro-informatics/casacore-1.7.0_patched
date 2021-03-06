//# tContainerIO.cc: This program tests Container IO
//# Copyright (C) 2011
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: tContainerIO.cc 21013 2011-01-06 08:35:09Z gervandiepen $

//# Includes

#include <casa/Containers/ContainerIO.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/sstream.h>

using namespace casa;

int main()
{
    Vector<Int> vec2(3);
    indgen(vec2, 1);
    {
      ostringstream oss;
      showContainer (oss, vec2);
      AlwaysAssertExit (oss.str() == "[1,2,3]");
    }
    {
      ostringstream oss;
      showContainer (oss, vec2, ", ");
      AlwaysAssertExit (oss.str() == "[1, 2, 3]");
    }
    {
      ostringstream oss;
      showDataIter (oss, vec2.data(), vec2.data()+vec2.size(), " ", "(", ")");
      AlwaysAssertExit (oss.str() == "(1 2 3)");
    }

    cout << "OK\n";
    return 0;
}

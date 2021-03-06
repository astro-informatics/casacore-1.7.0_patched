//# IncrStManAccessor.cc: Gives access to some IncrementalStMan functions
//# Copyright (C) 1996
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
//# $Id: IncrStManAccessor.cc 21014 2011-01-06 08:57:49Z gervandiepen $

//# Includes
#include <tables/Tables/IncrStManAccessor.h>
#include <tables/Tables/ISMBase.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/DataManError.h>
#include <casa/BasicSL/String.h>


namespace casa { //# NAMESPACE CASA - BEGIN

ROIncrementalStManAccessor::ROIncrementalStManAccessor (const Table& table,
                                                        const String& name,
                                                        Bool byColumn)
  : RODataManAccessor (table, name, byColumn),
    dataManPtr_p (0)
{
    dataManPtr_p = dynamic_cast<ISMBase*>(baseDataManager());
    if (dataManPtr_p == 0) {
	throw (DataManError ("ROIncrementalStManAccessor " + name +
                             " constructed for data manager type "
			     + baseDataManager()->dataManagerType() +
			     "; expected IncrementalStMan"));
    }
}

ROIncrementalStManAccessor::~ROIncrementalStManAccessor()
{}

ROIncrementalStManAccessor::ROIncrementalStManAccessor
                               (const ROIncrementalStManAccessor& that)
: RODataManAccessor(that),
  dataManPtr_p (that.dataManPtr_p)
{}

ROIncrementalStManAccessor& ROIncrementalStManAccessor::operator=
	                       (const ROIncrementalStManAccessor& that)
{
    dataManPtr_p = that.dataManPtr_p;
    return *this;
}


void ROIncrementalStManAccessor::setCacheSize (uInt size,
                                               Bool canExceedNrBuckets)
{
    dataManPtr_p->setCacheSize (size, canExceedNrBuckets);
}
uInt ROIncrementalStManAccessor::cacheSize() const
{
    return dataManPtr_p->cacheSize();
}

void ROIncrementalStManAccessor::clearCache()
{
    dataManPtr_p->clearCache();
}

void ROIncrementalStManAccessor::showCacheStatistics (ostream& os) const
{
    dataManPtr_p->showCacheStatistics (os);
}

} //# NAMESPACE CASA - END


//# HyperPlaneParam.cc: Parameters for a hyperplane function
//# Copyright (C) 2001,2002,2004
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
//# $Id: ConstantNDParam.tcc 21209 2012-03-23 06:58:05Z gervandiepen $

//# Includes
#include <scimath/Functionals/ConstantNDParam.h>

namespace casa { //# NAMESPACE CASA - BEGIN

template<class T>
ConstantNDParam<T>::ConstantNDParam(uInt m) :
  Function<T>(1), _ndim(m) {}

template<class T>
ConstantNDParam<T>::ConstantNDParam(const ConstantNDParam<T> &other) :
  Function<T>(other), _ndim(other._ndim) {}

template<class T>
ConstantNDParam<T>::~ConstantNDParam() {}

template<class T>
ConstantNDParam<T> &
ConstantNDParam<T>::operator=(const ConstantNDParam<T> &other) {
  if (this != &other) {
	  Function<T>::operator=(other);
	  _ndim = other._ndim;
  }
  return *this;
}

} //# NAMESPACE CASA - END


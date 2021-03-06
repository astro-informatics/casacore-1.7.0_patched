//# tMutex.cc: Test program for class Mutex
//# Copyright (C) 2011
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
//# $Id: tMutex.cc 21266 2012-09-14 09:43:30Z gervandiepen $

#include <casa/OS/Mutex.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions.h>
#include <casa/iostream.h>

using namespace casa;

// <summary>
// Test program for class File.
// </summary>

void testErrorCheck()
{
  cout << "Test ERRORCHECK ..." << endl;
  Mutex mutex(Mutex::ErrorCheck);
  Bool caught = True;
  // Unlock without a lock should fail.
  try {
    mutex.unlock();
    caught = False;
  } catch (AipsError& x) {
    cout << x.what() << endl;
  }
  AlwaysAssertExit (caught);
  // First lock should succeed.
  mutex.lock();
  // A trylock should fail.
  AlwaysAssertExit (! mutex.trylock());
  // Next lock should fail.
  try {
    mutex.lock();
    caught = False;
  } catch (AipsError& x) {
    cout << x.what() << endl;
  }
  AlwaysAssertExit (caught);
  // Unlock.
  mutex.unlock();
  // A trylock should succeed.
  AlwaysAssertExit (mutex.trylock());
  mutex.unlock();
  // Next unlock should fail.
  try {
    mutex.unlock();
    caught = False;
  } catch (AipsError& x) {
    cout << x.what() << endl;
  }
  AlwaysAssertExit (caught);
  // Lock should be fine here.
  mutex.lock();
  mutex.unlock();
}

void testRecursive()
{
  cout << "Test RECURSIVE ..." << endl;
  Mutex mutex(Mutex::Recursive);
  Bool caught = True;
  // Unlock without a lock should fail.
  try {
    mutex.unlock();
    caught = False;
  } catch (AipsError& x) {
    cout << x.what() << endl;
  }
  AlwaysAssertExit (caught);
  // First lock should succeed.
  mutex.lock();
  // A trylock should succeed.
  AlwaysAssertExit (mutex.trylock());
  // Next lock should succeed.
  mutex.lock();
  // Unlock.
  mutex.unlock();
  // A trylock should succeed.
  AlwaysAssertExit (mutex.trylock());
  mutex.unlock();
  mutex.unlock();
  mutex.unlock();
  // Unlock without a lock should fail.
  try {
    mutex.unlock();
    caught = False;
  } catch (AipsError& x) {
    cout << x.what() << endl;
  }
  AlwaysAssertExit (caught);
  // Lock should be fine here.
  mutex.lock();
  mutex.unlock();
}

void testNormal()
{
  cout << "Test NORMAL ..." << endl;
  Mutex mutex(Mutex::Normal);
  // Usually an an unlock does not fail.
  try {
    mutex.unlock();
  } catch (AipsError& x) {
    cout << x.what() << endl;
  }
  cout << 'a'<<endl;
  // First lock should succeed.
  mutex.lock();
  cout << 'b'<<endl;
  // Doing another lock results in a deadlock, so we don't do that.
  // A trylock should fail.
  AlwaysAssertExit (! mutex.trylock());
  cout << 'v'<<endl;
  // Unlock.
  mutex.unlock();
  cout << 'd'<<endl;
  // A trylock should succeed.
  AlwaysAssertExit (mutex.trylock());
  cout << 'e'<<endl;
  mutex.unlock();
  cout << 'f'<<endl;
  // Lock should be fine here.
  mutex.lock();
  cout << 'g'<<endl;
  mutex.unlock();
  cout << 'h'<<endl;
}

void testMutexedInitFunc (void* arg)
{
  int* count = static_cast<int*>(arg);
  (*count)++;
}

// Test serially.
void testMutexedInitSerial()
{
  cout <<"Test serial init ..." << endl;
  int count=0;
  MutexedInit safeInit (testMutexedInitFunc, &count);
  for (int i=0; i<16; ++i) {
    safeInit.exec();
  }
  AlwaysAssertExit (count==1);
}

// Test parallel.
void testMutexedInitParallel()
{
  cout<< "Test parallel init ..." << endl;
  int count=0;
  MutexedInit safeInit (testMutexedInitFunc, &count);
#pragma omp parallel for
  for (int i=0; i<16; ++i) {
    cout<<i<<endl;
    safeInit.exec();
  }
  AlwaysAssertExit (count==1);
}


int main()
{
  try {
    testMutexedInitSerial();
#ifdef USE_THREADS
    testErrorCheck();
    testRecursive();
    try {
      testNormal();
    } catch (AipsError& x) {
      cout << "testNormal should succeed, but got exception " << x.what() << endl;
    }
    testMutexedInitParallel();
#endif
  } catch (AipsError& x) {
    cout << "Caught an exception: " << x.getMesg() << endl;
    return 1;
  } 
  cout << "OK" << endl;
  return 0;                           // exit with success status
}

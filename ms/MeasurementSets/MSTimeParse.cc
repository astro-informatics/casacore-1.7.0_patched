//# MSTimeParse.cc: Classes to hold results from time grammar parser
//# Copyright (C) 1994,1995,1997,1998,1999,2000,2001,2003
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
//# $Id: MSTimeParse.cc 20652 2009-07-06 05:04:32Z Malte.Marquarding $

#include <ms/MeasurementSets/MSTimeParse.h>
#include <ms/MeasurementSets/MSMainColumns.h>
#include <casa/Quanta/MVTime.h>
#include <measures/Measures.h>
#include <measures/Measures/MEpoch.h>
#include <ms/MeasurementSets/MSSelectionError.h>
#include <casa/BasicSL/String.h>
#include <limits>

namespace casa { //# NAMESPACE CASA - BEGIN
  
  MSTimeParse*     MSTimeParse::thisMSTParser = 0x0; // Global pointer to the parser object
  TableExprNode*   MSTimeParse::node_p      = 0x0;
  MEpoch*          MSTimeParse::yeartime    = 0x0;
  MEpoch*          MSTimeParse::daytime     = 0x0;
  //  MSTimeParse      *thisMSTParser           = 0x0;
  MeasurementSet*  MSTimeParse::ms_p        = 0x0;
  TableExprNode*   MSTimeParse::otherTens_p = 0x0;
  Bool             MSTimeParse::defaultTimeComputed=False;
  Matrix<Double>   MSTimeParse::timeList(2,0);

  //-------------------------------------------------------------------  
  // Constructor
  //
  MSTimeParse::MSTimeParse ()
    : MSParse(), colName(MS::columnName(MS::TIME))
  {
    defaultYear = defaultMonth = defaultDay = defaultHour 
      = defaultMinute = defaultSeconds = defaultFractionalSec 
      = -1;
    defaultExposure = -100;
    if(node_p) delete node_p;
    node_p = new TableExprNode();
    ms_p= 0x0;
    otherTens_p=0x0;
  }
  //-------------------------------------------------------------------
  // Constructor with given ms name.
  //
  MSTimeParse::MSTimeParse (const MeasurementSet* ms, const TableExprNode& otherTens,
			    const Bool honourRowFlags)
    : MSParse(ms, "Time"), colName(MS::columnName(MS::TIME)), 
      honourRowFlags_p(honourRowFlags)
  {
    if(node_p) delete node_p;
    ms_p= (MeasurementSet*)ms;
    node_p = new TableExprNode();
    otherTens_p=(TableExprNode *)&otherTens;
    /*
    MVTime time(msCols.timeQuant()(i));
    Time t0(time.getTime());

    defaultYear = time.year();
    defaultMonth = time.month();
    defaultDay = time.monthday();
    defaultHour = t0.hours();
    defaultMinute = t0.minutes();
    defaultSeconds = t0.seconds();
    Time t1(defaultYear,defaultMonth,defaultDay,defaultHour,defaultMinute,defaultSeconds);
    defaultFractionalSec = (Int)((t0-t1)*1E3);
    */
  }
  void  MSTimeParse::getDefaults()
  {
    uInt firstLogicalRow=0; // This is the logical first row
    ROMSMainColumns msCols(*ms_p);
    if (!defaultTimeComputed)
      {
	uInt i=0,nrow=msCols.flagRow().nrow();
	if (!otherTens_p->isNull())
	  {
	    Bool selected=False;
	    for(i=0;i<nrow;i++)
	      {
		// Use the otherTens_p to get to the first logical row
		if (honourRowFlags_p)
		  {
		    if (!msCols.flagRow()(i))
		      otherTens_p->get(i,selected); 
		  }
		else
		  otherTens_p->get(i,selected);
		  
		if (selected) {firstLogicalRow=i;break;}
	      }
	  }
	else if (honourRowFlags_p)
	  {
	    //
	    // Find the first row which is not flagged.
	    //
	    for (i=0;i<nrow;i++) if (!msCols.flagRow()(i)) {firstLogicalRow=i;break;}
	  }
	if ( firstLogicalRow >= nrow)
	  throw(MSSelectionTimeError("MSTimeParse: No logical \"row zero\" found for time selection"));
      }
    //
    // Extract the values from the first valid timestamp in the MS to
    // be used as defaults
    //
    //
    // Get the exposure in seconds.
    //
    defaultExposure=msCols.exposureQuant()(firstLogicalRow,"s").getValue();
    firstRowTime = msCols.timeQuant()(firstLogicalRow);

    //    cout << firstRowTime.string(MVTime::DMY,7) << endl;

    Time t0(firstRowTime.getTime());

    defaultYear    = firstRowTime.year();
    defaultMonth   = firstRowTime.month();
    defaultDay     = firstRowTime.monthday();
    defaultHour    = t0.hours();
    defaultMinute  = t0.minutes();
    defaultSeconds = t0.seconds();
    Time t1(defaultYear,defaultMonth,defaultDay,defaultHour,defaultMinute,defaultSeconds);
    defaultFractionalSec = (Int)((t0-t1)*1E3);

    defaultTimeComputed=True;
  }
  //
  //-------------------------------------------------------------------
  //  
  const TableExprNode *MSTimeParse::addCondition(TableExprNode& condition)
  {
    if(node_p->isNull())
      *node_p = condition;
    else
      *node_p = *node_p || condition;
    
    return node_p;
  }
  //
  //-------------------------------------------------------------------
  //  
  const TableExprNode *MSTimeParse::selectTime(const MEpoch& time, bool)
  {

    Double timeInSec= toTAIInSec(time);
    Double dT= MSTimeParse::thisMSTParser->defaultExposure/2.0;

    TableExprNode condition = (abs(ms()->col(colName) - timeInSec) <= dT);
    accumulateTimeList(timeInSec,timeInSec);

    return addCondition(condition);
  }
  //
  //-------------------------------------------------------------------
  //  
  const TableExprNode *MSTimeParse::selectTimeGT(const MEpoch& lowboundTime,
						 bool)
  {
    Double timeInSec = toTAIInSec(lowboundTime);
    TableExprNode condition = (ms()->col(colName) >= timeInSec);
    accumulateTimeList(timeInSec,std::numeric_limits<Double>::max());

    return addCondition(condition);
  }
  //
  //-------------------------------------------------------------------
  //  
  const TableExprNode *MSTimeParse::selectTimeLT(const MEpoch& upboundTime,
						 bool)
  {
    Double timeInSec = toTAIInSec(upboundTime);
    TableExprNode condition = (ms()->col(colName) <= timeInSec);
    accumulateTimeList(0.0, timeInSec);


    return addCondition(condition);
  }
  //
  //-------------------------------------------------------------------
  //  
  const TableExprNode *MSTimeParse::selectTimeRange(const MEpoch& lowboundTime,
						    const MEpoch& upboundTime,
						    bool)
  {
    Double upperBound = toTAIInSec(upboundTime);
    Double lowerBound = toTAIInSec(lowboundTime);

    if (lowerBound > upperBound)
      throw(MSSelectionTimeError("lower bound > upper bound"));

    TableExprNode condition = (ms()->col(colName) >= lowerBound &&
 			       (ms()->col(colName) <= upperBound));
    accumulateTimeList(lowerBound, upperBound);

    return addCondition(condition);
  }
  //
  //-------------------------------------------------------------------
  //  
  /*
  const MEpoch *MSTimeParse::dayTimeConvert(Int day, Int hour, Int minute,
					    Int second, Int millisec)
  {
    if(daytime) delete daytime;
    
    if (day == -1) day=MSTimeParse().day0();
    if (hour == -1) hour=MSTimeParse().hour0();
    if (minute == -1) minute=MSTimeParse().minute0();
    if (second == -1) second=MSTimeParse().second0();
    if (millisec == -1) millisec = MSTimeParse().fractionalsec0();
    Double s = Double(second) + Double(millisec)/1000.0;
    Time t(0, 0, day, hour, minute, s);
    
    MVEpoch mv(t.modifiedJulianDay());
    
    //    return (daytime = new MEpoch(mv, MEpoch::UTC));
    return new MEpoch(mv, MEpoch::UTC);
  }
  */
  //
  //-------------------------------------------------------------------
  //  
  const MEpoch *MSTimeParse::yearTimeConvert(Int year, Int month, Int day,
					     Int hour, Int minute,
					     Int second, Int millisec)
  {
    if(yeartime) delete yeartime;
    
    Double s = Double(second) + Double(millisec)/1000.0;
    Time t(year, month, day, hour, minute, s);
    
    MVEpoch mv(t.modifiedJulianDay());
    
    return new MEpoch(mv, MEpoch::UTC);
  }
  //
  //-------------------------------------------------------------------
  //  
  const MEpoch *MSTimeParse::yearTimeConvert(const TimeFields& tf)
  {
    MSTimeParse::thisMSTParser->validate(tf);
    if(yeartime) delete yeartime;
    
    Double s = Double(tf.sec) + Double(tf.fsec)/1000.0;
    Time t(tf.year, tf.month, tf.day, tf.hour, tf.minute, s);
    
    MVEpoch mv(t.modifiedJulianDay());
    
    // This is the "original" code (from DdeB etc.)
    // yeartime is a global - don't know what's it's use and where it
    // is used. It's a pointer, which is returned on the parser stack.
    // In this method, it's first deleted, and re-assigned.  This will
    // do (and does!) strange things to the YY V-stack.
    // 
    //    return (yeartime = new MEpoch(mv, MEpoch::UTC));
    return new MEpoch(mv, MEpoch::UTC);
  }
  //
  //-------------------------------------------------------------------
  //  
  Double MSTimeParse::toTAIInSec(const MEpoch& whatEver)
  {
    //    MEpoch tai=MEpoch::Convert(whatEver,MEpoch::Ref(MEpoch::TAI))();
    MEpoch tai=whatEver;
    return Double(MVTime(tai.getValue())*86400);
  }
  //
  //-------------------------------------------------------------------
  //  
  const TableExprNode* MSTimeParse::node()
  {
    return node_p;
  }
  //
  //-------------------------------------------------------------------
  //  
  void MSTimeParse::setDefaults(TimeFields& tf, Bool dataOrigin)
  {
    if (dataOrigin)
      {
	MSTimeParse::thisMSTParser->getDefaults();
	if (tf.year == -1)     tf.year=MSTimeParse::thisMSTParser->year0();//MSTimeParse().year0();
	if (tf.month == -1)    tf.month=MSTimeParse::thisMSTParser->month0();
	if (tf.day == -1)      tf.day=MSTimeParse::thisMSTParser->day0();
	if (tf.hour == -1)     tf.hour=MSTimeParse::thisMSTParser->hour0();
	if (tf.minute == -1)   tf.minute=MSTimeParse::thisMSTParser->minute0();
	if (tf.sec == -1)      tf.sec=MSTimeParse::thisMSTParser->second0();
	if (tf.fsec == -1)     tf.fsec = MSTimeParse::thisMSTParser->fractionalsec0();
      }
    else
      {
	// This sets the defaults to MJD reference. This should be got
	// from one of the time related classes.
	//  MJDref = Wed Nov 17 00:00:00 1858
	Time time00(2400000.5);

	if (tf.year == -1)     tf.year=time00.year();//1858;
	if (tf.month == -1)    tf.month=time00.month();//11;
	if (tf.day == -1)      tf.day=time00.dayOfMonth();//17;
	if (tf.hour == -1)     tf.hour=0;
	if (tf.minute == -1)   tf.minute=0;
	if (tf.sec == -1)      tf.sec=0;
	if (tf.fsec == -1)     tf.fsec = 0;
      }
    //    cout << tf.year << " " << tf.month << " " << tf.day << " " << endl;
  }
  //
  //-------------------------------------------------------------------
  //  
  void MSTimeParse::validate(const TimeFields& tf)
  {
    if (tf.year < 1858) // This is not precise (should be < Wed Nov 17 00:00:00 1858)
      {                                       
	ostringstream mesg;
	mesg << "MSTime Selection error: Year = " << tf.year << " out of range";
	//	throw(MSSelectionTimeError(mesg.str()));
	throw(AipsError(mesg.str()));
      }
    if ((tf.month <= 0) || (tf.month > 12)) 
      {
	ostringstream mesg;
	mesg << "MSTime Selection error: Month = " << tf.month << " out of range";
	//	throw(MSSelectionTimeError(mesg.str()));
	throw(AipsError(mesg.str()));
      }
    if ((tf.day <= 0) || (tf.day > 31)) 
      {
	ostringstream mesg;
	mesg << "MSTime Selection error: Day = " << tf.day << " out of range";
	//	throw(MSSelectionTimeError(mesg.str()));
	throw(AipsError(mesg.str()));
      }
  }
  //
  //-------------------------------------------------------------------
  //  
  void MSTimeParse::copyDefaults(TimeFields& target, TimeFields& source)
  {
    if (target.year   == -1) target.year   = source.year;
    if (target.month  == -1) target.month  = source.month;
    if (target.day    == -1) target.day    = source.day;
    if (target.hour   == -1) target.hour   = source.hour;
    if (target.minute == -1) target.minute = source.minute;
    if (target.sec    == -1) target.sec    = source.sec;
    if (target.fsec   == -1) target.fsec   = source.fsec;
  }
  //
  //-------------------------------------------------------------------
  //  
  void MSTimeParse::accumulateTimeList(const Double t0, const Double t1)
  {
    Int n0=timeList.shape()(1);
    IPosition newShape(timeList.shape());
    newShape(1)++;
    timeList.resize(newShape,True);
    timeList(0,n0) = t0;//-4.68193e+09;
    timeList(1,n0) = t1;//-4.68193e+09;
  }
} //# NAMESPACE CASA - END

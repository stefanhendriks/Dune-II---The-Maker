

#pragma once

//#pragma message ("Source Profiler -- 1.1 NT/HWLib")


/* *************************************************************************
                                                                            
Source Profiler -- Copyright (C) 1995, 1996, 1997, 1998 - Harald Hoyer
-----------------------------------------------------

Strategy: 

With macros BEGIN_PROF and END_PROF you define regions of source code that 
build an entry of profiler table. At run time for such a region each call is 
counted and time elapsed is accumulated.
All data is stored in a global variable called Profile.
With member function dumpprint the results can be printed into Microsofts dump 
context at any time.
Time measurement is done by use of best timer available (1.12 MHz, about 900 nsec)

---

Usage:
                    
Include:	HWPROF.H
	Include this file in each source file you want to monitor and wherein 
	you want to put the Profile.dumpprint call.

Source:		HWPROF.CPP
    Include this file in your project list or make file.
    
---

Functions / Macros:

		BEGIN_PROF( <FlagText> );

Defines a start point of a region to monitor. HWPROF uses file name and line 
number as identification for accumulation. The parameter FlagText is an 
additional string that will be also presented in profile dump and serves as 
a comment. If at run time a region is aready active, its measurements are
stopped and the values are saved. 


		END_PROF();

Defines the ending point of a region to monitor. Each END_PROF must have
a corresponding BEGIN_PROF call. Association is done dynamically. After END_PROF
the former region is resumed, if there is one.

		
		NEXT_PROF( <FlagText> );

Ends up the current region and starts a new one.


		FUNC_PROF(expr)

Creates a profile region that suround the evaluation of expr. The expression itself
is used as flagtext.


		STMT_PROF(expr)

Same as FUNC_PROF, use it for void-type expressions.
	
	  
		Profile.dumpprint(int Hide = 10);

Dump accumulated values into MFCs dump context. The output format is:

  <FileName>(<LineNr>): [<SeqNo>]:	<CallCount>	<TimePerCall>	<ElapsedTime>	<FlagText>

For each BEGIN_PROF one line such line is generated. The lines are sorted by 
elapsed time. With MSVC you can use the F4-key ("Goto Error/Tag") on these lines.
The parameter Hide is used to restrict the amount of lines. If it is negative 
it determines the lines to print (-1 -> first line only, -2 -> first two 
lines and so on). If it is zero no restriction applies. If Hide is positive
the last lines that all together consume elapsed time of not more than 1/Hide
of all are hidden (1 -> no output, 2 -> one half is shown, 3 -> 2/3 is shown 
10 -> 90% is shown).

		
		Profile.reset();

Clear counters of all regions and retain the memory used. This function may
only be used when no region is active.

		
		#define PROFILE <Instance of CHWProfile>

Use this define before include of HWPROF.H to enable measurement and 
to provide the profile variable to use. The thing provided here should be a pointer 
to CHWProfile		

		
		#undef PROFILE
		
Use this before include of HWPROF.H to disable measurement (this is the default).

To avoid confusion, use #define / #undef of PROFILE only in CPP-Files and never 
in Headers.

************************************************************************* */


class CHWProfileEntry;
class CHWProfileStack;

class CHWProfile
{
	CHWProfileEntry* First;
	CHWProfileStack* Current;

	CHWProfileEntry* FindStart(const char*, int) const;
public:
	CHWProfile();

	void Start(const char*FileName,int Line, const char*Flag);
	void End();

	virtual void dumpprint(int Hide = 10)const;
	virtual void reset();
};


#ifndef PROFILE
#	define BEGIN_PROF(Flag)
#	define FUNC_PROF(expr) (expr)
#	define STMT_PROF(expr) (expr)
#	define NEXT_PROF(Flag)
#	define END_PROF()
#else
#	define BEGIN_PROF(Flag) if(PROFILE) (PROFILE)->Start(__FILE__, __LINE__,Flag )
#	define FUNC_PROF(expr) ((PROFILE)->Start(__FILE__, __LINE__,#expr ), _ReturnProfile(expr))
#	define STMT_PROF(expr) ((PROFILE)->Start(__FILE__, __LINE__,#expr ), expr, (PROFILE)->End())
#	define END_PROF() if(PROFILE) (PROFILE)->End()
#	define NEXT_PROF(Flag) END_PROF();BEGIN_PROF(Flag)
#endif


#ifndef NO_TEMPLATES
template<class cType>
cType _ReturnProfile(cType x)
{
	(PROFILE)->End();
	return x;
};
#endif




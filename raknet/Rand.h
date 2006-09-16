/// \file
/// \brief \b [Internal] Random number generator
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of Raknet is subject to the appropriate licence agreement.
/// "Shareware" Licensees with Rakkarsoft LLC are subject to the
/// shareware license found at
/// http://www.rakkarsoft.com/shareWareLicense.html
/// "Commercial" Licensees are subject to the commercial license found at
/// http://www.rakkarsoft.com/sourceCodeLicense.html
/// Custom license users are subject to the terms therein.
/// All other users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.


#ifndef __RAND_H
#define __RAND_H 

 /// Initialise seed for Random Generator
 /// \param[in] seed The seed value for the random number generator.
extern void seedMT( unsigned int seed );

/// \internal
extern unsigned int reloadMT( void );

/// Gets a random unsigned int
/// \return an integer random value.
extern unsigned int randomMT( void );

/// Gets a random float
/// \return 0 to 1.0f, inclusive
extern float frandomMT( void );

#endif


#pragma once

#include <tdme/utils/fwd-tdme.h>

/**
 * Character class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::utils::Character final
{
public:

	/**
	 * Convert character to lower case character
	 * @param character character
	 * @return lower case character
	 */
	static char toLowerCase(char character);

	/**
	 * Convert character to upper case character
	 * @param character character
	 * @return upper case character
	 */
	static char toUpperCase(char character);

	/**
	 * Returns if character is alpha numeric
	 * @return if character is alpha numeric
	 */
	static bool isAlphaNumeric(char character);

};

/*
||
|| @file Password.cpp
|| @version 1.2
|| @author Alexander Brevig
|| @contact alexanderbrevig@gmail.com
||
||  4/5/2012 Updates Nathan Sobieck: Nathan@Sobisource.com
||   Now v1.2 Arduino IDE v1.0 With BAckwards compatibility
||
|| @description
|| | Handle passwords easily
|| #
||
|| @license
|| | This library is free software; you can redistribute it and/or
|| | modify it under the terms of the GNU Lesser General Public
|| | License as published by the Free Software Foundation; version
|| | 2.1 of the License.
|| |
|| | This library is distributed in the hope that it will be useful,
|| | but WITHOUT ANY WARRANTY; without even the implied warranty of
|| | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
|| | Lesser General Public License for more details.
|| |
|| | You should have received a copy of the GNU Lesser General Public
|| | License along with this library; if not, write to the Free Software
|| | Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
|| #
||
*/

#include "Password.h"

#include <string.h>
#include <avr/io.h>


//construct object in memory, set all variables
Password::Password(char* pass){
    for(int i = 0; i < LAST_PASSWORD_NUM; i++)
    {
        target[i] = new char[MAX_PASSWORD_LENGTH];
        memset(target[i], 0, MAX_PASSWORD_LENGTH);
    }
	set( pass );
	reset();
}

//set the password
void Password::set(char* pass){
    for(int i = 0;i < LAST_PASSWORD_NUM-1; i++)
    {
        memcpy(target[i], target[i+1], MAX_PASSWORD_LENGTH);
    }
	memcpy(target[LAST_PASSWORD_NUM-1], pass, 6);

   // target[0] = pass;
}

bool Password::showPasswd(int i, char * string)
{
    memcpy(string, target[i], MAX_PASSWORD_LENGTH);
    //memcpy(string+7, guess, 7);
    //string = target[0];
}

bool Password::showguess(char *guesswd)
{
    memcpy(guesswd, guess, MAX_PASSWORD_LENGTH);
}

//evaluate a string, is it equal to the password?
bool Password::is(char* pass){ 
	byte i=0;
	while (*pass && i<MAX_PASSWORD_LENGTH){
		guess[i] = pass[i];
		i++;
	}
	return evaluate();
}

//append a char to the guessed password
bool Password::append(char character){ 
	if (currentIndex+1==MAX_PASSWORD_LENGTH){
		return false;
	}else{
		guess[currentIndex++] = character;
		guess[currentIndex] = STRING_TERMINATOR; //ensure a valid c string
	}
	return true;
}

//reset the guessed password, one can guess again
void Password::reset(){ 
	currentIndex = 0;
	guess[currentIndex] = STRING_TERMINATOR;
}

//is the current guessed password equal to the target password?
int Password::evaluate(){
    byte init = 0;

    for(byte j = 0; j< LAST_PASSWORD_NUM; j++)
    {
        // init code 000000
        for(byte i=1; i<MAX_PASSWORD_LENGTH;i++)
        {
            if(target[j][i]==0 && i==MAX_PASSWORD_LENGTH-1) 
            {
                init = 1;
            }
            else
            {
                break;
            }
        }
        if(init == 1)
        {// this is init code 000000, abandon
            init = 0;
            continue;
        }

    	char pass = target[j][0];
	char guessed = guess[0];

	for (byte i=1; i<MAX_PASSWORD_LENGTH; i++){
		
		//check if guessed char is equal to the password char
		if (pass==STRING_TERMINATOR && guessed==STRING_TERMINATOR){
			return true; //both strings ended and all previous characters are equal 
		}else if (pass!=guessed || pass==STRING_TERMINATOR || guessed==STRING_TERMINATOR){
    			break; //difference OR end of string has been reached
		}
		
		//read next char
    		pass = target[j][i];
		guessed = guess[i];
	}
    }
    
	return false; //a 'true' condition has not been met
}

//set password using operator =
Password &Password::operator=(char* pass){
	set( pass );
	return *this;
}

//test password using ==
bool Password::operator==(char* pass){
	return is( pass );
}

//test password using !=
bool Password::operator!=(char* pass){
	return !is( pass );
}

//append to currently guessed password using operator <<
Password &Password::operator<<(char character){
	append( character );
	return *this;
}

/*
|| @changelog
|| | 2009-06-17 - Alexander Brevig : Added assignment operator =, equality operators == != and insertion operator <<
|| | 2009-06-17 - Alexander Brevig : Initial Release
|| #
*/
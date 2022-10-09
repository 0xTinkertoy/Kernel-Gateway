//
//  Console.cpp
//  Kernel
//
//  Created by FireWolf on 1/27/21.
//

#include "PL011.hpp"
#include "Print.h"

/**
 * Output a character to a custom device like UART, used by the printf() function
 * This function is declared here only. You have to write your custom implementation somewhere
 * \param character Character to output
 */
void _putchar(char character)
{
    PL011::send(PL011::kUART0, character);
}

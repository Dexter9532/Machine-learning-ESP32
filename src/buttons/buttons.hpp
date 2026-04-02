/**
 *@brief Header file for buttons functions.
 *
 *@note Use init before using buttons.
 */
#pragma once

//! @note Jag ser att detta i praktiken är ett C-API och som sådant ser det klockrent ut.
//!       Men eftersom filen har ändelsen .hpp kommentarer jag på saker som borde ändras för att
//!       följa C++-praxis.

//! @note Inkludera som <cstdint> när ni använder C++.
//!       Jag förstår att stödet för C++ standardbibliotek kan vara begränsat, så OK.
#include <stdint.h>

//----------------------------------------------------------------------------------------------------
//! @note Ni kan skippa (void) i parenteser i C++; i C är det bra (för annars "kan" man skicka med ett
//!       ingående argument utan att kompilatorn gnäller), men i C++ är det totalt onödigt, då kompilatorn
//!       är strängare - () i C++ medför alltså att man inte kan skicka argument.
//!
//!       Ni kan också markera dessa funktioner noexcept, så länge ni inte kastar ett undantag.
//!       I ett inbyggt system låter inte detta särskilt troligt. 
void buttons_init_irq(void);

//----------------------------------------------------------------------------------------------------
uint8_t button0_get(void);

//----------------------------------------------------------------------------------------------------
uint8_t button1_get(void);

//----------------------------------------------------------------------------------------------------
uint8_t button2_get(void);

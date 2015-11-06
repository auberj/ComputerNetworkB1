//Displays time since turn on on the Il Matto Display in ms

#include <stdio.h>
#include "lcd/avrlcd.h"
#include "lcd/font.c"
#include "lcd/ili934x.c"
#include "lcd/lcd.c"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "transport/transport.cpp"

int main()
{
    init_lcd();
    set_orientation(East);
    display_string("Initialising...\n");
    char message[] = "Bacon short loin, kielbasa porchetta pancetta tongue tail venison. Tongue alcatra rump chicken pork belly. Turducken bresaola jerky drumstick short loin pork chop beef ribs ball tip turkey. Beef spare ribs kevin landjaeger alcatra. Pancetta tenderloin hamburger, short ribs jowl fatback beef ribs pork loin venison. Cow pig shank, capicola t-bone beef ribs short ribs rump. Tail jowl cow shankle bacon, sirloin picanha frankfurter porchetta filet mignon bresaola pork cupim shoulder. Turkey meatloaf ground round, bresaola capicola kielbasa tenderloin drumstick spare ribs rump meatball. Meatloaf brisket bresaola, shankle capicola tail pancetta filet mignon. Salami ham shankle kevin biltong, bresaola cow turkey kielbasa tri-tip pastrami. Pork belly bresaola flank, sausage pork loin tenderloin shoulder bacon pork chicken beef ribs filet mignon. Picanha shankle ribeye hamburger sirloin biltong beef ribs pork chop short loin porchetta strip steak shank spare ribs, ham hock t-bone. Kielbasa rump brisket sausage, pancetta turducken chicken filet mignon landjaeger tenderloin pork belly shoulder porchetta ball tip frankfurter. Frankfurter tenderloin alcatra spare ribs venison filet mignon jowl flank prosciutto strip steak ribeye shank turkey pork tongue. Corned beef fatback meatloaf cow short ribs biltong turkey beef pancetta drumstick strip steak kevin chuck doner. Ham hock tongue sausage, leberkas cow turducken jowl pancetta tail beef ribs shankle capicola. Boudin pig brisket tri-tip pork loin beef, ball tip chuck pancetta swine kevin corned beef shankle shoulder. Strip steak hamburger cow bacon corned beef salami beef ribs tri-tip.";
    senddata(message);
    while(1){}
    return 0;
}
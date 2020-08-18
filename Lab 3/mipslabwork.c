#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

#define period 0x7a12 // ((80 000 000) / 256(prescale) / 10(var 100:e ms))
//#define period ((80000000) / 256 / 100) //verify every second
// Vi sätter värdet på prescale till 1:256
// --> timer2 har 2 countregister som har 8-bits storlek --> 256 kombinationer (0-255)

int mytime = 0x5957;
volatile int *portE = 0xbf886110;
int timeoutcount = 0;

char textstring[] = "text, more text, and even more text!";

/* Interrupt Service Routine */
void user_isr( void )
{
  return;
}

/* Lab-specific initialization goes here */
void labinit( void )
{
	volatile int *trise	= (volatile int *) 0xbf886100;
	*trise = *trise & 0xfffffff00; //för att ändra en PORTE pin till en output pin så måste bitarna i trise registret clearas till 0
	//clearer 0-7, sätter till 0=outputs

  TRISD |= 0xfe0; // kopierar vilka LEDs som ska lysa genom att "kopiera" de 1:or som indikerar att LED ska vara på

  T2CON = 0x70; // 111 (7)
  PR2 = period;
  TMR2 = 0x0;   // Clear time register enligt Reference Manual
  T2CONSET = 0x8000; // enabling timer bit ON (bit 15 = 1), start timer

}

/* This function is called repetitively from the main program */
void labwork( void )
{
  int knapp = getbtns();
  int spak = getsw();

  if (knapp & 0x1) {    //går in i if om det är = 1
    mytime = mytime & 0xFF0F;
    mytime = mytime | (spak << 4);
  }

  if (knapp & 0x2) {    //går in i if om det är = 1
    mytime = mytime & 0xF0FF;
    mytime = mytime | (spak << 8);
  }

  if (knapp & 0x4) {    //går in i if om det är = 1
    mytime = mytime & 0x0FFF;
    mytime = mytime | (spak << 12);
  }

//  delay( 1000 );

//IFS(0) = (0x81030<<4); 9onde biten av the FLAGS register (kan settas eller clearas)

if (IFS(0) & 0x100) {
  timeoutcount++;
  // vi maskar ut 9:e biten av interrupt status och kollar om det är en 1:a eller 0:a
  // ökar hela tiden värdet på timeoutcount

  IFSCLR(0) = 0x100;    // clear t2 interrupt flag
  if (timeoutcount == 10) {    // efter att timeoutcount blivit 10 vill vi resetta flaggan
    time2string( textstring, mytime );
    display_string( 3, textstring );
    display_update();
    tick( &mytime );
    display_image(96, icon);

    timeoutcount = 0;    // börja om att räkna till 10 igen
  }

}
  //time2string( textstring, mytime );
  //*portE = mytime; //ökar med mytime
  //display_string( 3, textstring );
  //display_update();
  //tick( &mytime );
  //display_image(96, icon);
}

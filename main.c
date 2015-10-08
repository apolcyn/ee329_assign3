/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//  MSP430G2xx3 Demo - Timer_A, Toggle P1.0, CCR0 Cont. Mode ISR, DCO SMCLK
//
//  Description: Toggle P1.0 using software and TA_0 ISR. Toggles every
//  50000 SMCLK cycles. SMCLK provides clock source for TACLK.
//  During the TA_0 ISR, P1.0 is toggled and 50000 clock cycles are added to
//  CCR0. TA_0 ISR is triggered every 50000 cycles. CPU is normally off and
//  used only during TA_ISR.
//  ACLK = n/a, MCLK = SMCLK = TACLK = default DCO
//
//           MSP430G2xx3
//         ---------------
//     /|\|            XIN|-
//      | |               |
//      --|RST        XOUT|-
//        |               |
//        |           P1.0|-->LED
//
//  D. Dang
//  Texas Instruments Inc.
//  December 2010
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************

#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  P1DIR |= 0x01;                            // P1.0 output
  CCTL0 = CCIE;                             // CCR0 interrupt enabled
  //CCR0 = 50000;
  //TACTL = TASSEL_2 + MC_2;                  // SMCLK, contmode

  if (CALBC1_16MHZ==0xFF) // If calibration constant erased
  {
      while(1); // do not load, trap CPU!!
  }
  DCOCTL = 0; // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ; // Set range
  DCOCTL = CALDCO_1MHZ;
  BCSCTL2 = 0; /* set DIVSx to 0, to divide SMCLK by 1. set SELS to 0, to select DCOCLK for SMCLK.
                    * set DIVMx to 0 to divide MCLK by 1. set SELMCLK to 0 to select DCOCLK for MCLK. */

  // set TASSELxto 2 to select SMCLK, IDx to 0,
  // set MCx to 1 for up mode.
  // set it to compare mode with CAP = 0
  // set TACCR0 to whatever needs to be to get 25kHz

  // set TACTL, timer A control register to set IDx - set divider to 1, MCx, set mode to 1,
  //   TAIE to 1 to enable interrupts on TAIFG, which occur when counter goes back to 0.
  TACTL = TASSEL_2 + MC_1;// + TAIE;
  TACCR0 = 320;

  P1DIR |= BIT4;

  P1SEL2 &= ~BIT0;

  __enable_interrupt();

  //s_BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt
}

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
	int signal;
	P1SEL |= BIT4;

    P1DIR &= ~BIT0;
    signal = P1IN & BIT0;

  if(signal) {
	  P1DIR |= BIT0;
	  P1OUT &= ~BIT0;
	  TACCR0 = 60;
  }
  else {
	  P1DIR |= BIT0;
	  P1OUT |= BIT0;
	  TACCR0 = 60;
  }

  P1SEL &= ~BIT4;
}

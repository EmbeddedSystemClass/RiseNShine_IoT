/**
 * stepper.h
 */

 #pragma once

/* Stepper motor half-step sequence 
 *
 *
 *   State A B C D
 *     1   1 0 0 0
 *     2   1 1 0 0
 *     3   0 1 0 0
 *     4   0 1 1 0
 *     5   0 0 1 0
 *     6   0 0 1 1
 *     7   0 0 0 1
 *     8   1 0 0 1
 *
 */

#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"

class Stepper {
private:
	bool cwDirection = true;
	const int portNumber;
	const char bitA, bitB, bitC, bitD;
	int state = 1;
	int stepsLeft = 0;

	inline void changeState() {
	    //clockwise
	    if (cwDirection == true) {
	        state += 2;
	        if (state >= 8) {
	            state = 1;
	        }
	    }
	    //CCW
	    else if (cwDirection == false) {
	        state -= 2;
	        if (state <= -1) {
	            state = 7;
	        }
	    }
	}



public:
	Stepper(int portNum, int A_port, int B_port, int C_port, int D_port) :
	    portNumber(portNum), bitA(1 << A_port), bitB(1 << B_port), bitC(1 << C_port), bitD(1 << D_port) {}

	void setToClockwiseRotation(bool direction) {
		direction = cwDirection;
	}

	void changeDirection() {
		cwDirection = !cwDirection;
	}

	void moveStepper(int stepsnum) {
		stepsLeft += stepsnum;
	}

	void stopStepper() {
		stepsLeft = 0;
	}

	void applyState() {
		if (stepsLeft != 0) {
	        if (portNumber==1) {
	            switch(state) {
	                case 0:
	                    P1OUT |= bitA;
	                    P1OUT &= ~(bitB + bitC + bitD);
	                    break;
	                case 1:
	                    P1OUT |= bitA + bitB;
	                    P1OUT &= ~(bitC + bitD);
	                    break;
	                case 2:
	                    P1OUT |= bitB;
	                    P1OUT &= ~(bitA + bitC + bitD);
	                    break;
	                case 3:
	                    P1OUT |= bitB + bitC;
	                    P1OUT &= ~(bitA + bitD);
	                    break;
	                case 4:
	                    P1OUT |= bitC;
	                    P1OUT &= ~(bitA + bitB + bitD);
	                    break;
	                case 5:
	                    P1OUT |= bitC + bitD;
	                    P1OUT &= ~(bitA + bitB);
	                    break;
	                case 6:
	                    P1OUT |= bitD;
	                    P1OUT &= ~(bitA + bitB + bitC);
	                    break;
	                case 7:
	                    P1OUT |= bitA + bitD;
	                    P1OUT &= ~(bitB + bitC);
	                    break;
	            }
	        }
	        else {
	            switch(state) {
	                case 0:
	                    P2OUT |= bitA;
	                    P2OUT &= ~(bitB + bitC + bitD);
	                    break;
	                case 1:
	                    P2OUT |= bitA + bitB;
	                    P2OUT &= ~(bitC + bitD);
	                    break;
	                case 2:
	                    P2OUT |= bitB;
	                    P2OUT &= ~(bitA + bitC + bitD);
	                    break;
	                case 3:
	                    P2OUT |= bitB + bitC;
	                    P2OUT &= ~(bitA + bitD);
	                    break;
	                case 4:
	                    P2OUT |= bitC;
	                    P2OUT &= ~(bitA + bitB + bitD);
	                    break;
	                case 5:
	                    P2OUT |= bitC + bitD;
	                    P2OUT &= ~(bitA + bitB);
	                    break;
	                case 6:
	                    P2OUT |= bitD;
	                    P2OUT &= ~(bitA + bitB + bitC);
	                    break;
	                case 7:
	                    P2OUT |= bitA + bitD;
	                    P2OUT &= ~(bitB + bitC);
	                    break;
	            }
	        }
	        stepsLeft--;
	        this->changeState();
	    }
    }

};




 #endif /* stepperRiseNShine_H_ */
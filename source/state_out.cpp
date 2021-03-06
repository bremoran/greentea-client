#include "greentea-client/state_out.h"

#include "mbed-drivers/PortOut.h"
#include "mbed-drivers/DigitalOut.h"
#include "mbed-drivers/DigitalIn.h"
#include "cmsis.h"

#include <stdio.h>
/**
 * @file state_out.cpp
 * @brief report application states via PortOut
 *
 * This implementation of state_out works only for k64f, and requires the following pins:
 * PTC0, PTC1, PTC2, PTC3, PTC4, PTC5, PTC7, PTC8, PTC9, PTD0
 * The PortOut API is used to keep the time used by state_out to a minimum
 */

#ifdef TARGET_LIKE_FRDM_K64F
mbed::PortOut StateOut(PortC,0x1BF);
mbed::DigitalOut StateStrobe(PTC9);
mbed::DigitalIn  StateAck(PTD0);

/**
 * Report an application state
 *
 * Output the state an wait for it to be acknowledged by the state input jig.
 *
 * 1. state_out waits for the state-input jig to indicate it is ready, by pulling the acknowledge signal low.
 * 2. state_out writes the new state using the PortOut API.
 * 3. state_out raises the StateStrobe signal to indicate to the state-input jig that a new state is ready
 * 4. state_out waits for the state-input jig to acknowledge the new state by the raising acknowledge signal
 * 5. state_out lowers the StateStrobe to indicate that it is done
 *
 * @param[in] state The application state that should be reported to the test jig.
 */
void state_out(uint8_t state)
{
    uint32_t istate = state;
    istate = ((istate << 1) & ~0x7F) | (istate & 0x3F);
    // Wait for the other MCU to finish with a previous state
    volatile int ack = StateAck.read();
    while (StateAck.read())
    {}
    // Report the current state
    StateOut = istate;
    StateStrobe = 1;
    // Wait for the other MCU to acknowledge the state output
    while (!StateAck.read())
    {}
    StateStrobe = 0;
}
#else
#warning  The only supported target for state_out is frdm-k64f
void state_out(uint8_t state)
{}
#endif

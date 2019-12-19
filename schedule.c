/**
* @file schedule.c
* @author Shaswat Koirala
* @date December 12th, 2019
* @brief A scheduler utilizes the concept of a state machine and will
* make the code easier to develop, test, and debug
*
* @details
* void scheduler_open(void) : Opens the scheduler functionality by
* resetting the private variable event_scheduled to 0.
* void add_scheduled_event(unsigned int event) : ORs a new event into
* the existing state of the private variable event_scheduled.
* void remove_scheduled_event(unsigned int event) : Removes the event
* from the existing state of the private variable event_scheduled
* unsigned int get_scheduled_events(void) : Returns the current state of
* the private variable event_scheduled.
* @note
* we take private variable to pass the current event to app.c
*
* @event
* current event state
*
* @return
* return the private unsigned int event_scheduled
*
*/

#include "scheduler.h"
#include "em_emu.h"
//# include "em_assert.h"
/*****************
 * Private variables
 */
static unsigned int event_scheduled;
/***************************************************************************//**
 * @brief
 * 	Initialize  event  scheduler
 *
 * @details
 * 	Set the event scheduled equal to zero
 ******************************************************************************/
void scheduler_open(void){
	event_scheduled=0;

}
/***************************************************************************//**
 * @brief
 *	Add an event to the scheduler
 *
 * @details
 *	Add an event flag to the event scheduler
 *
 * @param event
 * 	 the event flag to be scheduled in
 ******************************************************************************/
void add_scheduled_event(unsigned int event){
	event_scheduled |= event;
}
/***************************************************************************//**
 * @brief
 *	Remove an event from the scheduler
 *
 * @details
 *	Remove an event from the event scheduler
 *
 * @param event
 *  event flag  to be removed from the scheduler
 ******************************************************************************/
void remove_scheduled_event(unsigned int event){
	__disable_irq();
	event_scheduled &=~ event;
	__enable_irq();

}
/***************************************************************************//**
 * @brief
 *	get the event scheduler variable
 *
 * @details
 *	Allow to get  the even scheduler variable
 *
 * @return
 * 	currently scheduled events
 ******************************************************************************/
unsigned int get_scheduled_events(void){
	return event_scheduled;
}

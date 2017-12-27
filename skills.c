/* ************************************************************************
 *   File: skills.c                                      Part of CircleMUD *
 *  Usage: commands for skills                                             *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 ************************************************************************ */

#include "conf.h" 
#include "sysdep.h"

CVSHEADER("$CVSHeader: cwg/rasputin/src/act.movement.c,v 1.5 2004/12/22 20:29:59 zizazat Exp $");

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "house.h"
#include "constants.h"
#include "dg_scripts.h"

/*****************************************************************************+

   Memorization time for each spell is equal to the level of the spell + 2
   multiplied by 5 at POS_STANDING.  A level one spell would take 
   (1+2)*5=15 hours(ticks) to memorize when standing.

   POS_SITTING and POS_RESTING simulate the reduction in time by multiplying
   the number subtracted from the MEMTIME each tick by 5.  
   
   A character who has 15 hours(ticks) to memorize a spell standing will see
   this on his display.  When he is sitting, he will have 15 hours in MEMTIME,
   but the display will divide by the value returned in find_memspeed to show
   a value of 15/5 --> 3 hours sitting time.  

   If a tick occurs while sitting, update_mem will subtract 5 hours of 
   "standing time" which is one hour of "sitting time" from the timer.
   
 ******************************************************************************/
int find_timerspeed(struct char_data *ch, int skill_num, bool display) {
	int speedfx = 1;

	

	if (GET_SKILL_BASE(ch, skill_num) < 100) {
		speedfx = 5;
	} else if (GET_SKILL_BASE(ch, skill_num) < 200) {
		speedfx = 6;
	} else if (GET_SKILL_BASE(ch, skill_num) < 300) {
		speedfx = 7;
	} else if (GET_SKILL_BASE(ch, skill_num) < 400) {
		speedfx = 8;
	} else {
		speedfx = 10;
	}
	if (display)
		speedfx = MAX(speedfx, 1);
	return speedfx;
}

/********************************************/
/* called during a tick to count down till  */
/* memorized in comm.c.                     */

/********************************************/

void update_timer(struct char_data *ch, int skill_num) {
	struct memorize_node *mem, *next_mem;
	struct descriptor_data *d;
	struct char_data *i;
	int speedfx = 0;

	for (d = descriptor_list; d; d = d->next) {
		if (ch)
			i = ch;
		else if (d->original)
			i = d->original; 
		else if (!(i = d->character))
			continue;
		speedfx = find_timerspeed(i, skill_num, FALSE);
		
			if (speedfx < mem->timer) {
				mem->timer -= speedfx;
			} else {
				free(mem);
			}
		if (ch)
			return;
	}
}

/* remove a spell from a character's memorize(in progress) linked list */
void memorize_remove(struct char_data * ch, struct memorize_node * mem) {
	struct memorize_node *temp;

	if (ch->memorized == NULL) {
		core_dump();
		return;
	}

	REMOVE_FROM_LIST(mem, ch->memorized, next, temp);
	free(mem);
}

/* add a spell to a character's memorize(in progress) linked list */
void timer_add(struct char_data * ch, char *argument, int timer) {
	struct memorize_node * mem;
	int skill_num;

	skill_num = find_skill_num(argument, SKTYPE_SKILL);
	
	CREATE(mem, struct memorize_node, 1);
	mem->timer = timer;
}
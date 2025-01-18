#include "main.h"
#include "stm32l4xx_it.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "MultiFunctionShield.h"

extern TIM_HandleTypeDef htim3;
extern void MX_TIM3_Init(void);
extern bool got_start_button;
extern bool got_stop_button;
extern bool got_fastest_button;
extern int best_reaction_time_in_millisec;

#define upper_limit_millisec_to_wait 7000
#define CHEAT_BLINK_COUNT 9  // Number of times to blink error pattern

int rand_millisec;
int last_reaction_time_in_millisec = 0;
bool started_doing_reaction_timers = false;

void show_a_random_number() {
    if (!started_doing_reaction_timers) {
        rand_millisec = rand() % upper_limit_millisec_to_wait;
        MultiFunctionShield_Display(rand_millisec);
        HAL_Delay(2000);
    }
}

void got_start() {
    started_doing_reaction_timers = true;
    Clear_LEDs();
    rand_millisec = rand() % upper_limit_millisec_to_wait;

    Display_Waiting();
    HAL_Delay(rand_millisec);
    MultiFunctionShield_Display(8888);
    HAL_TIM_Base_Start_IT(&htim3);
}

void got_stop() {
//    if (waiting_for_go) {
//        // Cheating detected - stopped during waiting period
//        HAL_TIM_Base_Stop(&htim3);
//
//        // Create an error display pattern
//        for(int i = 0; i < CHEAT_BLINK_COUNT; i++) {
//            Display_Waiting();  // Show dashes
//            HAL_Delay(200);    // Quick delay
//            MultiFunctionShield_Display(0000);  // Show zeros
//            HAL_Delay(200);    // Quick delay
//        }

        // End with zeros displayed
        MultiFunctionShield_Display(0);
        last_reaction_time_in_millisec = 0;  // Invalid attempt
        Clear_LEDs();

//    } else {
        // Valid stop
        HAL_TIM_Base_Stop(&htim3);
        last_reaction_time_in_millisec = __HAL_TIM_GetCounter(&htim3) / 10;
        MultiFunctionShield_Display(last_reaction_time_in_millisec);

        // Update best time only if it's a valid attempt
        if (last_reaction_time_in_millisec == 0) {
        	for(int i = 0; i < CHEAT_BLINK_COUNT; i++) {
        		Display_Waiting();
        	    HAL_Delay(200);
        	    MultiFunctionShield_Display(0000);
        	    HAL_Delay(200);
        	}
        }
        else if(last_reaction_time_in_millisec < best_reaction_time_in_millisec){
            best_reaction_time_in_millisec = last_reaction_time_in_millisec;
        }

        // Show stats
        printf("Random Delay was: %d\n\r", rand_millisec);
        printf("Reaction Time from Timer: %d\n\r", last_reaction_time_in_millisec);
        srand((unsigned)last_reaction_time_in_millisec);
    }
//}

void got_fastest() {
    got_fastest_button = false;
    MultiFunctionShield_Display(best_reaction_time_in_millisec);
}

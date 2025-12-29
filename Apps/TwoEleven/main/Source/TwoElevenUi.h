#ifndef TWOELEVEN_UI_H
#define TWOELEVEN_UI_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "TwoElevenHelpers.h"
#include "lvgl.h"

/***********************
 * FUNCTION PROTOTYPES
 **********************/
/**
 * @brief Create a new 2048 game object
 * @param parent Parent LVGL object
 * @param matrix_size Size of the game matrix (3 to 6, defaults to 4 if out of range)
 * @return Pointer to the created LVGL object
 */
lv_obj_t * twoeleven_create(lv_obj_t * parent, uint16_t matrix_size);

/**
 * @brief Start a new game (reset state)
 * @param obj 2048 game LVGL object
 */
void twoeleven_set_new_game(lv_obj_t * obj);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*TWOELEVEN_UI_H*/
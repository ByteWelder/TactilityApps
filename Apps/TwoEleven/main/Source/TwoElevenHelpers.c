#include "TwoElevenHelpers.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief Get the color for a given tile value
 */
lv_color_t get_num_color(uint16_t num)
{
    switch (num) {
        case 0:
            return ELEVENTWO_NUMBER_EMPTY_COLOR;
        case 1:
            return ELEVENTWO_NUMBER_EMPTY_COLOR;
        case 2:
            return ELEVENTWO_NUMBER_2_COLOR;
        case 4:
            return ELEVENTWO_NUMBER_4_COLOR;
        case 8:
            return ELEVENTWO_NUMBER_8_COLOR;
        case 16:
            return ELEVENTWO_NUMBER_16_COLOR;
        case 32:
            return ELEVENTWO_NUMBER_32_COLOR;
        case 64:
            return ELEVENTWO_NUMBER_64_COLOR;
        case 128:
            return ELEVENTWO_NUMBER_128_COLOR;
        case 256:
            return ELEVENTWO_NUMBER_256_COLOR;
        case 512:
            return ELEVENTWO_NUMBER_512_COLOR;
        case 1024:
            return ELEVENTWO_NUMBER_1024_COLOR;
        case 2048:
            return ELEVENTWO_NUMBER_2048_COLOR;
        default:
            return ELEVENTWO_NUMBER_2048_COLOR;
    }
}

/**
 * @brief Count empty cells in the matrix
 */
uint8_t count_empty(uint16_t matrix_size, const uint16_t **matrix) {
    uint8_t count = 0;
    for (uint8_t x = 0; x < matrix_size; x++) {
        for (uint8_t y = 0; y < matrix_size; y++) {
            if (matrix[x][y] == 0) count++;
        }
    }
    return count;
}

/**
 * @brief Find if any adjacent pairs exist (for move possibility)
 */
bool find_pair_down(uint16_t matrix_size, const uint16_t **matrix) {
    for (uint8_t x = 0; x < matrix_size; x++) {
        for (uint8_t y = 0; y < matrix_size - 1; y++) {
            if (matrix[x][y] == matrix[x][y+1]) return true;
        }
    }
    return false;
}

/**
 * @brief Rotate the matrix 90 degrees clockwise (in-place)
 *        Used for move logic
 */
void rotate_matrix(uint16_t matrix_size, uint16_t **matrix) {
    uint8_t n = matrix_size;
    for (uint8_t i = 0; i < n / 2; i++) {
        for (uint8_t j = i; j < n - i - 1; j++) {
            uint16_t tmp = matrix[i][j];
            matrix[i][j] = matrix[n - j - 1][i];
            matrix[n - j - 1][i] = matrix[n - i - 1][n - j - 1];
            matrix[n - i - 1][n - j - 1] = matrix[j][n - i - 1];
            matrix[j][n - i - 1] = tmp;
        }
    }
}

/**
 * @brief Slide and merge a single row/column
 */
bool slide_array(uint16_t * score, uint16_t matrix_size, uint16_t *array) {
    bool success = false;
    uint8_t stop = 0;
    for (uint8_t x = 0; x < matrix_size; x++) {
        if (array[x] != 0) {
            uint8_t t = x;
            while (t > stop && array[t-1] == 0) {
                array[t-1] = array[t];
                array[t] = 0;
                t--;
                success = true;
            }
            if (t > stop && array[t-1] == array[t]) {
                array[t-1]++;
                *score += (1U << array[t-1]);
                array[t] = 0;
                stop = t;
                success = true;
            }
        }
    }
    return success;
}

/**
 * @brief Update the button map with current matrix values
 */
void update_btnm_map(uint16_t matrix_size, char ** btnm_map, const uint16_t **matrix)
{
    uint8_t index = 0;
    for (uint8_t x = 0; x < matrix_size; x++) {
        for (uint8_t y = 0; y < matrix_size; y++) {
            if (((index + 1) % (matrix_size + 1)) == 0) {
                index++;
            }
            if (matrix[x][y] != 0) {
                snprintf(btnm_map[index], 16, "%d", (1 << matrix[x][y]));
            } else {
                strcpy(btnm_map[index], " ");
            }
            index++;
        }
    }
}

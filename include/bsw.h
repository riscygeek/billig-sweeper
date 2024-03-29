/*
 * Copyright (C) 2022 Benjamin Stürz
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef FILE_BSW_H
#define FILE_BSW_H
#include <stdnoreturn.h>
#include <SDL2/SDL_pixels.h>
#include <stdbool.h>
#include <time.h>

#define TITLE "Billig Sweeper"
#define GITHUB_URL "https://github.com/riscygeek/billig-sweeper"

extern bool game_over;
extern time_t start_time, end_time;
extern bool first_launch;

extern int default_n_mines;
extern int default_width;
extern int default_height;
extern SDL_Color default_color;
extern int default_presets[3][3]; // [3; [width, height, n_mines]]

void reset_game (void);

void load_settings (void);
void save_settings (void);

noreturn void relaunch (void);

#endif // FILE_BSW_H

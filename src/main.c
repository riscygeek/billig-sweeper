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
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "dialog.h"
#include "config.h"
#include "video.h"
#include "tile.h"
#include "menu.h"
#include "bsw.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *sprite;
bool game_over;
static char **args;
time_t start_time, end_time;

void
reset_game (void)
{
    game_over = false;
    reset_tiles ();
}

noreturn void
relaunch (void)
{
    video_quit ();
    free (tiles);
    execv ("/proc/self/exe", args);
    _exit (1);
}

int
main (int argc, char *argv[])
{
    int option;

    args = argv;
    load_settings ();

    while ((option = getopt (argc, argv, ":hVr:s:n:")) != -1) {
        char *endp;
        switch (option) {
        case 'h':
            puts (
                TITLE " v" MSW_VERSION "\n"
                "\n"
                "Usage: billig-sweeper [OPTION]...\n"
                "A minesweeper clone made in an afternoon.\n"
                "\n"
                "Options:\n"
                "  -h                    Show this help page.\n"
                "  -V                    Print the version.\n"
                "  -s <width>x<height>   Specify the map size. (default: 10x10)\n"
                "  -n <integer>          Specify how many bombs you want. (default: 10)\n"
                "\n"
                "Report bugs to <benni@stuerz.xyz>"
            );
            return 0;
        case 'V':
            printf ("%s v%s.\n", TITLE, MSW_VERSION);
            return 0;
        case 's':
            if (sscanf (optarg, "%dx%d", &default_width, &default_height) != 2) {
                printf ("Invalid size: %s\n", optarg);
                return 1;
            }
            break;
        case 'n':
            default_n_mines = (int)strtol (optarg, &endp, 10);
            if (*endp || default_n_mines < 1) {
                printf ("Invalid number of bombs: %s\n", optarg);
                return 1;
            }
            break;
        case '?':
            printf ("Invalid option '-%c'.\n", optopt);
            return 1;
        case ':':
            printf ("Expected argument for option '-%c'.\n", optopt);
            return 1;
        }
    }

    // Game initialization.
    srand (time (NULL));
    if (!init_tiles () || !video_init ())
        return 1;

    menu_init ();
    dialog_init ();

    game_over = false;
    menu.shown = false;

    video_post_init ();

    render ();

    while (true) {
        SDL_Event e;

        if (SDL_WaitEvent (&e) && !handle_event (&e))
            break;
    }

    video_quit ();
    free (tiles);
    return 0;
}

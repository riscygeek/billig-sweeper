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
#include "video.h"
#include "menu.h"
#include "tile.h"
#include "util.h"
#include "bsw.h"

struct menu menu;

static bool
btn_quit_on_click (struct menu_button *btn)
{
    (void)btn;
    return false;
}
static void
btn_quit_on_move (struct menu_button *btn)
{
    const float aspect = (float)btn->trect.w / (float)btn->trect.h;
    btn->mrect.h = menu.rect.h / 10;
    btn->mrect.w = (int)(btn->mrect.h * aspect);
    btn->mrect.x = menu.rect.w - btn->mrect.w - 10;
    btn->mrect.y = menu.rect.h - btn->mrect.h - 10;
}

static bool
btn_restart_on_click (struct menu_button *btn)
{
    (void)btn;
    reset_game ();
    menu.shown = false;
    return true;
}
static void
btn_restart_on_move (struct menu_button *btn)
{
    const float aspect = (float)btn->trect.w / (float)btn->trect.h;
    btn->mrect.h = menu.rect.h / 10;
    btn->mrect.w = (int)(btn->mrect.h * aspect);
    btn->mrect.x = 10;
    btn->mrect.y = menu.rect.h - btn->mrect.h - 10;
}

static bool
btn_addsub_bomb_on_click (struct menu_button *btn)
{
    const int inc = shift_pressed ? 10 : 1;
    switch (btn->id) {
    case BTN_ADD_BOMB:
        default_n_mines += inc;
        break;
    case BTN_SUB_BOMB:
        default_n_mines -= inc;
        break;
    case BTN_ADD_HORIZ:
        default_width += inc;
       break;
    case BTN_SUB_HORIZ:
        default_width -= inc;
        break;
    case BTN_ADD_VERT:
        default_height += inc;
        break;
    case BTN_SUB_VERT:
        default_height -= inc;
        break;
    default:
        abort ();
    }
    default_width = my_clamp (t_width, 2, 999);
    default_height = my_clamp (t_height, 2, 999);
    default_n_mines = my_clamp (default_n_mines, 1, my_min (999, t_width * t_height - 1));
    init_tiles ();
    game_over = false;
    save_settings ();
    return true;
}

static void
btn_addsub_bomb_on_move (struct menu_button *btn)
{
    const int id = btn->id - BTN_ADD_BOMB;

    btn->mrect.w = menu.rect.w / 6;
    btn->mrect.h = menu.rect.h / 6;
    btn->mrect.x = (4 + (id % 2)) * btn->mrect.w;
    btn->mrect.y = 5 + (id / 2) * btn->mrect.h;
}

static bool
lbl_on_click (struct menu_button *btn)
{
    if (btn->id == LBL_BOMB)
        open_url (GITHUB_URL);
    return true;
}
static void
lbl_on_move (struct menu_button *btn)
{
    btn->mrect.w = menu.rect.w / 6;
    btn->mrect.h = menu.rect.h / 6;
    btn->mrect.x = 0;
    btn->mrect.y = 5 + (btn->id - LBL_BOMB) * btn->mrect.h;
}

static void
draw_button (const struct menu_button *btn)
{
    SDL_RenderCopy (renderer, sprite, &btn->trect, &btn->wrect);
}

static bool
btn_select_on_click(struct menu_button *btn)
{
    const int n = btn->id - BTN_MIDGET;

    default_width   = default_presets[n][0];
    default_height  = default_presets[n][1];
    default_n_mines = default_presets[n][2];
    init_tiles ();
    game_over = false;
    save_settings ();
    return true;
}
static void
btn_select_on_move (struct menu_button *btn)
{
    const int n = btn->id - BTN_MIDGET;

    btn->mrect.w = menu.rect.w / 4;
    btn->mrect.h = menu.rect.h / 3 - 5;
    btn->mrect.x = menu.rect.w / 3 * n;
    btn->mrect.y = menu.rect.h / 2 + 10;
}

void
menu_init (void)
{
    int ww, wh;

    // Init quit button
    menu.buttons[BTN_QUIT].trect.x = 73;
    menu.buttons[BTN_QUIT].trect.y = 32;
    menu.buttons[BTN_QUIT].trect.w = 47;
    menu.buttons[BTN_QUIT].trect.h = 32;
    menu.buttons[BTN_QUIT].on_click = &btn_quit_on_click;
    menu.buttons[BTN_QUIT].on_move = &btn_quit_on_move;

    // Init restart button
    menu.buttons[BTN_RESTART].trect.x = 0;
    menu.buttons[BTN_RESTART].trect.y = 32;
    menu.buttons[BTN_RESTART].trect.w = 73;
    menu.buttons[BTN_RESTART].trect.h = 32;
    menu.buttons[BTN_RESTART].on_click = &btn_restart_on_click;
    menu.buttons[BTN_RESTART].on_move = &btn_restart_on_move;

    // Midget
    for (int i = BTN_MIDGET; i <= BTN_NON_MIDGET; ++i) {
        menu.buttons[i].trect.x = (i - BTN_MIDGET) * 32;
        menu.buttons[i].trect.y = 192;
        menu.buttons[i].trect.w = 32;
        menu.buttons[i].trect.h = 64;
        menu.buttons[i].on_click = &btn_select_on_click;
        menu.buttons[i].on_move = &btn_select_on_move;
    }

    // Init +/- buttons
    for (int i = BTN_ADD_BOMB; i <= BTN_SUB_VERT; ++i) {
        menu.buttons[i].trect.x = 80 + ((i - BTN_ADD_BOMB) % 2) * 16;
        menu.buttons[i].trect.y = 16;
        menu.buttons[i].trect.w = 16;
        menu.buttons[i].trect.h = 16;
        menu.buttons[i].on_click = &btn_addsub_bomb_on_click;
        menu.buttons[i].on_move = &btn_addsub_bomb_on_move;
    }

    // Initialize labels.
    for (int i = LBL_BOMB; i <= LBL_VERT; ++i) {
        int x;
        if (i == LBL_BOMB) {
            x = 48;
        } else {
            x = (i - LBL_HORIZ + 7) * 16;
        }

        menu.buttons[i].trect.x = x;
        menu.buttons[i].trect.y = 16;
        menu.buttons[i].trect.w = 16;
        menu.buttons[i].trect.h = 16;
        menu.buttons[i].on_click = &lbl_on_click;
        menu.buttons[i].on_move = &lbl_on_move;
    }

    for (size_t i = 0; i < N_BUTTONS; ++i)
        menu.buttons[i].id = i;

    menu.shown = false;

    SDL_GetWindowSize (window, &ww, &wh);
    menu_update (ww, wh);
}

static void
button_update (struct menu_button *btn)
{
    btn->on_move (btn);
    btn->wrect.x = btn->mrect.x + menu.rect.x;
    btn->wrect.y = btn->mrect.y + menu.rect.y;
    btn->wrect.w = btn->mrect.w;
    btn->wrect.h = btn->mrect.h;
}

void
menu_draw_int (unsigned value, unsigned len, int x0, int y0, int w, int h)
{
    const unsigned max_val = powui (10, len) - 1;
    SDL_Rect srect, rect;

    if (value > max_val)
        value = max_val;

    rect.x = x0;
    rect.y = y0;
    rect.w = len * w;
    rect.h = h;

    srect.y = 0;
    srect.w = 16;
    srect.h = 16;

    SDL_SetRenderDrawColor (renderer, 255, 255, 255, 255);
    SDL_RenderFillRect (renderer, &rect);
    SDL_SetRenderDrawColor (renderer, 128, 128, 128, 255);
    SDL_RenderDrawRect (renderer, &rect);

    for (unsigned i = len; i != 0; --i) {
        const unsigned digit = value % 10;
        const unsigned x = i-1;

        rect.x = x0 + (x * w);
        rect.y = y0;
        rect.w = w;
        rect.h = h;

        srect.x = 16 * digit;

        SDL_RenderCopy (renderer, sprite, &srect, &rect);

        value /= 10;
    }
}

void
menu_update (int ww, int wh)
{
    menu.rect.w = ww * 3 / 4;
    menu.rect.h = wh * 3 / 4;
    menu.rect.x = (ww - menu.rect.w) / 2;
    menu.rect.y = (wh - menu.rect.h) / 2;

    for (size_t i = 0; i < N_BUTTONS; ++i)
        button_update (&menu.buttons[i]);
}

void
menu_draw (void)
{
    const int xt = menu.rect.w / 6, yt = menu.rect.h / 6;

    SDL_SetRenderDrawColor (renderer, 192, 192, 192, 255);
    SDL_RenderFillRect (renderer, &menu.rect);
    SDL_SetRenderDrawColor (renderer, 128, 128, 128, 255);
    SDL_RenderDrawRect (renderer, &menu.rect);

    menu_draw_int (default_n_mines, 3, menu.rect.x + xt + 5, yt + 5, xt, yt);
    menu_draw_int (t_width, 3, menu.rect.x + xt + 5, yt * 2 + 5, xt, yt);
    menu_draw_int (t_height, 3, menu.rect.x + xt + 5, yt * 3 + 5, xt, yt);

    for (size_t i = 0; i < N_BUTTONS; ++i)
        draw_button (&menu.buttons[i]);
}
bool
menu_click (SDL_Point p, int button)
{
    (void)button;

    for (size_t i = 0; i < N_BUTTONS; ++i) {
        struct menu_button *btn = &menu.buttons[i];
        if (SDL_PointInRect (&p, &btn->wrect)) {
            const bool v = btn->on_click (btn);
            render ();
            return v;
        }
    }
    return true;
}

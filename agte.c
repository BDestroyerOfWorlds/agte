/*
 *  agte - A Graphical (GUI) Text Editor
 *  Copyright (C) 2026 B. Keskin
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *  Contact me at bkeskinsoftware@gmail.com
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#define RAYGUI_IMPLEMENTATION
#include "font_data.h"
#include "raygui.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define NOT_SAVED "\U000F0F42"
#define CHANGES "\U000F0CFB"
#define SAVED "\U000F0193"
#define CAPS "\U000F0A9B"

#define BETTER_BLACK (Color){ 0x11, 0x11, 0x1B, 255 }
#define BETTER_WHITE (Color){ 0xF7, 0xF8, 0xFD, 255 }
#define MAUVE (Color){ 0xCB, 0xA6, 0xF7, 255 }
#define BETTER_BLUE (Color){ 0x89, 0xB4, 0xFA, 255 }
#define BETTER_ORANGE (Color){ 0xFE, 0x64, 0x0B, 255 }
#define BETTER_RED (Color){ 0xD2, 0x0F, 0x39, 255 }

/*****************************************************************************/

typedef struct
{
  Font Lilex;
  Font icons;
} Fonts;

/*****************************************************************************/

char *
load_file (const char *path, int *out_chr_cnt)
{
  char *temp_buffer = LoadFileText (path);

  if (temp_buffer == NULL)
    {
      return NULL;
    }

  int len = TextLength (temp_buffer);
  char *buffer = malloc (len + 1);

  if (buffer == NULL)
    {
      UnloadFileText (temp_buffer);
      return NULL;
    }

  memcpy (buffer, temp_buffer, len);
  buffer[len] = '\0';
  UnloadFileText (temp_buffer);

  *out_chr_cnt = len;
  return buffer;
}

/*****************************************************************************/

bool
cap_enough (char **buffer, size_t *current_cap, size_t needed_cap)
{
  if (needed_cap <= *current_cap)
    {
      return true;
    }

  size_t next_cap = (*current_cap * 2);
  if (next_cap < needed_cap)
    {
      next_cap = needed_cap;
    }
  char *new_buf = realloc (*buffer, next_cap);
  if (new_buf == NULL)
    return false;

  *buffer = new_buf;
  *current_cap = next_cap;
  return true;
}

/*****************************************************************************/

void
get_cursor_coordinates (const char *buffer, int cursor_posi, int *out_line,
                        int *out_col)
{
  int line = 0;
  int col = 0;
  for (int i = 0; i < cursor_posi; i++)
    {
      if (buffer[i] == '\n')
        {
          line++;
          col = 0;
        }
      else
        {
          col++;
        }
    }
  *out_line = line;
  *out_col = col;
}

/*****************************************************************************/

void
draw_editor_borders ()
{
  DrawLine (1200, 0, 1200, 720, MAUVE);
  DrawLine (1, 1, 1200, 1, MAUVE);
  DrawLine (1, 1, 1, 708, MAUVE);
  DrawLine (1, 720, 1200, 720, MAUVE);

  DrawLine (1200, 1, 1279, 1, MAUVE);
  DrawLine (1279, 1, 1279, 719, MAUVE);
  DrawLine (1200, 1, 1200, 719, MAUVE);

  DrawLine (1188, 1, 1188, 719, MAUVE);

  DrawLine (1200, 707, 1, 707, MAUVE);
}

/*****************************************************************************/

void
set_style ()
{
  GuiSetStyle (DEFAULT, BACKGROUND_COLOR, ColorToInt (BETTER_BLACK));
  GuiSetStyle (DEFAULT, LINE_COLOR, ColorToInt (BETTER_WHITE));

  GuiSetStyle (LISTVIEW, BORDER_COLOR_NORMAL, ColorToInt (BETTER_BLUE));
  GuiSetStyle (LISTVIEW, BORDER_COLOR_FOCUSED, ColorToInt (BETTER_BLUE));
  GuiSetStyle (LISTVIEW, BORDER_COLOR_PRESSED, ColorToInt (BETTER_BLUE));

  GuiSetStyle (BUTTON, BASE_COLOR_NORMAL, ColorToInt (VIOLET));

  GuiSetStyle (SLIDER, BORDER_COLOR_NORMAL, ColorToInt (MAUVE));
  GuiSetStyle (SLIDER, BORDER_COLOR_FOCUSED, ColorToInt (BETTER_BLUE));
  GuiSetStyle (SLIDER, BORDER_COLOR_PRESSED, ColorToInt (BETTER_BLACK));
}

/*****************************************************************************/

Fonts
fetch_fonts (void)
{
  Fonts f;

  const char *icons_available = "\U000F0F42"
                                "\U000F0CFB"
                                "\U000F0193"
                                "\U000F0A9B";

  int codepoint_count = 0; // just to flush

  int *codepoints = LoadCodepoints (icons_available, &codepoint_count);

  f.icons = LoadFontFromMemory (".ttf", LilexNerdFontMono_Regular_ttf,
                                LilexNerdFontMono_Regular_ttf_len, 64,
                                codepoints, codepoint_count);

  UnloadCodepoints (codepoints);

  f.Lilex
      = LoadFontFromMemory (".ttf", LilexNerdFontMono_Regular_ttf,
                            LilexNerdFontMono_Regular_ttf_len, 20, NULL, 0);
  return f;
}

/*****************************************************************************/

typedef struct
{
  char *buffer;
  size_t capacity;
  int length;
  int cursor_position;
  bool modified;
  bool file_exists;
  char *file_path;
} editor_state;

/*****************************************************************************/

void editor_init (editor_state *state, const char *path);

void editor_handle_input (editor_state *state);

void editor_render (editor_state *state, Fonts *fonts);

void cleanup (editor_state);

/*****************************************************************************/

int
main (int argc,
      char *argv[]) // I need to refactor this whole thing tbh...
{

  if (argc < 2)
    {
      printf ("usage: agte <filename>\n");
      /* BUT launching without arguements could launch a file explorer which im
       * looking into building tbh. */
      return -1;
    }

  char *buffer = NULL;
  size_t buffer_capacity = 0;
  int chr_count = 0;
  char *path = argv[1];

  bool file_exists = false;
  bool file_modified = false;

  if (FileExists (path))
    {

      buffer = load_file (path, &chr_count);
      if (buffer == NULL)
        {

          buffer = malloc (1);
          if (buffer == NULL) // git test comment
            {
              printf ("FATAL ERROR: out of memory\n");
              return -1;
            }
          buffer[0] = '\0';
          buffer_capacity = 1;
          chr_count = 0;
        }
      else
        {
          buffer_capacity = chr_count + 1;
        }
      file_exists = true;
      file_modified = false;
    }
  else
    {
      buffer = malloc (1);
      if (buffer == NULL)
        {
          printf ("FATAL ERROR: out of memory\n");
          return -1;
        }
      buffer[0] = '\0';
      buffer_capacity = 1;
      chr_count = 0;
      file_exists = false;
      file_modified = false;
    }

  int cursor_posi = chr_count;

  InitWindow (1280, 720, "agte");
  SetTargetFPS (60);

  Fonts fonts = fetch_fonts ();

  float char_width = MeasureTextEx (fonts.Lilex, "WW", 20, 1).x / 2.0f;

  bool caps = false; // not a great idea, explained
                     // further down.

  Vector2 scroll = { 0, 0 };
  Rectangle view;

  set_style ();

  while (!WindowShouldClose ())
    {
      BeginDrawing ();

      int key = GetCharPressed (); // how actual letters are handled
      while (key > 0)
        {
          if ((key > 31) && (key < 126))
            {
              if (cap_enough (&buffer, &buffer_capacity, chr_count + 2))
                {

                  for (int i = chr_count; i > cursor_posi; i--)
                    {
                      buffer[i] = buffer[i - 1];
                    }
                  buffer[cursor_posi] = (char)key;
                  chr_count++;
                  cursor_posi++;
                  buffer[chr_count] = '\0';
                  file_modified = true;
                }
            }
          key = GetCharPressed ();
        }

      //// CONTROLS SECTION

      if ((IsKeyPressed (KEY_BACKSPACE)
           || (IsKeyPressedRepeat (KEY_BACKSPACE)))
          && cursor_posi > 0)
        {
          for (int i = cursor_posi; i < chr_count; i++)
            {
              buffer[i - 1] = buffer[i];
            }
          chr_count--;
          cursor_posi--;
          buffer[chr_count] = '\0';
          file_modified = true;
        }

      if ((IsKeyPressedRepeat (KEY_LEFT) || IsKeyPressed (KEY_LEFT))
          && cursor_posi > 0)
        cursor_posi--;

      if ((IsKeyPressed (KEY_RIGHT) || IsKeyPressedRepeat (KEY_RIGHT))
          && chr_count > cursor_posi)
        cursor_posi++;

      if (IsKeyDown (KEY_LEFT_CONTROL) && IsKeyPressed (KEY_S))
        {
          SaveFileText (path, buffer);
          file_exists = true;
          file_modified = false;
        }

      int cursor_line, cursor_col;
      get_cursor_coordinates (buffer, cursor_posi, &cursor_line, &cursor_col);

      if ((IsKeyPressed (KEY_UP) || IsKeyPressedRepeat (KEY_UP))
          && cursor_line > 0)
        {
          int target_line = cursor_line - 1;
          int line = 0;
          int start = 0;
          int length = 0;

          for (int i = 0; i < chr_count; i++)
            {
              if ((line == target_line) && ((buffer[i] == '\n')))
                {
                  length = i - start;
                  break;
                }
              if (buffer[i] == '\n')
                {
                  line++;
                  start = i + 1;
                }
            }
          int new_col;
          if (cursor_col < length)
            new_col = cursor_col;
          else
            new_col = length;
          cursor_posi = start + new_col;
        }

      if (IsKeyPressed (KEY_DOWN) || IsKeyPressedRepeat (KEY_DOWN))
        {
          int target_line = cursor_line + 1;
          int line = 0;
          int start = -1;
          int length = 0;
          bool line_present = false;
          for (int i = 0; i < chr_count; i++)
            {
              if ((line == target_line) && ((buffer[i] == '\n')))
                {
                  length = i - start;
                  line_present = true;
                  break;
                }
              if (buffer[i] == '\n')
                {
                  line++;
                  start = i + 1;
                }
            }
          if (start != -1)
            {
              if (!line_present)
                {
                  length = chr_count - start;
                }

              int new_col;
              if (cursor_col < length)
                new_col = cursor_col;
              else
                new_col = length;
              cursor_posi = start + new_col;
            }
        }

      if (IsKeyPressed (KEY_ENTER) || (IsKeyPressedRepeat (KEY_ENTER)))
        {
          if (cap_enough (&buffer, &buffer_capacity, chr_count + 2))
            {
              for (int i = chr_count; i > cursor_posi; i--)
                {
                  buffer[i] = buffer[i - 1];
                }
              buffer[cursor_posi] = '\n';
              chr_count++;
              cursor_posi++;
              buffer[chr_count] = '\0';
              file_modified = true;
            }
        }
      if (IsKeyPressed (KEY_PAGE_UP))
        {
          cursor_posi = 0;
        }
      if (IsKeyPressed (KEY_PAGE_DOWN))
        {
          cursor_posi = chr_count;
        }

      if ((IsKeyDown (KEY_LEFT_CONTROL)) && (IsKeyPressed (KEY_C)))
        {
          int copy_line_start = cursor_posi;
          while ((copy_line_start > 0) && buffer[copy_line_start - 1] != '\n')
            {
              copy_line_start--;
            }

          int copy_line_end = cursor_posi;
          while ((copy_line_end < chr_count) && buffer[copy_line_end] != '\n')
            {
              copy_line_end++;
            }

          int copy_len = copy_line_end - copy_line_start;

          if (copy_len > 0)
            {
              char *copy_line = malloc (copy_len + 1);
              if (copy_line)
                {
                  memcpy (copy_line, &buffer[copy_line_start], copy_len);
                  copy_line[copy_len] = '\0';
                  SetClipboardText (copy_line);
                  free (copy_line);
                }
            }
        }

      if ((IsKeyDown (KEY_LEFT_CONTROL)) && (IsKeyPressed (KEY_X)))
        {
          int cut_line_start = cursor_posi;
          while ((cut_line_start > 0) && buffer[cut_line_start - 1] != '\n')
            {
              cut_line_start--;
            }

          int cut_line_end = cursor_posi;
          while ((cut_line_end < chr_count) && buffer[cut_line_end] != '\n')
            {
              cut_line_end++;
            }

          int cut_len = cut_line_end - cut_line_start;

          if (cut_len > 0)
            {
              char *cut_line = malloc (cut_len + 1);
              if (cut_line)
                {
                  memcpy (cut_line, &buffer[cut_line_start], cut_len);
                  cut_line[cut_len] = '\0';
                  SetClipboardText (cut_line);
                  free (cut_line);
                }
            }
          int delete_len = cut_len;
          if ((cut_line_end < chr_count) && buffer[cut_line_end] == '\n')
            {
              delete_len++;
            }

          for (int i = cut_line_start + delete_len; i <= chr_count; i++)
            {
              buffer[i - delete_len] = buffer[i];
            }

          chr_count -= delete_len;
          cursor_posi = cut_line_start;

          buffer[chr_count] = '\0';
          file_modified = true;
        }

      if ((IsKeyDown (KEY_LEFT_CONTROL)) && (IsKeyPressed (KEY_V)))
        {
          const char *clipboard = GetClipboardText ();
          if (clipboard && clipboard[0] != '\0')
            {
              int clipboard_len = strlen (clipboard);
              if (cap_enough (&buffer, &buffer_capacity,
                              chr_count + clipboard_len + 1))
                {
                  for (int i = chr_count; i >= cursor_posi; i--)
                    {
                      buffer[i + clipboard_len] = buffer[i];
                    }
                  memcpy (buffer + cursor_posi, clipboard, clipboard_len);
                  chr_count += clipboard_len;
                  cursor_posi += clipboard_len;
                  buffer[chr_count] = '\0';
                  file_modified = true;
                }
            }
        }

      if (IsKeyPressed (KEY_TAB))
        {
          if (cap_enough (&buffer, &buffer_capacity, chr_count + 3))
            {
              for (int i = chr_count; i >= cursor_posi; i--)
                {
                  buffer[i + 2] = buffer[i];
                }
              memcpy (&buffer[cursor_posi], "  ", 2);

              chr_count += 2;
              cursor_posi += 2;
              buffer[chr_count] = '\0';
              file_modified = true;
            }
        }

      int caps_helper = GetKeyPressed ();

      if (caps_helper == KEY_CAPS_LOCK) /* this is a
                                           really bad
                                           solution
                                           becuase we have
                                           no idea if its
                                           on or off in
                                           the beginning
                                           and it defaults
                                           to off but a
                                           better solution
                                           kinda
                                           overcomplicates
                                           is for now so
                                           im sleeping on
                                           it */
        {
          caps = !caps;
        }

      //// CONTROLS SECTION END

      ClearBackground (BETTER_BLACK);

      int line_count = 1; // calculated for content area, for scroll logic.
      int max_line_len = 0;
      int current_len = 0;

      for (int i = 0; i < chr_count; i++)
        {
          if (buffer[i] == '\n')
            {
              line_count++;
              if (current_len > max_line_len)
                {
                  max_line_len = current_len;
                }
              current_len = 0;
            }
          else
            {
              current_len++;
            }
        }

      if (current_len > max_line_len)
        {
          max_line_len = current_len;
        }

      Rectangle panel = { 0, 0, 1200, 720 };
      Rectangle content
          = { 0, 0,
              fmaxf (panel.width, 32 + max_line_len * (char_width + 0.5F)),
              fmaxf (707, (line_count * 22) + 22) };

      GuiScrollPanel (panel, NULL, content, &scroll, &view);
      BeginScissorMode (view.x, view.y, view.width, view.height);

      DrawTextEx (fonts.Lilex, buffer,
                  (Vector2){ 32 + scroll.x, 16 + scroll.y }, 20, 1,
                  BETTER_WHITE);

      float cursor_x = 32 + scroll.x + (cursor_col * (char_width + 0.5f));
      float cursor_y = 16 + scroll.y + (cursor_line * 22);

      DrawRectangle (cursor_x, cursor_y, 2, 16, BETTER_WHITE);

      EndScissorMode ();

      draw_editor_borders ();

      // ICONS SECTION

      Color saved_icon_color;
      const char *saved_icon_text;
      int saved_icon_size;

      if (!file_modified && file_exists)
        {
          saved_icon_text = SAVED;
          saved_icon_color = BETTER_BLUE;
          saved_icon_size = 59;
        }
      else if (file_modified && file_exists)
        {
          saved_icon_text = CHANGES;
          saved_icon_color = BETTER_ORANGE;
          saved_icon_size = 64;
        }
      else
        {
          saved_icon_text = NOT_SAVED;
          saved_icon_color = BETTER_RED;
          saved_icon_size = 64;
        }

      DrawTextEx (fonts.icons, saved_icon_text, (Vector2){ 1225, 16 },
                  saved_icon_size, 1, saved_icon_color); /* icon placement
                                                            needs its own
                                                            helper logic
                                                            because they
                                                            are slightly
                                                            different
                                                            sizes so thats
                                                            TODO */

      if (caps)
        {
          DrawTextEx (fonts.icons, CAPS, (Vector2){ 1223, 66 }, 64, 1,
                      BETTER_BLUE);
        }

      // ICONS SECTION

      EndDrawing ();
    }

  /* Cleanup section that will revieve its own helper after the struct system
   * is implemented */

  UnloadFont (fonts.Lilex);
  UnloadFont (fonts.icons);
  free (buffer);
  buffer = NULL;
  WindowShouldClose ();
  CloseWindow ();
  return 0;
}

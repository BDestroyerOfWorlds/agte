/*
   agte - A Graphical (GUI) Text Editor
   Copyright (C) 2026 B. Keskin

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.

   Contact me at bkeskinsoftware@gmail.com
*/

#define RAYGUI_IMPLEMENTATION
#include "font_data.h"
#include "raygui.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>

#define LOADED 0
#define NOT_FOUND 1
#define OVERSIZE_LOAD 2

#define MAX_BUFFER_LEN 16384

/*****************************************************************************/

int
load_file (const char *path, char *buffer, int max_len, int *out_chr_cnt)
{
  char *file_text = LoadFileText (path);

  if (file_text == NULL)
    {
      return NOT_FOUND;
    }

  int len = TextLength (file_text);

  if (len > max_len)
    {
      UnloadFileText (file_text);
      return OVERSIZE_LOAD;
    }

  for (int i = 0; i < len; i++)
    {
      buffer[i] = file_text[i];
    }

  buffer[len] = '\0';
  *out_chr_cnt = len;
  UnloadFileText (file_text);

  return LOADED;
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

int
main (int argc, char *argv[])
{

  if (argc < 2)
    {
      printf ("usage: agte <filename>\n");
      return -1;
    }

  char buffer[MAX_BUFFER_LEN + 1];
  int chr_count = 0;
  int cursor_posi = chr_count;
  char *path = argv[1];

  int load_result = load_file (path, buffer, MAX_BUFFER_LEN, &chr_count);

  switch (load_result)
    {
    case NOT_FOUND:
      printf ("file not found, creating...\n");
      break;

    case OVERSIZE_LOAD:
      printf ("FATAL ERROR: oversize load\n");
      return -1;

    case LOADED:
      break;
    }

  InitWindow (1280, 720, "agte");
  SetTargetFPS (60);

  Font Lilex
      = LoadFontFromMemory (".ttf", LilexNerdFontMono_Regular_ttf,
                            LilexNerdFontMono_Regular_ttf_len, 20, NULL, 0);

  float char_width = MeasureTextEx (Lilex, "WW", 20, 1).x / 2.0f;

  Vector2 scroll = { 0, 0 };
  Rectangle view;

  GuiSetStyle (DEFAULT, BACKGROUND_COLOR,
               ColorToInt ((Color){ 0x0A, 0x0C, 0x10, 255 }));
  GuiSetStyle (DEFAULT, LINE_COLOR,
               ColorToInt ((Color){ 0xF0, 0xF3, 0xF6, 255 }));

  while (!WindowShouldClose ())
    {
      BeginDrawing ();

      int key = GetCharPressed ();
      while (key > 0)
        {
          if ((key > 31) && (key < 126) && chr_count < MAX_BUFFER_LEN)
            {
              for (int i = chr_count; i > cursor_posi; i--)
                {
                  buffer[i] = buffer[i - 1];
                }
              buffer[cursor_posi] = (char)key;
              chr_count++;
              cursor_posi++;
              buffer[chr_count] = '\0';
            }
          key = GetCharPressed ();
        }

      //// CONTROLS SECTION

      if ((IsKeyPressed (KEY_BACKSPACE)
           || (IsKeyPressedRepeat (KEY_BACKSPACE)) && cursor_posi > 0))
        {
          for (int i = cursor_posi; i < chr_count; i++)
            {
              buffer[i - 1] = buffer[i];
            }
          chr_count--;
          cursor_posi--;
          buffer[chr_count] = '\0';
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
              if ((line == target_line)
                  && ((buffer[i] == '\n') || (i == chr_count)))
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
          for (int i = 0; i < chr_count; i++)
            {
              if ((line == target_line)
                  && ((buffer[i] == '\n') || (i == chr_count)))
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
          if (start != -1)
            {
              int new_col;
              if (cursor_col < length)
                new_col = cursor_col;
              else
                new_col = length;
              cursor_posi = start + new_col;
            }
        }

      if (IsKeyPressed (KEY_ENTER)
          || (IsKeyPressedRepeat (KEY_ENTER)) && chr_count < MAX_BUFFER_LEN)
        {
          for (int i = chr_count; i > cursor_posi; i--)
            {
              buffer[i] = buffer[i - 1];
            }
          buffer[cursor_posi] = '\n';
          chr_count++;
          cursor_posi++;
          buffer[chr_count] = '\0';
        }

      //// CONTROLS SECTION

      ClearBackground ((Color){ 0x0A, 0x0C, 0x10, 255 });

      int line_count = 1;
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
                  current_len = 0;
                }
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

      Rectangle panel = { 0, 0, 1280, 720 };
      Rectangle content
          = { 0, 0, fmaxf (panel.width, max_line_len * (char_width + 0.5F)),
              (line_count * 22) + 22 };

      GuiScrollPanel (panel, NULL, content, &scroll, &view);

      BeginScissorMode (view.x, view.y, view.width, view.height);

      DrawTextEx (Lilex, buffer, (Vector2){ 32 + scroll.x, 16 + scroll.y }, 20,
                  1, (Color){ 0xF0, 0xF3, 0xF6, 255 });

      float cursor_x = 32 + scroll.x + (cursor_col * (char_width + 0.5f));
      float cursor_y = 16 + scroll.y + (cursor_line * 22);

      DrawRectangle (cursor_x, cursor_y, 2, 16,
                     (Color){ 0xF0, 0xF3, 0xF6, 255 });

      EndScissorMode ();

      EndDrawing ();
    }

  UnloadFont (Lilex);

  WindowShouldClose ();
  return 0;
}

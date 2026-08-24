/*
   agte - A Graphical (GUI) Text Editor
   Copyright (C) 2026 B Keskin

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
*/

#include "font_data.h"
#include <raylib.h>
#include <stdio.h>

#define LOADED 0
#define NOT_FOUND 1
#define OVERSIZE_LOAD 2
#define MAX_BUFFER_LEN 16384

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
  char *path = argv[1];

  int load_result = load_file (path, buffer, MAX_BUFFER_LEN, &chr_count);

  switch (load_result)
    {
    case NOT_FOUND:
      printf (
          "file not found, creating...\n"); // i know it says creating but it
                                            // doesnt do shit rn that TODO
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
                            LilexNerdFontMono_Regular_ttf_len, 16, NULL, 0);

  while (!WindowShouldClose ())
    {
      BeginDrawing ();
      ClearBackground (GRAY);

      DrawTextEx (Lilex, buffer, (Vector2){ 16, 16 }, 16, 1, WHITE);

      EndDrawing ();
    }

  UnloadFont (Lilex);

  WindowShouldClose ();
  return 0;
}

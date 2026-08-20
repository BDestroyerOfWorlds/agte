#include "raylib.h"

#define MAX_INPUT_LEN 64

int
main ()
{

  InitWindow (1280, 720, "agte");
  SetTargetFPS (60);

  char input_buffer[MAX_INPUT_LEN + 1] = "\0";
  int chr_count = 0;

  while (!WindowShouldClose ())
    {
      int key = GetCharPressed ();

      while (key > 0)
        {
          if ((key >= 32) && (key <= 125) && (chr_count < MAX_INPUT_LEN))
            {
              input_buffer[chr_count] = (char)key;
              input_buffer[chr_count + 1] = '\0';
              chr_count++;
            }

          key = GetCharPressed ();
        }

      BeginDrawing ();
      ClearBackground (GRAY);

      DrawText (input_buffer, 32, 32, 20, WHITE);
      DrawText (TextFormat ("%i/%i", chr_count, MAX_INPUT_LEN), 1200, 680, 20,
                WHITE);

      EndDrawing ();
    }

  WindowShouldClose ();
  return 0;
}

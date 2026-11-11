# Mario OOP practice

This directory is based on the archived Mario project and keeps two stages of
the same game:

- `mario.cpp` is a compact procedural implementation.
- `mario_refactoring.cpp` separates the map, game objects, movement, and game
  controller into classes.

Both programs use Linux `ncurses` and can be built independently:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic mario.cpp -lncurses -o mario
g++ -std=c++17 -Wall -Wextra -pedantic mario_refactoring.cpp -lncurses -o mario_refactoring
```

Use `A`/`D` or the arrow keys to move, Space to jump, and Escape to quit.

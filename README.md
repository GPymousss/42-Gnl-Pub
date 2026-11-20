*This project has been created as part of the 42 curriculum by guletill.*

# get_next_line

## Description

get_next_line is a project of the 42 common core. The goal is to write a
function that returns the next line read from a file descriptor, one line per
call:

```c
char	*get_next_line(int fd);
```

Called in a loop, it reads a whole file (or the standard input, a pipe, a
socket…) line by line. Each returned line includes its terminating `\n`,
except the last line of a file that does not end with one. When there is
nothing left to read, or when an error occurs, the function returns `NULL`.
The returned line is allocated with `malloc` and must be freed by the caller.

The main lesson of the project is the use of a **static variable**: a local
variable that keeps its value between two calls of the function. It is what
allows `get_next_line` to remember the characters it has already read but not
yet returned.

The subject sets the following constraints:

- the only external functions allowed are `read`, `malloc` and `free`;
- the libft, `lseek()` and global variables are forbidden;
- the size of each `read` is set at compile time with `-D BUFFER_SIZE=n`, and
  the function must work for any value (1, 42, 9999, 10000000…);
- the file must be read as little as possible: as soon as a newline is found,
  the current line is returned, instead of reading the whole file first.

Only the mandatory part is implemented (one file descriptor at a time).

### Files

| File | Content |
|---|---|
| `get_next_line.c` | `get_next_line` and its static helpers `read_file`, `get_line`, `save_rest`. |
| `get_next_line_utils.c` | String helpers: `ft_strlen`, `ft_strchr`, `ft_strjoin`, `ft_substr`. |
| `get_next_line.h` | Prototypes and the default `BUFFER_SIZE` (42). |

## Instructions

### Requirements

- `cc` (gcc or clang)

### Compilation

The project has no `Makefile`: the files are compiled together with your own
`main`. As required by the subject:

```sh
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c
```

The `-D BUFFER_SIZE=n` flag is optional: without it, the default value of 42
defined in `get_next_line.h` is used.

### Testing

Create a `main.c` that opens a file and prints it line by line:

```c
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int	main(int argc, char **argv)
{
	char	*line;
	int		fd;

	if (argc != 2)
		return (1);
	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
		return (1);
	line = get_next_line(fd);
	while (line)
	{
		printf("%s", line);
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	return (0);
}
```

Then build and run it with different buffer sizes, under `valgrind` to check
for memory leaks:

```sh
cc -Wall -Wextra -Werror -D BUFFER_SIZE=1 get_next_line.c get_next_line_utils.c main.c -o gnl
valgrind --leak-check=full ./gnl file.txt
```

Useful cases to test: an empty file, a file made only of `\n`, a last line
without `\n`, a very long line, an invalid file descriptor, and buffer sizes
of 1, 42 and 10000000.

## Algorithm

### Overview

`get_next_line` keeps one static string, `save`, which holds everything that
has been read from the file but not returned yet. Each call works in three
steps:

1. **Read** (`read_file`): if `save` does not already contain a `\n`, read
   `BUFFER_SIZE` bytes at a time and append them to `save` with `ft_strjoin`,
   until a `\n` is read or the end of the file is reached.
2. **Extract the line** (`get_line`): copy the beginning of `save`, up to and
   including the first `\n` (or up to the end if there is none), into a new
   string. This is the line returned to the caller.
3. **Keep the rest** (`save_rest`): replace `save` with what comes after that
   line, so the next call starts from there. When nothing is left, `save` is
   freed and set to `NULL`.

Example with `BUFFER_SIZE=8` and a file containing `"Hello\nWorld\n"`:

| Call | `read` returns | `save` before extraction | Returned line | `save` after |
|---|---|---|---|---|
| 1 | `"Hello\nWo"` | `"Hello\nWo"` | `"Hello\n"` | `"Wo"` |
| 2 | `"rld\n"` | `"World\n"` | `"World\n"` | `NULL` |
| 3 | `""` (end of file) | `""` | `NULL` | `NULL` |

### Details

- **Searching for the newline only in the new data**: inside the reading loop,
  `ft_strchr` looks for `\n` in the bytes that were just read (`buffer`), not
  in the whole `save`. A new newline can only appear there, so scanning `save`
  again at every iteration would be wasted work on long lines.
- **Types**: sizes and indexes use `size_t`, like the libc. The return value of
  `read` is stored in a `ssize_t`, which is large enough for a size and signed,
  so the `-1` error value can be detected.
- **Error handling**: if `fd` is negative, `BUFFER_SIZE` is not positive,
  `read` fails or any `malloc` fails, everything that was allocated (the
  buffer, `save`, the current line) is freed, `save` is reset to `NULL` and the
  function returns `NULL`. `save_rest` returns `-1` on allocation failure, so
  that `get_next_line` can tell an error apart from a simple "nothing left".

### Why this design

- **One static variable, no global**: the leftover data is private to
  `get_next_line` and survives between calls, as the subject requires.
- **Reading as little as possible**: the loop stops at the first `\n`, so a
  line is returned as soon as it is complete, which also works on infinite
  inputs such as the standard input.
- **Simple helpers**: each step is a short function that follows the Norm and
  can be tested on its own.
- **Known trade-off**: every `ft_strjoin` copies the whole `save`, so on a
  single very long line with a very small `BUFFER_SIZE` (for example 1), the
  cost grows quadratically with the length of the line. A buffer that doubles
  its capacity would avoid it, but would make the code much more complex for a
  case that does not matter for normal files.

## Resources

### References

- `read(2)` man page: <https://man7.org/linux/man-pages/man2/read.2.html>
- `malloc(3)` / `free(3)` man page:
  <https://man7.org/linux/man-pages/man3/malloc.3.html>
- C reference, storage duration and `static`:
  <https://en.cppreference.com/w/c/language/storage_duration>
- File descriptors, `open(2)` man page:
  <https://man7.org/linux/man-pages/man2/open.2.html>
- 42 Norm (`fr.norm.pdf`) and the get_next_line subject (version 14.3)
- `norminette`, the official 42 Norm checker: <https://github.com/42School/norminette>

### Use of AI

AI (Claude, by Anthropic) was used for:

- **Code review**: reviewing the existing code, which led to using `size_t`
  and `ssize_t` instead of `int`, fixing an out-of-bounds read in `ft_substr`,
  and handling every `malloc` failure without leaks or lost data (including
  the new return value of `save_rest`).
- **Optimisation**: searching for `\n` in the newly read buffer instead of the
  whole `save`, after analysing timeouts on giant lines with testers.
- **Documentation**: writing this README from the subject's requirements.

The overall algorithm and the first version of every function were written
without AI; each change suggested during the review was understood and
integrated by hand.

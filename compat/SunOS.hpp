#include <termios.h>

namespace et {
void cfmakeraw(termios *terminal_local);
int forkpty( int *amaster, char *name, void *unused1, void *unused2 );
}

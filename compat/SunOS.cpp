#if __sun__
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stropts.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

namespace et {

void cfmakeraw(termios *terminal_local) {
  terminal_local->c_iflag &= ~(IMAXBEL|IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
  terminal_local->c_oflag &= ~OPOST;
  terminal_local->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
  terminal_local->c_cflag &= ~(CSIZE|PARENB);
  terminal_local->c_cflag |= CS8;
}

int forkpty( int *amaster, char *name, void *unused1, void *unused2 ) {

  char	*slave; 
  int  pid, pty, tty;

  unused1 = unused1;
  unused2 = unused2;
  
   pty = open( "/dev/ptmx", O_RDWR | O_NOCTTY );

   if( pty < 0 )
   {
	   perror("open(/dev/ptmx): ");
	   return( -1 );
   }

   if( grantpt( pty ) < 0 )
   {
	   perror("grantpt(): ");
	   return( -1 );
   }
	
   if( unlockpt( pty ) < 0 )
   {
	   perror("unlockpt(): ");
	   return( -1 );
   }
	
   slave = ptsname( pty );

   if( slave == NULL )
   {
	   perror("ptsname(): ");
	   return( -1 );
   }
	
   if ( name ) strcpy( name, slave );

   if ( amaster ) *amaster = pty;
   printf( "\tpty is fd = %i\n", pty);

   /* fork to spawn a shell */
   pid = fork();

   if( pid < 0 )
   {
	   perror( " ! fork():" );
	   return( -1 );
   }

   if( pid == 0 )
   {
	   /* close the pty (master side) */
	   /* the client socket is closed by the caller */
	   close( pty );

	   tty = open( slave, O_RDWR | O_NOCTTY );

	   if( tty < 0 )
	   {
		   perror("open( slave ): ");
		   return( -1 );
	   }

	   if( ioctl( tty, I_PUSH, "ptem" ) < 0 )
	   {
		   perror("ioctl( ptem ):");
		   return( -1 );
	   }

	   if( ioctl( tty, I_PUSH, "ldterm" ) < 0 )
	   {
		   perror("ioctl( ldterm ):");
		   return( -1 );
	   }

	   if( ioctl( tty, I_PUSH, "ttcompat" ) < 0 )
	   {
		   perror("ioctl( ttcompat ):");
		   return( -1 );
	   }

	   /* create a new session */
	   if( setsid() < 0 )
	   {
		   perror("setsid():");
		   return( -1 );
	   }

	   /* set controlling tty, to have job control */

	   int fd;

	   fd = open( slave, O_RDWR );

	   if( fd < 0 )
	   {
		   perror("open( slave )");
		   return( 46 );
	   }

	   close( tty );

	   tty = fd;

	   /* tty becomes stdin, stdout, stderr */
	   dup2( tty, 0 );
	   dup2( tty, 1 );
	   dup2( tty, 2 );

	   if( tty > 2 )
	   {
		   close( tty );
	   }

	   //		execl( "/bin/sh", "sh", (char *)0 );

	   return( 0 );
   }
   else
   {
	   return( pid );
   }

   /* not reached */
   return( -1 );
}
}  // namespace et
#endif // #if __sun__

/*
#define forkpty my_forkpty

int 
my_forkpty (int *amaster, char *name, 
	    void *unused1, void *unused2) { 
  int master, slave; 
  char *slave_name; 
  pid_t pid; 

  master = open("/dev/ptmx", O_RDWR); 
  if (master < 0) 
	  return -1; 

  if (grantpt (master) < 0) { 
    close (master); 
    return -1; 
  } 

  if (unlockpt (master) < 0) { 
    close (master); 
    return -1; 
  } 

  slave_name = ptsname (master); 
  if (slave_name == NULL) { 
    close (master); 
    return -1; 
  } 

  slave = open (slave_name, O_RDWR); 
  if (slave < 0) { 
    close (master); 
    return -1; 
  } 

  if (ioctl (slave, I_PUSH, "ptem") < 0 
		  || ioctl (slave, I_PUSH, "ldterm") < 0) { 
    close (slave); 
    close (master); 
    return -1; 
  } 

  if (amaster) 
    *amaster = master; 

  if (name) 
    strcpy (name, slave_name); 

  pid = fork (); 
  switch (pid) { 
    case -1: // Error / 
      return -1; 
    case 0: // Child / 
      close (master); 
      dup2 (slave, STDIN_FILENO); 
      dup2 (slave, STDOUT_FILENO); 
      dup2 (slave, STDERR_FILENO); 
      return 0; 
    default: // Parent / 
      close (slave); 
      return pid; 
  } 

  return -1; 
} */

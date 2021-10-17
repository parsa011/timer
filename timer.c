#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

int s , m , h;

struct termios orig_term;

void disable_raw_mode()
{
	tcsetattr(STDIN_FILENO,TCSAFLUSH,&orig_term);
}

void enable_raw_mode() 
{
	tcgetattr(STDIN_FILENO, &orig_term);

	atexit(disable_raw_mode);

	struct termios raw = orig_term;
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); 
}

void cls() 
{
	write(STDOUT_FILENO,"\x1b[2J",5);
}

void cls_row()
{
	write(STDOUT_FILENO,"\x1b[2K",5);
}

void refresh_cursor()
{
	write(STDOUT_FILENO,"\x1b[H",3);
}

void increase_timer() 
{
	s++;
	if (s >= 60) {
		s = 0;
		m++;
		if (m >= 60) {
			m = 0;
			h++;
		}
	}
}

void show_timer()
{
	printf("%d:%d:%d\n",h,m,s);
}

int main () 
{
	enable_raw_mode();
	cls();
	int stopped = 0;
	while (1) {
		refresh_cursor();
		cls_row();
		
		if (!stopped)
			increase_timer();
		show_timer();
		sleep(1);

		char c = '\0';
		read(STDIN_FILENO, &c, 1);
		if (c == 'q')
			break;
		else if (c == 's')
			stopped = !stopped;
	}
	return 0;
}

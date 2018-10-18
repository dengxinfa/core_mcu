#include "mode.h"

u16 mode_status;

u8 get_mode(u8 mode)
{
	if(mode_status & (0x01 << mode))
		return 1;
	else
		return 0;
}

void set_mode(u8 mode)
{
	mode_status |= 0x01 << mode;
}

void reset_mode(u8 mode)
{
	mode_status &= ~(0x01 << mode);
}
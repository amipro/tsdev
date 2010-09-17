/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/vt.h>

#include "utils.h"

int debug_level=0;
void set_debug(int level){
	debug_level=level;
}
void debug(const char *s,...)
{
	if(!debug_level)
		return;
	
    va_list va;
	va_start(va,s);

    vfprintf(stdout,s,va);

    va_end(va);
}

void error(const char *s,...)
{
    va_list va;
	va_start(va,s);

    vfprintf(stderr,s,va);

    va_end(va);
}

void syserror(const char *s,...)
{
    va_list va;
	va_start(va,s);

    vfprintf(stderr,s,va);

    va_end(va);
}


/* Copyright (C) 2000-2023 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

unsigned long
__udivmodsi4(unsigned long num, unsigned long den, int modwanted)
{
  unsigned long bit = 1;
  unsigned long res = 0;

  while (den < num && bit && !(den & (1L<<31)))
    {
      den <<=1;
      bit <<=1;
    }
  while (bit)
    {
      if (num >= den)
	{
	  num -= den;
	  res |= bit;
	}
      bit >>=1;
      den >>=1;
    }
  if (modwanted) return num;
  return res;
}

unsigned long long
__udivmoddi4(unsigned long long num, unsigned long long den, int modwanted)
{
  unsigned long long bit = 1;
  unsigned long long res = 0;

  while (den < num && bit && !(den & (1LL<<63)))
    {
      den <<=1;
      bit <<=1;
    }
  while (bit)
    {
      if (num >= den)
  {
    num -= den;
    res |= bit;
  }
      bit >>=1;
      den >>=1;
    }
  if (modwanted) return num;
  return res;
}

unsigned long
__umodsi3 (unsigned long a, unsigned long b)
{
  return __udivmodsi4 (a, b, 1);
}

unsigned long
__udivsi3 (unsigned long a, unsigned long b)
{
  return __udivmodsi4 (a, b, 0);
}

unsigned long long
__udivdi3 (unsigned long long n, unsigned long long d)
{
  return __udivmoddi4 (n, d, 0);
}

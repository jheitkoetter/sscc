/*
 *  sscc - Copyright (c) 2016 Joerg Heitkoetter (joke). All rights reserved.
 *
 *  Send bugs, comments, and all the fish to joke@verizon.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, 59 Temple Place--Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef lint
static char *rcsid = "$Id: sscc.c,v 1.1 2016/03/31 17:01:41 jabber Exp $";
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>

void note();
void usage ();
int luhn ();
int sscc ();
int cda ();

char *program_name = "sscc";
char *program_version = "1.0";
char *program_uri = "https://github.com/jheitkoetter/sscc.git";

int debug = 0;
int strict = 0;

void
main (argc, argv)
     int argc;
     char **argv;
{
  FILE *fp;
  int c;

  while ((c = getopt (argc, argv, "vds")) != EOF)
    switch (c)
      {
      case 'd':
	debug++;
	break;

      case 's':
	strict++;
	break;

      case 'v':
	note (EXIT_SUCCESS);
	break;

      default:
	usage (EXIT_FAILURE);
	break;
      }

  if (!(argc - optind))		/* nothing left */
    usage (EXIT_FAILURE);
  else
    for (; optind < argc; optind++)
      {
	cda (argv[optind]);
      }

  exit (EXIT_SUCCESS);
}

/* Secure Shipping Container Code, see: www.gs1.org
   http://www.gs1.org/check-digit-calculator */
int
sscc (s)
    char *s;
{
  /* allowable characters within identifier */
  char *valid = "0123456789";

  /* this will be a running total */
  int sum = 0;

  /* loop through digits from right to left */
  int len = strlen (s);
  for (int i = 0; i < len; i++) {

    /* set ch to "current" character to be processed */
    char ch = s[len - i - 1];

    /* throw exception for invalid characters*/
    if (strchr (valid, ch) == NULL) {
      fprintf (stderr, "Sorry, \"%s\" contains '%c' which is an invalid character\n", s, ch);
      exit (EXIT_FAILURE);
    }

    /* our "digit" is calculated using ASCII value - 48 */
    int digit = (int)ch - 48;

    /* weight will be the current digit's contribution to
       the running total */
    int weight;
    if (i % 2 == 0) {
      /* for alternating digits starting with the rightmost
         contribute 3 x their value */
      weight = (3 * digit);
    } else {
      /* even-positioned digits just contribute their value */
      weight = digit;
    }

    /* keep a running total of weights */
    sum += weight;
  }

  /* check digit is amount needed to reach next number divisible by ten */
  return (10 - (sum % 10)) % 10;
}

/* variation of Luhn's algorithm, which by default only allows 0-9 digits,
   see: https://wiki.openmrs.org/display/docs/Check+Digit+Algorithm */
int
luhn (s)
  char *s;
{

  /* allowable characters within identifier */
  char *valid = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_";

  /* this will be a running total */
  int sum = 0;
  int len = strlen (s);

  /* loop through digits from right to left */
  for (int i = 0; i < len; i++) {

    /* set ch to "current" character to be processed */
    char ch = toupper (s[len - i - 1]);

    /* throw exception for invalid characters*/
    if (strchr (valid, ch) == NULL) {
      fprintf (stderr, "Sorry, \"%s\" contains '%c' which is an invalid character\n", s, ch);
      exit (EXIT_FAILURE);
    }

    /* our "digit" is calculated using ASCII value - 48 */
    int digit = (int)ch - 48;

    /* weight will be the current digit's contribution to
       the running total */
    int weight;
    if (i % 2 == 0) {
      /* for alternating digits starting with the rightmost, we
         use our formula this is the same as multiplying x 2 and
         adding digits together for values 0 to 9.  Using the
         following formula allows us to gracefully calculate a
         weight for non-numeric "digits" as well (from their
         ASCII value - 48). */
      weight = (2 * digit) - (int) (digit / 5) * 9;
    } else {
      /* even-positioned digits just contribute their ascii
         value minus 48 */
      weight = digit;
    }

    /* keep a running total of weights */
    sum += weight;
  }

  /* avoid sum less than 10 (if characters below "0" allowed, this could happen) */
  if (sum < 0 )
    sum = sum * -1;

  /* check digit is amount needed to reach next number divisible by ten */
  return (10 - (sum % 10)) % 10;
}

int
cda (s)
  char *s;
{
  int len = strlen (s);

  if (debug)
    fprintf (stderr, "Check digit using Luhn on %s\n", s);
  fprintf (stdout, "%s-%d\n", s, luhn(s));

  if (debug)
    fprintf (stderr, "Check digit using SSCC on %s\n", s);

  /* in strict mode only accept defined lengths */
  if (strict && (len != 7 || len != 11 || len != 12 || len != 13 || len != 16 || len != 17)) {
    fprintf (stderr, "Sorry, \"%s\" has invalid length.\n", s);
    return (-1);
  } else {
    fprintf (stdout, "%s-%d\n", s, sscc(s));
  }
  return (0);
}

void
usage (code)
  int code;
{
  fprintf (stderr, "usage: %s [-dsv] string(s)\n", program_name);
  exit (code);
}

void
note (code)
  int code;
{
  fprintf (stderr, "This is %s, version %s, available at %s\n", program_name, program_version, program_uri);
  fprintf (stderr, "Copyright (c) 2016 Joerg Heitkoetter (joke). All rights reserved.\n");
  fprintf (stderr, "Send comments, bugs, and all the fish to <joke@verizon.com>\n");
  exit (code);
}

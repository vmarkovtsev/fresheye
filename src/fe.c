/*                     Fresh Eye </> Свежий Взгляд                   */
/*                   (с) OnMind Systems, 1994, 1995                  */

/*
  Compiler options:
    model = compact
    default char = unsigned
    nested comments = yes
*/

/*
Быстрая адаптация под 2016 год.

gcc -O2 -g -lm -funsigned-char fe.c -o fe
iconv -f utf-8 -t cp866 file.txt -o file.866.txt
./fe -a file.866.txt
iconv -f cp866 -t utf-8 fresheye.log -o fresheye2.log
cat fresheye2.log
*/


#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define PROGABOUT "Fresh Eye </> Свежий Взгляд  v1.21  (C) OnMind Systems, 1994, 95\n"
#define LOGNAME "fresheye.log"

#define WS 3000       /* wordcount storage size (items) */
#define MAXWLEN 20    /* maximum word length to be stored in wordcount */
#define MAXWIDTH 30
       /* maximum number of words checked (length of context) */
#define LETNO(x) if (*c<176) return (*c-'\xa0'); else return (16 + *c - '\xe0');
#define LINES 7
       /* number of lines stored */
#define VOCSIZE 65
       /* number of exceptions */
#define lnum(c)  (*c<176)?(*c-'\xa0'):(16+*c-'\xe0')
       /* letter number in the alphabet */

const short int sim_ch [32] [32] =

{                   /* letters' similarity map */
/* а б в г д е ж з и й к л м н о п р с т у ф х ц ч ш щ ъ ы ь э ю я */
  {9,0,0,0,0,1,0,0,1,0,0,0,0,0,2,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,1,2}, /* а */
  {0,9,1,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0}, /* б */
  {0,1,9,1,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,1,3,0,0,0,0,0,0,0,0,0,0,0}, /* в */
  {0,0,1,9,0,0,3,0,0,0,3,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0}, /* г */
  {0,0,0,0,9,0,0,1,0,0,0,0,0,0,0,0,0,1,3,0,0,0,1,0,0,0,0,0,0,0,0,0}, /* д */
  {1,0,0,0,0,9,0,0,2,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,2,1,1}, /* е */
  {0,0,0,3,0,0,9,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0}, /* ж */
  {0,0,0,0,1,0,3,9,0,0,0,0,0,0,0,0,0,3,1,0,0,0,3,1,1,1,0,0,0,0,0,0}, /* з */
  {1,0,0,0,0,2,0,0,9,3,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,2,0,1,1,1}, /* и */
  {0,0,0,0,0,0,0,0,2,9,0,1,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, /* й */
  {0,0,0,3,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0}, /* к */
  {0,0,1,0,0,0,0,0,0,1,0,9,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, /* л */
  {0,0,1,0,0,0,0,0,0,1,0,1,9,3,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, /* м */
  {0,0,1,0,0,0,0,0,0,1,0,1,3,9,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, /* н */
  {2,0,0,0,0,1,0,0,1,0,0,0,0,0,9,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,1,1}, /* о */
  {0,3,1,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0}, /* п */
  {0,0,0,0,0,0,0,0,0,1,0,1,1,1,0,0,9,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0}, /* р */
  {0,0,0,0,1,0,0,3,0,0,0,0,0,0,0,0,0,9,1,0,0,0,3,1,0,0,0,0,0,0,0,0}, /* с */
  {0,0,0,0,3,0,0,1,0,0,0,0,0,0,0,0,0,1,9,0,0,0,1,1,0,0,0,0,0,0,0,0}, /* т */
  {1,0,1,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,9,0,0,0,0,0,0,0,1,0,1,2,1}, /* у */
  {0,1,3,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0}, /* ф */
  {0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,9,0,1,0,0,0,0,0,0,0,0}, /* х */
  {0,0,0,0,1,0,0,3,0,0,0,0,0,0,0,0,0,3,1,0,0,0,9,0,0,0,0,0,0,0,0,0}, /* ц */
  {0,0,0,0,0,0,3,1,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,9,3,3,0,0,0,0,0,0}, /* ч */
  {0,0,0,0,0,0,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,9,3,0,0,0,0,0,0}, /* ш */
  {0,0,0,0,0,0,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,9,0,0,0,0,0,0}, /* щ */
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,3,0,0,0}, /* ъ */
  {1,0,0,0,0,1,0,0,2,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,9,0,1,1,1}, /* ы */
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,9,0,0,0}, /* ь */
  {1,0,0,0,0,3,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,9,1,1}, /* э */
  {1,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,2,0,0,0,0,0,0,0,1,0,1,9,1}, /* ю */
  {2,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,1,9}  /* я */
/* а б в г д е ж з и й к л м н о п р с т у ф х ц ч ш щ ъ ы ь э ю я */
};

const int inf_letters [32] [3] = {  /* quantity of information in letters */
                              /* relative, average = 1000 */
/*by itself - in the beginning of a word */
    {  802,    959 },  /* а */
    { 1232,   1129 },  /* б */
    {  944,    859 },  /* в */
    { 1253,   1193 },  /* г */
    { 1064,    951 },  /* д */
    {  759,   1232 },  /* е */
    { 1432,   1432 },  /* ж */
    { 1193,    993 },  /* з */
    {  802,    767 },  /* и */
    { 1329,   1993 },  /* й */
    { 1032,    929 },  /* к */
    {  967,   1276 },  /* л */
    { 1053,    944 },  /* м */
    {  848,    711 },  /* н */
    {  695,    853 },  /* о */
    { 1088,    454 },  /* п */
    {  929,   1115 },  /* р */
    {  895,    793 },  /* с */
    {  848,   1002 },  /* т */
    { 1115,   1129 },  /* у */
    { 1793,   1022 },  /* ф */
    { 1259,   1329 },  /* х */  /* [0] manually decreased! was 1359 */
    { 1593,   1393 },  /* ц */
    { 1276,   1212 },  /* ч */
    { 1476,   1012 },  /* ш */
    { 1676,   1676 },  /* щ */
    { 1993,   3986 },  /* ъ */
    { 1193,   3986 },  /* ы */
    { 1253,   3986 },  /* ь */
    { 1676,   1232 },  /* э */
    { 1476,   1793 },  /* ю */
    { 1159,    967 }   /* я */
  };

const char *voc [VOCSIZE] [2] = {       /* exceptions vocabulary */

#include "voc.cp866.inc"

  {"", ""},
  {"", ""}
};

const char *prname = "Fresh Eye </> Свежий Взгляд";
const char *rule = "==================================================================";

char fname [256];                  /* input file name */
char fout [256];                   /* input file name */
char wrd [34];                     /* current word */
char *wpoint = wrd;
char s [255];                      /* current line */
char sprevs [LINES] [255];         /* previous lines */
char sforw [255];                  /* next line (not always read in) */
int forw = 0;                      /* 1 = next line is read in, 2 = eof */
int eofi = 0;                      /* eof */
char *sp;                          /* current pointer */
FILE *f, *fo;                      /* input, output files */
int code = 0;                      /* cyrillic coding scheme (not used) */
int lcnt = 0;                      /* line counter */
int wcnt = 0;                      /* word counter */
int cries, ogos;                   /* counters: found, recorded */

typedef struct {int co; char *po;} cp;
                   /* word item: frequency, char pointer */
int l0, l1, l2;                    /* temporals for fastening wordcmp */
char *list;                   /* array of words counted */
/* char list [50000]; */                     /* [WS * MAXWLEN]; */
int i [WS];
cp  i1 [WS];

int width = 9;                     /* length of the context (default) */
int twosigmasqr;                   /* coefficient for Gaussian distribution */
int razd [MAXWIDTH];               /* separators */
int ra, rb;
char context [MAXWIDTH] [MAXWLEN];       /* recent words */
int spaces, glpar;                 /* spaces flag, global \par flag */
int flush = 0, wcount = 1,
    dumpcount = 0;                 /* various modes */
int fstr = 0;                      /* first line of file to process */
int quiet;                         /* quiet mode for nextword when used by wordcount */
int not_all_words_counted;
int wcsize;
int usecount = 50;                 /* percent of using the wordcount info */

#if 0
union REGS r;                      /* used for keyboard handling */
#endif
int cbrk;                          /* stores BREAK value (ON/OFF) */

int threshold = 600;               /* sensitivity threshold (default) */
int wordmode = 0, wordwrap = 70;
int readlog = 0;
int resume_no = 0, resume_yes = 0;
int new_sentence = 1,              /* used to catch proper names */
    immediate_new = 0,
    proper_no = 0;

/* ------------------------------------------------------ */

#define sqr(x) (x * x)
#define min(a, b) (a < b? a : b)

/* ------------------------------------------------------ */
/*
    Clears keyboard buffer.
*/

void kbcl (void) {

}

/* ----------------------------------------------------- */
/*
    General query function.
    Positive: 'Y', 'Д'; Negative: 'N', <Esc> (caseproof)
    Include:
        <ctype.h>
*/

int whatkey (void) {

  register char c;

  kbcl ();

  while (1) {

    if  ((c = toupper (getc (stdin))) == 'Y' ||
         c == 132 ||
         c == 164 ||
         c == 13
        ) return (1);
    if  (c == 'N' || c == ' ')
         return (0);
    if  (c == 27)
         return (2);
    if (c == 'A')
         return (3);
  }
}

/* ----------------------------------------------------- */

int ask (const char *string) {

  if (string)
    printf ("%s", string);
  printf ("? (y/n/all) ");
  return (whatkey ());

}

/* ------------------------------------------------------ */
/*
    Checks for pressing <Esc>. If any other key
    pressed, clears keyboard buffer.
    Globals:
      union REGS r;        it shouldn't interfere with cursor management
                            functions (cur_init, cur_off & cur_on)
    Includes:
      <dos.h>
*/

int esc_pressed (void) {

/*  if (bioskey (1) == -1)
    return (1);
*/

  return (0);
}

/* ------------------------------------------------------ */

short isletter (char *c) {

 switch (code) {
        /* the rudiment of the epoch of many cyrillic code tables */
   case 0:
        /* alternative */
     return ((*c > 127 && *c < 176) || (*c > 223 && *c < 240));
   case 1:
     return (*c > 175 && *c < 240);
   case 2:
     return (*c > 127 && *c < 192);
 }
 return (0);
}

/* ------------------------------------------------------ */

short iscap (char *c) {

  if (code == 0 || code == 2)
    return (*c > 127 && *c < 160);
  return (*c > 175 && *c < 208);
}

/* ------------------------------------------------------ */

short islow (char *c) {

  return (isletter (c) && !iscap (c));
}

/* ------------------------------------------------------ */

void upc (char *c) {

  if (!islow (c))
    return;
  if (code || *c < 176)
    *c -= 32;
  else
    *c -= 80;
}

/* ------------------------------------------------------ */

void  downc (char *c) {

  if (!iscap (c))
    return;
  if (code || *c < 144)
    *c += 32;
  else
    *c += 80;
}

/* ------------------------------------------------------ */

void toggle (char *c) {

  if (iscap (c))
    downc (c);
  else
    upc (c);
}

/* ------------------------------------------------------ */

short letters (char *s, char l) {

  while (l --)
    if (!isletter (s + l))
      return (0);
  return (1);
}

/* ------------------------------------------------------ */

char *fgs (char *p, FILE *fp) {

  char *c;
  int el = 0;  /* end of line */

  if ((wordmode && !readlog) ? fgets (p, wordwrap, fp) : fgets (p, 255, fp)) {
    c = p + strlen (p) - 1;
    while (*c == '\n' || *c == '\r') {
      *c -- = 0;
      el = 1;
    }
    if (wordmode && !readlog && !el && strlen (p) == wordwrap - 1) {  /* cutting long lines in word mode */
      for (el = 0;
           *(c - el) != ' ' && *(c - el) != '\t' && (c - el) >= p;
           el ++);
      if (el < wordwrap - 1) {
        *(c - el + 1) = 0;
        fseek (fp, (long) -el, SEEK_CUR);
      }
    }
    if (wordmode) {
      while (c = strchr (p, 0x1f)) /* deleting optional hyphen chars */
        strcpy (c, c + 1);
      if (*p == '.')
        if (c = strchr (p + 1, '.'))
          strcpy (p, c + 1);
    }
    return (p);
  } return (NULL);
}

/* ------------------------------------------------------ */

char *move (void) {

  int t;

  for (t = 0; t < LINES - 1; t ++)
    strcpy (sprevs [t], sprevs [t + 1]);
  strcpy (sprevs [LINES - 1], s);

  if (forw == 2)
    return (NULL);        /* eof */
  if (forw) {
    strcpy (s, sforw);    /* use stored string */
    forw = 0;
  } else
    if (!fgs (s, f))      /* read string */
      return (NULL);      /* eof */

  lcnt ++;

  return (s);
}

/* ------------------------------------------------------ */

void unify_word (char *a) {
  assert(a);

  char *p = a + strlen (a) - 1;

  while (!isletter (p))
    *p -- = 0;

  while (!isletter (a))
    strcpy (a, a + 1);

  for (p = a; *p; p ++)
    downc (p);
}

/* ------------------------------------------------------ */

int raz (char *p) {

  int res = 0;

  if (quiet) return (0);
  if (immediate_new && new_sentence)
    immediate_new = new_sentence = 0;

  switch (*p) {
    case ' ':
      if (!spaces) {
        res ++;
        spaces = 1;
      }
      break;
    case ',':
      res += 2;
      break;
    case '.':
      res += 4;
      new_sentence = 1;
      break;
    case ';':
      res += 3;
      break;
    case ':':
      res += 3;
      break;
    case '!':
      res += 4;
      new_sentence = 1;
      break;
    case '?':
      res += 4;
      new_sentence = 1;
      break;
    case ')':
      res += 3;
      break;
    case '(':
      res += 3;
      break;
    case '"':
      res += 3;
      new_sentence = 1;
      immediate_new = 1; /* to switch new_sentence off
                            if the word doesn't follow immediately */
      break;
    case '-':
      if (spaces)
        res += 3;
      else
        res ++;
      break;
    case '^':
      break;
    default:
      if (!spaces) {
        res ++;
    spaces = 1;
      }
      break;
  }
  return (res);
}

/* ------------------------------------------------------ */

char *nextword (void) {

  register char *p = wrd;
  int newline = 0;
  int par = 0;

  spaces = ra = 0;

  while (! isletter (sp) && *sp)
    ra += raz (sp ++);

  while (sp >= s + strlen (s)) {
    if (! move ())
      return (NULL);
    if (!newline)
      newline = 1;
    else {
    par = 1;
    glpar = 1;
    }
    sp = s;
    while (! isletter (sp) && *sp)
      ra += raz (sp ++);
  }

  if (par || (wordmode && newline)) ra += 8;

  while (isletter (sp) && p - wrd < 32)
    *p ++ = *sp ++;
  *p = 0;

  immediate_new = 0;
  if (new_sentence)
    new_sentence = 0;
  else if (!quiet && !proper_no && iscap (wrd) && islow (wrd + 1))
         /* probably a proper name */
    strcpy (wrd, "\xae");
         /* mock the word to prohibit any actions */

  unify_word (wrd);
  razd [width - 1] = ra;
  wcnt ++;
  if (!quiet) {
    printf ("Слово %u  ", wcnt);
    if (flush)
      printf (" Срабатываний %u\n", cries);
    if (esc_pressed ())
      return (NULL);
  }
  return (sp);
}

/* ------------------------------------------------------ */

short wordcmp (const char *w1) {

  if (*w1 != *wpoint)
    return (0);

  /* register */
  l0 = 0;
  /* register */
  l1 = strlen (w1);
  /* register */
  l2 = strlen (wpoint);

/*  if (strcmp (w1, wpoint) == 0)
    return (1);
  return (0);
*/

  if ((abs(l1 - l2)) > 2)
    return (0);

  if (min(l1, l2) <= 2) {
    if (strcmp (w1, wpoint) == 0)
      return (1);
    else
      return (0);
  }

  while (*(w1 + l0) && *(wpoint + l0) && (*(w1 + l0) == *(wpoint + l0)))
    l0 ++;

  if (abs (l1 - l2) == 2 ||
      (abs (l1 - l2) == 1 && min(l1, l2) <= 3)
     )
    if (*(w1 + l0) != 0 && *(wpoint + l0) != 0)
      return (0);

  if (l0 > (min(l1, l2) / 2))
    return (1);
  return (0);
}

/* ------------------------------------------------------ */

int comp (const void *w1, const void *w2) {

  register cp *ww1 = (cp *) w1, *ww2 = (cp *) w2;

  if ((ww1 -> co) > (ww2 -> co))
    return (-1);
  if ((ww1 -> co) < (ww2 -> co))
    return (1);
  if (*(ww1 -> po) > *(ww2 -> po))
    return (1);
  if (*(ww1 -> po) < *(ww2 -> po))
    return (-1);
  return (0);
}

/* ------------------------------------------------------ */

int icomp (int *i1, int *i2) {

  if (*i1 > *i2)
    return (-1);
  if (*i1 < *i2)
    return (1);
  return (0);
}

/* ------------------------------------------------------ */

int checkvoc (char *w1, char *w2) {

  register int t;

  for (t = 0; t < VOCSIZE; t ++)
    if ((!strcmp (w1, voc [t] [0])) && (!strcmp (w2, voc [t] [1])))
      return (1);

  return (0);
}

/* ------------------------------------------------------ */

int simch (char *a, char *b) {    /* accessing the table */

   return (sim_ch [lnum (a)] [lnum (b)]);

}

/* ------------------------------------------------------ */

#if 0
int comppart (char *a, char *b) {  /* similarity of word fragments */

  register int res = 0;

  while (*a != 0)
    res += sqr (sim_ch [lnum (a ++)] [lnum (b ++)]);

  return (res);
}
#endif

/* ------------------------------------------------------ */

int implen (int x) { /* psych. importance of the word x ch. long */
                     /* big for small words, then slowly lagging
                        behind the real length */

  if (x == 2) return (5);
  return (x - sqr ((x - 1) / 6) + (int) (4.1 / (float) x));

}

/* ------------------------------------------------------ */

int infor (char *a, char *b) {   /* calculates quantity of information */

  int count = 0;
  int res = 0;
  int beg = 1;
  char *p, *pp = a;

  while (*pp) {                      /* bipresent letters - add */
    if (p = strchr (b, *pp)) {
      if (beg && (p == b))
    res += inf_letters [lnum (pp)] [1];
      else
    res += inf_letters [lnum (pp)] [0];
      count ++;
    }
    beg = 0;
    pp ++;
  }

  pp = a;                     /* letters in a only - subtract */
  while (*pp) {
    if (!(p = strchr (b, *pp))) {
      if (pp == a)
    res += 2000 - inf_letters [lnum (pp)] [1];
      else
    res += 2000 - inf_letters [lnum (pp)] [0];
      count ++;
    }
    pp ++;
  }

  pp = b;                     /* letters in b only - subtract */
  while (*pp) {
    if (!(p = strchr (a, *pp))) {
      if (pp == b)
    res += 2000 - inf_letters [lnum (pp)] [1];
      else
    res += 2000 - inf_letters [lnum (pp)] [0];
      count ++;
    }
    pp ++;
  }

  if (count)
    return (res / count);
  else
    return (0);
}

/* ------------------------------------------------------ */

int simwords (char *a, char *b) {  /* yields similarity of words */

  register char *tx, *ty, *ta, *tb;
  char parta [64] = {0};
  unsigned long int res = 0;
  unsigned long int resa = 0;
  int part, partmax, partlen;
  register int prir, tmp;
  int lena, lenb;
  int rever = 0;
  int dist;

  if (checkvoc (a, b)) /* an exception? */
    return (0);

  if (strlen (a) > strlen (b)) {
                       /* a must be always the shortest */
   ta = a;
    a = b;
    b = ta;
    rever = 1;         /* set the reverse position flag */
  }
  lena = strlen (a);   /* remember lengths to speed up calculations */
  lenb = strlen (b);

  for (partlen = 1;  partlen <= lena;  partlen ++, resa = 0) {
                       /* compare fragments from 1 to strlen(a) ch. long */
    for (ta = a;  (lena - (int) (ta - a)) >= partlen;  ta ++) {

      strncpy (parta, ta, partlen);
      for (tb = b;  partlen <= (lenb - (int) (tb - b));  tb ++) {

/*        if ((prir = comppart (parta, tb)) == 0)
          continue;  */    /* comppart fetched here to speed up */

        for (prir = 0, tx = parta, ty = tb; *tx != 0; tx ++, ty ++) {
                     /* this loop seems to be the most critical
                        part of the whole algorithm */
#if 0
          tmp = sim_ch [lnum (tx)] [lnum (ty)];
                     /* accessing the table */
          prir += tmp * tmp;
          if (tmp == 2) prir --; /* experiment! */
                     /* squaring values to sharpen similarity */
#endif
          prir += sim_ch [lnum (tx)] [lnum (ty)];
                     /* accessing the table */
        }

        if (prir == 0) continue;

        if (ta > a)
          prir -= (prir * (ta - a)) / (3 * lena);
        if (tb > b)
          prir -= (prir * (tb - b)) / (3 * lenb);
                     /* decreasing value towards the word end */

        if ((dist = (rever) ? (lenb - (tb - b + partlen)) + (ta - a)
                            : (lena - (ta - a + partlen)) + (tb - b)
            ) < 3)
          prir += ((prir * (2 - dist)) / 3);
                     /* dist is the shortest distance between the fragments
                        supposing the words are adjacent */

        if (prir > resa)
      resa = prir;
                     /* resa accumulates the greatest value */
      }
    }
    if (resa / partlen > 6) {
                     /* is the density more than some threshold? */
      prir = resa;
      dist = 3 * (lena + lenb) / 8 + 1;
                     /* here dist is 3/4 of average length of the words */
/*      res += resa + prir * (partlen - dist / 2) / (2 * dist);  */
      res += resa + prir * (partlen - min(dist, lena)) / (2 * dist);
                     /* sharpening the dependance on the fragment length */

    }
  }

  for (partlen = 1, resa = 0;  partlen <= lena;  partlen ++)
    resa += 9 * partlen;
              /* now resa is the maximal value that res could have for
                 the words of these lengths */

  res = ((res * infor (a, b)) / resa);
              /* allowing for the info contained in the letters */
  res -= (res * (lenb - lena)) / (2 * lenb);
              /* decreasing if words are too different in length */
  return (res * strlen (a) * strlen (b) / (implen (strlen (a)) * implen (strlen (b))));
              /* finally, taking into account the psychological length */

}

/* ------------------------------------------------------ */

int show (int num, int bad, int sim, int dist) {

  int t = LINES;
  int inw,   /* "inside a word" flag */
      nnum,
      otw;   /* to keep the user's responce */
  char *st1, *wr1;  /* string and word to show, first */
  char *st2, *wr2;  /* string and word to show, second */
  char *point;

  st2 = s;
  for (wr2 = sp - 1; isletter (wr2) && (wr2 >= st2); wr2 --);  wr2 ++;
    /* rewind st2 to the beginning of current line */

  st1 = st2;
  wr1 = wr2;
  inw = 0;

  nnum = num = width + 1 - num;
  while (num > 1) {
    wr1 --;
    if (wr1 < st1) {
      if (inw) {
        inw = 0;
        num --;
        continue;
      }
      st1 = sprevs [-- t];
      if (t < 0)
    return (0);
      wr1 = st1 + strlen (st1) - 1;
      if (isletter (wr1))
        inw = 1;
      else
        inw = 0;
    }
    if (!isletter (wr1))
      if (inw) {
        inw = 0;
    num --;
      }
    if (isletter (wr1))
      if (!inw)
        inw = 1;
  }
  wr1 ++;

  if ((glpar) && (glpar + 1 <= nnum)) {
                        /* there's a \par between the words */
    if ((t > 0) && (*sprevs [t - 1] == 0))
                        /* \par comes before the first word */
      return (0);
    if (!forw)
      if (!fgs (sforw, f))   /* read ahead */
    forw = 2;         /* end of file encountered */
      else
    forw = 1;

    if (forw != 2) {
      if (*sforw == 0) /* \par comes after the second word */
    return (0);
    }
  }

  cries ++;
  if (flush) goto logging;  /* skip screen stuffing */
  if (st1 == st2) {         /* pendant le meme ligne */
    printf ("\r");
    for (point = st1; point < wr1; point ++)
      (*point == '\t') ? printf ("\t") : putc (*point, stdout);
    for (; isletter (point); point ++)
      printf ("%c", *point);
    for (; point < wr2; point ++)
      (*point == '\t') ? printf ("\t") : putc (*point, stdout);
    for (; isletter (point); point ++)
      putc (*point, stdout);
    for (; *point; point ++)
      (*point == '\t') ? printf ("\t") : putc (*point, stdout);
  } else {                  /* deux lignes differents */
    printf ("\r");
    for (point = st1; point < wr1; point ++)
      (*point == '\t') ? printf ("\t") : putc (*point, stdout);
    for (; isletter (point); point ++)
      putc (*point, stdout);
    for (; *point; point ++)
      (*point == '\t') ? printf ("\t") : putc (*point, stdout);

    printf ("\r\n");
    inw = t + 1;
    while (inw <= LINES - 1) {
      printf ("%s\r\n", sprevs [inw ++]);
    }

    for (point = st2; point < wr2; point ++)
      (*point == '\t') ? printf ("\t") : putc (*point, stdout);
    for (; isletter (point); point ++)
      putc (*point, stdout);
    for (; *point; point ++)
      (*point == '\t') ? printf ("\t") : putc (*point, stdout);
  }

  printf ("\r\n                         line %u   sim = %u  dist = %u  badness = %u", lcnt, sim, dist, bad);
  if ((otw = ask ("\rЗапомнить")) == 1 || otw == 3) {
logging:
    ogos ++;
    if (st1 == st2) {
      fprintf (fo, "\r\n%s\r\n", st1);
      for (point = st1; point < wr1; point ++)
        if (*point != '\t')
          fprintf (fo, " ");
        else
          fprintf (fo, "\t");
      for (; isletter (point); point ++)
        fprintf (fo, "^");
      for (; point < wr2; point ++)
        if (*point != '\t')
          fprintf (fo, " ");
        else
          fprintf (fo, "\t");
      for (; isletter (point); point ++)
        fprintf (fo, "^");
    } else {
      fprintf (fo, "\r\n%s\r\n", st1);
      for (point = st1; point < wr1; point ++)
        if (*point != '\t')
          fprintf (fo, " ");
        else
          fprintf (fo, "\t");
      for (; isletter (point); point ++)
        fprintf (fo, "^");
      fprintf (fo, "\r\n");
      inw = t + 1;
      while (inw <= LINES - 1)
        fprintf (fo, "%s\r\n", sprevs [inw ++]);
      fprintf (fo, "%s\r\n", st2);
      for (point = st2; point < wr2; point ++)
        if (*point != '\t')
          fprintf (fo, " ");
        else
          fprintf (fo, "\t");
      for (; isletter (point); point ++)
        fprintf (fo, "^");
    }
    fprintf (fo, "\r\n                        line %u   sim = %u  dist = %u  badness = %u\r\n", lcnt, sim, dist, bad);
    if (flush) goto end;
    if (otw == 3) {
      flush = 1;
      putc ('A', stdout);
    } else {
      putc ('Y', stdout);
    }
  }
  else
    if (otw == 0) {
      putc ('N', stdout);
    } else
      return (1);

  printf ("\n\n");
end:
  return (0);
}

/* ------------------------------------------------------ */

int inf_w (char *w) {
     /* returns information quantity in a word basing on the wordcount */

  char *save = wpoint;
  register int t = 0;

  wpoint = w;
  while (t < wcsize) {
    if (wordcmp (i1[t].po)) {
      wpoint = save;
      return (i1[t].co);
    }
    t ++;
  }

  wpoint = save;
  return (1000);
}

/* ------------------------------------------------------ */

int check (void) {

  int t, t1;
  int similarity;
  int badness;
  double dal;
  long dist;

  for (t = 0; t < width; t ++) /* compare with every word within
                                  the context */
    if (*context [t]) {

      if (!(similarity = simwords (context [t], wrd)))
        continue;

      for (t1 = t, dist = 0; t1 < width; t1 ++)
        dist += razd [t1];
      t1 = t + 1;
      while (t1 < width)
        dist += ((strlen (context [t1 ++])) / 3 + 1);
                 /* calculating the adduced context distance */

      if (wcount) {  /* allowing for the wordcount info */
        dist *= 2000;
        dist /= (inf_w (context [t]) + inf_w (wrd));
      }

      dal = exp (((double) (- dist * dist)) / ((double) twosigmasqr));

      badness = (int) (((float) similarity) * dal);

      if (badness > threshold)
        if (show (t, badness, similarity, dist))
          return (1);
/*      printf ("\n%s <%u> %s: %u (стр. %u)\n", context [t], dist, wrd, badness, lcnt);  */
    }
    return (0);
}

/* ------------------------------------------------------ */

void shift (void) {

  int t;

  for (t = 0; t < width - 1; t ++) {
    strcpy (context [t], context [t + 1]);
    razd [t] = razd [t + 1];
  }

  if (glpar)
    glpar ++;
  if (glpar > width)
    glpar = 0;

  strcpy (context [width - 1], wrd);
  razd [width - 1] = ra;
}

/* ------------------------------------------------------ */

void eggog () {

  puts ("!  Bad argument");
  exit (1);
}

/* ------------------------------------------------------ */

void displ_help () {

  puts ("  Usage: fe [options] <filename> [logname=fresheye.log]\n");
  puts ("    options:");
  puts ("    -l<nn>    length of context (default = 9, max = 30)");
  puts ("    -s<nn>    sensitivity threshold (default = 600)");
  puts ("    -c<nn>    coefficient of using wordcount information");
  puts ("              (0..100, 0 = off, default = 50)");
  puts ("    -a        output into log file (fresheye.log) without queries");
  puts ("    -d        dump wordcount into log file");
  puts ("    -w[<nn>]  MS Word mode (default for *.doc)");
  puts ("              nn: page width (characters, default = 70)");
  puts ("    -p        do not exclude proper names");
  puts ("    -ry       resume processing the file");
  puts ("    -rn       process the file anew from the beginning");
  exit (0);
}

/* ------------------------------------------------------ */

int check_log (void) {

  int mid = 0;

  if (resume_no)
    return (1);

  if ((fo = fopen (fout, "ab+")) == NULL)
    return (1);

  while (!feof (fo)) {
    fgs (s, fo);
    if (memcmp (s, prname, strlen (prname)) == 0) {
                         /* this is really fresheye! */
      if (!memcmp (fname, strstr (s, "Файл:") + 6, strlen (fname)))
        mid = 2;
    }
    if (mid && memcmp (s, rule, strlen (rule)) == 0) {
                         /* if we ran across the ========= line */
      mid --;
      if (! mid) {
        if (fgs (s, fo))
          fstr = atoi (s + 7);   /* Строк: xxx */
      }                          /*        ^   */
    }
  }

  mid = 0;
  if (fstr >= 1) {
    if (!resume_yes)
      printf ("!  Файл %s проверен до строки %u.\r\n   Продолжить (Y) или начать сначала (N)? ", fname, fstr);
    if (resume_yes || (mid = whatkey ())) {  /* tricky: whatkey should not be called if resume_yes */
      if (mid == 2)
        exit (0);
      if (!resume_yes) {
        putc ('Y', stdout);
      }
      while (lcnt < fstr && !feof (f)) {    /* wind up f to the fstr'th line */
        fgs (s, f);
        lcnt ++;
      }
      if (!resume_yes)
        printf ("\r\n\r\n");
      return (fstr);
    } else {
      putc ('N', stdout);
      printf ("\r\n\r\n");
      return (0);
    }
  }
  return (fstr);
}

/* ------------------------------------------------------ */

void cbrk_handler (int sig_type) {

/*  puts ("^C"); */  /* turned out to be redundant */
  exit (1);
}

/* ------------------------------------------------------ */

int wordcount (void) { /* compiling the wordcount */

  char *w, *p;
  register unsigned int l, ll;
  double tmp;

  register int l0, l1, l2;      /* temporals for fastening wordcmp */

  wcsize = 0;

/*  printf ("%lu", farcoreleft ()); */

  if ((list = (char *) malloc (((unsigned long) MAXWLEN) * WS)) == NULL) {
    puts ("!  Cannot allocate memory");
    exit (0);
  }
  memset ((void *) list, 0, (unsigned) MAXWLEN * WS);

  #if 0
  cbrk = getcbrk ();                 /* this stuff placed here 'cause */
  setcbrk (1);                       /* it's after most accidental exits */
  signal (SIGINT, cbrk_handler);     /* but before any long operations */
  #endif
  printf ("Compiling the wordcount...\n");
  quiet = 1;

  *s = 0;
  sp = s;
  while (nextword ()) {

    if (strlen (wrd) >= MAXWLEN)
      continue;

    for (l = 0, ll = WS * MAXWLEN;
         *(p = list + l) && l < ll;
         l += MAXWLEN)
    {
/*    if (wordcmp (p))   */
            /* wordcmp fetched here to speed it up */

/* former wordcmp start */

/* short wordcmp (const char *w1) {  */

      if (*p != *wpoint)
        continue;                    /* return (0); */

      l0 = 0;
      l1 = strlen (p);
      l2 = strlen (wpoint);

      if ((abs(l1 - l2)) > 2)
        continue;                    /* return (0); */

      if (min(l1, l2) <= 2)
        if (strcmp (p, wpoint) == 0)
          goto yes;                  /* return (1); */
        else
          continue;                  /* return (0); */

      while (*(p + l0) && *(wpoint + l0) && (*(p + l0) == *(wpoint + l0)))
        l0 ++;                       /* twist forward while the words do coincide */

      if (abs (l1 - l2) == 2 || (abs (l1 - l2) == 1 && min(l1, l2) <= 3))
        if (*(p + l0) != 0 && *(wpoint + l0) != 0)
          continue;                  /* return (0); */

      if (l0 <= (min(l1, l2) / 2))
        continue;                    /* return (0); */

/* } */
/* end of former wordcmp */

yes:
      i [l / MAXWLEN] ++;
/*    strcpy (p, wrd);  */
                    /* considered harmful :) */
                    /* seriously, just unnecessary and time-consuming */
      goto e;
    }

    if (l >= ll) {
      not_all_words_counted = 1;
/*    puts ("\rNo more memory."); */
      break;
    }

    strcpy (p, wrd);
    i [l0 = l / MAXWLEN] = 1;    /* what a mess! sorry, "optimization"... */
    wcsize = l0;

e:  if (esc_pressed ()) {
      lcnt = wcnt = 0;
      return (1);
    }
  }

  for (l = ll = 0; l <= wcsize; l ++) /* composing the list */
    if (i [l] > 1) {                  /* criteria: occured twice or more */
      i1[ll].co = i [l];
      i1[ll].po = list + (unsigned) l * MAXWLEN;
      ll ++;
    }
  wcsize = ll;

  printf ("Sorting...\n");
  qsort (i1, ll, sizeof (int) + sizeof (char *), comp);
           /* at least compact model is assumed */
  printf ("Calculating...\n");

  if (dumpcount) {
    fprintf (fo, "=== WORDCOUNT\r\n");
    fprintf (fo, "=== Lines: %i %s\r\n=== Word occurrences: %i\r\n=== Words listed: %i\r\n",
      lcnt, not_all_words_counted ? "(could not process all of file)" : "", wcnt, wcsize);
    for (l = 0; l < wcsize; l ++) {
      fprintf (fo, "%s\t%s%s%i\r\n", i1[l].po,
        (strlen (i1[l].po) > 7) ? "" : "\t",
    (strlen (i1[l].po) > 15) ? " " : "\t ",
        i1[l].co);
    }
    fprintf (fo, "=== END WORDCOUNT\r\n\r\n");
  }

  fseek (f, wordmode ? 0x80 : 0, SEEK_SET);
  new_sentence = 1;

  for (l = 0; l < wcsize; l ++) {
               /* recalculating the vocabulary */
    tmp = ((double) i1[l].co) / ((double) wcnt);
    i1[l].co = 1 - log (tmp) * 1000 / ((8.0 + ((float) usecount) / 13) * log (2));
            /* ^ in case the text repeats one word only */      /* ^^ decreasing this will sharpen
                                                                 the dependance on the -c coefficient */
    if (i1[l].co > 1000)
      i1[l].co = 1000;

/*    i1[l].co = 1000 - (((long) i1[l].co) * ((long) 20000 * usecount / 100)) / wcnt;
*/
  }

  wcnt = lcnt = quiet = 0;   /* reset all counters */
  printf ("\r");
  while (lcnt < fstr && !feof (f)) {
    fgs (s, f); lcnt ++;
  }
  return (0);
}

/* ------------------------------------------------------ */

void main (char argc, char **argv) {

  int t, fcount = 0;
  char c, help = 0;
  time_t ts;
  struct tm dt;

/*
  while (1) {
    gets (s1);
    gets (s2);
    if (*s1 == 0 || *s2 == 0)
      break;
    t = (int) (1.11 * (float) simwords (s1, s2));
    if (t > 400)
      printf ("%s & %s = %u\n", s1, s2, t);
    else
      printf ("%u\n", t);
  }
*/

  puts (PROGABOUT);
  if (argc < 2) {
    displ_help ();
  }

  for (c = 1; c < argc; c ++)
  {
    if (**(argv + c) == '-')
    {
      switch (toupper (*(*(argv + c) + 1)))
      {
        case '?':
          help = 1;
          break;
        case 'A':
          flush = 1;
          break;
        case 'C':
          usecount = atoi (*(argv + c) + 2);
          if (usecount == 0)
            wcount = 0;
          break;
        case 'D':
          dumpcount = 1;
          break;
        case 'S':
          threshold = atoi (*(argv + c) + 2);
          if (threshold == 0)
            eggog ();
          break;
        case 'L':
          width = atoi (*(argv + c) + 2);
          if (width == 0 || width > MAXWIDTH)
            eggog ();
          break;
        case 'W':
          wordmode = 1;
          if (*(*(argv + c) + 2) == 0)
            break;
          if (isdigit (*(*(argv + c) + 2))) {
            wordwrap = atoi (*(argv + c) + 2);
            break;
          }
          eggog ();
          break;
        case 'P':
          proper_no = 1;
          break;
        case 'R':
          switch (toupper (*(*(argv + c) + 2))) {
            case 'N':
              resume_no = 1;
              break;
            case 'Y':
              resume_yes = 1;
              break;
            default:
              eggog ();
          }
          break;
        default:
          eggog ();
      }
    } else {
      fcount++;
      if (fcount == 1)
        strcpy (fname, *(argv + c));
      else
        strcpy (fout, *(argv + c));
    }
  }

  if (help)
    displ_help ();

  if (dumpcount && !wcount)
    eggog ();

  if (strcasecmp (fname + strlen (fname) - 4, ".doc") == 0)
    wordmode = 1;

  if (!(f = fopen (fname, "rb")))
  {
    puts ("!  Cannot open file");
    exit (1);
  }

  if (wordmode)
    fseek (f, 0x80, SEEK_SET);

  twosigmasqr = 2 * sqr (width * 4);

  if (fcount < 2 || strcmp(fname, fout) == 0)
    strcpy (fout, LOGNAME);
  readlog = 1;
  fstr = check_log ();
  readlog = 0;

  fo = fopen (fout, "ab");
  if (!fo)
  {
    puts ("!  Cannot open log file");
    exit (1);
  }
  ts = time(NULL);
  dt = *localtime(&ts);
  fprintf (fo, "\r\n\r\n%s  v1.2\t Файл: %s%s%02i.%02i.%i\r\n",
            prname, fname, (strlen (fname) > 8) ? " " : "\t",
            dt.tm_mday, 1 + dt.tm_mon, 1900 + dt.tm_year);
  fprintf (fo, "%s", rule);
  fprintf (fo, "\r\n");

  if (wcount == 0 || wordcount () == 0)
  {
    *s = 0;
    sp = s;
    while (nextword ())
    {         /* the main program's loop */
      if (check ())
        break;
      shift ();
    }
  }

  fprintf (fo, "\r\n");
  fprintf (fo, "%s", rule);
  fprintf (fo, "\r\nСтрок: %u  Начало: %u  Слов: %u      Срабатываний: %u  Записано: %u\r\n", lcnt, fstr, wcnt, cries, ogos);
  fclose (fo);
}

/*  GARBAGE

char ch_groups [8] [7] [4] =
{
{{кхг} {гж} {} {} {} {} {}}
{ая эе ую иы оа еи ий}
{й лмн р}
{бп вф}
{дт сзц}
{жшщч жз}
{ъь}

float ch_freq [32] = { }

*/

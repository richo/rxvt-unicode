#ifndef _RXVTLIB_H_             /* include once only */
#define _RXVTLIB_H_

#include <cstdio>
#include <cctype>
#include <cerrno>
#include <cstdarg>
#include <cstdlib>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
/* #include <util.h> */
#include <assert.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <sys/select.h>
/* #include <sys/strredir.h> */

#include <sys/wait.h>
#include <sys/stat.h>

#if HAVE_CWCHAR
# include <cwchar>
#elif HAVE_WCHAR_H
# include <wchar.h>
#else
// stdlib.h might provide it
#endif

#include "ptytty.h"

extern "C" {
#include <X11/Intrinsic.h>      /* Xlib, Xutil, Xresource, Xfuncproto */
}

/*
 * If we haven't pulled in typedef's like  int16_t  then do them ourself
 * type of (normal and unsigned) basic sizes
 */
/* typedef short int16_t; */
/* typedef unsigned short uint16_t; */
/* typedef int int32_t; */
/* typedef unsigned int uint32_t; */

/* whatever normal size corresponds to a integer pointer */
#define intp_t int64_t
/* whatever normal size corresponds to a unsigned integer pointer */
#define u_intp_t u_int64_t

struct rxvt_fontset;
struct rxvt_color;
struct rxvt_vars;               /* defined later on */
struct rxvt_term;
struct rxvt_display;
struct rxvt_im;
struct rxvt_drawable;

typedef struct rxvt_term *rxvt_t;

extern rxvt_t rxvt_current_term;

#define SET_R(r) rxvt_current_term = (r)
#define GET_R rxvt_current_term

#define scrollbar_visible()    scrollBar.state
#define menubar_visible()      menuBar.state

typedef struct {
  int row;
  int col;
} row_col_t;

#if UNICODE_3
typedef uint32_t text_t;
#else
typedef uint16_t text_t; // saves lots of memory
#endif
typedef uint32_t rend_t;
typedef  int32_t tlen_t;  // was int16_t, but this results in smaller code and memory use
typedef  int32_t tlen_t_; // specifically for use in the line_t structure

struct line_t;

/*
 * terminal limits:
 *
 *  width      : 1 <= width
 *  height     : 1 <= height
 *  ncol       : 1 <= ncol       <= MAX(tlen_t)
 *  nrow       : 1 <= nrow       <= MAX(int)
 *  saveLines  : 0 <= saveLines  <= MAX(int)
 *  total_rows : nrow + saveLines
 *  nsaved     : 0 <= nsaved     <= saveLines
 *  term_start : 0 <= term_start < saveLines
 *  view_start : 0 <= view_start < saveLines
 *
 *          | most coordinates are stored relative to term_start,
 *  ROW_BUF | which is the first line of the terminal screen
 *  |························= row_buf[0]
 *  |························= row_buf[1]
 *  |························= row_buf[2] etc.
 *  |
 *  +------------+···········= term_start - nsaved
 *  | scrollback |                                      
 *  | scrollback +---------+·= term_start - view_start
 *  | scrollback | display |
 *  | scrollback | display |
 *  +------------+·display·+·= term_start
 *  |  terminal  | display |
 *  |  terminal  +---------+
 *  |  terminal  |
 *  |  terminal  |
 *  +------------+···········= term_stat + nrow - 1
 *  |
 *  |
 *  END······················= total_rows
 */

struct TermWin_t {
  int            width;         /* window width                    [pixels] */
  int            height;        /* window height                   [pixels] */
  int            fwidth;        /* font width                      [pixels] */
  int            fheight;       /* font height                     [pixels] */
  int            fbase;         /* font ascent (baseline)          [pixels] */
  int            ncol;          /* window columns              [characters] */
  int            nrow;          /* window rows                 [characters] */
  int            focus;         /* window has focus                         */
  int            mapped;        /* window state mapped?                     */
  int            int_bwidth;    /* internal border width                    */
  int            ext_bwidth;    /* external border width                    */
  int            lineSpace;     /* number of extra pixels between rows      */
  int            saveLines;     /* number of lines that fit in scrollback   */
  int            total_rows;    /* total number of rows in this terminal    */
  int            nsaved;        /* number of rows saved to scrollback       */
  int            term_start;    /* term lines start here                    */
  int            view_start;    /* scrollback view starts here              */
  Window         parent[6];     /* parent identifiers - we're parent[0]     */
  Window         vt;            /* vt100 window                             */
  GC             gc;            /* GC for drawing                           */
  Pixmap         pixmap;
  rxvt_drawable *drawable;
  rxvt_fontset  *fontset[4];
};

/*
 * screen accounting:
 * screen_t elements
 *   row:       Cursor row position                   : 0 <= row < nrow
 *   col:       Cursor column position                : 0 <= col < ncol
 *   tscroll:   Scrolling region top row inclusive    : 0 <= row < nrow
 *   bscroll:   Scrolling region bottom row inclusive : 0 <= row < nrow
 *
 * selection_t elements
 *   clicks:    1, 2 or 3 clicks - 4 indicates a special condition of 1 where
 *              nothing is selected
 *   beg:       row/column of beginning of selection  : never past mark
 *   mark:      row/column of initial click           : never past end
 *   end:       row/column of one character past end of selection
 * * Note: -nsaved <= beg.row <= mark.row <= end.row < nrow
 * * Note: col == -1 ==> we're left of screen
 *
 */
typedef struct {
  row_col_t       cur;          /* cursor position on the screen             */
  int             tscroll;      /* top of settable scroll region             */
  int             bscroll;      /* bottom of settable scroll region          */
  unsigned int    charset;      /* character set number [0..3]               */
  unsigned int    flags;        /* see below                                 */
  row_col_t       s_cur;        /* saved cursor position                     */
  unsigned int    s_charset;    /* saved character set number [0..3]         */
  char            s_charset_char;
  rend_t          s_rstyle;     /* saved rendition style                     */
} screen_t;

enum selection_op_t {
  SELECTION_CLEAR = 0,  /* nothing selected                          */
  SELECTION_INIT,       /* marked a point                            */
  SELECTION_BEGIN,      /* started a selection                       */
  SELECTION_CONT,       /* continued selection                       */
  SELECTION_DONE        /* selection put in CUT_BUFFER0              */
};

typedef struct {
  wchar_t          *text;       /* selected text                             */
  unsigned int      len;        /* length of selected text                   */
  unsigned int      screen;     /* screen being used                         */
  unsigned int      clicks;     /* number of clicks                          */
  selection_op_t    op;         /* current operation                         */
  bool              rect;       /* rectangluar selection?                    */
  row_col_t         beg;        /* beginning of selection   <= mark          */
  row_col_t         mark;       /* point of initial click   <= end           */
  row_col_t         end;        /* one character past end point              */
} selection_t;

/* ------------------------------------------------------------------------- */

/* screen_t flags */
#define Screen_Relative          (1<<0)  /* relative origin mode flag         */
#define Screen_VisibleCursor     (1<<1)  /* cursor visible?                   */
#define Screen_Autowrap          (1<<2)  /* auto-wrap flag                    */
#define Screen_Insert            (1<<3)  /* insert mode (vs. overstrike)      */
#define Screen_WrapNext          (1<<4)  /* need to wrap for next char?       */
#define Screen_DefaultFlags      (Screen_VisibleCursor | Screen_Autowrap)

/* rxvt_vars.options */
enum {
# define def(name,idx) Opt_ ## name = 1UL << (idx),
# define nodef(name)   Opt_ ## name = 0,
# include "optinc.h"
# undef nodef
# undef def
};

/* ------------------------------------------------------------------------- */

typedef struct {
  short           state;
  Window          win;
  struct rxvt_drawable *drawable;
} menuBar_t;

typedef struct {
  char            state;        /* scrollbar state                          */
  char            init;         /* scrollbar has been initialised           */
  unsigned int    beg;          /* slider sub-window begin height           */
  unsigned int    end;          /* slider sub-window end height             */
  unsigned int    top;          /* slider top position                      */
  unsigned int    bot;          /* slider bottom position                   */
  unsigned int    style;        /* style: rxvt, xterm, next                 */
  unsigned int    width;        /* scrollbar width                          */
  Window          win;
  int             (rxvt_term::*update)(int, int, int, int);

  void setIdle()   { state =  1 ; }
  void setMotion() { state = 'm'; }
  void setUp()     { state = 'U'; }
  void setDn()     { state = 'D'; }
} scrollBar_t;

struct rxvt_vars : TermWin_t {
  scrollBar_t     scrollBar;
  menuBar_t       menuBar;
  uint32_t        options;
  XSizeHints      szHint;
  rxvt_display   *display;
  rxvt_color     *pix_colors;
  rxvt_color     *pix_colors_focused;
#ifdef OFF_FOCUS_FADING
  rxvt_color     *pix_colors_unfocused;
#endif
  short           numpix_colors;
  Cursor          TermWin_cursor;       /* cursor for vt window */
  int             sb_shadow;    /* scrollbar shadow width                    */
  rxvt_ptytty     pty;
  int             numlock_state;
  line_t         *row_buf;      // all lines, scrollback + terminal, circular, followed by temp_buf
  line_t         *drawn_buf;    // text on screen
  line_t         *swap_buf;     // lines for swap buffer
  char           *tabs;         /* per location: 1 == tab-stop               */
  screen_t        screen;
  screen_t        swap;
  selection_t     selection;
};

void rxvt_init ();
bool rxvt_tainted ();

#endif                          /* _RXVTLIB_H_ */


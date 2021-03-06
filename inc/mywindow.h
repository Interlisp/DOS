/* @(#) MyWindow.h Version 1.2 (6/1/90). copyright Venue   */

/************************************************************************/
/*									*/
/*			    M y W i n d o w . h				*/
/*									*/
/*									*/
/*									*/
/************************************************************************/



/************************************************************************/
/*									*/
/*	(C) Copyright 1989-92 Venue. All Rights Reserved.		*/
/*	Manufactured in the United States of America.			*/
/*									*/
/*	The contents of this file are proprietary information 		*/
/*	belonging to Venue, and are provided to you under license.	*/
/*	They may not be further distributed or disclosed to third	*/
/*	parties without the specific permission of Venue.		*/
/*									*/
/************************************************************************/


#define VERTICAL   0
#define HORIZONTAL 1

#define PERCENT_OF_SCREEN 95
#define SCROLL_PITCH      30

typedef struct _MyEvent
  {
    int       type; 		/* Event type */
    int     (*func)();     	/* Pointer to Event Handler */
    struct _MyEvent *next;	/* Pointer to next event */
  } MyEvent;

typedef struct _MyWindow
  {
    char      *name;	/* name of this window */
    Window     win;         /* window id */
    int        x,y;         /* x and y coordinates */
    int        width,height;/* window size */
    int        border;      /* border width */
    GC        *gc; 		/* Current GC */
    unsigned   long event_mask;
    Cursor    *cursor;      /* Current Cursor */
    int       (*before_create)();        /* Pointer to Initializer */
    int       (*after_create)();
    int	      (*before_resize)();        /* Pointer to Configurator */
    int	      (*after_resize)(); 
    int       (*event_func)();           /* Pointer to Event Handler */
    MyEvent   *event_head;
    struct _MyWindow *parent;      /* pointer to parent window */
    struct _MyWindow *next;	/* pointer of next window */
  } MyWindow;

typedef struct
  {
    int left_x;  /* x cordinate of upperleft corner */
    int top_y;   /* y cordinate of upperlert corner */
    int right_x; /* x cordinate of lowerright corner */
    int bottom_y;/* y cordinate of lowerright corner */
  } DisplayArea;

#define CreateWindow(parent_win,child_win) { \
		if( parent_win && child_win ) { \
			if( (child_win)->before_create ) \
			   ((child_win)->before_create)(parent_win,child_win); \
 			(child_win)->win = XCreateSimpleWindow( Xdisplay \
						, (parent_win)->win \
						, (child_win)->x \
						, (child_win)->y \
						, (child_win)->width \
						, (child_win)->height \
						, (child_win)->border \
						, Black_Pixel \
						, White_Pixel ); \
			XFlush( Xdisplay ); 	\
			(child_win)->parent = parent_win; \
			if( (child_win)->after_create ) \
			    ((child_win)->after_create)(parent_win,child_win);\
		} \
}

#define ResizeWindow( window ) { \
		if( window ) { \
			if( (window)->before_resize ) \
				((window)->before_resize)( window ); \
			XMoveResizeWindow( Xdisplay \
					, (window)->win \
					, (window)->x \
					, (window)->y \
					, (window)->width \
					, (window)->height ); \
			XFlush( Xdisplay ); 	\
			if( (window)->after_resize ) \
				((window)->after_resize)( window ); \
		} \
}

#define DefineCursor(display, window,mycursor) { \
		XDefineCursor( display, (window)->win, *(mycursor) ); \
		XFlush( display ); 	 \
		(window)->cursor = mycursor; \
}


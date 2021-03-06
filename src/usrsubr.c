/* @(#) usrsubr.c Version 1.2 (4/22/92). copyright venue & Fuji Xerox  */
static char *id = "@(#) usrsubr.c	1.2 4/22/92	(venue & Fuji Xerox)";





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

/** User defined subrs here.  Do NOT attempt to use this unless you FULLY
    understand the dependencies of the LDE architecture.                 **/

#define DO_UFN	{return(-1);}

UserSubr(user_subr_index, num_args, args)

int user_subr_index;
int num_args;
int args[28];

{
int result = 0;

/* *** remove the printf when finished debugging your user subr *** */

	printf("debug: case: 0x%x, args: 0x%x\n",user_subr_index, num_args);
	{int i;
	 for (i = 0; i < num_args; i++) printf("debug: arg[%d]: 0x%x\n",i,args[i]);
	};


	switch (user_subr_index) {

		case 0:	 printf("sample UFN\n"); result = args[0]; break;
		default: DO_UFN;
		}

	return(result);
}


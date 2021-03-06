/* @(#) ufs.c Version 2.44 (9/18/90). copyright venue & Fuji Xerox  */
static char *id = "@(#) ufs.c	2.44 9/18/90		(venue & Fuji Xerox)";




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


#include	<sys/types.h>
#ifndef DOS
#include	<sys/param.h>
#ifndef HPUX
#include	<strings.h>
#endif /* HPUX */

#include	<sys/file.h>
#ifndef INDIGO
	/* Indigo has this duplicated in dirent.h, sigh. */
#include	<sys/dir.h>
#endif /* INDIGO */
#include	<sys/stat.h>
#include	<sys/time.h>
#ifndef AIX
#include	<sys/vfs.h>
#endif /* AIX */

#ifdef SYSVONLY
#include	<dirent.h>
#include	<unistd.h>
#endif /* SYSVONLY */

#include	<pwd.h>
#else /* DOS */
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <dos.h>
#include <string.h>
#define MAXPATHLEN _MAX_PATH
#define MAXNAMLEN  _MAX_PATH
#define index strchr
#define rindex strrchr
#define alarm(x) 1
#endif /* DOS */

#include	<setjmp.h>
#include	<fcntl.h>
#include	<stdio.h>
#include	<errno.h>
#include	"lispemul.h"
#include	"lispmap.h"
#include	"adr68k.h"
#include	"lsptypes.h"
#include	"lspglob.h"
#include	"arith.h"
#include	"stream.h"
#include	"timeout.h"
#include	"locfile.h"
#include	"dbprint.h"

int	*Lisp_errno;
int	Dummy_errno;	/* If errno cell is not provided by Lisp, dummy_errno is used. */




/***********************************************/
/*  file-system-specific defns                 */
/***********************************************/
#ifdef DOS
#define DIRSEP '\\'
#define DIRSEPSTR "\\"
#else
#define DIRSEP '/'
#define DIRSEPSTR "/"
#endif
#define DRIVESEP ':'

   /* Used to limit DOS filenames to 8.3 format */

#ifdef DOS
#define NameValid extensionp ? (extlen < 3) : (namelen < 8)
#define CountNameChars { extensionp ? extlen++ : namelen++; }

#else
   /* Other file systems don't care */
#define NameValid 1
#define CountNameChars

#endif /* DOS */


/*		
 * Name:	UFS_getfilename
 *
 * Argument:	LispPTR	*args	args[0]
 *				 Full file name in Lisp format.
 *				args[1]
 *				 Rcoginition mode.  See IRM.
 *				args[2]
 *				 Name area where the recognized full file name
 *				 will be stored.
 *				args[3]
 *				 The place where the error number should be
 *				 stored.
 *
 * Value:	If succeed, returns the Lisp smallp which represents the length
 *		of the recognized full file name, otherwise Lisp NIL.
 *
 * Side Effect:	If succeed, name area (args[2]) will be replaced with the 
 *		recognized full file name.
 *
 * Description:
 *
 * The implementation of GETFILENAME FDEV method for UNIX device.  Performs the
 * recognition on the specified name.  Does not check if OPENFILE actually
 * can open the file with the specified mode or not.
 */

UFS_getfilename(args)
	register LispPTR	*args;
{
	register char	*base;
	register int	len, rval;
	char		lfname[MAXPATHLEN], file[MAXPATHLEN];

	ERRSETJMP(NIL);
	Lisp_errno = (int *)(Addr68k_from_LADDR(args[3]));
	
	LispStringLength(args[0], len, rval);
	len += 1;		/* Add 1 for tuminating NULL char. */
	if (len > MAXPATHLEN) FileNameTooLong(NIL);

	LispStringToCString(args[0], lfname, MAXPATHLEN);
	/*
	 * Convert a Lisp file name to UNIX one.  This is a UNIX device method.
	 * Thus we don't need to convert a version field.  Third argument for
	 * unixpathname specifies it.
	 */
	if (unixpathname(lfname, file, 0, 0, 0, 0, 0) == 0) return(NIL);

	switch (args[1]) {

	      case RECOG_OLD:
	      case RECOG_OLDEST:
		/*
		 * "Old" and "Oldest" means the "existing" file.  All we have to do
		 * is to make sure it is an existing file or not.
		 */
		TIMEOUT(rval = access(file, F_OK));
		if (rval == -1) {
			*Lisp_errno = errno;
			return(NIL);
		}
		break;

	      case RECOG_NEW:
	      case RECOG_OLD_NEW:
	      case RECOG_NON:
		/*
		 * "New" file means the "not existing" file.  UNIX device always
		 * recognizes a not existing file as if, the subsequent OPENFILE will
		 * find the truth.
		 * "Non" reognition is used to recognize a sysout file.
		 */
		break;
	}
	/*
	 * Now, we convert a file name back to Lisp format.  The version field have not
	 * to be converted.  The fourth argument for lisppathname specifies it.
	 */
	if (lisppathname(file, lfname, 0, 0) == 0) return(NIL);

	STRING_BASE(args[2], base);
	len = strlen(lfname);

#ifndef BYTESWAP
	strncpy(base, lfname, len + 1);
#else
	StrNCpyFromCToLisp(base, lfname, len + 1);
#endif /* BYTESWAP */


	return(GetSmallp(len));
}




/*		
 * Name:	UFS_deletefile
 *
 * Argument:	LispPTR	*args	args[0]
 *				 Full file name in Lisp format.
 *				args[1]
 *				 The place where the error number should be
 *				 stored.
 *
 * Value:	If succeed, returns the Lisp symbol T, otherwise Lisp NIL.
 *
 * Side Effect:	If succeed, the specified file is unlinked.
 *
 * Description:
 *
 * The implementation of DELETEFILE FDEV method for UNIX device.  Try to delete
 * a specified file.
 */

UFS_deletefile(args)
	register LispPTR	*args;
{
	char			file[MAXPATHLEN], fbuf[MAXPATHLEN];
	register int		len, rval;
	
	ERRSETJMP(NIL);
	Lisp_errno = (int *)(Addr68k_from_LADDR(args[1]));
	
	LispStringLength(args[0], len, rval);
	len += 1;
	if (len > MAXPATHLEN) FileNameTooLong(NIL);

	LispStringToCString(args[0], fbuf, MAXPATHLEN);
	unixpathname(fbuf, file, 0, 0, 0, 0, 0);

	/*
	 * On UNIX device, all we have to do is just to unlink the file.
	 */
	TIMEOUT(rval = unlink(file));
	if (rval == -1) {
		*Lisp_errno = errno;
		return(NIL);
	}

	return(ATOM_T);
}




/*		
 * Name:	UFS_renamefile
 *
 * Argument:	LispPTR	*args	args[0]
 *				 Full file name in Lisp format.  The file which
 *				 is being renamed.
 *				args[1]
 *				 Full file name in Lisp format.  The file to which
 *				 args[0] is being renamed.
 *				args[2]
 *				 The place where the error number should be
 *				 stored.
 *
 * Value:	If succeed, returns the Lisp symbol T, otherwise Lisp NIL.
 *
 * Side Effect:	If succeed, the specified file is unlinked.
 *
 * Description:
 *
 * The implementation of RENAMEFILE FDEV method for UNIX device.  Try to rename
 * a specified file.
 */

UFS_renamefile(args)
	register LispPTR	*args;
{
	char			fbuf[MAXPATHLEN], src[MAXPATHLEN], dst[MAXPATHLEN];
	int			rval, len;
	
	ERRSETJMP(NIL);
	Lisp_errno = (int *)(Addr68k_from_LADDR(args[2]));
	
	LispStringLength(args[0], len, rval);
	len += 1;
	if (len > MAXPATHLEN) FileNameTooLong(NIL);

	LispStringLength(args[1], len, rval);
	len += 1;
	if (len > MAXPATHLEN) FileNameTooLong(NIL);

	LispStringToCString(args[0], fbuf, MAXPATHLEN);
	unixpathname(fbuf, src, 0, 0, 0, 0, 0);
	LispStringToCString(args[1], fbuf, MAXPATHLEN);
	unixpathname(fbuf, dst, 0, 0, 0, 0, 0);

	TIMEOUT(rval = rename(src, dst));
	if (rval == -1) {
		*Lisp_errno = errno;
		return(NIL);
	}

	return(ATOM_T);
}




/*
 * Name:	UFS_directorynamep
 *
 * Argument:	LispPTR	*args	args[0]
 *				 Directory name in Lisp format.  Both of the initial
 *				 and trail directory delimiter are stripped by Lisp
 *				 code.  Only one exception is a "root directory".
 *				 "Root directory is represented as ">".
 *				args[1]
 *				 The place where the "true" name of the directory
 *				 in Lisp format will be stored.
 *				args[2]
 *				 The place where the error number should be stored.
 *				 Not use in the current Lisp code implementation.
 *
 * Value:	If succeed, returns the Lisp smallp which represents the length
 *		of the "true" name of the directory, otherwise Lisp NIL.
 *
 * Side Effect:	If the directory is recognized as a valid directory representation,
 *		args[1] is replaced with the "true" directory name.
 *
 * Description:
 *
 * The implementation of the DIRECTORYNAMEP FDEV method for UNIX device.
 * Performs the recoginition as well. Accepts the directory representation which
 * obeys the Xerox Lisp file naming convention. The "true" name which is stored
 * on the area specified with the second argument also follows the Xerox Lisp
 * file naming convention, and it includes the initial and trail directory
 * delimiter. Thus the Lisp code does not have to worry about the conversion of
 * the directory name representation.
 */

UFS_directorynamep(args)
 	register LispPTR	*args;
{
	char		dirname[MAXPATHLEN];
	char		fullname[MAXPATHLEN];
	register int	len, rval;
	register char	*base;
	struct stat	sbuf;

	ERRSETJMP(NIL);
	Lisp_errno = (int *)(Addr68k_from_LADDR(args[2]));
	
	LispStringLength(args[0], len, rval);
	len += 1;
	/* -2 for the initial and trail directory delimitor. */
	if (len > MAXPATHLEN - 2) FileNameTooLong(NIL);

	LispStringToCString(args[0], dirname, MAXPATHLEN);

	/* Convert Xerox Lisp file naming convention to Unix one. */
	if (unixpathname(dirname, fullname, 0, 0, 0, 0, 0) == 0) return(NIL);

	TIMEOUT(rval = stat(fullname, &sbuf));
	if (rval == -1) {
		*Lisp_errno = errno;
		return(NIL);
	}

	if ((sbuf.st_mode & S_IFMT) != S_IFDIR) return(NIL);
 
	/* Convert Unix file naming convention to Xerox Lisp one. */
	if (lisppathname(fullname, dirname, 1, 0) == 0) return(NIL);
 
	len = strlen(dirname);
	STRING_BASE(args[1], base);
 
#ifndef	BYTESWAP
	strncpy(base, dirname, len + 1);
#else
	StrNCpyFromCToLisp(base, dirname, len + 1);
#endif /* BYTESWAP */

 
	return(GetSmallp(len));
}




/*		
 * Name:	unixpathname
 *
 * Argument:	char	*src	Xerox Lisp syntax pathname.
 *				The HOST name field is not included.
 *				The initial directory delimiter is not included and
 *				if the pathname is passed as a directory, the
 *				tail delimiter may be included.
 *		char	*dst	The buffer to which the converted pathname is stored.
 *		int	versionp
 *				If 1, version field in src is converted to UNIX
 *				version form.  {DSK} device invokes unixpathname
 *				with versionp.
 *		int	genp	If 1, it indicates unixpathname is called from
 *				directory enmumerateion routine.  In this case,
 *				trail period which is used to specify an empty
 *				extension field is treated specially.
 *
 * Value:	If succeed, returns 1, otherwise 0.
 *
 * Side Effect:	None.
 *
 * Description:
 *
 * Converts the Xerox Lisp syntax pathname to Unix syntax pathname.
 * Src pathname might start with the one of the meta characters. And it
 * might include Xerox Lisp pathname quote notation.  The characters
 * must be quoted in the Xerox Lisp file naming convention are valid in the UNIX
 * file naming convention.  So only skipping the quote character would be
 * sufficient.
 * If the trail directory delimiter, '>', is included in src, dst will also include
 * UNIX trail directory delimiter '/' or DOS delim '\'.
 *
 */

unixpathname(src, dst, versionp, genp
#ifdef DOS
	     , drive, extlenptr, rawname
#endif /* DOS */
	     )
	register char	*src;
	register char	*dst;
	register int	versionp;
	register int	genp;
#ifdef DOS
  char *drive;	/* The DOS drive designator */
  int *extlenptr;	/* points to int for length fo the extension */
  char *rawname;	/* holds the raw file name, for later version hacking */
#endif /* DOS */
  {
    register char	*cp, *dp, *np, *rp;
    register int	newdirflg;
    char		name[64];
    char		lfname[MAXPATHLEN], fbuf1[MAXPATHLEN], fbuf2[MAXPATHLEN];
    char		ver1[VERSIONLEN], ver2[VERSIONLEN];
    struct passwd	*pwd;

#ifdef DOS
    int namelen = 0, extlen = 0;	/* lengths of name & extension */
    int extensionp = 0;		/* T if we're in the extension */
    int version = 1;		/* version # for this file */
#endif /* DOS */


    /* If there's a drive letter, it and a colon come first */
#ifdef DOS
    if (drive && (*drive))
      {
	*dst++ = *drive;
	*dst++ = DRIVESEP;
      }
#endif /* DOS */
    /*
     * The UNIX root directory is represented as "<" in Xerox Lisp generic
     * file system code.
     */

    if (strcmp(src, "<") == 0)
      {
	strcpy(dst, DIRSEPSTR);
	return(1);
      }


    /* Copy src to protect it from destructive modification. */
    strcpy(lfname, src);

    /*
     * If versionp is specified, we have to deal with the version field first,
     * because the quotation mark which quotes the semicolon might be lost
     * in the course of the following conversion.
     */

#ifdef DOS
    if (versionp) LispVersionToUnixVersion(lfname, version);
#else
    if (versionp) LispVersionToUnixVersion(lfname);
#endif /* DOS */

    cp = lfname;
    dp = dst;

    /*
     * We have to deal with the case in which the pathname is started with
     * the meta character ('.', '~').
     */

    switch (*cp)
      {
		
	case '.':
	  switch (*(cp + 1))
	    {

	      case '.':
		if (*(cp + 2) == '>' || *(cp +2) == '\0' || *(cp+2) == DIRSEP)
		  {
		    /*
		     * "..>" or ".." means the parent directory of the
		     * user's current working directory.
		     */
		    if (getwd(dst) == 0) return(0);

#ifdef DOS
		    dp = max(rindex(dst, '/'), rindex(dst,DIRSEP));
#else
		    dp = rindex(dst, DIRSEP);
#endif
		
		    dp++;
		    if (*(cp + 2) == '\0') cp += 2;
		    else  cp += 3;
		  }
		else
		  {
		    /* Insert the initial directory delimiter. */
		    *dp++ = DIRSEP;
		  }
		break;

#ifdef DOS
	      case DIRSEP:
	      case '/':
#endif /* DOS */
	      case '>':
		/* ".>" means the user's current working directory. */
		if (getwd(dst) == 0) return(0);
		while (*dp != '\0') dp++;

		*dp++ = DIRSEP;
		cp += 2;
		break;
			
	     case '\0':
		/* "." also means the user's current working directory. */
		if (getwd(dst) == 0) return(0);
		while (*dp != '\0') dp++;

		*dp++ = DIRSEP;
		cp++;
		break;

	      default:
		/* Insert the initial directory delimiter. */
		*dp++ = DIRSEP;
		break;
	    }
	  break;
#ifndef DOS
	case '~':
	  if (*(cp + 1) == '>' || *(cp +  1) == '\0')
	    {
	      /* "~>" or "~" means the user's home directory. */
	      TIMEOUT(pwd = getpwuid(getuid()));
	      if (pwd == NULL) return(0);

	      strcpy(dst, pwd->pw_dir);
	      while (*dp != '\0') dp++;
	      if (*(dp -1) != DIRSEP)
		{
		  /*
		   * Usually system administrators specify the users'
		   * home directories in the /etc/passwd without
		   * the trail directory delimiter.
		   */
		  *dp++ = DIRSEP;
		}
	      if (*(cp + 1) == '\0') cp++;
	      else cp += 2;
	    }
	  else
	    {
	      /*
	       * In this case, we assume some user's home directory
	       * is specifed in the form "~username".
	       */
	      for (++cp, np = name; *cp != '\0' && *cp != '>';) *np++ = *cp++ ;
	      *np = '\0';
	      TIMEOUT(pwd = getpwnam(name));
	      if (pwd == NULL) return(0);

	      strcpy(dst, pwd->pw_dir);
	      while (*dp != '\0') dp++;
	      if (*(dp -1) != DIRSEP)
		{
		  /*
		   * Usually system administrators specify the users'
		   * home directories in the /etc/passwd without
		   * the trail directory delimiter.
		   */
		  *dp++ = DIRSEP;
		}

	      if (*cp == '>') cp++;
	    }
	  break;
#else
	/* For DOS, ignore ~> or ~/ or ~ */
	case '~':
	  if (*(cp + 1) == '>' || *(cp +  1) == '\0')
	    {
	      /* "~>" or "~" means the user's home directory. */

	      *dp++ = DIRSEP;
	      if (*(cp + 1) == '\0') cp++;
	      else cp += 2;
	    }
	  else
	    {
	      /*
	       * In this case, we assume some user's home directory
	       * is specifed in the form "~username".
	       */
	      for (++cp, np = name; *cp != '\0' && *cp != '>';) *np++ = *cp++ ;
	      *dp++ = DIRSEP;

	      if (*cp == '>') cp++;
	    }
	  break;

#endif /* DOS */

	default:
	  *dp++ = DIRSEP;	/* Insert the initial directory delimiter. */
	  break;
      }

    /*
     * At this point, cp is placed at the point from which the source pathname
     * will be scaned, and dp is placed at the point on dst from which the
     * pathname will be copied.
     */

    newdirflg = 1;
    while (*cp != '\0')
      {
	if (newdirflg)
	  {
	    /*
	     * The new directory component starts.  We have to care about
	     * the meta characters again.  This time, the tilde character
	     * has no special meaning.
	     */
	    switch (*cp)
	      {

		case '.':
		  switch (*(cp + 1))
		    {

		      case '.':
			/* "..>" or ".." */
			if (*(cp + 2) == '>' || *(cp + 2) == '\0')
			  {
			    /*
			     * We have to check if we have already
			     * backed to the root directory or not.
			     */
			    if ((dp - 1) != dst)
			      {
				/*
				 * We are not at the root
				 * directory.  Back to the
				 * parent directory.
				 */
				for (dp -= 2; (*dp != '/') && (*dp != DIRSEP); dp--) {}
				dp++;
			      }
			    if (*(cp + 2) == '\0') cp += 2;
			    else cp += 3;
			  }
			else
			  {
			    /*
			     * (IL:DIRECTORY "{DSK}.") is translated
			     * as (IL:DIRECTORY "{DSK}~>.;*").
			     * The Lisp directory is translated as
			     * like "/users/akina/..~*~" by
			     * unixpathname.   Although such
			     * file name representaion makes no sense,
			     * to avoid infinite loop, skip the
			     * first period here, as well as down
			     * a newdirflg.
			     */
			    cp++;
			    newdirflg = 0;
			  }
			break;

		      case '>':
			/* ".>" */
			cp += 2;
			break;

		      case '\0':
			/* "." */
			cp++;
			break;

		      default:
			*dp++ = *cp++;
			newdirflg = 0;
			break;
		    }
		  break;

		case '\'':
		  /*
		   * The first character of the new directory component
		   * is a quotation mark which is the quote character
		   * in the Xerox Lisp file naming convention.  Copy the
		   * next character and skip the quoted character.
		   */
		  *dp++ = *(cp + 1);
		  cp += 2;
		  newdirflg = 0;
		  break;

		default:
		  *dp++ = *cp++;
		  newdirflg = 0;
		  break;
	      }
	    }
	  else
	    { /* scanning a plain file name, as best we can tell */
	      switch (*cp)
		{

#ifdef DOS
		  case '/':	/* in DOS, must xlate / also */
#endif /* DOS */
		  case '>':
		    /*
		     * Xerox Lisp directory delimiter '>' is translated into
		     * UNIX one, '/'.
		     */
		    *dp = DIRSEP;
		    dp++;
		    cp++;
		    newdirflg = 1; /* Turn on the new directory flag. */
#ifdef DOS
		    namelen = extlen = 0;
		    rp = dp;	/* remember where the raw file name starts */
#endif /* DOS */
		    break;
				
		  case '\'':
		    /*
		     * The special characters in the Xerox Lisp file naming
		     * convention are quoted with the quote character.
		     * They are all valid in the UNIX file naming convention.
		     * So only we have to do is to skip the quotation mark
		     * and copy the next character.
		     */
		    if (NameValid) *dp++ = *(cp + 1);
		    CountNameChars;
		    cp += 2;
		    break;
#ifdef DOS
		  case '.':	/* start of extension, if not already */
		    if (!extensionp) *dp++ = *cp++; else cp++;
		    extensionp = 1;
#endif /* DOS */    break;

		  default:
		    if (NameValid) *dp++ = *cp++; else cp++;
		    CountNameChars;
		    break;
		}
	      }
	    }

    *dp = '\0';
#ifdef neverDOS
    if (rawname) strcpy(rawname, rp);
    if (extlenptr) *extlenptr = extlen;
#endif /* DOS */

    if (!newdirflg && !genp)
      {
	/*
	 * If the last chracter in dst is a period, it have to be handled
	 * specially, because it might be used to specify that src has no
	 * extension field.  This case can be distinguish by examining the
	 * character just before the period.
	 * If the specified pathname is one like "~>..", the last meta character
	 * matches this case.  Thus we check newdirflg first so as not to be
	 * confused by this case.
	 *
	 * Only case in which genp is 1 is unixpathname is used to convert
	 * a pattern which is used to enumerate directory.  In this csae,
	 * for the convinience of the pattern matching routines, we don't
	 * care about the last period character.
	 */
	strcpy(fbuf1, lfname);
	strcpy(fbuf2, dst);
	separate_version(fbuf1, ver1, 1);
	separate_version(fbuf2, ver2, 1);
	for (cp = fbuf1; *cp; cp++) {}
	for (dp = fbuf2; *dp; dp++) {}
	if (*(cp - 1) == '.')
	  {
	    if (*(cp - 2) != '\'' ||
	        ((cp - 3) > fbuf1 && *(cp - 3) == '\''))
	      {
		/*
		 * The last period is not been quoted.  It is used
		 * to specify the no extension case.  We have to
		 * omit this period.
		 */
		*(dp - 1) = '\0';
	      }
	  }
#ifdef DOS
	if (version >= 0) sprintf(ver2, "%d", version);
	else *ver2 = '\0';
#endif /* DOS */
	ConcNameAndVersion(fbuf2, ver2, dst);
      }

    return(1);
  }




/*		
 * Name:	lisppathname
 *
 * Argument:	char *fullname	UNIX full pathname.
 *		char *lispname	The pathname following the Xerox
 *				Lisp naming convention.
 *				The first argument fullname is assumed
 *				the "true" name of lispname.
 *				The lispname is used to determine which
 *				charcter should be quoted in the result
 *				Xerox Lisp pathname representation.
 *		int dirp	If 1, fullname is a directory.  If 0,
 *				fullname is a file.
 *		int versionp	If 1, version field is also converted
 *				to the Xerox Lisp version.  {DSK} device
 *				invokes lisppathname with versionp but
 *				{UNIX} device without versionp.  If
 *				versionp is 1, dirp must be 0.
 *
 * Value:	If succeed, returns 1, otherwise 0.
 *		of the "true" name of the directory, otherwise Lisp NIL.
 *
 * Side Effect:	If succeed, lispname is replaced with "true" name which follows
 *		the Xerox Lisp file naming convention.
 *
 * Description:
 *
 * Converts the UNIX file name to Xerox Lisp file name.  The fields which might
 * be included in the result filename are directory, name, extension, and version.
 * The result file name can use the quotation mark to quote the chracters which
 * are dealt with specially in the Xerox Lisp file naming convention.  These
 * characters include "<", ">", ";", ".", and "'" itself.  Notice that "." must be
 * quoted if it is used as a part of the extension field.  Also notice that "<"
 * is quoted only if it is used as a first character of the initial directory.
 *
 */

lisppathname(fullname, lispname, dirp, versionp)
  register char	*fullname;
  register char	*lispname;
  register int	dirp;
  register int	versionp;
{
    register char *cp, *dp, *lnamep, *cnamep;
    char namebuf[MAXPATHLEN], fbuf[MAXPATHLEN], ver[VERSIONLEN], drive;
    register int i, mask, extensionp;
	
    if (strcmp(fullname, DIRSEPSTR) == 0)
      {
	strcpy(lispname, "<");
	return(1);
      }

    /* Split off the drive, if there is one. */
    if (fullname[1] == DRIVESEP)
      {
	*lispname++ = *fullname++;
	*lispname++ = *fullname++;
      }


    if (!dirp)
      {
	/*
	 * The characters which are dealt with specialy (i.e. are quoted)
	 * in the Xerox Lisp file naming convention are all valid in UNIX
	 * file name convetion.  So the conversion rule is almost
	 * straightforward except the "extension" field.  Only glancing
	 * the UNIX file name, we cannot determine which period character
	 * should be quoted in the result Xerox Lisp file name when more
	 * than one period are included in the UNIX file name.  In such
	 * case, we have to refer to the Xerox Lisp file name representation
	 * which is specified the user.  Thus, at first, extract the
	 * name field from the original Lisp file name.
	 */
	cp = lispname;
	lnamep = cp - 1;
	while (*cp != '\0')
	  {
	    switch (*cp)
	      {

		case '>':
		  lnamep = cp + 1;
		  cp++;
		  break;

		case '\'':
		  if (*(cp + 1) != '\0')
	    	    cp += 2;
		  else
	    	    cp++;
		  break;

		default:
		  cp++;
		  break;
	      }
	  }

	/* Name field in the UNIX file name representation. */

	cnamep = rindex(fullname, DIRSEP) + 1;
      }
    else
      {	/* It's a directory name only. */
	cnamep = fullname + strlen(fullname);
      }



    /*
     * Convertion rule of file name from UNIX to Xerox Lisp.
     *	UNIX		Lisp
     *	/		<    only if it is used as a root directory
     *			     delimiter.
     *	/		>    if used as a directory delimiter for other
     *			     directories than root directory.
     *	>		'>
     *	;		';
     *	'		''
     *	.		'.   only if it is used as a part of the extension
     *			     field.
     *	others		as if
     */

    cp = fullname + 1;
    dp = namebuf;
    *dp++ = '<';

    if (*cp == '<')
      {
	/*
	 * If the first character of the initial directory is '<',
	 * it should be quoted in the result Lisp file name.
	 */
	*dp++ = '\'';
	*dp++ = *cp++;
      }
	
    while (cp < cnamep)
      {
	switch (*cp)
	  {
	    case '>':
	    case ';':
#ifndef DOS
	    case '\'':
#endif /* DOS */
	      *dp++ = '\'';
	      *dp++ = *cp++;
	      break;

#ifdef DOS
	    case '/':
#endif /* DOS */
	    case DIRSEP:
	      *dp++ = '>';
	      cp++;
	      break;

	    default:
	      *dp++ = *cp++;
	      break;
	  }
      }

    if (dirp)
      {
	if (*(dp - 1) != '>' ||  *(dp - 2) == '\'') *dp++ = '>';
	*dp = '\0';
	strcpy(lispname, namebuf);
	return(1);
      }

    /*
     * Be careful dealing with the extension field.  If we encounter with the
     * period mark which was quoted in the original Lisp file name, we have
     * to quote it in the result file name.
     * First we count the period mark included in the Lisp file name, and
     * remember the position of the quoted period.  Then when we met the
     * period while we are converting the UNIX file name into Lisp one,
     * examine it if it is a quoted one or not, then if so, we quote it.
     */

    mask = 0;
    i = 1;
    lnamep++;

    while (*lnamep)
      {
	if (*lnamep == '.')
	  {
	    if (lnamep != lispname && *(lnamep - 1) == '\'') mask |= i;
	    i <<= 1;
	  }
	lnamep++;
      }
			
    i = 1;
    while (*cp) {
		switch (*cp) {
			
		      case '>':
		      case ';':
		      case '\'':
			*dp++ = '\'';
			*dp++ = *cp++;
			break;

		      case '.':
			if ((i & mask) == i) {
				/* This period should be quoted. */
				*dp++ = '\'';
				*dp++ = *cp++;
			} else {
				*dp++ = *cp++;
			}
			i <<= 1;
			break;

		      default:
			*dp++ = *cp++;
			break;
		}
	}

	*dp = '\0';

	/*
	 * exntensionp indicates whether exentsion field is included in a file name
	 * or not.  If extenstion field is not included, we have to add a period
	 * to specify empty extension field.
	 */
	strcpy(fbuf, namebuf);
	dp = cp = fbuf;
	while (*cp) {
		switch (*cp) {

		      case '>':
			dp = cp;
			cp++;
			break;

		      case '\'':
			if (*(cp + 1) != '\0')
			  cp += 2;
			else
			  cp++;
			break;

		      default:
			cp++;
			break;
		}
	}
	cp = dp + 1;
	if (versionp) separate_version(fbuf, ver, 1);
	extensionp = 0;
	while (*cp && !extensionp) {
		switch (*cp) {

		      case '.':
			extensionp = 1;
			break;

		      case '\'':
			if (*(cp + 1) != '\0')
			  cp += 2;
			else
			  cp++;
			break;

		      default:
			cp++;
			break;
		}
	}
	if (!extensionp) {
		*cp++ = '.';
		*cp = '\0';
	}
	if (versionp && *ver != '\0') {
		ConcNameAndVersion(fbuf, ver, namebuf);
	} else {
		strcpy(namebuf, fbuf);
	}

	/*
	 * Now, it's time to convert the version field.
	 */
	if (!dirp && versionp) UnixVersionToLispVersion(namebuf, 0);
	
	strcpy(lispname, namebuf);
	return(1);
}




/*		
 * Name:	quote_fname
 *
 * Argument:	char	*file	The root file name in UNIX format.  "Root"
 *				file name contains the name, extenstion and
 *				version fields.  A valid version field is in a
 *				form as ".~##~".
 *
 * Value:	If succeed, returns 1, otherwise 0.
 *
 * Side Effect:	If succeed, file is replaced with the file name in Xerox Lisp format
 *		in which special characters are quoted.
 *
 * Description:
 *
 * Converts a UNIX root file name to Xerox Lisp one.  This routine only quotes special
 * characters in Xerox file naming convention, does not care about the "true" name
 * which might be specified directly by the user as like lisppathname.   Thus, this
 * routine can be invoked when you don't know how to escape the period character.  This
 * is the case when you convert a file name in the course of the directory enumeration.
 *
 * This routine is used when file is a "FILE" name and being converted to {DSK} name.
 *
 * The special characters which is quoted include "<", ">", ";", and "'" itself.  Notice
 * again that "." is not quoted, because we don't know it is a extension separator in
 * Lisp sense or not.
 */

quote_fname(file)
	register char	*file;
{
	register char	*cp, *dp;
	register int	extensionp;
	char		fbuf[MAXNAMLEN + 1], namebuf[MAXNAMLEN + 1], ver[VERSIONLEN];
	
	cp = file;
	dp = fbuf;

	while (*cp) {
		switch (*cp) {
#ifdef DOS
			/* turn \ -> / for dir separator back to lisp */
		      case DIRSEP: *dp++ = '/'; cp++; break;
#endif /* DOS */
		      case '>':
		      case ';':
		      case '\'':
			*dp++ = '\'';
			*dp++ = *cp++;
			break;

		      default:
			*dp++ = *cp++;
			break;
		}
	}
	*dp = '\0';

	/*
	 * exntensionp indicates whether exentsion field is included in a file
	 * name or not.  If extenstion field is not included, we have to add a
	 * period to specify empty extension field.
	 */
	separate_version(fbuf, ver, 1);
	cp = fbuf;
	extensionp = 0;
	while (*cp && !extensionp) {
		switch (*cp) {

		      case '.':
			if (*(cp + 1)) extensionp = 1;
			cp++;
			break;

		      case '\'':
			if (*(cp + 1) != '\0')
			  cp += 2;
			else
			  cp++;
			break;

		      default:
			cp++;
			break;
		}
	}
	if (!extensionp) {
		if (*(cp - 1) == '.') {
			*(cp - 1) = '\'';
			*cp++ = '.';
		}
		*cp++ = '.';
		*cp = '\0';
	}
	if (*ver != '\0') {
		ConcNameAndVersion(fbuf, ver, namebuf);
	} else {
		strcpy(namebuf, fbuf);
	}
	UnixVersionToLispVersion(namebuf, 1);
	strcpy(file, namebuf);
}




/*		
 * Name:	quote_fname_ufs
 *
 * Argument:	char	*file		The root file name in UNIX format.  "Root"
 *					file name contains the name, extenstion and
 *					version fields.  A valid version field is in a
 *					form as ".~##~".
 *
 * Value:	If succeed, returns 1, otherwise 0.
 *
 * Side Effect:	If succeed, file is replaced with the file name in Xerox Lisp format
 *		in which special characters are quoted.
 *
 * Description:
 *
 * Similar to quote_fname, but this routine is only used when file is a "FILE" name
 * and being converted to {UNIX} name.
 */

quote_fname_ufs(file)
	register char	*file;
{
	register char	*cp, *dp;
	register int	extensionp;
	char		fbuf[MAXNAMLEN + 1];
	
	cp = file;
	dp = fbuf;

	while (*cp) {
		switch (*cp) {

		      case '>':
		      case ';':
		      case '\'':
			*dp++ = '\'';
			*dp++ = *cp++;
			break;

		      default:
			*dp++ = *cp++;
			break;
		}
	}
	*dp = '\0';

	/*
	 * exntensionp indicates whether exentsion field is included in a file
	 * name or not.  If extenstion field is not included, we have to add a
	 * period to specify empty extension field.
	 */
	cp = fbuf;
	extensionp = 0;
	while (*cp && !extensionp) {
		switch (*cp) {

		      case '.':
			if (*(cp + 1)) extensionp = 1;
			cp++;
			break;

		      case '\'':
			if (*(cp + 1) != '\0')
			  cp += 2;
			else
			  cp++;
			break;

		      default:
			cp++;
			break;
		}
	}
	if (!extensionp) {
		if (*(cp - 1) == '.') {
			*(cp - 1) = '\'';
			*cp++ = '.';
		}
		*cp++ = '.';
		*cp = '\0';
	}
	strcpy(file, fbuf);
}




/*		
 * Name:	quote_dname
 *
 * Argument:	char	*dir		The directory name in UNIX format.  Does not
 *					include its parent name.
 *
 * Value:	If succeed, returns 1, otherwise 0.
 *
 * Side Effect:	If succeed, dir is replaced with the directory name in Xerox Lisp
 * 		format in which special characters are quoted.
 *
 * Description:
 *
 * Similar to quote_fname, but this routine is only used when dir is a "DIRECTORY"
 * name.  Both {DSK} and {UNIX} uses this routine.
 */

quote_dname(dir)
	register char	*dir;
{
	register char	*cp, *dp;
	char		fbuf[MAXNAMLEN + 1];
	
	cp = dir;
	dp = fbuf;

	while (*cp) {
		switch (*cp) {

		      case '>':
		      case ';':
		      case '\'':
			*dp++ = '\'';
			*dp++ = *cp++;
			break;

		      default:
			*dp++ = *cp++;
			break;
		}
	}
	*dp = '\0';

	if (*(dp - 1) == '.') {
		/* Trail period should be quoted. */
		*(dp - 1) = '\'';
		*dp++ = '.';
	}

	strcpy(dir, fbuf);
}

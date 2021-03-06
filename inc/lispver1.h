/* lispver1.h */
/* DOS version of LispVersionToUnixVersion */
#define LispVersionToUnixVersion(pathname, ver)				\
  {									\
									\
    register char	*cp;						\
    register char	*vp;						\
    char		ver_buf[VERSIONLEN];				\
									\
    cp = pathname;							\
    vp = NULL;								\
    while (*cp)								\
      {									\
	switch (*cp)							\
	  {								\
									\
	    case ';':							\
		*cp = 0;						\
		cp++;							\
		vp = cp;						\
		break;							\
									\
	    case '\'':							\
		if (*(cp + 1) != 0) cp += 2; 				\
		else cp++;						\
		break;							\
									\
	    default:							\
		cp++;							\
		break;							\
	  }								\
      }									\
									\
    if (vp)								\
      {									\
	NumericStringP(vp, YES, NO);					\
       NO: *vp = 0;							\
       YES:								\
	if ((*vp)) ver = atoi(vp);					\
	else ver = -1;							\
      }									\
      else ver = -1;							\
  }

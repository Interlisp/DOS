/* This is G-file @(#) ether.h Version 2.4 (4/21/92). copyright Xerox & Fuji Xerox  */

/*
 *	ether.h
 *	Copyright (C) 1987 by Fuji Xerox Co., Ltd. All rights reserved.
 */



/************************************************************************/
/*									*/
/*	Copyright 1989, 1990 Venue, Fuji Xerox Co., Ltd, Xerox Corp.	*/
/*									*/
/*	This file is work-product resulting from the Xerox/Venue	*/
/*	Agreement dated 18-August-1989 for support of Medley.		*/
/*									*/
/************************************************************************/




#ifndef BYTESWAP

typedef struct { 
	DLword	DLLOCALHOST0;
	DLword	DLLOCALHOST1;
	DLword	DLLOCALHOST2;
	DLword	DLFIRSTICB;
	DLword	DLINPUTMASK;
	DLword	DLFIRSTOCB;
	DLword	DLOUTPUTMASK;
	DLword	DLMISSEDPACKETS;
	DLword	DLLASTICB;
	DLword	DLLASTOCB;
} DLETHERCSB;

typedef struct{ 
	DLword	DLIOCBLENGTH;
	DLword	DLIOCBBUFFERLO;
	DLword	DLIOCBBUFFERHI;
	DLword	DLRETRANSMISSIONMASK;
	DLword	DLIOCBBYTESUSED;
	DLword	DLIOCBSTATUS;
	DLword	DLNEXTIOCB;
	DLword	DLFOROUTPUTUSE;
} DLETHERIOCB;

typedef struct { 
	unsigned int	NETTYPE : 8;
	unsigned int	NDBNEXT : 24;
	unsigned int	NDBPUPNET : 8;
	unsigned int	NDBNSNET : 24;
	unsigned int	NDBTASK : 8;
	unsigned int	NDBBROADCASTP : 24;
	unsigned int	NDBPUPHOST : 8;
	unsigned int	NDBTRANSMITTER : 24;
	unsigned int	nil1: 8;
	unsigned int	NDBENCAPSULATOR : 24;
	unsigned int	NDBIQLENGTH : 8; /* DATATYPE */
	unsigned int	NDBCSB : 24; /* DATATYPE */
	LispPTR		NDBIQ;
	LispPTR		NDBTQ;
	LispPTR		NDBTRANSLATIONS;
	LispPTR		NDBETHERFLUSHER;
	LispPTR		NDBWATCHER;
	LispPTR		NDBCANHEARSELF;
	LispPTR		NDBIPNET;
	LispPTR		NDBIPHOST;
	DLword		NDBPUPTYPE;
	DLword		nil2;
	LispPTR		nil3;
} NDB;
#else
	/***********************************************/
	/* Byte-swapped definitions, for e.g., 80386's */
	/***********************************************/
typedef struct { 
	DLword	DLLOCALHOST1;
	DLword	DLLOCALHOST0;
	DLword	DLFIRSTICB;
	DLword	DLLOCALHOST2;
	DLword	DLFIRSTOCB;
	DLword	DLINPUTMASK;
	DLword	DLMISSEDPACKETS;
	DLword	DLOUTPUTMASK;
	DLword	DLLASTOCB;
	DLword	DLLASTICB;
} DLETHERCSB;

typedef struct{ 
	DLword	DLIOCBBUFFERLO;
	DLword	DLIOCBLENGTH;
	DLword	DLRETRANSMISSIONMASK;
	DLword	DLIOCBBUFFERHI;
	DLword	DLIOCBSTATUS;
	DLword	DLIOCBBYTESUSED;
	DLword	DLFOROUTPUTUSE;
	DLword	DLNEXTIOCB;
} DLETHERIOCB;

typedef struct { 
	unsigned int	NDBNEXT : 24;
	unsigned int	NETTYPE : 8;
	unsigned int	NDBNSNET : 24;
	unsigned int	NDBPUPNET : 8;
	unsigned int	NDBBROADCASTP : 24;
	unsigned int	NDBTASK : 8;
	unsigned int	NDBTRANSMITTER : 24;
	unsigned int	NDBPUPHOST : 8;
	unsigned int	NDBENCAPSULATOR : 24;
	unsigned int	nil1: 8;
	unsigned int	NDBCSB : 24; /* DATATYPE */
	unsigned int	NDBIQLENGTH : 8; /* DATATYPE */
	LispPTR		NDBIQ;
	LispPTR		NDBTQ;
	LispPTR		NDBTRANSLATIONS;
	LispPTR		NDBETHERFLUSHER;
	LispPTR		NDBWATCHER;
	LispPTR		NDBCANHEARSELF;
	LispPTR		NDBIPNET;
	LispPTR		NDBIPHOST;
	DLword		nil2;
	DLword		NDBPUPTYPE;
	LispPTR		nil3;
} NDB;

#endif /* BYTESWAP */


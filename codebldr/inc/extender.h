/* extender.h - DOS extender Global Data Area and Profile structures
 * $Version: 1.14 $
 * Copyright 1990,91 Intel Corporation, ALL RIGHTS RESERVED.
 *
 * CAUTION: Changing the data referenced by these structures may have
 * detrimental consequences not only for your software application, but also
 * your computer system (ie file system CORRUPTION).  Ensure a complete and
 * thorough understanding of the extender environment prior to attempting to
 * modify data through these structures.
 */

#ifndef _extenderh
#define _extenderh

#pragma noalign (_rex_head_t)
typedef struct _rex_head_t              /* -------- REX File Header -------- */
   {                                    /* ================================= */
   unsigned short  signature;           /* Signature: 'MQ'                   */
   unsigned short  size_mod;            /* File size (bytes) modulo 512      */
   unsigned short  size_blks;           /* File size in 512-byte blocks      */
   unsigned short  reloc_items;         /* Number of relocation items        */
   unsigned short  header_size;         /* Size of header in paragraphs      */
   unsigned short  min_data_4k;         /* Minimum data in 4K pages          */
   unsigned short  max_data_4k;         /* Maximum data in 4K pages          */
   unsigned long   initial_esp;         /* Initial ESP (Stack pointer)       */
   unsigned short  checksum;            /* Checksum for file                 */
   unsigned long   initial_eip;         /* Initial EIP (Instruction pointer) */
   unsigned short  first_reloc;         /* Offset of first relocation item   */
   unsigned short  overlay;             /* Overlay number (Always 0)         */
   unsigned short  constant_1;          /* Constant: 0001h                   */
   } REX_HEAD;                          /* --------------------------------- */

#pragma noalign (_intel_table_t)
typedef struct _intel_table_t           /* ----- Intel Interrupt Table ----- */
   {                                    /* ================================= */
   unsigned long   address;             /* Interrupt handler    Flat address */
   char            reserved[4];         /*                                   */
   } INTEL_TABLE;                       /* --------------------------------- */

#pragma noalign (_hint_table_t)
typedef struct _hint_table_t            /* ----- High Interrupt Table ------ */
   {                                    /* ================================= */
   void          (*user_routine)();     /* User handler         Flat address */
   void          (*v86_routine)();      /* V86 Handler     Segmented address */
   } HINT_TABLE;                        /* --------------------------------- */

#pragma noalign (_ovl_struc_t)
typedef struct _ovl_struc_t             /* --- Overlay Parameter Block ----- */
   {                                    /* ================================= */
   unsigned long   ovl_tgtp;            /* Load target          Flat address */
   unsigned long   ovl_relp;            /* Relocation           Flat address */
   unsigned long   ovl_modl;            /* Overlay module length             */
   void           *ovl_buf;             /* I/O buffer address                */
   unsigned long   ovl_buffsz;          /* I/O buffer size                   */
   unsigned short  ovl_rexh[14];        /* REX header information            */
   char            ovl_file[65];        /* Overlay module filespec           */
   unsigned long   ovl_eip;             /* Entry point                       */
   } OVL_STRUC;                         /* --------------------------------- */

#pragma noalign (_gda_t)
typedef struct _gda_t                   /* -- Global Data Area Structure --- */
   {                                    /* ================================= */
   char            gda_eye[4];          /* Eye catcher                       */
   char            gda_rlse[4];         /* Release level                     */

   struct _gda_t  *gda_prv;             /* Previous GDA         Flat address */
   struct _gda_t  *gda_nxt;             /* Next GDA             Flat address */

   void           *gda_pspa;            /* PSP                  Flat address */
   unsigned long   gda_pspa6;           /*                 Segmented address */

   void           *gda_pro;             /* Profile              Flat address */
   void           *gda_ldpt;            /* Application loadpt   Flat address */
   HINT_TABLE     *gda_hint;            /* High INT table       Flat address */
   unsigned long   gda_term;            /* PSP_TERM        Segmented address */
   INTEL_TABLE    *gda_intel;           /* Intel INT table      Flat address */

   char            reserved00[4];

   void           *gda_serv;            /* Extender services    Flat address */
                                        /* entry point                       */

   unsigned long   gda_ldcsel;          /* Application selectors        Code */
   unsigned long   gda_lddsel;          /*                              Data */

   unsigned long   gda_hmem;            /* Available high memory (Kbytes)    */
   unsigned long   gda_lmem;            /* Available low memory (Kbytes)     */
   unsigned long   gda_tstg;            /* Top of virtual storage (Kbytes)   */

   void           *gda_vstk;            /* V86 INT low          Flat address */
   unsigned long   gda_vstk6;           /* memory stack    Segmented address */

   void           *gda_hlod;            /* Highest used load address         */
   void           *gda_tlow;            /* Highest used low memory address   */

   void           *gda_lbuff;           /* Low memory           Flat address */
   unsigned long   gda_lbuff6;          /* user buffer     Segmented address */
   unsigned long   gda_buffsz;          /*              Buffer size (Kbytes) */

   void           *gda_work;            /* Low memory           Flat address */
   unsigned long   gda_work6;           /* user work       Segmented address */
   unsigned long   gda_worksz;          /* area          Buffer size (bytes) */

   void           *gda_stk;             /* Stack                Flat address */
   unsigned long   gda_stksz;           /*                   Length (Kbytes) */
   unsigned long   gda_mstksz;          /*               Max length (Kbytes) */

   void           *gda_vbuff;           /* Virtual memory       Flat address */
   unsigned long   gda_vbuff6;          /* low memory      Segmented address */
   unsigned long   gda_vbuffsz;         /* buffers       Max length (Kbytes) */

   void           *gda_vwrk;            /* Virtual memory       Flat address */
   unsigned long   gda_vwrk6;           /* low memory      Segmented address */
   unsigned long   gda_vwrksz;          /* work area            Size (bytes) */

   void           *gda_dta;             /* Low memory           Flat address */
   unsigned long   gda_dta6;            /* disk transfer    Segmented offset */
   unsigned long   gda_dtasz;           /* area             Max size (bytes) */

   unsigned long   gda_hbase;           /* Origin of high memory             */

   char            reserved01[8];

   void           *gda_vmb;             /* Location             Flat address */
   unsigned long   gda_vmb6;            /* of VMLD         Segmented address */

   char            reserved02[3];

   unsigned char   gda_env;             /* Environment byte                  */

   char            reserved03[2];

   unsigned short  gda_vid;             /* Display buffer size (64-byte blks)*/

   char            reserved04[30];

   unsigned char   gda_rc;              /* Termination code                  */
   unsigned char   gda_sys;             /* System type information           */

   unsigned long   gda_vstksz;          /* V86 INT stack size                */
   OVL_STRUC      *gda_ovlprm;          /* Overlay load params  Flat address */

   void           *gda_loader;          /* Entry point for overlay loader    */
   void           *gda_mba;             /* Memory block area address         */

   char            reserved06[28];
   void           *gda_i24;             /* Reserved for internal use         */
   void           *gda_mouse;           /* Mouse cleanup routine             */

   unsigned long   gda_r1;              /* Reserved for internal use         */
   unsigned long   gda_r2;              /*  "        "     "      "          */
   unsigned long   gda_r3;              /*  "        "     "      "          */
   unsigned long   gda_r4;              /*  "        "     "      "          */

   unsigned long   gda_u1;              /* Reserved for user                 */
   unsigned long   gda_u2;              /*  "        "   "                   */
   unsigned long   gda_u3;              /*  "        "   "                   */
   unsigned long   gda_u4;              /*  "        "   "                   */
   unsigned long   gda_u5;              /*  "        "   "                   */
   unsigned long   gda_u6;              /*  "        "   "                   */
   unsigned long   gda_u7;              /*  "        "   "                   */
   unsigned long   gda_u8;              /*  "        "   "                   */
   void           *gda_emul;            /* Pointer to Emulator cleanup func  */

   char            reserved07[60];
   } _GDA;                              /* --------------------------------- */

#pragma noalign (_profile_t)
typedef struct _profile_t               /* ------- Profile Structure ------- */
   {                                    /* ================================= */
   char          pro_eye[4];            /* 'PRO '                            */
   char          pro_rlse[4];           /* Profile release level             */

   unsigned long pro_pro;               /* Pointer to           Flat address */
   unsigned long pro_pro6;              /* profile         Segmented address */

   char          pro_sop_eye[4];        /* 'SOP='               Stack Option */
   char          pro_sop;               /* = I  - Internal stack             */

   char          reserved00[11];

   char          pro_ssz_eye[4];        /* 'SSZ='        Stack size (Kbytes) */
   unsigned long pro_ssz;               /* (May be zero if option is I)      */

   char          pro_mst_eye[4];        /* 'MST='                            */
   unsigned long pro_mst;               /* Max stack size (Kbytes)           */

   char          reserved01[24];

   char          pro_anm_eye[4];        /* 'ANM='                            */
   char          pro_anm[65];           /* ASCIIZ program name               */

   char          reserved02[3];

   char          pro_pnm_eye[4];        /* 'PNM='                            */
   char          pro_pnm[65];           /* 386 External Profile Filename     */
                                        /* Used by the graphics library      */

   char          reserved03[75];

   char          pro_bsz_eye[4];        /* 'BSZ='       User low buffer size */
   unsigned long pro_bsz;               /* Default: 40 Kbytes                */

   char          pro_vbf_eye[4];        /* 'VBF='         VM low buffer size */
   unsigned long pro_vbf;               /* Default: 4 Kbytes                 */

   char          pro_dta_eye[4];        /* 'DTA='    Disk Transfer Area size */
   unsigned long pro_dta;               /* Default: 0 bytes                  */

   char          pro_wrk_eye[4];        /* 'WRK='    User low work area size */
   unsigned long pro_wrk;               /* Default: 16 Kbytes                */

   char          pro_vwk_eye[4];        /* 'VWK='      VM low work area size */
   unsigned long pro_vwk;               /* Default: 16 Kbytes                */

   char          pro_dos_eye[4];        /* 'DOS='          DOS reserved area */
   unsigned long pro_dos;               /* Default: -1 (All available)       */

   char          reserved04[96];

   char          pro_org_eye[4];        /* 'ORG='                            */
   unsigned long pro_org;               /* Linear address high origin        */

   char          pro_rgn_eye[4];        /* 'RGN='                            */
   unsigned long pro_rgn;               /* Minimum region size (bytes)       */
                                        /* Default is 3.0 MBytes             */
   char          reserved05[64];
                                        /* User parameters:                  */
   char          pro_us1_eye[4];        /* 'US1='                            */
   unsigned long pro_us1;               /* User parameter 1                  */
   char          pro_us2_eye[4];        /* 'US2='                            */
   unsigned long pro_us2;               /* User parameter 2                  */
   char          pro_us3_eye[4];        /* 'US3='                            */
   unsigned long pro_us3;               /* User parameter 3                  */
   char          pro_us4_eye[4];        /* 'US4='                            */
   unsigned long pro_us4;               /* User parameter 4                  */
   char          pro_us5_eye[4];        /* 'US5='                            */
   unsigned long pro_us5;               /* User parameter 5                  */
   char          pro_us6_eye[4];        /* 'US6='                            */
   unsigned long pro_us6;               /* User parameter 6                  */
   char          pro_us7_eye[4];        /* 'US7='                            */
   unsigned long pro_us7;               /* User parameter 7                  */
   char          pro_us8_eye[4];        /* 'US8='                            */
   unsigned long pro_us8;               /* User parameter 8                  */

   char          pro_swp_eye[4];        /* 'SWP='                            */
   char          pro_swp[65];           /* Demand paging swap file           */

   char          reserved06[39];
   unsigned long end;
   } _PROFILE;                          /* --------------------------------- */

                                        /* gda_env values ------------------ */
#define GDA_COLOR       0x80            /* Color monitor                     */
#define GDA_PGO         0x40            /* Stand alone page data set opened  */
#define GDA_PGC         0x20            /* Stand alone page data set created */
#define GDA_DPMI        0x10            /* DPMI-hosted (not stand alone)     */
#define GDA_XMS         0x08            /* Extended memoru allocated via XMS */
#define GDA_IPL         0x04            /* Reboot the system                 */
#define GDA_WIN30       0x02            /* DPMI host is Windows 3.0          */
#define GDA_WEITEK      0x01            /* WEITEK Co-processor detected      */

                                        /* gda_sys values -------------------*/
#define GDA_VCPI        0x80            /* VCPI host is active               */
#define GDA_WINXX       0x40            /* Windows is active                 */
#define GDA_EMACT       0x04            /* FP emulation is active            */

#define RC_IDT          1               /* Error during IDT processing       */
#define RC_LDT          2               /* Error building LDT entries        */
#define RC_CMD          3               /* Error during command line scan    */
#define RC_ENV          4               /* Error during environment scan     */
#define RC_REX          5               /* Error during rex header processing*/
#define RC_HMEM         6               /* Error during high memory analysis */
#define RC_AMEM         7               /* Error during high memory alloc    */

#endif /* _extenderh */


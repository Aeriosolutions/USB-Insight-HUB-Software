   1                     ; C Compiler for STM8 (COSMIC Software)
   2                     ; Parser V4.12.9 - 19 Apr 2023
   3                     ; Generator (Limited) V4.5.6 - 18 Jul 2023
  14                     	bsct
  15  0000               _extdebcc1:
  16  0000 00            	dc.b	0
  17  0001               _extdebcc2:
  18  0001 00            	dc.b	0
  19  0002               _hostdebcc1:
  20  0002 00            	dc.b	0
  21  0003               _hostdebcc2:
  22  0003 00            	dc.b	0
  55                     ; 30 void BaseR_GPIO_Init(void){
  57                     	switch	.text
  58  0000               _BaseR_GPIO_Init:
  62                     ; 32 	GPIO_DeInit(GPIOA);
  64  0000 ae5000        	ldw	x,#20480
  65  0003 cd0000        	call	_GPIO_DeInit
  67                     ; 33 	GPIO_DeInit(GPIOB);
  69  0006 ae5005        	ldw	x,#20485
  70  0009 cd0000        	call	_GPIO_DeInit
  72                     ; 34 	GPIO_DeInit(GPIOC);
  74  000c ae500a        	ldw	x,#20490
  75  000f cd0000        	call	_GPIO_DeInit
  77                     ; 35 	GPIO_DeInit(GPIOD);
  79  0012 ae500f        	ldw	x,#20495
  80  0015 cd0000        	call	_GPIO_DeInit
  82                     ; 36 	GPIO_DeInit(GPIOE);
  84  0018 ae5014        	ldw	x,#20500
  85  001b cd0000        	call	_GPIO_DeInit
  87                     ; 37 	GPIO_DeInit(GPIOF);
  89  001e ae5019        	ldw	x,#20505
  90  0021 cd0000        	call	_GPIO_DeInit
  92                     ; 39 	GPIO_Init(BLINK, GPIO_MODE_OUT_PP_LOW_SLOW);
  94  0024 4bc0          	push	#192
  95  0026 4b40          	push	#64
  96  0028 ae500f        	ldw	x,#20495
  97  002b cd0000        	call	_GPIO_Init
  99  002e 85            	popw	x
 100                     ; 41 	GPIO_Init(CH1_PWR_EN, GPIO_MODE_OUT_PP_LOW_SLOW);
 102  002f 4bc0          	push	#192
 103  0031 4b20          	push	#32
 104  0033 ae500a        	ldw	x,#20490
 105  0036 cd0000        	call	_GPIO_Init
 107  0039 85            	popw	x
 108                     ; 42 	GPIO_Init(CH1_ILIM_H, GPIO_MODE_OUT_OD_HIZ_SLOW);
 110  003a 4b90          	push	#144
 111  003c 4b80          	push	#128
 112  003e ae500a        	ldw	x,#20490
 113  0041 cd0000        	call	_GPIO_Init
 115  0044 85            	popw	x
 116                     ; 43 	GPIO_Init(CH1_ILIM_L, GPIO_MODE_OUT_OD_HIZ_SLOW);
 118  0045 4b90          	push	#144
 119  0047 4b40          	push	#64
 120  0049 ae500a        	ldw	x,#20490
 121  004c cd0000        	call	_GPIO_Init
 123  004f 85            	popw	x
 124                     ; 44 	GPIO_Init(CH1_FAULT, GPIO_MODE_IN_PU_NO_IT);
 126  0050 4b40          	push	#64
 127  0052 4b01          	push	#1
 128  0054 ae500f        	ldw	x,#20495
 129  0057 cd0000        	call	_GPIO_Init
 131  005a 85            	popw	x
 132                     ; 45 	GPIO_Init(CH1_DATA_EN, GPIO_MODE_OUT_PP_LOW_SLOW);
 134  005b 4bc0          	push	#192
 135  005d 4b04          	push	#4
 136  005f ae500a        	ldw	x,#20490
 137  0062 cd0000        	call	_GPIO_Init
 139  0065 85            	popw	x
 140                     ; 47 	GPIO_Init(CH2_PWR_EN, GPIO_MODE_OUT_PP_LOW_SLOW);
 142  0066 4bc0          	push	#192
 143  0068 4b04          	push	#4
 144  006a ae500f        	ldw	x,#20495
 145  006d cd0000        	call	_GPIO_Init
 147  0070 85            	popw	x
 148                     ; 48 	GPIO_Init(CH2_ILIM_H, GPIO_MODE_OUT_OD_HIZ_SLOW);
 150  0071 4b90          	push	#144
 151  0073 4b10          	push	#16
 152  0075 ae500f        	ldw	x,#20495
 153  0078 cd0000        	call	_GPIO_Init
 155  007b 85            	popw	x
 156                     ; 49 	GPIO_Init(CH2_ILIM_L, GPIO_MODE_OUT_OD_HIZ_SLOW);
 158  007c 4b90          	push	#144
 159  007e 4b08          	push	#8
 160  0080 ae500f        	ldw	x,#20495
 161  0083 cd0000        	call	_GPIO_Init
 163  0086 85            	popw	x
 164                     ; 50 	GPIO_Init(CH2_FAULT, GPIO_MODE_IN_PU_NO_IT);
 166  0087 4b40          	push	#64
 167  0089 4b80          	push	#128
 168  008b ae500f        	ldw	x,#20495
 169  008e cd0000        	call	_GPIO_Init
 171  0091 85            	popw	x
 172                     ; 51 	GPIO_Init(CH2_DATA_EN, GPIO_MODE_OUT_PP_LOW_SLOW);
 174  0092 4bc0          	push	#192
 175  0094 4b08          	push	#8
 176  0096 ae500a        	ldw	x,#20490
 177  0099 cd0000        	call	_GPIO_Init
 179  009c 85            	popw	x
 180                     ; 53 	GPIO_Init(CH3_PWR_EN, GPIO_MODE_OUT_PP_LOW_SLOW);
 182  009d 4bc0          	push	#192
 183  009f 4b02          	push	#2
 184  00a1 ae5000        	ldw	x,#20480
 185  00a4 cd0000        	call	_GPIO_Init
 187  00a7 85            	popw	x
 188                     ; 54 	GPIO_Init(CH3_ILIM_H, GPIO_MODE_OUT_OD_HIZ_SLOW);
 190  00a8 4b90          	push	#144
 191  00aa 4b08          	push	#8
 192  00ac ae5000        	ldw	x,#20480
 193  00af cd0000        	call	_GPIO_Init
 195  00b2 85            	popw	x
 196                     ; 55 	GPIO_Init(CH3_ILIM_L, GPIO_MODE_OUT_OD_HIZ_SLOW);
 198  00b3 4b90          	push	#144
 199  00b5 4b04          	push	#4
 200  00b7 ae5000        	ldw	x,#20480
 201  00ba cd0000        	call	_GPIO_Init
 203  00bd 85            	popw	x
 204                     ; 56 	GPIO_Init(CH3_FAULT, GPIO_MODE_IN_PU_NO_IT);
 206  00be 4b40          	push	#64
 207  00c0 4b10          	push	#16
 208  00c2 ae5019        	ldw	x,#20505
 209  00c5 cd0000        	call	_GPIO_Init
 211  00c8 85            	popw	x
 212                     ; 57 	GPIO_Init(CH3_DATA_EN, GPIO_MODE_OUT_PP_LOW_SLOW);
 214  00c9 4bc0          	push	#192
 215  00cb 4b10          	push	#16
 216  00cd ae500a        	ldw	x,#20490
 217  00d0 cd0000        	call	_GPIO_Init
 219  00d3 85            	popw	x
 220                     ; 59 	GPIO_Init(VEXT_PRESENT, GPIO_MODE_IN_FL_NO_IT);
 222  00d4 4b00          	push	#0
 223  00d6 4b02          	push	#2
 224  00d8 ae500a        	ldw	x,#20490
 225  00db cd0000        	call	_GPIO_Init
 227  00de 85            	popw	x
 228                     ; 60 	GPIO_Init(VHOST_PRESENT, GPIO_MODE_IN_FL_NO_IT);
 230  00df 4b00          	push	#0
 231  00e1 4b20          	push	#32
 232  00e3 ae5014        	ldw	x,#20500
 233  00e6 cd0000        	call	_GPIO_Init
 235  00e9 85            	popw	x
 236                     ; 62 	GPIO_Init(USB3_MUX_OE, GPIO_MODE_OUT_PP_HIGH_SLOW);
 238  00ea 4bd0          	push	#208
 239  00ec 4b80          	push	#128
 240  00ee ae5005        	ldw	x,#20485
 241  00f1 cd0000        	call	_GPIO_Init
 243  00f4 85            	popw	x
 244                     ; 63 	GPIO_Init(USB3_MUX_SEL, GPIO_MODE_OUT_PP_HIGH_SLOW);
 246  00f5 4bd0          	push	#208
 247  00f7 4b40          	push	#64
 248  00f9 ae5005        	ldw	x,#20485
 249  00fc cd0000        	call	_GPIO_Init
 251  00ff 85            	popw	x
 252                     ; 66 	GPIO_WriteHigh(USB3_MUX_OE);
 254  0100 4b80          	push	#128
 255  0102 ae5005        	ldw	x,#20485
 256  0105 cd0000        	call	_GPIO_WriteHigh
 258  0108 84            	pop	a
 259                     ; 67 	GPIO_WriteLow(USB3_MUX_SEL);
 261  0109 4b40          	push	#64
 262  010b ae5005        	ldw	x,#20485
 263  010e cd0000        	call	_GPIO_WriteLow
 265  0111 84            	pop	a
 266                     ; 70 	GPIO_Init(VHOST_CC1_AIN0, GPIO_MODE_IN_FL_NO_IT);
 268  0112 4b00          	push	#0
 269  0114 4b01          	push	#1
 270  0116 ae5005        	ldw	x,#20485
 271  0119 cd0000        	call	_GPIO_Init
 273  011c 85            	popw	x
 274                     ; 71 	GPIO_Init(VHOST_CC2_AIN1, GPIO_MODE_IN_FL_NO_IT);
 276  011d 4b00          	push	#0
 277  011f 4b02          	push	#2
 278  0121 ae5005        	ldw	x,#20485
 279  0124 cd0000        	call	_GPIO_Init
 281  0127 85            	popw	x
 282                     ; 72 	GPIO_Init(VEXT_CC1_AIN2, GPIO_MODE_IN_FL_NO_IT);
 284  0128 4b00          	push	#0
 285  012a 4b04          	push	#4
 286  012c ae5005        	ldw	x,#20485
 287  012f cd0000        	call	_GPIO_Init
 289  0132 85            	popw	x
 290                     ; 73 	GPIO_Init(VEXT_CC2_AIN3, GPIO_MODE_IN_FL_NO_IT);
 292  0133 4b00          	push	#0
 293  0135 4b08          	push	#8
 294  0137 ae5005        	ldw	x,#20485
 295  013a cd0000        	call	_GPIO_Init
 297  013d 85            	popw	x
 298                     ; 74 }
 301  013e 81            	ret
 332                     ; 76 void Update_GPIO_from_I2CRegisters(void){
 333                     	switch	.text
 334  013f               _Update_GPIO_from_I2CRegisters:
 338                     ; 81 	if(r20_CH1REG & ILIMH_MASK)GPIO_WriteLow(CH1_ILIM_H); else GPIO_WriteHigh(CH1_ILIM_H); //Active Low
 340  013f b600          	ld	a,_r20_CH1REG
 341  0141 a502          	bcp	a,#2
 342  0143 270b          	jreq	L13
 345  0145 4b80          	push	#128
 346  0147 ae500a        	ldw	x,#20490
 347  014a cd0000        	call	_GPIO_WriteLow
 349  014d 84            	pop	a
 351  014e 2009          	jra	L33
 352  0150               L13:
 355  0150 4b80          	push	#128
 356  0152 ae500a        	ldw	x,#20490
 357  0155 cd0000        	call	_GPIO_WriteHigh
 359  0158 84            	pop	a
 360  0159               L33:
 361                     ; 82 	if(r20_CH1REG & ILIML_MASK)GPIO_WriteLow(CH1_ILIM_L); else GPIO_WriteHigh(CH1_ILIM_L); //Active Low  				
 363  0159 b600          	ld	a,_r20_CH1REG
 364  015b a504          	bcp	a,#4
 365  015d 270b          	jreq	L53
 368  015f 4b40          	push	#64
 369  0161 ae500a        	ldw	x,#20490
 370  0164 cd0000        	call	_GPIO_WriteLow
 372  0167 84            	pop	a
 374  0168 2009          	jra	L73
 375  016a               L53:
 378  016a 4b40          	push	#64
 379  016c ae500a        	ldw	x,#20490
 380  016f cd0000        	call	_GPIO_WriteHigh
 382  0172 84            	pop	a
 383  0173               L73:
 384                     ; 83 	if(r20_CH1REG & PWREN_MASK)GPIO_WriteHigh(CH1_PWR_EN); else GPIO_WriteLow(CH1_PWR_EN); //Active High
 386  0173 b600          	ld	a,_r20_CH1REG
 387  0175 a510          	bcp	a,#16
 388  0177 270b          	jreq	L14
 391  0179 4b20          	push	#32
 392  017b ae500a        	ldw	x,#20490
 393  017e cd0000        	call	_GPIO_WriteHigh
 395  0181 84            	pop	a
 397  0182 2009          	jra	L34
 398  0184               L14:
 401  0184 4b20          	push	#32
 402  0186 ae500a        	ldw	x,#20490
 403  0189 cd0000        	call	_GPIO_WriteLow
 405  018c 84            	pop	a
 406  018d               L34:
 407                     ; 84 	if(r20_CH1REG & DATAEN_MASK)GPIO_WriteLow(CH1_DATA_EN); else GPIO_WriteHigh(CH1_DATA_EN); //Active Low
 409  018d b600          	ld	a,_r20_CH1REG
 410  018f a540          	bcp	a,#64
 411  0191 270b          	jreq	L54
 414  0193 4b04          	push	#4
 415  0195 ae500a        	ldw	x,#20490
 416  0198 cd0000        	call	_GPIO_WriteLow
 418  019b 84            	pop	a
 420  019c 2009          	jra	L74
 421  019e               L54:
 424  019e 4b04          	push	#4
 425  01a0 ae500a        	ldw	x,#20490
 426  01a3 cd0000        	call	_GPIO_WriteHigh
 428  01a6 84            	pop	a
 429  01a7               L74:
 430                     ; 85 	if(GPIO_ReadInputPin(CH1_FAULT))r20_CH1REG &= 0x7F; else r20_CH1REG |= 0x80; 
 432  01a7 4b01          	push	#1
 433  01a9 ae500f        	ldw	x,#20495
 434  01ac cd0000        	call	_GPIO_ReadInputPin
 436  01af 5b01          	addw	sp,#1
 437  01b1 4d            	tnz	a
 438  01b2 2706          	jreq	L15
 441  01b4 721f0000      	bres	_r20_CH1REG,#7
 443  01b8 2004          	jra	L35
 444  01ba               L15:
 447  01ba 721e0000      	bset	_r20_CH1REG,#7
 448  01be               L35:
 449                     ; 87 	if(r21_CH2REG & ILIMH_MASK)GPIO_WriteLow(CH2_ILIM_H); else GPIO_WriteHigh(CH2_ILIM_H);
 451  01be b600          	ld	a,_r21_CH2REG
 452  01c0 a502          	bcp	a,#2
 453  01c2 270b          	jreq	L55
 456  01c4 4b10          	push	#16
 457  01c6 ae500f        	ldw	x,#20495
 458  01c9 cd0000        	call	_GPIO_WriteLow
 460  01cc 84            	pop	a
 462  01cd 2009          	jra	L75
 463  01cf               L55:
 466  01cf 4b10          	push	#16
 467  01d1 ae500f        	ldw	x,#20495
 468  01d4 cd0000        	call	_GPIO_WriteHigh
 470  01d7 84            	pop	a
 471  01d8               L75:
 472                     ; 88 	if(r21_CH2REG & ILIML_MASK)GPIO_WriteLow(CH2_ILIM_L); else GPIO_WriteHigh(CH2_ILIM_L);  				
 474  01d8 b600          	ld	a,_r21_CH2REG
 475  01da a504          	bcp	a,#4
 476  01dc 270b          	jreq	L16
 479  01de 4b08          	push	#8
 480  01e0 ae500f        	ldw	x,#20495
 481  01e3 cd0000        	call	_GPIO_WriteLow
 483  01e6 84            	pop	a
 485  01e7 2009          	jra	L36
 486  01e9               L16:
 489  01e9 4b08          	push	#8
 490  01eb ae500f        	ldw	x,#20495
 491  01ee cd0000        	call	_GPIO_WriteHigh
 493  01f1 84            	pop	a
 494  01f2               L36:
 495                     ; 89 	if(r21_CH2REG & PWREN_MASK)GPIO_WriteHigh(CH2_PWR_EN); else GPIO_WriteLow(CH2_PWR_EN);
 497  01f2 b600          	ld	a,_r21_CH2REG
 498  01f4 a510          	bcp	a,#16
 499  01f6 270b          	jreq	L56
 502  01f8 4b04          	push	#4
 503  01fa ae500f        	ldw	x,#20495
 504  01fd cd0000        	call	_GPIO_WriteHigh
 506  0200 84            	pop	a
 508  0201 2009          	jra	L76
 509  0203               L56:
 512  0203 4b04          	push	#4
 513  0205 ae500f        	ldw	x,#20495
 514  0208 cd0000        	call	_GPIO_WriteLow
 516  020b 84            	pop	a
 517  020c               L76:
 518                     ; 90 	if(r21_CH2REG & DATAEN_MASK)GPIO_WriteLow(CH2_DATA_EN); else GPIO_WriteHigh(CH2_DATA_EN);
 520  020c b600          	ld	a,_r21_CH2REG
 521  020e a540          	bcp	a,#64
 522  0210 270b          	jreq	L17
 525  0212 4b08          	push	#8
 526  0214 ae500a        	ldw	x,#20490
 527  0217 cd0000        	call	_GPIO_WriteLow
 529  021a 84            	pop	a
 531  021b 2009          	jra	L37
 532  021d               L17:
 535  021d 4b08          	push	#8
 536  021f ae500a        	ldw	x,#20490
 537  0222 cd0000        	call	_GPIO_WriteHigh
 539  0225 84            	pop	a
 540  0226               L37:
 541                     ; 91 	if(GPIO_ReadInputPin(CH2_FAULT))r21_CH2REG &= 0x7F; else r21_CH2REG |= 0x80; 		
 543  0226 4b80          	push	#128
 544  0228 ae500f        	ldw	x,#20495
 545  022b cd0000        	call	_GPIO_ReadInputPin
 547  022e 5b01          	addw	sp,#1
 548  0230 4d            	tnz	a
 549  0231 2706          	jreq	L57
 552  0233 721f0000      	bres	_r21_CH2REG,#7
 554  0237 2004          	jra	L77
 555  0239               L57:
 558  0239 721e0000      	bset	_r21_CH2REG,#7
 559  023d               L77:
 560                     ; 93 	if(r22_CH3REG & ILIMH_MASK)GPIO_WriteLow(CH3_ILIM_H); else GPIO_WriteHigh(CH3_ILIM_H);
 562  023d b600          	ld	a,_r22_CH3REG
 563  023f a502          	bcp	a,#2
 564  0241 270b          	jreq	L101
 567  0243 4b08          	push	#8
 568  0245 ae5000        	ldw	x,#20480
 569  0248 cd0000        	call	_GPIO_WriteLow
 571  024b 84            	pop	a
 573  024c 2009          	jra	L301
 574  024e               L101:
 577  024e 4b08          	push	#8
 578  0250 ae5000        	ldw	x,#20480
 579  0253 cd0000        	call	_GPIO_WriteHigh
 581  0256 84            	pop	a
 582  0257               L301:
 583                     ; 94 	if(r22_CH3REG & ILIML_MASK)GPIO_WriteLow(CH3_ILIM_L); else GPIO_WriteHigh(CH3_ILIM_L);  				
 585  0257 b600          	ld	a,_r22_CH3REG
 586  0259 a504          	bcp	a,#4
 587  025b 270b          	jreq	L501
 590  025d 4b04          	push	#4
 591  025f ae5000        	ldw	x,#20480
 592  0262 cd0000        	call	_GPIO_WriteLow
 594  0265 84            	pop	a
 596  0266 2009          	jra	L701
 597  0268               L501:
 600  0268 4b04          	push	#4
 601  026a ae5000        	ldw	x,#20480
 602  026d cd0000        	call	_GPIO_WriteHigh
 604  0270 84            	pop	a
 605  0271               L701:
 606                     ; 95 	if(r22_CH3REG & PWREN_MASK)GPIO_WriteHigh(CH3_PWR_EN); else GPIO_WriteLow(CH3_PWR_EN);
 608  0271 b600          	ld	a,_r22_CH3REG
 609  0273 a510          	bcp	a,#16
 610  0275 270b          	jreq	L111
 613  0277 4b02          	push	#2
 614  0279 ae5000        	ldw	x,#20480
 615  027c cd0000        	call	_GPIO_WriteHigh
 617  027f 84            	pop	a
 619  0280 2009          	jra	L311
 620  0282               L111:
 623  0282 4b02          	push	#2
 624  0284 ae5000        	ldw	x,#20480
 625  0287 cd0000        	call	_GPIO_WriteLow
 627  028a 84            	pop	a
 628  028b               L311:
 629                     ; 96 	if(r22_CH3REG & DATAEN_MASK)GPIO_WriteLow(CH3_DATA_EN); else GPIO_WriteHigh(CH3_DATA_EN);		
 631  028b b600          	ld	a,_r22_CH3REG
 632  028d a540          	bcp	a,#64
 633  028f 270b          	jreq	L511
 636  0291 4b10          	push	#16
 637  0293 ae500a        	ldw	x,#20490
 638  0296 cd0000        	call	_GPIO_WriteLow
 640  0299 84            	pop	a
 642  029a 2009          	jra	L711
 643  029c               L511:
 646  029c 4b10          	push	#16
 647  029e ae500a        	ldw	x,#20490
 648  02a1 cd0000        	call	_GPIO_WriteHigh
 650  02a4 84            	pop	a
 651  02a5               L711:
 652                     ; 97 	if(GPIO_ReadInputPin(CH3_FAULT))r22_CH3REG &= 0x7F; else r22_CH3REG |= 0x80; 
 654  02a5 4b10          	push	#16
 655  02a7 ae5019        	ldw	x,#20505
 656  02aa cd0000        	call	_GPIO_ReadInputPin
 658  02ad 5b01          	addw	sp,#1
 659  02af 4d            	tnz	a
 660  02b0 2706          	jreq	L121
 663  02b2 721f0000      	bres	_r22_CH3REG,#7
 665  02b6 2004          	jra	L321
 666  02b8               L121:
 669  02b8 721e0000      	bset	_r22_CH3REG,#7
 670  02bc               L321:
 671                     ; 100 	if(GPIO_ReadInputPin(VHOST_PRESENT))r24_AUXREG |= 0x01; else r24_AUXREG &= 0xFE;  
 673  02bc 4b20          	push	#32
 674  02be ae5014        	ldw	x,#20500
 675  02c1 cd0000        	call	_GPIO_ReadInputPin
 677  02c4 5b01          	addw	sp,#1
 678  02c6 4d            	tnz	a
 679  02c7 2706          	jreq	L521
 682  02c9 72100000      	bset	_r24_AUXREG,#0
 684  02cd 2004          	jra	L721
 685  02cf               L521:
 688  02cf 72110000      	bres	_r24_AUXREG,#0
 689  02d3               L721:
 690                     ; 101 	if(GPIO_ReadInputPin(VEXT_PRESENT))r24_AUXREG |= 0x02; else r24_AUXREG &= 0xFD;  
 692  02d3 4b02          	push	#2
 693  02d5 ae500a        	ldw	x,#20490
 694  02d8 cd0000        	call	_GPIO_ReadInputPin
 696  02db 5b01          	addw	sp,#1
 697  02dd 4d            	tnz	a
 698  02de 2706          	jreq	L131
 701  02e0 72120000      	bset	_r24_AUXREG,#1
 703  02e4 2004          	jra	L331
 704  02e6               L131:
 707  02e6 72130000      	bres	_r24_AUXREG,#1
 708  02ea               L331:
 709                     ; 103 }
 712  02ea 81            	ret
 801                     ; 105 void Update_CC_signals(void) {
 802                     	switch	.text
 803  02eb               _Update_CC_signals:
 805  02eb 5209          	subw	sp,#9
 806       00000009      OFST:	set	9
 809                     ; 107 	unsigned int ADC_Ext_CC1 = ADC_Read(Ext_CC1_pin);
 811  02ed a602          	ld	a,#2
 812  02ef cd042b        	call	_ADC_Read
 814  02f2 1f01          	ldw	(OFST-8,sp),x
 816                     ; 108 	unsigned int ADC_Ext_CC2 = ADC_Read(Ext_CC2_pin);
 818  02f4 a603          	ld	a,#3
 819  02f6 cd042b        	call	_ADC_Read
 821  02f9 1f08          	ldw	(OFST-1,sp),x
 823                     ; 109 	unsigned int ADC_Host_CC1 = ADC_Read(Host_CC1_pin);
 825  02fb 4f            	clr	a
 826  02fc cd042b        	call	_ADC_Read
 828  02ff 1f03          	ldw	(OFST-6,sp),x
 830                     ; 110 	unsigned int ADC_Host_CC2 = ADC_Read(Host_CC2_pin);
 832  0301 a601          	ld	a,#1
 833  0303 cd042b        	call	_ADC_Read
 835  0306 1f05          	ldw	(OFST-4,sp),x
 837                     ; 111 	u8 ccsumtemp = 0xff;
 839  0308 a6ff          	ld	a,#255
 840  030a 6b07          	ld	(OFST-2,sp),a
 842                     ; 112 	unsigned int ccActive = 0;
 844                     ; 115 	r30_VEXTCC1L = ADC_Ext_CC1&(0xff);
 846  030c 7b02          	ld	a,(OFST-7,sp)
 847  030e a4ff          	and	a,#255
 848  0310 b700          	ld	_r30_VEXTCC1L,a
 849                     ; 116 	r31_VEXTCC1H = (ADC_Ext_CC1>>8)&(0xff);
 851  0312 7b01          	ld	a,(OFST-8,sp)
 852  0314 b700          	ld	_r31_VEXTCC1H,a
 853                     ; 117 	r32_VEXTCC2L = ADC_Ext_CC2&(0xff);
 855  0316 7b09          	ld	a,(OFST+0,sp)
 856  0318 a4ff          	and	a,#255
 857  031a b700          	ld	_r32_VEXTCC2L,a
 858                     ; 118 	r33_VEXTCC2H = (ADC_Ext_CC2>>8)&(0xff);
 860  031c 7b08          	ld	a,(OFST-1,sp)
 861  031e b700          	ld	_r33_VEXTCC2H,a
 862                     ; 120 	r34_VHOSTCC1L = ADC_Host_CC1&(0xff);
 864  0320 7b04          	ld	a,(OFST-5,sp)
 865  0322 a4ff          	and	a,#255
 866  0324 b700          	ld	_r34_VHOSTCC1L,a
 867                     ; 121 	r35_VHOSTCC1H = (ADC_Host_CC1>>8)&(0xff);
 869  0326 7b03          	ld	a,(OFST-6,sp)
 870  0328 b700          	ld	_r35_VHOSTCC1H,a
 871                     ; 122 	r36_VHOSTCC2L = ADC_Host_CC2&(0xff);
 873  032a 7b06          	ld	a,(OFST-3,sp)
 874  032c a4ff          	and	a,#255
 875  032e b700          	ld	_r36_VHOSTCC2L,a
 876                     ; 123 	r37_VHOSTCC2H = (ADC_Host_CC2>>8)&(0xff);
 878  0330 7b05          	ld	a,(OFST-4,sp)
 879  0332 b700          	ld	_r37_VHOSTCC2H,a
 880                     ; 125 	if(ADC_Ext_CC1 <= NOPULLH && ADC_Ext_CC2 <= NOPULLH) ccsumtemp &=0xF0;
 882  0334 1e01          	ldw	x,(OFST-8,sp)
 883  0336 a30030        	cpw	x,#48
 884  0339 240f          	jruge	L771
 886  033b 1e08          	ldw	x,(OFST-1,sp)
 887  033d a30030        	cpw	x,#48
 888  0340 2408          	jruge	L771
 891  0342 7b07          	ld	a,(OFST-2,sp)
 892  0344 a4f0          	and	a,#240
 893  0346 6b07          	ld	(OFST-2,sp),a
 896  0348 2060          	jra	L102
 897  034a               L771:
 898                     ; 128 		if(ADC_Ext_CC1 > ADC_Ext_CC2){
 900  034a 1e01          	ldw	x,(OFST-8,sp)
 901  034c 1308          	cpw	x,(OFST-1,sp)
 902  034e 230c          	jrule	L302
 903                     ; 129 			ccActive= ADC_Ext_CC1;
 905  0350 1e01          	ldw	x,(OFST-8,sp)
 906  0352 1f08          	ldw	(OFST-1,sp),x
 908                     ; 130 			ccsumtemp &=0xF7;
 910  0354 7b07          	ld	a,(OFST-2,sp)
 911  0356 a4f7          	and	a,#247
 912  0358 6b07          	ld	(OFST-2,sp),a
 915  035a 2006          	jra	L502
 916  035c               L302:
 917                     ; 133 			ccActive=ADC_Ext_CC2;
 920                     ; 134 			ccsumtemp &=0xFB;
 922  035c 7b07          	ld	a,(OFST-2,sp)
 923  035e a4fb          	and	a,#251
 924  0360 6b07          	ld	(OFST-2,sp),a
 926  0362               L502:
 927                     ; 137 		if (ccActive >= DEFLTL && ccActive <= DEFLTH)ccsumtemp &= 0xFD;
 929  0362 1e08          	ldw	x,(OFST-1,sp)
 930  0364 a3004e        	cpw	x,#78
 931  0367 250f          	jrult	L702
 933  0369 1e08          	ldw	x,(OFST-1,sp)
 934  036b a300be        	cpw	x,#190
 935  036e 2408          	jruge	L702
 938  0370 7b07          	ld	a,(OFST-2,sp)
 939  0372 a4fd          	and	a,#253
 940  0374 6b07          	ld	(OFST-2,sp),a
 943  0376 2032          	jra	L102
 944  0378               L702:
 945                     ; 138 		else if (ccActive >= _1A5L && ccActive <= _1A5H)ccsumtemp &= 0xFE;
 947  0378 1e08          	ldw	x,(OFST-1,sp)
 948  037a a300d9        	cpw	x,#217
 949  037d 250f          	jrult	L312
 951  037f 1e08          	ldw	x,(OFST-1,sp)
 952  0381 a30169        	cpw	x,#361
 953  0384 2408          	jruge	L312
 956  0386 7b07          	ld	a,(OFST-2,sp)
 957  0388 a4fe          	and	a,#254
 958  038a 6b07          	ld	(OFST-2,sp),a
 961  038c 201c          	jra	L102
 962  038e               L312:
 963                     ; 139 		else if (ccActive >= _3A0L && ccActive <= _3A0H)ccsumtemp &= 0xFF;
 965  038e 1e08          	ldw	x,(OFST-1,sp)
 966  0390 a30196        	cpw	x,#406
 967  0393 250f          	jrult	L712
 969  0395 1e08          	ldw	x,(OFST-1,sp)
 970  0397 a30279        	cpw	x,#633
 971  039a 2408          	jruge	L712
 974  039c 7b07          	ld	a,(OFST-2,sp)
 975  039e a4ff          	and	a,#255
 976  03a0 6b07          	ld	(OFST-2,sp),a
 979  03a2 2006          	jra	L102
 980  03a4               L712:
 981                     ; 140 		else ccsumtemp &= 0xFC;		
 983  03a4 7b07          	ld	a,(OFST-2,sp)
 984  03a6 a4fc          	and	a,#252
 985  03a8 6b07          	ld	(OFST-2,sp),a
 987  03aa               L102:
 988                     ; 143 	if(ADC_Host_CC1 <= NOPULLH && ADC_Host_CC2 <= NOPULLH) ccsumtemp &=0x0F;
 990  03aa 1e03          	ldw	x,(OFST-6,sp)
 991  03ac a30030        	cpw	x,#48
 992  03af 240f          	jruge	L322
 994  03b1 1e05          	ldw	x,(OFST-4,sp)
 995  03b3 a30030        	cpw	x,#48
 996  03b6 2408          	jruge	L322
 999  03b8 7b07          	ld	a,(OFST-2,sp)
1000  03ba a40f          	and	a,#15
1001  03bc 6b07          	ld	(OFST-2,sp),a
1004  03be 2064          	jra	L522
1005  03c0               L322:
1006                     ; 146 		if(ADC_Host_CC1 > ADC_Host_CC2){
1008  03c0 1e03          	ldw	x,(OFST-6,sp)
1009  03c2 1305          	cpw	x,(OFST-4,sp)
1010  03c4 230c          	jrule	L722
1011                     ; 147 			ccActive= ADC_Host_CC1;
1013  03c6 1e03          	ldw	x,(OFST-6,sp)
1014  03c8 1f08          	ldw	(OFST-1,sp),x
1016                     ; 148 			ccsumtemp &=0x7F;
1018  03ca 7b07          	ld	a,(OFST-2,sp)
1019  03cc a47f          	and	a,#127
1020  03ce 6b07          	ld	(OFST-2,sp),a
1023  03d0 200a          	jra	L132
1024  03d2               L722:
1025                     ; 151 			ccActive=ADC_Host_CC2;
1027  03d2 1e05          	ldw	x,(OFST-4,sp)
1028  03d4 1f08          	ldw	(OFST-1,sp),x
1030                     ; 152 			ccsumtemp &=0xBF;
1032  03d6 7b07          	ld	a,(OFST-2,sp)
1033  03d8 a4bf          	and	a,#191
1034  03da 6b07          	ld	(OFST-2,sp),a
1036  03dc               L132:
1037                     ; 155 		if (ccActive >= DEFLTL && ccActive <= DEFLTH)ccsumtemp &= 0xDF;
1039  03dc 1e08          	ldw	x,(OFST-1,sp)
1040  03de a3004e        	cpw	x,#78
1041  03e1 250f          	jrult	L332
1043  03e3 1e08          	ldw	x,(OFST-1,sp)
1044  03e5 a300be        	cpw	x,#190
1045  03e8 2408          	jruge	L332
1048  03ea 7b07          	ld	a,(OFST-2,sp)
1049  03ec a4df          	and	a,#223
1050  03ee 6b07          	ld	(OFST-2,sp),a
1053  03f0 2032          	jra	L522
1054  03f2               L332:
1055                     ; 156 		else if (ccActive >= _1A5L && ccActive <= _1A5H)ccsumtemp &= 0xEF;
1057  03f2 1e08          	ldw	x,(OFST-1,sp)
1058  03f4 a300d9        	cpw	x,#217
1059  03f7 250f          	jrult	L732
1061  03f9 1e08          	ldw	x,(OFST-1,sp)
1062  03fb a30169        	cpw	x,#361
1063  03fe 2408          	jruge	L732
1066  0400 7b07          	ld	a,(OFST-2,sp)
1067  0402 a4ef          	and	a,#239
1068  0404 6b07          	ld	(OFST-2,sp),a
1071  0406 201c          	jra	L522
1072  0408               L732:
1073                     ; 157 		else if (ccActive >= _3A0L && ccActive <= _3A0H)ccsumtemp &= 0xFF;
1075  0408 1e08          	ldw	x,(OFST-1,sp)
1076  040a a30196        	cpw	x,#406
1077  040d 250f          	jrult	L342
1079  040f 1e08          	ldw	x,(OFST-1,sp)
1080  0411 a30279        	cpw	x,#633
1081  0414 2408          	jruge	L342
1084  0416 7b07          	ld	a,(OFST-2,sp)
1085  0418 a4ff          	and	a,#255
1086  041a 6b07          	ld	(OFST-2,sp),a
1089  041c 2006          	jra	L522
1090  041e               L342:
1091                     ; 158 		else ccsumtemp &= 0xCF;		
1093  041e 7b07          	ld	a,(OFST-2,sp)
1094  0420 a4cf          	and	a,#207
1095  0422 6b07          	ld	(OFST-2,sp),a
1097  0424               L522:
1098                     ; 161 	r23_CCSUM=ccsumtemp;
1100  0424 7b07          	ld	a,(OFST-2,sp)
1101  0426 b700          	ld	_r23_CCSUM,a
1102                     ; 163 }
1105  0428 5b09          	addw	sp,#9
1106  042a 81            	ret
1240                     ; 166 unsigned int ADC_Read(ADC1_Channel_TypeDef ADC_Channel_Number){
1241                     	switch	.text
1242  042b               _ADC_Read:
1244  042b 88            	push	a
1245  042c 89            	pushw	x
1246       00000002      OFST:	set	2
1249                     ; 167    unsigned int result = 0;
1251                     ; 168 	 ADC1_DeInit();
1253  042d cd0000        	call	_ADC1_DeInit
1255                     ; 169 	 ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, 
1255                     ; 170 							 ADC_Channel_Number, 
1255                     ; 171 							 ADC1_PRESSEL_FCPU_D18, 
1255                     ; 172 							 ADC1_EXTTRIG_TIM, 
1255                     ; 173 							 DISABLE, 
1255                     ; 174 							 ADC1_ALIGN_RIGHT, 
1255                     ; 175 							 ADC1_SCHMITTTRIG_ALL, 
1255                     ; 176 							 DISABLE);
1257  0430 4b00          	push	#0
1258  0432 4bff          	push	#255
1259  0434 4b08          	push	#8
1260  0436 4b00          	push	#0
1261  0438 4b00          	push	#0
1262  043a 4b70          	push	#112
1263  043c 7b09          	ld	a,(OFST+7,sp)
1264  043e ae0100        	ldw	x,#256
1265  0441 97            	ld	xl,a
1266  0442 cd0000        	call	_ADC1_Init
1268  0445 5b06          	addw	sp,#6
1269                     ; 177 	ADC1_Cmd(ENABLE);
1271  0447 a601          	ld	a,#1
1272  0449 cd0000        	call	_ADC1_Cmd
1274                     ; 178 	ADC1_StartConversion();
1276  044c cd0000        	call	_ADC1_StartConversion
1279  044f               L523:
1280                     ; 179 	while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == FALSE);
1282  044f a680          	ld	a,#128
1283  0451 cd0000        	call	_ADC1_GetFlagStatus
1285  0454 4d            	tnz	a
1286  0455 27f8          	jreq	L523
1287                     ; 180 	result = ADC1_GetConversionValue();
1289  0457 cd0000        	call	_ADC1_GetConversionValue
1291  045a 1f01          	ldw	(OFST-1,sp),x
1293                     ; 181 	ADC1_ClearFlag(ADC1_FLAG_EOC);
1295  045c a680          	ld	a,#128
1296  045e cd0000        	call	_ADC1_ClearFlag
1298                     ; 182 	ADC1_DeInit();
1300  0461 cd0000        	call	_ADC1_DeInit
1302                     ; 183 	return result;
1304  0464 1e01          	ldw	x,(OFST-1,sp)
1307  0466 5b03          	addw	sp,#3
1308  0468 81            	ret
1359                     	xdef	_hostdebcc2
1360                     	xdef	_hostdebcc1
1361                     	xdef	_extdebcc2
1362                     	xdef	_extdebcc1
1363                     	xref.b	_r37_VHOSTCC2H
1364                     	xref.b	_r36_VHOSTCC2L
1365                     	xref.b	_r35_VHOSTCC1H
1366                     	xref.b	_r34_VHOSTCC1L
1367                     	xref.b	_r33_VEXTCC2H
1368                     	xref.b	_r32_VEXTCC2L
1369                     	xref.b	_r31_VEXTCC1H
1370                     	xref.b	_r30_VEXTCC1L
1371                     	xref.b	_r24_AUXREG
1372                     	xref.b	_r23_CCSUM
1373                     	xref.b	_r22_CH3REG
1374                     	xref.b	_r21_CH2REG
1375                     	xref.b	_r20_CH1REG
1376                     	xdef	_Update_CC_signals
1377                     	xdef	_ADC_Read
1378                     	xdef	_Update_GPIO_from_I2CRegisters
1379                     	xdef	_BaseR_GPIO_Init
1380                     	xref	_ADC1_ClearFlag
1381                     	xref	_ADC1_GetFlagStatus
1382                     	xref	_ADC1_GetConversionValue
1383                     	xref	_ADC1_StartConversion
1384                     	xref	_ADC1_Cmd
1385                     	xref	_ADC1_Init
1386                     	xref	_ADC1_DeInit
1387                     	xref	_GPIO_ReadInputPin
1388                     	xref	_GPIO_WriteLow
1389                     	xref	_GPIO_WriteHigh
1390                     	xref	_GPIO_Init
1391                     	xref	_GPIO_DeInit
1410                     	end

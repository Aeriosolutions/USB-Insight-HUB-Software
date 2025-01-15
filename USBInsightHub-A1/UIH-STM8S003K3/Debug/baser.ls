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
 338                     ; 81 	if(r20_CH1REG & ILIMH_MASK)GPIO_WriteLow(CH1_ILIM_H); 
 340  013f b600          	ld	a,_r20_CH1REG
 341  0141 a502          	bcp	a,#2
 342  0143 270b          	jreq	L13
 345  0145 4b80          	push	#128
 346  0147 ae500a        	ldw	x,#20490
 347  014a cd0000        	call	_GPIO_WriteLow
 349  014d 84            	pop	a
 351  014e 2009          	jra	L33
 352  0150               L13:
 353                     ; 82 	else GPIO_WriteHigh(CH1_ILIM_H); //Active Low
 355  0150 4b80          	push	#128
 356  0152 ae500a        	ldw	x,#20490
 357  0155 cd0000        	call	_GPIO_WriteHigh
 359  0158 84            	pop	a
 360  0159               L33:
 361                     ; 83 	if(r20_CH1REG & ILIML_MASK)GPIO_WriteLow(CH1_ILIM_L); 
 363  0159 b600          	ld	a,_r20_CH1REG
 364  015b a504          	bcp	a,#4
 365  015d 270b          	jreq	L53
 368  015f 4b40          	push	#64
 369  0161 ae500a        	ldw	x,#20490
 370  0164 cd0000        	call	_GPIO_WriteLow
 372  0167 84            	pop	a
 374  0168 2009          	jra	L73
 375  016a               L53:
 376                     ; 84 	else GPIO_WriteHigh(CH1_ILIM_L); //Active Low  				
 378  016a 4b40          	push	#64
 379  016c ae500a        	ldw	x,#20490
 380  016f cd0000        	call	_GPIO_WriteHigh
 382  0172 84            	pop	a
 383  0173               L73:
 384                     ; 85 	if(r20_CH1REG & PWREN_MASK)GPIO_WriteHigh(CH1_PWR_EN); 
 386  0173 b600          	ld	a,_r20_CH1REG
 387  0175 a510          	bcp	a,#16
 388  0177 270b          	jreq	L14
 391  0179 4b20          	push	#32
 392  017b ae500a        	ldw	x,#20490
 393  017e cd0000        	call	_GPIO_WriteHigh
 395  0181 84            	pop	a
 397  0182 2009          	jra	L34
 398  0184               L14:
 399                     ; 86 	else GPIO_WriteLow(CH1_PWR_EN); //Active High
 401  0184 4b20          	push	#32
 402  0186 ae500a        	ldw	x,#20490
 403  0189 cd0000        	call	_GPIO_WriteLow
 405  018c 84            	pop	a
 406  018d               L34:
 407                     ; 87 	if(r20_CH1REG & DATAEN_MASK)GPIO_WriteLow(CH1_DATA_EN); 
 409  018d b600          	ld	a,_r20_CH1REG
 410  018f a540          	bcp	a,#64
 411  0191 270b          	jreq	L54
 414  0193 4b04          	push	#4
 415  0195 ae500a        	ldw	x,#20490
 416  0198 cd0000        	call	_GPIO_WriteLow
 418  019b 84            	pop	a
 420  019c 2009          	jra	L74
 421  019e               L54:
 422                     ; 88 	else GPIO_WriteHigh(CH1_DATA_EN); //Active Low
 424  019e 4b04          	push	#4
 425  01a0 ae500a        	ldw	x,#20490
 426  01a3 cd0000        	call	_GPIO_WriteHigh
 428  01a6 84            	pop	a
 429  01a7               L74:
 430                     ; 89 	if(GPIO_ReadInputPin(CH1_FAULT))r20_CH1REG &= 0x7F; 
 432  01a7 4b01          	push	#1
 433  01a9 ae500f        	ldw	x,#20495
 434  01ac cd0000        	call	_GPIO_ReadInputPin
 436  01af 5b01          	addw	sp,#1
 437  01b1 4d            	tnz	a
 438  01b2 2706          	jreq	L15
 441  01b4 721f0000      	bres	_r20_CH1REG,#7
 443  01b8 2004          	jra	L35
 444  01ba               L15:
 445                     ; 90 	else r20_CH1REG |= 0x80; 
 447  01ba 721e0000      	bset	_r20_CH1REG,#7
 448  01be               L35:
 449                     ; 92 	if(r21_CH2REG & ILIMH_MASK)GPIO_WriteLow(CH2_ILIM_H); 
 451  01be b600          	ld	a,_r21_CH2REG
 452  01c0 a502          	bcp	a,#2
 453  01c2 270b          	jreq	L55
 456  01c4 4b10          	push	#16
 457  01c6 ae500f        	ldw	x,#20495
 458  01c9 cd0000        	call	_GPIO_WriteLow
 460  01cc 84            	pop	a
 462  01cd 2009          	jra	L75
 463  01cf               L55:
 464                     ; 93 	else GPIO_WriteHigh(CH2_ILIM_H);
 466  01cf 4b10          	push	#16
 467  01d1 ae500f        	ldw	x,#20495
 468  01d4 cd0000        	call	_GPIO_WriteHigh
 470  01d7 84            	pop	a
 471  01d8               L75:
 472                     ; 94 	if(r21_CH2REG & ILIML_MASK)GPIO_WriteLow(CH2_ILIM_L); 
 474  01d8 b600          	ld	a,_r21_CH2REG
 475  01da a504          	bcp	a,#4
 476  01dc 270b          	jreq	L16
 479  01de 4b08          	push	#8
 480  01e0 ae500f        	ldw	x,#20495
 481  01e3 cd0000        	call	_GPIO_WriteLow
 483  01e6 84            	pop	a
 485  01e7 2009          	jra	L36
 486  01e9               L16:
 487                     ; 95 	else GPIO_WriteHigh(CH2_ILIM_L);  				
 489  01e9 4b08          	push	#8
 490  01eb ae500f        	ldw	x,#20495
 491  01ee cd0000        	call	_GPIO_WriteHigh
 493  01f1 84            	pop	a
 494  01f2               L36:
 495                     ; 96 	if(r21_CH2REG & PWREN_MASK)GPIO_WriteHigh(CH2_PWR_EN); 
 497  01f2 b600          	ld	a,_r21_CH2REG
 498  01f4 a510          	bcp	a,#16
 499  01f6 270b          	jreq	L56
 502  01f8 4b04          	push	#4
 503  01fa ae500f        	ldw	x,#20495
 504  01fd cd0000        	call	_GPIO_WriteHigh
 506  0200 84            	pop	a
 508  0201 2009          	jra	L76
 509  0203               L56:
 510                     ; 97 	else GPIO_WriteLow(CH2_PWR_EN);
 512  0203 4b04          	push	#4
 513  0205 ae500f        	ldw	x,#20495
 514  0208 cd0000        	call	_GPIO_WriteLow
 516  020b 84            	pop	a
 517  020c               L76:
 518                     ; 98 	if(r21_CH2REG & DATAEN_MASK)GPIO_WriteLow(CH2_DATA_EN); 
 520  020c b600          	ld	a,_r21_CH2REG
 521  020e a540          	bcp	a,#64
 522  0210 270b          	jreq	L17
 525  0212 4b08          	push	#8
 526  0214 ae500a        	ldw	x,#20490
 527  0217 cd0000        	call	_GPIO_WriteLow
 529  021a 84            	pop	a
 531  021b 2009          	jra	L37
 532  021d               L17:
 533                     ; 99 	else GPIO_WriteHigh(CH2_DATA_EN);
 535  021d 4b08          	push	#8
 536  021f ae500a        	ldw	x,#20490
 537  0222 cd0000        	call	_GPIO_WriteHigh
 539  0225 84            	pop	a
 540  0226               L37:
 541                     ; 100 	if(GPIO_ReadInputPin(CH2_FAULT))r21_CH2REG &= 0x7F; 
 543  0226 4b80          	push	#128
 544  0228 ae500f        	ldw	x,#20495
 545  022b cd0000        	call	_GPIO_ReadInputPin
 547  022e 5b01          	addw	sp,#1
 548  0230 4d            	tnz	a
 549  0231 2706          	jreq	L57
 552  0233 721f0000      	bres	_r21_CH2REG,#7
 554  0237 2004          	jra	L77
 555  0239               L57:
 556                     ; 101 	else r21_CH2REG |= 0x80; 		
 558  0239 721e0000      	bset	_r21_CH2REG,#7
 559  023d               L77:
 560                     ; 103 	if(r22_CH3REG & ILIMH_MASK)GPIO_WriteLow(CH3_ILIM_H); 
 562  023d b600          	ld	a,_r22_CH3REG
 563  023f a502          	bcp	a,#2
 564  0241 270b          	jreq	L101
 567  0243 4b08          	push	#8
 568  0245 ae5000        	ldw	x,#20480
 569  0248 cd0000        	call	_GPIO_WriteLow
 571  024b 84            	pop	a
 573  024c 2009          	jra	L301
 574  024e               L101:
 575                     ; 104 	else GPIO_WriteHigh(CH3_ILIM_H);
 577  024e 4b08          	push	#8
 578  0250 ae5000        	ldw	x,#20480
 579  0253 cd0000        	call	_GPIO_WriteHigh
 581  0256 84            	pop	a
 582  0257               L301:
 583                     ; 105 	if(r22_CH3REG & ILIML_MASK)GPIO_WriteLow(CH3_ILIM_L); 
 585  0257 b600          	ld	a,_r22_CH3REG
 586  0259 a504          	bcp	a,#4
 587  025b 270b          	jreq	L501
 590  025d 4b04          	push	#4
 591  025f ae5000        	ldw	x,#20480
 592  0262 cd0000        	call	_GPIO_WriteLow
 594  0265 84            	pop	a
 596  0266 2009          	jra	L701
 597  0268               L501:
 598                     ; 106 	else GPIO_WriteHigh(CH3_ILIM_L);  				
 600  0268 4b04          	push	#4
 601  026a ae5000        	ldw	x,#20480
 602  026d cd0000        	call	_GPIO_WriteHigh
 604  0270 84            	pop	a
 605  0271               L701:
 606                     ; 107 	if(r22_CH3REG & PWREN_MASK)GPIO_WriteHigh(CH3_PWR_EN); 
 608  0271 b600          	ld	a,_r22_CH3REG
 609  0273 a510          	bcp	a,#16
 610  0275 270b          	jreq	L111
 613  0277 4b02          	push	#2
 614  0279 ae5000        	ldw	x,#20480
 615  027c cd0000        	call	_GPIO_WriteHigh
 617  027f 84            	pop	a
 619  0280 2009          	jra	L311
 620  0282               L111:
 621                     ; 108 	else GPIO_WriteLow(CH3_PWR_EN);
 623  0282 4b02          	push	#2
 624  0284 ae5000        	ldw	x,#20480
 625  0287 cd0000        	call	_GPIO_WriteLow
 627  028a 84            	pop	a
 628  028b               L311:
 629                     ; 109 	if(r22_CH3REG & DATAEN_MASK)GPIO_WriteLow(CH3_DATA_EN); 
 631  028b b600          	ld	a,_r22_CH3REG
 632  028d a540          	bcp	a,#64
 633  028f 270b          	jreq	L511
 636  0291 4b10          	push	#16
 637  0293 ae500a        	ldw	x,#20490
 638  0296 cd0000        	call	_GPIO_WriteLow
 640  0299 84            	pop	a
 642  029a 2009          	jra	L711
 643  029c               L511:
 644                     ; 110 	else GPIO_WriteHigh(CH3_DATA_EN);		
 646  029c 4b10          	push	#16
 647  029e ae500a        	ldw	x,#20490
 648  02a1 cd0000        	call	_GPIO_WriteHigh
 650  02a4 84            	pop	a
 651  02a5               L711:
 652                     ; 111 	if(GPIO_ReadInputPin(CH3_FAULT))r22_CH3REG &= 0x7F; 
 654  02a5 4b10          	push	#16
 655  02a7 ae5019        	ldw	x,#20505
 656  02aa cd0000        	call	_GPIO_ReadInputPin
 658  02ad 5b01          	addw	sp,#1
 659  02af 4d            	tnz	a
 660  02b0 2706          	jreq	L121
 663  02b2 721f0000      	bres	_r22_CH3REG,#7
 665  02b6 2004          	jra	L321
 666  02b8               L121:
 667                     ; 112 	else r22_CH3REG |= 0x80; 
 669  02b8 721e0000      	bset	_r22_CH3REG,#7
 670  02bc               L321:
 671                     ; 115 	if(GPIO_ReadInputPin(VHOST_PRESENT))r24_AUXREG 	|= 0x01; 
 673  02bc 4b20          	push	#32
 674  02be ae5014        	ldw	x,#20500
 675  02c1 cd0000        	call	_GPIO_ReadInputPin
 677  02c4 5b01          	addw	sp,#1
 678  02c6 4d            	tnz	a
 679  02c7 2706          	jreq	L521
 682  02c9 72100000      	bset	_r24_AUXREG,#0
 684  02cd 2004          	jra	L721
 685  02cf               L521:
 686                     ; 116 	else r24_AUXREG &= 0xFE;  
 688  02cf 72110000      	bres	_r24_AUXREG,#0
 689  02d3               L721:
 690                     ; 117 	if(GPIO_ReadInputPin(VEXT_PRESENT))r24_AUXREG 	|= 0x02; 
 692  02d3 4b02          	push	#2
 693  02d5 ae500a        	ldw	x,#20490
 694  02d8 cd0000        	call	_GPIO_ReadInputPin
 696  02db 5b01          	addw	sp,#1
 697  02dd 4d            	tnz	a
 698  02de 2706          	jreq	L131
 701  02e0 72120000      	bset	_r24_AUXREG,#1
 703  02e4 2004          	jra	L331
 704  02e6               L131:
 705                     ; 118 	else r24_AUXREG &= 0xFD;
 707  02e6 72130000      	bres	_r24_AUXREG,#1
 708  02ea               L331:
 709                     ; 120 	if(GPIO_ReadInputPin(USB3_MUX_OE))r24_AUXREG 		|= 0x04; 
 711  02ea 4b80          	push	#128
 712  02ec ae5005        	ldw	x,#20485
 713  02ef cd0000        	call	_GPIO_ReadInputPin
 715  02f2 5b01          	addw	sp,#1
 716  02f4 4d            	tnz	a
 717  02f5 2706          	jreq	L531
 720  02f7 72140000      	bset	_r24_AUXREG,#2
 722  02fb 2004          	jra	L731
 723  02fd               L531:
 724                     ; 121 	else r24_AUXREG &= 0xFB;	
 726  02fd 72150000      	bres	_r24_AUXREG,#2
 727  0301               L731:
 728                     ; 123 	if(GPIO_ReadInputPin(USB3_MUX_SEL))r24_AUXREG 	|= 0x08; 
 730  0301 4b40          	push	#64
 731  0303 ae5005        	ldw	x,#20485
 732  0306 cd0000        	call	_GPIO_ReadInputPin
 734  0309 5b01          	addw	sp,#1
 735  030b 4d            	tnz	a
 736  030c 2706          	jreq	L141
 739  030e 72160000      	bset	_r24_AUXREG,#3
 741  0312 2004          	jra	L341
 742  0314               L141:
 743                     ; 124 	else r24_AUXREG &= 0xF7;	
 745  0314 72170000      	bres	_r24_AUXREG,#3
 746  0318               L341:
 747                     ; 125 }
 750  0318 81            	ret
 839                     ; 127 void Update_CC_signals(void) {
 840                     	switch	.text
 841  0319               _Update_CC_signals:
 843  0319 5209          	subw	sp,#9
 844       00000009      OFST:	set	9
 847                     ; 129 	unsigned int ADC_Ext_CC1 = ADC_Read(Ext_CC1_pin);
 849  031b a602          	ld	a,#2
 850  031d cd0459        	call	_ADC_Read
 852  0320 1f01          	ldw	(OFST-8,sp),x
 854                     ; 130 	unsigned int ADC_Ext_CC2 = ADC_Read(Ext_CC2_pin);
 856  0322 a603          	ld	a,#3
 857  0324 cd0459        	call	_ADC_Read
 859  0327 1f08          	ldw	(OFST-1,sp),x
 861                     ; 131 	unsigned int ADC_Host_CC1 = ADC_Read(Host_CC1_pin);
 863  0329 4f            	clr	a
 864  032a cd0459        	call	_ADC_Read
 866  032d 1f03          	ldw	(OFST-6,sp),x
 868                     ; 132 	unsigned int ADC_Host_CC2 = ADC_Read(Host_CC2_pin);
 870  032f a601          	ld	a,#1
 871  0331 cd0459        	call	_ADC_Read
 873  0334 1f05          	ldw	(OFST-4,sp),x
 875                     ; 133 	u8 ccsumtemp = 0xff;
 877  0336 a6ff          	ld	a,#255
 878  0338 6b07          	ld	(OFST-2,sp),a
 880                     ; 134 	unsigned int ccActive = 0;
 882                     ; 137 	r30_VEXTCC1L = ADC_Ext_CC1&(0xff);
 884  033a 7b02          	ld	a,(OFST-7,sp)
 885  033c a4ff          	and	a,#255
 886  033e b700          	ld	_r30_VEXTCC1L,a
 887                     ; 138 	r31_VEXTCC1H = (ADC_Ext_CC1>>8)&(0xff);
 889  0340 7b01          	ld	a,(OFST-8,sp)
 890  0342 b700          	ld	_r31_VEXTCC1H,a
 891                     ; 139 	r32_VEXTCC2L = ADC_Ext_CC2&(0xff);
 893  0344 7b09          	ld	a,(OFST+0,sp)
 894  0346 a4ff          	and	a,#255
 895  0348 b700          	ld	_r32_VEXTCC2L,a
 896                     ; 140 	r33_VEXTCC2H = (ADC_Ext_CC2>>8)&(0xff);
 898  034a 7b08          	ld	a,(OFST-1,sp)
 899  034c b700          	ld	_r33_VEXTCC2H,a
 900                     ; 142 	r34_VHOSTCC1L = ADC_Host_CC1&(0xff);
 902  034e 7b04          	ld	a,(OFST-5,sp)
 903  0350 a4ff          	and	a,#255
 904  0352 b700          	ld	_r34_VHOSTCC1L,a
 905                     ; 143 	r35_VHOSTCC1H = (ADC_Host_CC1>>8)&(0xff);
 907  0354 7b03          	ld	a,(OFST-6,sp)
 908  0356 b700          	ld	_r35_VHOSTCC1H,a
 909                     ; 144 	r36_VHOSTCC2L = ADC_Host_CC2&(0xff);
 911  0358 7b06          	ld	a,(OFST-3,sp)
 912  035a a4ff          	and	a,#255
 913  035c b700          	ld	_r36_VHOSTCC2L,a
 914                     ; 145 	r37_VHOSTCC2H = (ADC_Host_CC2>>8)&(0xff);
 916  035e 7b05          	ld	a,(OFST-4,sp)
 917  0360 b700          	ld	_r37_VHOSTCC2H,a
 918                     ; 147 	if(ADC_Ext_CC1 <= NOPULLH && ADC_Ext_CC2 <= NOPULLH) ccsumtemp &=0xF0;
 920  0362 1e01          	ldw	x,(OFST-8,sp)
 921  0364 a30030        	cpw	x,#48
 922  0367 240f          	jruge	L702
 924  0369 1e08          	ldw	x,(OFST-1,sp)
 925  036b a30030        	cpw	x,#48
 926  036e 2408          	jruge	L702
 929  0370 7b07          	ld	a,(OFST-2,sp)
 930  0372 a4f0          	and	a,#240
 931  0374 6b07          	ld	(OFST-2,sp),a
 934  0376 2060          	jra	L112
 935  0378               L702:
 936                     ; 150 		if(ADC_Ext_CC1 > ADC_Ext_CC2){
 938  0378 1e01          	ldw	x,(OFST-8,sp)
 939  037a 1308          	cpw	x,(OFST-1,sp)
 940  037c 230c          	jrule	L312
 941                     ; 151 			ccActive= ADC_Ext_CC1;
 943  037e 1e01          	ldw	x,(OFST-8,sp)
 944  0380 1f08          	ldw	(OFST-1,sp),x
 946                     ; 152 			ccsumtemp &=0xF7;
 948  0382 7b07          	ld	a,(OFST-2,sp)
 949  0384 a4f7          	and	a,#247
 950  0386 6b07          	ld	(OFST-2,sp),a
 953  0388 2006          	jra	L512
 954  038a               L312:
 955                     ; 155 			ccActive=ADC_Ext_CC2;
 958                     ; 156 			ccsumtemp &=0xFB;
 960  038a 7b07          	ld	a,(OFST-2,sp)
 961  038c a4fb          	and	a,#251
 962  038e 6b07          	ld	(OFST-2,sp),a
 964  0390               L512:
 965                     ; 159 		if (ccActive >= DEFLTL && ccActive <= DEFLTH)ccsumtemp &= 0xFD;
 967  0390 1e08          	ldw	x,(OFST-1,sp)
 968  0392 a3004e        	cpw	x,#78
 969  0395 250f          	jrult	L712
 971  0397 1e08          	ldw	x,(OFST-1,sp)
 972  0399 a300be        	cpw	x,#190
 973  039c 2408          	jruge	L712
 976  039e 7b07          	ld	a,(OFST-2,sp)
 977  03a0 a4fd          	and	a,#253
 978  03a2 6b07          	ld	(OFST-2,sp),a
 981  03a4 2032          	jra	L112
 982  03a6               L712:
 983                     ; 160 		else if (ccActive >= _1A5L && ccActive <= _1A5H)ccsumtemp &= 0xFE;
 985  03a6 1e08          	ldw	x,(OFST-1,sp)
 986  03a8 a300d9        	cpw	x,#217
 987  03ab 250f          	jrult	L322
 989  03ad 1e08          	ldw	x,(OFST-1,sp)
 990  03af a30169        	cpw	x,#361
 991  03b2 2408          	jruge	L322
 994  03b4 7b07          	ld	a,(OFST-2,sp)
 995  03b6 a4fe          	and	a,#254
 996  03b8 6b07          	ld	(OFST-2,sp),a
 999  03ba 201c          	jra	L112
1000  03bc               L322:
1001                     ; 161 		else if (ccActive >= _3A0L && ccActive <= _3A0H)ccsumtemp &= 0xFF;
1003  03bc 1e08          	ldw	x,(OFST-1,sp)
1004  03be a30196        	cpw	x,#406
1005  03c1 250f          	jrult	L722
1007  03c3 1e08          	ldw	x,(OFST-1,sp)
1008  03c5 a30279        	cpw	x,#633
1009  03c8 2408          	jruge	L722
1012  03ca 7b07          	ld	a,(OFST-2,sp)
1013  03cc a4ff          	and	a,#255
1014  03ce 6b07          	ld	(OFST-2,sp),a
1017  03d0 2006          	jra	L112
1018  03d2               L722:
1019                     ; 162 		else ccsumtemp &= 0xFC;		
1021  03d2 7b07          	ld	a,(OFST-2,sp)
1022  03d4 a4fc          	and	a,#252
1023  03d6 6b07          	ld	(OFST-2,sp),a
1025  03d8               L112:
1026                     ; 165 	if(ADC_Host_CC1 <= NOPULLH && ADC_Host_CC2 <= NOPULLH) ccsumtemp &=0x0F;
1028  03d8 1e03          	ldw	x,(OFST-6,sp)
1029  03da a30030        	cpw	x,#48
1030  03dd 240f          	jruge	L332
1032  03df 1e05          	ldw	x,(OFST-4,sp)
1033  03e1 a30030        	cpw	x,#48
1034  03e4 2408          	jruge	L332
1037  03e6 7b07          	ld	a,(OFST-2,sp)
1038  03e8 a40f          	and	a,#15
1039  03ea 6b07          	ld	(OFST-2,sp),a
1042  03ec 2064          	jra	L532
1043  03ee               L332:
1044                     ; 168 		if(ADC_Host_CC1 > ADC_Host_CC2){
1046  03ee 1e03          	ldw	x,(OFST-6,sp)
1047  03f0 1305          	cpw	x,(OFST-4,sp)
1048  03f2 230c          	jrule	L732
1049                     ; 169 			ccActive= ADC_Host_CC1;
1051  03f4 1e03          	ldw	x,(OFST-6,sp)
1052  03f6 1f08          	ldw	(OFST-1,sp),x
1054                     ; 170 			ccsumtemp &=0x7F;
1056  03f8 7b07          	ld	a,(OFST-2,sp)
1057  03fa a47f          	and	a,#127
1058  03fc 6b07          	ld	(OFST-2,sp),a
1061  03fe 200a          	jra	L142
1062  0400               L732:
1063                     ; 173 			ccActive=ADC_Host_CC2;
1065  0400 1e05          	ldw	x,(OFST-4,sp)
1066  0402 1f08          	ldw	(OFST-1,sp),x
1068                     ; 174 			ccsumtemp &=0xBF;
1070  0404 7b07          	ld	a,(OFST-2,sp)
1071  0406 a4bf          	and	a,#191
1072  0408 6b07          	ld	(OFST-2,sp),a
1074  040a               L142:
1075                     ; 177 		if (ccActive >= DEFLTL && ccActive <= DEFLTH)ccsumtemp &= 0xDF;
1077  040a 1e08          	ldw	x,(OFST-1,sp)
1078  040c a3004e        	cpw	x,#78
1079  040f 250f          	jrult	L342
1081  0411 1e08          	ldw	x,(OFST-1,sp)
1082  0413 a300be        	cpw	x,#190
1083  0416 2408          	jruge	L342
1086  0418 7b07          	ld	a,(OFST-2,sp)
1087  041a a4df          	and	a,#223
1088  041c 6b07          	ld	(OFST-2,sp),a
1091  041e 2032          	jra	L532
1092  0420               L342:
1093                     ; 178 		else if (ccActive >= _1A5L && ccActive <= _1A5H)ccsumtemp &= 0xEF;
1095  0420 1e08          	ldw	x,(OFST-1,sp)
1096  0422 a300d9        	cpw	x,#217
1097  0425 250f          	jrult	L742
1099  0427 1e08          	ldw	x,(OFST-1,sp)
1100  0429 a30169        	cpw	x,#361
1101  042c 2408          	jruge	L742
1104  042e 7b07          	ld	a,(OFST-2,sp)
1105  0430 a4ef          	and	a,#239
1106  0432 6b07          	ld	(OFST-2,sp),a
1109  0434 201c          	jra	L532
1110  0436               L742:
1111                     ; 179 		else if (ccActive >= _3A0L && ccActive <= _3A0H)ccsumtemp &= 0xFF;
1113  0436 1e08          	ldw	x,(OFST-1,sp)
1114  0438 a30196        	cpw	x,#406
1115  043b 250f          	jrult	L352
1117  043d 1e08          	ldw	x,(OFST-1,sp)
1118  043f a30279        	cpw	x,#633
1119  0442 2408          	jruge	L352
1122  0444 7b07          	ld	a,(OFST-2,sp)
1123  0446 a4ff          	and	a,#255
1124  0448 6b07          	ld	(OFST-2,sp),a
1127  044a 2006          	jra	L532
1128  044c               L352:
1129                     ; 180 		else ccsumtemp &= 0xCF;		
1131  044c 7b07          	ld	a,(OFST-2,sp)
1132  044e a4cf          	and	a,#207
1133  0450 6b07          	ld	(OFST-2,sp),a
1135  0452               L532:
1136                     ; 183 	r23_CCSUM=ccsumtemp;
1138  0452 7b07          	ld	a,(OFST-2,sp)
1139  0454 b700          	ld	_r23_CCSUM,a
1140                     ; 185 }
1143  0456 5b09          	addw	sp,#9
1144  0458 81            	ret
1278                     ; 188 unsigned int ADC_Read(ADC1_Channel_TypeDef ADC_Channel_Number){
1279                     	switch	.text
1280  0459               _ADC_Read:
1282  0459 88            	push	a
1283  045a 89            	pushw	x
1284       00000002      OFST:	set	2
1287                     ; 189    unsigned int result = 0;
1289                     ; 190 	 ADC1_DeInit();
1291  045b cd0000        	call	_ADC1_DeInit
1293                     ; 191 	 ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, 
1293                     ; 192 							 ADC_Channel_Number, 
1293                     ; 193 							 ADC1_PRESSEL_FCPU_D18, 
1293                     ; 194 							 ADC1_EXTTRIG_TIM, 
1293                     ; 195 							 DISABLE, 
1293                     ; 196 							 ADC1_ALIGN_RIGHT, 
1293                     ; 197 							 ADC1_SCHMITTTRIG_ALL, 
1293                     ; 198 							 DISABLE);
1295  045e 4b00          	push	#0
1296  0460 4bff          	push	#255
1297  0462 4b08          	push	#8
1298  0464 4b00          	push	#0
1299  0466 4b00          	push	#0
1300  0468 4b70          	push	#112
1301  046a 7b09          	ld	a,(OFST+7,sp)
1302  046c ae0100        	ldw	x,#256
1303  046f 97            	ld	xl,a
1304  0470 cd0000        	call	_ADC1_Init
1306  0473 5b06          	addw	sp,#6
1307                     ; 199 	ADC1_Cmd(ENABLE);
1309  0475 a601          	ld	a,#1
1310  0477 cd0000        	call	_ADC1_Cmd
1312                     ; 200 	ADC1_StartConversion();
1314  047a cd0000        	call	_ADC1_StartConversion
1317  047d               L533:
1318                     ; 201 	while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == FALSE);
1320  047d a680          	ld	a,#128
1321  047f cd0000        	call	_ADC1_GetFlagStatus
1323  0482 4d            	tnz	a
1324  0483 27f8          	jreq	L533
1325                     ; 202 	result = ADC1_GetConversionValue();
1327  0485 cd0000        	call	_ADC1_GetConversionValue
1329  0488 1f01          	ldw	(OFST-1,sp),x
1331                     ; 203 	ADC1_ClearFlag(ADC1_FLAG_EOC);
1333  048a a680          	ld	a,#128
1334  048c cd0000        	call	_ADC1_ClearFlag
1336                     ; 204 	ADC1_DeInit();
1338  048f cd0000        	call	_ADC1_DeInit
1340                     ; 205 	return result;
1342  0492 1e01          	ldw	x,(OFST-1,sp)
1345  0494 5b03          	addw	sp,#3
1346  0496 81            	ret
1397                     	xdef	_hostdebcc2
1398                     	xdef	_hostdebcc1
1399                     	xdef	_extdebcc2
1400                     	xdef	_extdebcc1
1401                     	xref.b	_r37_VHOSTCC2H
1402                     	xref.b	_r36_VHOSTCC2L
1403                     	xref.b	_r35_VHOSTCC1H
1404                     	xref.b	_r34_VHOSTCC1L
1405                     	xref.b	_r33_VEXTCC2H
1406                     	xref.b	_r32_VEXTCC2L
1407                     	xref.b	_r31_VEXTCC1H
1408                     	xref.b	_r30_VEXTCC1L
1409                     	xref.b	_r24_AUXREG
1410                     	xref.b	_r23_CCSUM
1411                     	xref.b	_r22_CH3REG
1412                     	xref.b	_r21_CH2REG
1413                     	xref.b	_r20_CH1REG
1414                     	xdef	_Update_CC_signals
1415                     	xdef	_ADC_Read
1416                     	xdef	_Update_GPIO_from_I2CRegisters
1417                     	xdef	_BaseR_GPIO_Init
1418                     	xref	_ADC1_ClearFlag
1419                     	xref	_ADC1_GetFlagStatus
1420                     	xref	_ADC1_GetConversionValue
1421                     	xref	_ADC1_StartConversion
1422                     	xref	_ADC1_Cmd
1423                     	xref	_ADC1_Init
1424                     	xref	_ADC1_DeInit
1425                     	xref	_GPIO_ReadInputPin
1426                     	xref	_GPIO_WriteLow
1427                     	xref	_GPIO_WriteHigh
1428                     	xref	_GPIO_Init
1429                     	xref	_GPIO_DeInit
1448                     	end

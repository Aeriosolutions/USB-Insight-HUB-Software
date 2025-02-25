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
  55                     ; 32 void BaseR_GPIO_Init(void){
  57                     	switch	.text
  58  0000               _BaseR_GPIO_Init:
  62                     ; 34 	GPIO_DeInit(GPIOA);
  64  0000 ae5000        	ldw	x,#20480
  65  0003 cd0000        	call	_GPIO_DeInit
  67                     ; 35 	GPIO_DeInit(GPIOB);
  69  0006 ae5005        	ldw	x,#20485
  70  0009 cd0000        	call	_GPIO_DeInit
  72                     ; 36 	GPIO_DeInit(GPIOC);
  74  000c ae500a        	ldw	x,#20490
  75  000f cd0000        	call	_GPIO_DeInit
  77                     ; 37 	GPIO_DeInit(GPIOD);
  79  0012 ae500f        	ldw	x,#20495
  80  0015 cd0000        	call	_GPIO_DeInit
  82                     ; 38 	GPIO_DeInit(GPIOE);
  84  0018 ae5014        	ldw	x,#20500
  85  001b cd0000        	call	_GPIO_DeInit
  87                     ; 39 	GPIO_DeInit(GPIOF);
  89  001e ae5019        	ldw	x,#20505
  90  0021 cd0000        	call	_GPIO_DeInit
  92                     ; 41 	GPIO_Init(BLINK, GPIO_MODE_OUT_PP_LOW_SLOW);
  94  0024 4bc0          	push	#192
  95  0026 4b40          	push	#64
  96  0028 ae500f        	ldw	x,#20495
  97  002b cd0000        	call	_GPIO_Init
  99  002e 85            	popw	x
 100                     ; 43 	GPIO_Init(CH1_PWR_EN, GPIO_MODE_OUT_PP_LOW_SLOW);
 102  002f 4bc0          	push	#192
 103  0031 4b20          	push	#32
 104  0033 ae500a        	ldw	x,#20490
 105  0036 cd0000        	call	_GPIO_Init
 107  0039 85            	popw	x
 108                     ; 44 	GPIO_Init(CH1_ILIM_H, GPIO_MODE_OUT_OD_HIZ_SLOW);
 110  003a 4b90          	push	#144
 111  003c 4b80          	push	#128
 112  003e ae500a        	ldw	x,#20490
 113  0041 cd0000        	call	_GPIO_Init
 115  0044 85            	popw	x
 116                     ; 45 	GPIO_Init(CH1_ILIM_L, GPIO_MODE_OUT_OD_HIZ_SLOW);
 118  0045 4b90          	push	#144
 119  0047 4b40          	push	#64
 120  0049 ae500a        	ldw	x,#20490
 121  004c cd0000        	call	_GPIO_Init
 123  004f 85            	popw	x
 124                     ; 46 	GPIO_Init(CH1_FAULT, GPIO_MODE_IN_PU_NO_IT);
 126  0050 4b40          	push	#64
 127  0052 4b01          	push	#1
 128  0054 ae500f        	ldw	x,#20495
 129  0057 cd0000        	call	_GPIO_Init
 131  005a 85            	popw	x
 132                     ; 47 	GPIO_Init(CH1_DATA_EN, GPIO_MODE_OUT_PP_LOW_SLOW);
 134  005b 4bc0          	push	#192
 135  005d 4b04          	push	#4
 136  005f ae500a        	ldw	x,#20490
 137  0062 cd0000        	call	_GPIO_Init
 139  0065 85            	popw	x
 140                     ; 49 	GPIO_Init(CH2_PWR_EN, GPIO_MODE_OUT_PP_LOW_SLOW);
 142  0066 4bc0          	push	#192
 143  0068 4b04          	push	#4
 144  006a ae500f        	ldw	x,#20495
 145  006d cd0000        	call	_GPIO_Init
 147  0070 85            	popw	x
 148                     ; 50 	GPIO_Init(CH2_ILIM_H, GPIO_MODE_OUT_OD_HIZ_SLOW);
 150  0071 4b90          	push	#144
 151  0073 4b10          	push	#16
 152  0075 ae500f        	ldw	x,#20495
 153  0078 cd0000        	call	_GPIO_Init
 155  007b 85            	popw	x
 156                     ; 51 	GPIO_Init(CH2_ILIM_L, GPIO_MODE_OUT_OD_HIZ_SLOW);
 158  007c 4b90          	push	#144
 159  007e 4b08          	push	#8
 160  0080 ae500f        	ldw	x,#20495
 161  0083 cd0000        	call	_GPIO_Init
 163  0086 85            	popw	x
 164                     ; 52 	GPIO_Init(CH2_FAULT, GPIO_MODE_IN_PU_NO_IT);
 166  0087 4b40          	push	#64
 167  0089 4b80          	push	#128
 168  008b ae500f        	ldw	x,#20495
 169  008e cd0000        	call	_GPIO_Init
 171  0091 85            	popw	x
 172                     ; 53 	GPIO_Init(CH2_DATA_EN, GPIO_MODE_OUT_PP_LOW_SLOW);
 174  0092 4bc0          	push	#192
 175  0094 4b08          	push	#8
 176  0096 ae500a        	ldw	x,#20490
 177  0099 cd0000        	call	_GPIO_Init
 179  009c 85            	popw	x
 180                     ; 55 	GPIO_Init(CH3_PWR_EN, GPIO_MODE_OUT_PP_LOW_SLOW);
 182  009d 4bc0          	push	#192
 183  009f 4b02          	push	#2
 184  00a1 ae5000        	ldw	x,#20480
 185  00a4 cd0000        	call	_GPIO_Init
 187  00a7 85            	popw	x
 188                     ; 56 	GPIO_Init(CH3_ILIM_H, GPIO_MODE_OUT_OD_HIZ_SLOW);
 190  00a8 4b90          	push	#144
 191  00aa 4b08          	push	#8
 192  00ac ae5000        	ldw	x,#20480
 193  00af cd0000        	call	_GPIO_Init
 195  00b2 85            	popw	x
 196                     ; 57 	GPIO_Init(CH3_ILIM_L, GPIO_MODE_OUT_OD_HIZ_SLOW);
 198  00b3 4b90          	push	#144
 199  00b5 4b04          	push	#4
 200  00b7 ae5000        	ldw	x,#20480
 201  00ba cd0000        	call	_GPIO_Init
 203  00bd 85            	popw	x
 204                     ; 58 	GPIO_Init(CH3_FAULT, GPIO_MODE_IN_PU_NO_IT);
 206  00be 4b40          	push	#64
 207  00c0 4b10          	push	#16
 208  00c2 ae5019        	ldw	x,#20505
 209  00c5 cd0000        	call	_GPIO_Init
 211  00c8 85            	popw	x
 212                     ; 59 	GPIO_Init(CH3_DATA_EN, GPIO_MODE_OUT_PP_LOW_SLOW);
 214  00c9 4bc0          	push	#192
 215  00cb 4b10          	push	#16
 216  00cd ae500a        	ldw	x,#20490
 217  00d0 cd0000        	call	_GPIO_Init
 219  00d3 85            	popw	x
 220                     ; 61 	GPIO_Init(VEXT_PRESENT, GPIO_MODE_IN_FL_NO_IT);
 222  00d4 4b00          	push	#0
 223  00d6 4b02          	push	#2
 224  00d8 ae500a        	ldw	x,#20490
 225  00db cd0000        	call	_GPIO_Init
 227  00de 85            	popw	x
 228                     ; 62 	GPIO_Init(VHOST_PRESENT, GPIO_MODE_IN_FL_NO_IT);
 230  00df 4b00          	push	#0
 231  00e1 4b20          	push	#32
 232  00e3 ae5014        	ldw	x,#20500
 233  00e6 cd0000        	call	_GPIO_Init
 235  00e9 85            	popw	x
 236                     ; 64 	GPIO_Init(USB3_MUX_OE, GPIO_MODE_OUT_PP_HIGH_SLOW);
 238  00ea 4bd0          	push	#208
 239  00ec 4b80          	push	#128
 240  00ee ae5005        	ldw	x,#20485
 241  00f1 cd0000        	call	_GPIO_Init
 243  00f4 85            	popw	x
 244                     ; 65 	GPIO_Init(USB3_MUX_SEL, GPIO_MODE_OUT_PP_HIGH_SLOW);
 246  00f5 4bd0          	push	#208
 247  00f7 4b40          	push	#64
 248  00f9 ae5005        	ldw	x,#20485
 249  00fc cd0000        	call	_GPIO_Init
 251  00ff 85            	popw	x
 252                     ; 68 	GPIO_WriteHigh(USB3_MUX_OE);
 254  0100 4b80          	push	#128
 255  0102 ae5005        	ldw	x,#20485
 256  0105 cd0000        	call	_GPIO_WriteHigh
 258  0108 84            	pop	a
 259                     ; 69 	GPIO_WriteLow(USB3_MUX_SEL);
 261  0109 4b40          	push	#64
 262  010b ae5005        	ldw	x,#20485
 263  010e cd0000        	call	_GPIO_WriteLow
 265  0111 84            	pop	a
 266                     ; 72 	GPIO_Init(VHOST_CC1_AIN0, GPIO_MODE_IN_FL_NO_IT);
 268  0112 4b00          	push	#0
 269  0114 4b01          	push	#1
 270  0116 ae5005        	ldw	x,#20485
 271  0119 cd0000        	call	_GPIO_Init
 273  011c 85            	popw	x
 274                     ; 73 	GPIO_Init(VHOST_CC2_AIN1, GPIO_MODE_IN_FL_NO_IT);
 276  011d 4b00          	push	#0
 277  011f 4b02          	push	#2
 278  0121 ae5005        	ldw	x,#20485
 279  0124 cd0000        	call	_GPIO_Init
 281  0127 85            	popw	x
 282                     ; 74 	GPIO_Init(VEXT_CC1_AIN2, GPIO_MODE_IN_FL_NO_IT);
 284  0128 4b00          	push	#0
 285  012a 4b04          	push	#4
 286  012c ae5005        	ldw	x,#20485
 287  012f cd0000        	call	_GPIO_Init
 289  0132 85            	popw	x
 290                     ; 75 	GPIO_Init(VEXT_CC2_AIN3, GPIO_MODE_IN_FL_NO_IT);
 292  0133 4b00          	push	#0
 293  0135 4b08          	push	#8
 294  0137 ae5005        	ldw	x,#20485
 295  013a cd0000        	call	_GPIO_Init
 297  013d 85            	popw	x
 298                     ; 76 }
 301  013e 81            	ret
 333                     ; 78 void Update_GPIO_from_I2CRegisters(void){
 334                     	switch	.text
 335  013f               _Update_GPIO_from_I2CRegisters:
 339                     ; 83 	if(r20_CH1REG & ILIMH_MASK)GPIO_WriteLow(CH1_ILIM_H); 
 341  013f b600          	ld	a,_r20_CH1REG
 342  0141 a502          	bcp	a,#2
 343  0143 270b          	jreq	L13
 346  0145 4b80          	push	#128
 347  0147 ae500a        	ldw	x,#20490
 348  014a cd0000        	call	_GPIO_WriteLow
 350  014d 84            	pop	a
 352  014e 2009          	jra	L33
 353  0150               L13:
 354                     ; 84 	else GPIO_WriteHigh(CH1_ILIM_H); //Active Low
 356  0150 4b80          	push	#128
 357  0152 ae500a        	ldw	x,#20490
 358  0155 cd0000        	call	_GPIO_WriteHigh
 360  0158 84            	pop	a
 361  0159               L33:
 362                     ; 85 	if(r20_CH1REG & ILIML_MASK)GPIO_WriteLow(CH1_ILIM_L); 
 364  0159 b600          	ld	a,_r20_CH1REG
 365  015b a504          	bcp	a,#4
 366  015d 270b          	jreq	L53
 369  015f 4b40          	push	#64
 370  0161 ae500a        	ldw	x,#20490
 371  0164 cd0000        	call	_GPIO_WriteLow
 373  0167 84            	pop	a
 375  0168 2009          	jra	L73
 376  016a               L53:
 377                     ; 86 	else GPIO_WriteHigh(CH1_ILIM_L); //Active Low  				
 379  016a 4b40          	push	#64
 380  016c ae500a        	ldw	x,#20490
 381  016f cd0000        	call	_GPIO_WriteHigh
 383  0172 84            	pop	a
 384  0173               L73:
 385                     ; 87 	if(r20_CH1REG & PWREN_MASK)GPIO_WriteHigh(CH1_PWR_EN); 
 387  0173 b600          	ld	a,_r20_CH1REG
 388  0175 a510          	bcp	a,#16
 389  0177 270b          	jreq	L14
 392  0179 4b20          	push	#32
 393  017b ae500a        	ldw	x,#20490
 394  017e cd0000        	call	_GPIO_WriteHigh
 396  0181 84            	pop	a
 398  0182 2009          	jra	L34
 399  0184               L14:
 400                     ; 88 	else GPIO_WriteLow(CH1_PWR_EN); //Active High
 402  0184 4b20          	push	#32
 403  0186 ae500a        	ldw	x,#20490
 404  0189 cd0000        	call	_GPIO_WriteLow
 406  018c 84            	pop	a
 407  018d               L34:
 408                     ; 89 	if(r20_CH1REG & DATAEN_MASK)GPIO_WriteLow(CH1_DATA_EN); 
 410  018d b600          	ld	a,_r20_CH1REG
 411  018f a540          	bcp	a,#64
 412  0191 270b          	jreq	L54
 415  0193 4b04          	push	#4
 416  0195 ae500a        	ldw	x,#20490
 417  0198 cd0000        	call	_GPIO_WriteLow
 419  019b 84            	pop	a
 421  019c 2009          	jra	L74
 422  019e               L54:
 423                     ; 90 	else GPIO_WriteHigh(CH1_DATA_EN); //Active Low
 425  019e 4b04          	push	#4
 426  01a0 ae500a        	ldw	x,#20490
 427  01a3 cd0000        	call	_GPIO_WriteHigh
 429  01a6 84            	pop	a
 430  01a7               L74:
 431                     ; 91 	if(GPIO_ReadInputPin(CH1_FAULT))r20_CH1REG &= 0x7F; 
 433  01a7 4b01          	push	#1
 434  01a9 ae500f        	ldw	x,#20495
 435  01ac cd0000        	call	_GPIO_ReadInputPin
 437  01af 5b01          	addw	sp,#1
 438  01b1 4d            	tnz	a
 439  01b2 2706          	jreq	L15
 442  01b4 721f0000      	bres	_r20_CH1REG,#7
 444  01b8 2004          	jra	L35
 445  01ba               L15:
 446                     ; 92 	else r20_CH1REG |= 0x80; 
 448  01ba 721e0000      	bset	_r20_CH1REG,#7
 449  01be               L35:
 450                     ; 94 	if(r21_CH2REG & ILIMH_MASK)GPIO_WriteLow(CH2_ILIM_H); 
 452  01be b600          	ld	a,_r21_CH2REG
 453  01c0 a502          	bcp	a,#2
 454  01c2 270b          	jreq	L55
 457  01c4 4b10          	push	#16
 458  01c6 ae500f        	ldw	x,#20495
 459  01c9 cd0000        	call	_GPIO_WriteLow
 461  01cc 84            	pop	a
 463  01cd 2009          	jra	L75
 464  01cf               L55:
 465                     ; 95 	else GPIO_WriteHigh(CH2_ILIM_H);
 467  01cf 4b10          	push	#16
 468  01d1 ae500f        	ldw	x,#20495
 469  01d4 cd0000        	call	_GPIO_WriteHigh
 471  01d7 84            	pop	a
 472  01d8               L75:
 473                     ; 96 	if(r21_CH2REG & ILIML_MASK)GPIO_WriteLow(CH2_ILIM_L); 
 475  01d8 b600          	ld	a,_r21_CH2REG
 476  01da a504          	bcp	a,#4
 477  01dc 270b          	jreq	L16
 480  01de 4b08          	push	#8
 481  01e0 ae500f        	ldw	x,#20495
 482  01e3 cd0000        	call	_GPIO_WriteLow
 484  01e6 84            	pop	a
 486  01e7 2009          	jra	L36
 487  01e9               L16:
 488                     ; 97 	else GPIO_WriteHigh(CH2_ILIM_L);  				
 490  01e9 4b08          	push	#8
 491  01eb ae500f        	ldw	x,#20495
 492  01ee cd0000        	call	_GPIO_WriteHigh
 494  01f1 84            	pop	a
 495  01f2               L36:
 496                     ; 98 	if(r21_CH2REG & PWREN_MASK)GPIO_WriteHigh(CH2_PWR_EN); 
 498  01f2 b600          	ld	a,_r21_CH2REG
 499  01f4 a510          	bcp	a,#16
 500  01f6 270b          	jreq	L56
 503  01f8 4b04          	push	#4
 504  01fa ae500f        	ldw	x,#20495
 505  01fd cd0000        	call	_GPIO_WriteHigh
 507  0200 84            	pop	a
 509  0201 2009          	jra	L76
 510  0203               L56:
 511                     ; 99 	else GPIO_WriteLow(CH2_PWR_EN);
 513  0203 4b04          	push	#4
 514  0205 ae500f        	ldw	x,#20495
 515  0208 cd0000        	call	_GPIO_WriteLow
 517  020b 84            	pop	a
 518  020c               L76:
 519                     ; 100 	if(r21_CH2REG & DATAEN_MASK)GPIO_WriteLow(CH2_DATA_EN); 
 521  020c b600          	ld	a,_r21_CH2REG
 522  020e a540          	bcp	a,#64
 523  0210 270b          	jreq	L17
 526  0212 4b08          	push	#8
 527  0214 ae500a        	ldw	x,#20490
 528  0217 cd0000        	call	_GPIO_WriteLow
 530  021a 84            	pop	a
 532  021b 2009          	jra	L37
 533  021d               L17:
 534                     ; 101 	else GPIO_WriteHigh(CH2_DATA_EN);
 536  021d 4b08          	push	#8
 537  021f ae500a        	ldw	x,#20490
 538  0222 cd0000        	call	_GPIO_WriteHigh
 540  0225 84            	pop	a
 541  0226               L37:
 542                     ; 102 	if(GPIO_ReadInputPin(CH2_FAULT))r21_CH2REG &= 0x7F; 
 544  0226 4b80          	push	#128
 545  0228 ae500f        	ldw	x,#20495
 546  022b cd0000        	call	_GPIO_ReadInputPin
 548  022e 5b01          	addw	sp,#1
 549  0230 4d            	tnz	a
 550  0231 2706          	jreq	L57
 553  0233 721f0000      	bres	_r21_CH2REG,#7
 555  0237 2004          	jra	L77
 556  0239               L57:
 557                     ; 103 	else r21_CH2REG |= 0x80; 		
 559  0239 721e0000      	bset	_r21_CH2REG,#7
 560  023d               L77:
 561                     ; 105 	if(r22_CH3REG & ILIMH_MASK)GPIO_WriteLow(CH3_ILIM_H); 
 563  023d b600          	ld	a,_r22_CH3REG
 564  023f a502          	bcp	a,#2
 565  0241 270b          	jreq	L101
 568  0243 4b08          	push	#8
 569  0245 ae5000        	ldw	x,#20480
 570  0248 cd0000        	call	_GPIO_WriteLow
 572  024b 84            	pop	a
 574  024c 2009          	jra	L301
 575  024e               L101:
 576                     ; 106 	else GPIO_WriteHigh(CH3_ILIM_H);
 578  024e 4b08          	push	#8
 579  0250 ae5000        	ldw	x,#20480
 580  0253 cd0000        	call	_GPIO_WriteHigh
 582  0256 84            	pop	a
 583  0257               L301:
 584                     ; 107 	if(r22_CH3REG & ILIML_MASK)GPIO_WriteLow(CH3_ILIM_L); 
 586  0257 b600          	ld	a,_r22_CH3REG
 587  0259 a504          	bcp	a,#4
 588  025b 270b          	jreq	L501
 591  025d 4b04          	push	#4
 592  025f ae5000        	ldw	x,#20480
 593  0262 cd0000        	call	_GPIO_WriteLow
 595  0265 84            	pop	a
 597  0266 2009          	jra	L701
 598  0268               L501:
 599                     ; 108 	else GPIO_WriteHigh(CH3_ILIM_L);  				
 601  0268 4b04          	push	#4
 602  026a ae5000        	ldw	x,#20480
 603  026d cd0000        	call	_GPIO_WriteHigh
 605  0270 84            	pop	a
 606  0271               L701:
 607                     ; 109 	if(r22_CH3REG & PWREN_MASK)GPIO_WriteHigh(CH3_PWR_EN); 
 609  0271 b600          	ld	a,_r22_CH3REG
 610  0273 a510          	bcp	a,#16
 611  0275 270b          	jreq	L111
 614  0277 4b02          	push	#2
 615  0279 ae5000        	ldw	x,#20480
 616  027c cd0000        	call	_GPIO_WriteHigh
 618  027f 84            	pop	a
 620  0280 2009          	jra	L311
 621  0282               L111:
 622                     ; 110 	else GPIO_WriteLow(CH3_PWR_EN);
 624  0282 4b02          	push	#2
 625  0284 ae5000        	ldw	x,#20480
 626  0287 cd0000        	call	_GPIO_WriteLow
 628  028a 84            	pop	a
 629  028b               L311:
 630                     ; 111 	if(r22_CH3REG & DATAEN_MASK)GPIO_WriteLow(CH3_DATA_EN); 
 632  028b b600          	ld	a,_r22_CH3REG
 633  028d a540          	bcp	a,#64
 634  028f 270b          	jreq	L511
 637  0291 4b10          	push	#16
 638  0293 ae500a        	ldw	x,#20490
 639  0296 cd0000        	call	_GPIO_WriteLow
 641  0299 84            	pop	a
 643  029a 2009          	jra	L711
 644  029c               L511:
 645                     ; 112 	else GPIO_WriteHigh(CH3_DATA_EN);		
 647  029c 4b10          	push	#16
 648  029e ae500a        	ldw	x,#20490
 649  02a1 cd0000        	call	_GPIO_WriteHigh
 651  02a4 84            	pop	a
 652  02a5               L711:
 653                     ; 113 	if(GPIO_ReadInputPin(CH3_FAULT))r22_CH3REG &= 0x7F; 
 655  02a5 4b10          	push	#16
 656  02a7 ae5019        	ldw	x,#20505
 657  02aa cd0000        	call	_GPIO_ReadInputPin
 659  02ad 5b01          	addw	sp,#1
 660  02af 4d            	tnz	a
 661  02b0 2706          	jreq	L121
 664  02b2 721f0000      	bres	_r22_CH3REG,#7
 666  02b6 2004          	jra	L321
 667  02b8               L121:
 668                     ; 114 	else r22_CH3REG |= 0x80; 
 670  02b8 721e0000      	bset	_r22_CH3REG,#7
 671  02bc               L321:
 672                     ; 117 	if(GPIO_ReadInputPin(VHOST_PRESENT))r24_AUXREG 	|= 0x01; 
 674  02bc 4b20          	push	#32
 675  02be ae5014        	ldw	x,#20500
 676  02c1 cd0000        	call	_GPIO_ReadInputPin
 678  02c4 5b01          	addw	sp,#1
 679  02c6 4d            	tnz	a
 680  02c7 2706          	jreq	L521
 683  02c9 72100000      	bset	_r24_AUXREG,#0
 685  02cd 2004          	jra	L721
 686  02cf               L521:
 687                     ; 118 	else r24_AUXREG &= 0xFE;  
 689  02cf 72110000      	bres	_r24_AUXREG,#0
 690  02d3               L721:
 691                     ; 119 	if(GPIO_ReadInputPin(VEXT_PRESENT))r24_AUXREG 	|= 0x02; 
 693  02d3 4b02          	push	#2
 694  02d5 ae500a        	ldw	x,#20490
 695  02d8 cd0000        	call	_GPIO_ReadInputPin
 697  02db 5b01          	addw	sp,#1
 698  02dd 4d            	tnz	a
 699  02de 2706          	jreq	L131
 702  02e0 72120000      	bset	_r24_AUXREG,#1
 704  02e4 2004          	jra	L331
 705  02e6               L131:
 706                     ; 120 	else r24_AUXREG &= 0xFD;
 708  02e6 72130000      	bres	_r24_AUXREG,#1
 709  02ea               L331:
 710                     ; 122 	if(GPIO_ReadInputPin(USB3_MUX_OE))r24_AUXREG 		|= 0x04; 
 712  02ea 4b80          	push	#128
 713  02ec ae5005        	ldw	x,#20485
 714  02ef cd0000        	call	_GPIO_ReadInputPin
 716  02f2 5b01          	addw	sp,#1
 717  02f4 4d            	tnz	a
 718  02f5 2706          	jreq	L531
 721  02f7 72140000      	bset	_r24_AUXREG,#2
 723  02fb 2004          	jra	L731
 724  02fd               L531:
 725                     ; 123 	else r24_AUXREG &= 0xFB;	
 727  02fd 72150000      	bres	_r24_AUXREG,#2
 728  0301               L731:
 729                     ; 125 	if(GPIO_ReadInputPin(USB3_MUX_SEL))r24_AUXREG 	|= 0x08; 
 731  0301 4b40          	push	#64
 732  0303 ae5005        	ldw	x,#20485
 733  0306 cd0000        	call	_GPIO_ReadInputPin
 735  0309 5b01          	addw	sp,#1
 736  030b 4d            	tnz	a
 737  030c 2706          	jreq	L141
 740  030e 72160000      	bset	_r24_AUXREG,#3
 742  0312 2004          	jra	L341
 743  0314               L141:
 744                     ; 126 	else r24_AUXREG &= 0xF7;
 746  0314 72170000      	bres	_r24_AUXREG,#3
 747  0318               L341:
 748                     ; 128 	if(firstPowerFlag) r24_AUXREG 	|= 0x10;
 750  0318 3d00          	tnz	_firstPowerFlag
 751  031a 2706          	jreq	L541
 754  031c 72180000      	bset	_r24_AUXREG,#4
 756  0320 2004          	jra	L741
 757  0322               L541:
 758                     ; 129 	else r24_AUXREG &= 0xEF;
 760  0322 72190000      	bres	_r24_AUXREG,#4
 761  0326               L741:
 762                     ; 131 }
 765  0326 81            	ret
 854                     ; 133 void Update_CC_signals(void) {
 855                     	switch	.text
 856  0327               _Update_CC_signals:
 858  0327 5209          	subw	sp,#9
 859       00000009      OFST:	set	9
 862                     ; 135 	unsigned int ADC_Ext_CC1 = ADC_Read(Ext_CC1_pin);
 864  0329 a602          	ld	a,#2
 865  032b cd0467        	call	_ADC_Read
 867  032e 1f01          	ldw	(OFST-8,sp),x
 869                     ; 136 	unsigned int ADC_Ext_CC2 = ADC_Read(Ext_CC2_pin);
 871  0330 a603          	ld	a,#3
 872  0332 cd0467        	call	_ADC_Read
 874  0335 1f08          	ldw	(OFST-1,sp),x
 876                     ; 137 	unsigned int ADC_Host_CC1 = ADC_Read(Host_CC1_pin);
 878  0337 4f            	clr	a
 879  0338 cd0467        	call	_ADC_Read
 881  033b 1f03          	ldw	(OFST-6,sp),x
 883                     ; 138 	unsigned int ADC_Host_CC2 = ADC_Read(Host_CC2_pin);
 885  033d a601          	ld	a,#1
 886  033f cd0467        	call	_ADC_Read
 888  0342 1f05          	ldw	(OFST-4,sp),x
 890                     ; 139 	u8 ccsumtemp = 0xff;
 892  0344 a6ff          	ld	a,#255
 893  0346 6b07          	ld	(OFST-2,sp),a
 895                     ; 140 	unsigned int ccActive = 0;
 897                     ; 143 	r30_VEXTCC1L = ADC_Ext_CC1&(0xff);
 899  0348 7b02          	ld	a,(OFST-7,sp)
 900  034a a4ff          	and	a,#255
 901  034c b700          	ld	_r30_VEXTCC1L,a
 902                     ; 144 	r31_VEXTCC1H = (ADC_Ext_CC1>>8)&(0xff);
 904  034e 7b01          	ld	a,(OFST-8,sp)
 905  0350 b700          	ld	_r31_VEXTCC1H,a
 906                     ; 145 	r32_VEXTCC2L = ADC_Ext_CC2&(0xff);
 908  0352 7b09          	ld	a,(OFST+0,sp)
 909  0354 a4ff          	and	a,#255
 910  0356 b700          	ld	_r32_VEXTCC2L,a
 911                     ; 146 	r33_VEXTCC2H = (ADC_Ext_CC2>>8)&(0xff);
 913  0358 7b08          	ld	a,(OFST-1,sp)
 914  035a b700          	ld	_r33_VEXTCC2H,a
 915                     ; 148 	r34_VHOSTCC1L = ADC_Host_CC1&(0xff);
 917  035c 7b04          	ld	a,(OFST-5,sp)
 918  035e a4ff          	and	a,#255
 919  0360 b700          	ld	_r34_VHOSTCC1L,a
 920                     ; 149 	r35_VHOSTCC1H = (ADC_Host_CC1>>8)&(0xff);
 922  0362 7b03          	ld	a,(OFST-6,sp)
 923  0364 b700          	ld	_r35_VHOSTCC1H,a
 924                     ; 150 	r36_VHOSTCC2L = ADC_Host_CC2&(0xff);
 926  0366 7b06          	ld	a,(OFST-3,sp)
 927  0368 a4ff          	and	a,#255
 928  036a b700          	ld	_r36_VHOSTCC2L,a
 929                     ; 151 	r37_VHOSTCC2H = (ADC_Host_CC2>>8)&(0xff);
 931  036c 7b05          	ld	a,(OFST-4,sp)
 932  036e b700          	ld	_r37_VHOSTCC2H,a
 933                     ; 153 	if(ADC_Ext_CC1 <= NOPULLH && ADC_Ext_CC2 <= NOPULLH) ccsumtemp &=0xF0;
 935  0370 1e01          	ldw	x,(OFST-8,sp)
 936  0372 a30030        	cpw	x,#48
 937  0375 240f          	jruge	L312
 939  0377 1e08          	ldw	x,(OFST-1,sp)
 940  0379 a30030        	cpw	x,#48
 941  037c 2408          	jruge	L312
 944  037e 7b07          	ld	a,(OFST-2,sp)
 945  0380 a4f0          	and	a,#240
 946  0382 6b07          	ld	(OFST-2,sp),a
 949  0384 2060          	jra	L512
 950  0386               L312:
 951                     ; 156 		if(ADC_Ext_CC1 > ADC_Ext_CC2){
 953  0386 1e01          	ldw	x,(OFST-8,sp)
 954  0388 1308          	cpw	x,(OFST-1,sp)
 955  038a 230c          	jrule	L712
 956                     ; 157 			ccActive= ADC_Ext_CC1;
 958  038c 1e01          	ldw	x,(OFST-8,sp)
 959  038e 1f08          	ldw	(OFST-1,sp),x
 961                     ; 158 			ccsumtemp &=0xF7;
 963  0390 7b07          	ld	a,(OFST-2,sp)
 964  0392 a4f7          	and	a,#247
 965  0394 6b07          	ld	(OFST-2,sp),a
 968  0396 2006          	jra	L122
 969  0398               L712:
 970                     ; 161 			ccActive=ADC_Ext_CC2;
 973                     ; 162 			ccsumtemp &=0xFB;
 975  0398 7b07          	ld	a,(OFST-2,sp)
 976  039a a4fb          	and	a,#251
 977  039c 6b07          	ld	(OFST-2,sp),a
 979  039e               L122:
 980                     ; 165 		if (ccActive >= DEFLTL && ccActive <= DEFLTH)ccsumtemp &= 0xFD;
 982  039e 1e08          	ldw	x,(OFST-1,sp)
 983  03a0 a3004e        	cpw	x,#78
 984  03a3 250f          	jrult	L322
 986  03a5 1e08          	ldw	x,(OFST-1,sp)
 987  03a7 a300be        	cpw	x,#190
 988  03aa 2408          	jruge	L322
 991  03ac 7b07          	ld	a,(OFST-2,sp)
 992  03ae a4fd          	and	a,#253
 993  03b0 6b07          	ld	(OFST-2,sp),a
 996  03b2 2032          	jra	L512
 997  03b4               L322:
 998                     ; 166 		else if (ccActive >= _1A5L && ccActive <= _1A5H)ccsumtemp &= 0xFE;
1000  03b4 1e08          	ldw	x,(OFST-1,sp)
1001  03b6 a300d9        	cpw	x,#217
1002  03b9 250f          	jrult	L722
1004  03bb 1e08          	ldw	x,(OFST-1,sp)
1005  03bd a30169        	cpw	x,#361
1006  03c0 2408          	jruge	L722
1009  03c2 7b07          	ld	a,(OFST-2,sp)
1010  03c4 a4fe          	and	a,#254
1011  03c6 6b07          	ld	(OFST-2,sp),a
1014  03c8 201c          	jra	L512
1015  03ca               L722:
1016                     ; 167 		else if (ccActive >= _3A0L && ccActive <= _3A0H)ccsumtemp &= 0xFF;
1018  03ca 1e08          	ldw	x,(OFST-1,sp)
1019  03cc a30196        	cpw	x,#406
1020  03cf 250f          	jrult	L332
1022  03d1 1e08          	ldw	x,(OFST-1,sp)
1023  03d3 a30279        	cpw	x,#633
1024  03d6 2408          	jruge	L332
1027  03d8 7b07          	ld	a,(OFST-2,sp)
1028  03da a4ff          	and	a,#255
1029  03dc 6b07          	ld	(OFST-2,sp),a
1032  03de 2006          	jra	L512
1033  03e0               L332:
1034                     ; 168 		else ccsumtemp &= 0xFC;		
1036  03e0 7b07          	ld	a,(OFST-2,sp)
1037  03e2 a4fc          	and	a,#252
1038  03e4 6b07          	ld	(OFST-2,sp),a
1040  03e6               L512:
1041                     ; 171 	if(ADC_Host_CC1 <= NOPULLH && ADC_Host_CC2 <= NOPULLH) ccsumtemp &=0x0F;
1043  03e6 1e03          	ldw	x,(OFST-6,sp)
1044  03e8 a30030        	cpw	x,#48
1045  03eb 240f          	jruge	L732
1047  03ed 1e05          	ldw	x,(OFST-4,sp)
1048  03ef a30030        	cpw	x,#48
1049  03f2 2408          	jruge	L732
1052  03f4 7b07          	ld	a,(OFST-2,sp)
1053  03f6 a40f          	and	a,#15
1054  03f8 6b07          	ld	(OFST-2,sp),a
1057  03fa 2064          	jra	L142
1058  03fc               L732:
1059                     ; 174 		if(ADC_Host_CC1 > ADC_Host_CC2){
1061  03fc 1e03          	ldw	x,(OFST-6,sp)
1062  03fe 1305          	cpw	x,(OFST-4,sp)
1063  0400 230c          	jrule	L342
1064                     ; 175 			ccActive= ADC_Host_CC1;
1066  0402 1e03          	ldw	x,(OFST-6,sp)
1067  0404 1f08          	ldw	(OFST-1,sp),x
1069                     ; 176 			ccsumtemp &=0x7F;
1071  0406 7b07          	ld	a,(OFST-2,sp)
1072  0408 a47f          	and	a,#127
1073  040a 6b07          	ld	(OFST-2,sp),a
1076  040c 200a          	jra	L542
1077  040e               L342:
1078                     ; 179 			ccActive=ADC_Host_CC2;
1080  040e 1e05          	ldw	x,(OFST-4,sp)
1081  0410 1f08          	ldw	(OFST-1,sp),x
1083                     ; 180 			ccsumtemp &=0xBF;
1085  0412 7b07          	ld	a,(OFST-2,sp)
1086  0414 a4bf          	and	a,#191
1087  0416 6b07          	ld	(OFST-2,sp),a
1089  0418               L542:
1090                     ; 183 		if (ccActive >= DEFLTL && ccActive <= DEFLTH)ccsumtemp &= 0xDF;
1092  0418 1e08          	ldw	x,(OFST-1,sp)
1093  041a a3004e        	cpw	x,#78
1094  041d 250f          	jrult	L742
1096  041f 1e08          	ldw	x,(OFST-1,sp)
1097  0421 a300be        	cpw	x,#190
1098  0424 2408          	jruge	L742
1101  0426 7b07          	ld	a,(OFST-2,sp)
1102  0428 a4df          	and	a,#223
1103  042a 6b07          	ld	(OFST-2,sp),a
1106  042c 2032          	jra	L142
1107  042e               L742:
1108                     ; 184 		else if (ccActive >= _1A5L && ccActive <= _1A5H)ccsumtemp &= 0xEF;
1110  042e 1e08          	ldw	x,(OFST-1,sp)
1111  0430 a300d9        	cpw	x,#217
1112  0433 250f          	jrult	L352
1114  0435 1e08          	ldw	x,(OFST-1,sp)
1115  0437 a30169        	cpw	x,#361
1116  043a 2408          	jruge	L352
1119  043c 7b07          	ld	a,(OFST-2,sp)
1120  043e a4ef          	and	a,#239
1121  0440 6b07          	ld	(OFST-2,sp),a
1124  0442 201c          	jra	L142
1125  0444               L352:
1126                     ; 185 		else if (ccActive >= _3A0L && ccActive <= _3A0H)ccsumtemp &= 0xFF;
1128  0444 1e08          	ldw	x,(OFST-1,sp)
1129  0446 a30196        	cpw	x,#406
1130  0449 250f          	jrult	L752
1132  044b 1e08          	ldw	x,(OFST-1,sp)
1133  044d a30279        	cpw	x,#633
1134  0450 2408          	jruge	L752
1137  0452 7b07          	ld	a,(OFST-2,sp)
1138  0454 a4ff          	and	a,#255
1139  0456 6b07          	ld	(OFST-2,sp),a
1142  0458 2006          	jra	L142
1143  045a               L752:
1144                     ; 186 		else ccsumtemp &= 0xCF;		
1146  045a 7b07          	ld	a,(OFST-2,sp)
1147  045c a4cf          	and	a,#207
1148  045e 6b07          	ld	(OFST-2,sp),a
1150  0460               L142:
1151                     ; 189 	r23_CCSUM=ccsumtemp;
1153  0460 7b07          	ld	a,(OFST-2,sp)
1154  0462 b700          	ld	_r23_CCSUM,a
1155                     ; 191 }
1158  0464 5b09          	addw	sp,#9
1159  0466 81            	ret
1293                     ; 194 unsigned int ADC_Read(ADC1_Channel_TypeDef ADC_Channel_Number){
1294                     	switch	.text
1295  0467               _ADC_Read:
1297  0467 88            	push	a
1298  0468 89            	pushw	x
1299       00000002      OFST:	set	2
1302                     ; 195    unsigned int result = 0;
1304                     ; 196 	 ADC1_DeInit();
1306  0469 cd0000        	call	_ADC1_DeInit
1308                     ; 197 	 ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, 
1308                     ; 198 							 ADC_Channel_Number, 
1308                     ; 199 							 ADC1_PRESSEL_FCPU_D18, 
1308                     ; 200 							 ADC1_EXTTRIG_TIM, 
1308                     ; 201 							 DISABLE, 
1308                     ; 202 							 ADC1_ALIGN_RIGHT, 
1308                     ; 203 							 ADC1_SCHMITTTRIG_ALL, 
1308                     ; 204 							 DISABLE);
1310  046c 4b00          	push	#0
1311  046e 4bff          	push	#255
1312  0470 4b08          	push	#8
1313  0472 4b00          	push	#0
1314  0474 4b00          	push	#0
1315  0476 4b70          	push	#112
1316  0478 7b09          	ld	a,(OFST+7,sp)
1317  047a ae0100        	ldw	x,#256
1318  047d 97            	ld	xl,a
1319  047e cd0000        	call	_ADC1_Init
1321  0481 5b06          	addw	sp,#6
1322                     ; 205 	ADC1_Cmd(ENABLE);
1324  0483 a601          	ld	a,#1
1325  0485 cd0000        	call	_ADC1_Cmd
1327                     ; 206 	ADC1_StartConversion();
1329  0488 cd0000        	call	_ADC1_StartConversion
1332  048b               L143:
1333                     ; 207 	while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == FALSE);
1335  048b a680          	ld	a,#128
1336  048d cd0000        	call	_ADC1_GetFlagStatus
1338  0490 4d            	tnz	a
1339  0491 27f8          	jreq	L143
1340                     ; 208 	result = ADC1_GetConversionValue();
1342  0493 cd0000        	call	_ADC1_GetConversionValue
1344  0496 1f01          	ldw	(OFST-1,sp),x
1346                     ; 209 	ADC1_ClearFlag(ADC1_FLAG_EOC);
1348  0498 a680          	ld	a,#128
1349  049a cd0000        	call	_ADC1_ClearFlag
1351                     ; 210 	ADC1_DeInit();
1353  049d cd0000        	call	_ADC1_DeInit
1355                     ; 211 	return result;
1357  04a0 1e01          	ldw	x,(OFST-1,sp)
1360  04a2 5b03          	addw	sp,#3
1361  04a4 81            	ret
1412                     	xref.b	_firstPowerFlag
1413                     	xdef	_hostdebcc2
1414                     	xdef	_hostdebcc1
1415                     	xdef	_extdebcc2
1416                     	xdef	_extdebcc1
1417                     	xref.b	_r37_VHOSTCC2H
1418                     	xref.b	_r36_VHOSTCC2L
1419                     	xref.b	_r35_VHOSTCC1H
1420                     	xref.b	_r34_VHOSTCC1L
1421                     	xref.b	_r33_VEXTCC2H
1422                     	xref.b	_r32_VEXTCC2L
1423                     	xref.b	_r31_VEXTCC1H
1424                     	xref.b	_r30_VEXTCC1L
1425                     	xref.b	_r24_AUXREG
1426                     	xref.b	_r23_CCSUM
1427                     	xref.b	_r22_CH3REG
1428                     	xref.b	_r21_CH2REG
1429                     	xref.b	_r20_CH1REG
1430                     	xdef	_Update_CC_signals
1431                     	xdef	_ADC_Read
1432                     	xdef	_Update_GPIO_from_I2CRegisters
1433                     	xdef	_BaseR_GPIO_Init
1434                     	xref	_ADC1_ClearFlag
1435                     	xref	_ADC1_GetFlagStatus
1436                     	xref	_ADC1_GetConversionValue
1437                     	xref	_ADC1_StartConversion
1438                     	xref	_ADC1_Cmd
1439                     	xref	_ADC1_Init
1440                     	xref	_ADC1_DeInit
1441                     	xref	_GPIO_ReadInputPin
1442                     	xref	_GPIO_WriteLow
1443                     	xref	_GPIO_WriteHigh
1444                     	xref	_GPIO_Init
1445                     	xref	_GPIO_DeInit
1464                     	end

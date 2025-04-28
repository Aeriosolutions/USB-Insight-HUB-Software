   1                     ; C Compiler for STM8 (COSMIC Software)
   2                     ; Parser V4.12.9 - 19 Apr 2023
   3                     ; Generator (Limited) V4.5.6 - 18 Jul 2023
  14                     	bsct
  15  0000               _muxoeReceived:
  16  0000 00            	dc.b	0
  17  0001               _firstPowerFlag:
  18  0001 01            	dc.b	1
  19  0002               _r10_WHOAMI:
  20  0002 10            	dc.b	16
  21  0003               _r12_VERSION:
  22  0003 12            	dc.b	18
  23  0004               _r20_CH1REG:
  24  0004 00            	dc.b	0
  25  0005               _r21_CH2REG:
  26  0005 00            	dc.b	0
  27  0006               _r22_CH3REG:
  28  0006 00            	dc.b	0
  29  0007               _r23_CCSUM:
  30  0007 23            	dc.b	35
  31  0008               _r24_AUXREG:
  32  0008 24            	dc.b	36
  33  0009               _r26_MUXOECTR:
  34  0009 01            	dc.b	1
  35  000a               _r30_VEXTCC1L:
  36  000a 30            	dc.b	48
  37  000b               _r31_VEXTCC1H:
  38  000b 31            	dc.b	49
  39  000c               _r32_VEXTCC2L:
  40  000c 32            	dc.b	50
  41  000d               _r33_VEXTCC2H:
  42  000d 33            	dc.b	51
  43  000e               _r34_VHOSTCC1L:
  44  000e 34            	dc.b	52
  45  000f               _r35_VHOSTCC1H:
  46  000f 35            	dc.b	53
  47  0010               _r36_VHOSTCC2L:
  48  0010 36            	dc.b	54
  49  0011               _r37_VHOSTCC2H:
  50  0011 37            	dc.b	55
  82                     ; 74 	void I2C_transaction_begin(void)
  82                     ; 75 	{
  84                     	switch	.text
  85  0000               _I2C_transaction_begin:
  89                     ; 76 		MessageBegin = TRUE;
  91  0000 35010004      	mov	_MessageBegin,#1
  92                     ; 77 	}
  95  0004 81            	ret
 119                     ; 79 	void I2C_transaction_end(void)
 119                     ; 80 	{
 120                     	switch	.text
 121  0005               _I2C_transaction_end:
 125                     ; 82 	}
 128  0005 81            	ret
 165                     ; 84 	void I2C_byte_received(u8 u8_RxData)
 165                     ; 85 	{
 166                     	switch	.text
 167  0006               _I2C_byte_received:
 169  0006 88            	push	a
 170       00000000      OFST:	set	0
 173                     ; 86 		if (MessageBegin == TRUE) {			
 175  0007 b604          	ld	a,_MessageBegin
 176  0009 a101          	cp	a,#1
 177  000b 2608          	jrne	L74
 178                     ; 87 			reg_address= u8_RxData;
 180  000d 7b01          	ld	a,(OFST+1,sp)
 181  000f b703          	ld	_reg_address,a
 182                     ; 88 			MessageBegin = FALSE;
 184  0011 3f04          	clr	_MessageBegin
 186  0013 200b          	jra	L15
 187  0015               L74:
 188                     ; 92 			Writable_Registers(reg_address, u8_RxData);
 190  0015 7b01          	ld	a,(OFST+1,sp)
 191  0017 97            	ld	xl,a
 192  0018 b603          	ld	a,_reg_address
 193  001a 95            	ld	xh,a
 194  001b cd00b6        	call	_Writable_Registers
 196                     ; 93 			reg_address++;
 198  001e 3c03          	inc	_reg_address
 199  0020               L15:
 200                     ; 95 	}
 203  0020 84            	pop	a
 204  0021 81            	ret
 243                     ; 97 	u8 I2C_byte_write(void)
 243                     ; 98 	{
 244                     	switch	.text
 245  0022               _I2C_byte_write:
 249                     ; 101 		if(reg_address == WHOAMI) 				return WHOAMI_ID;
 251  0022 b603          	ld	a,_reg_address
 252  0024 a110          	cp	a,#16
 253  0026 2603          	jrne	L36
 256  0028 a635          	ld	a,#53
 259  002a 81            	ret
 260  002b               L36:
 261                     ; 102 		else if(reg_address == VERSION) 	return VERNUM;
 263  002b b603          	ld	a,_reg_address
 264  002d a112          	cp	a,#18
 265  002f 2603          	jrne	L76
 268  0031 a604          	ld	a,#4
 271  0033 81            	ret
 272  0034               L76:
 273                     ; 103 		else if(reg_address == CH1REG) 		return r20_CH1REG;
 275  0034 b603          	ld	a,_reg_address
 276  0036 a120          	cp	a,#32
 277  0038 2603          	jrne	L37
 280  003a b604          	ld	a,_r20_CH1REG
 283  003c 81            	ret
 284  003d               L37:
 285                     ; 104 		else if(reg_address == CH2REG) 		return r21_CH2REG;
 287  003d b603          	ld	a,_reg_address
 288  003f a121          	cp	a,#33
 289  0041 2603          	jrne	L77
 292  0043 b605          	ld	a,_r21_CH2REG
 295  0045 81            	ret
 296  0046               L77:
 297                     ; 105 		else if(reg_address == CH3REG) 		return r22_CH3REG;
 299  0046 b603          	ld	a,_reg_address
 300  0048 a122          	cp	a,#34
 301  004a 2603          	jrne	L301
 304  004c b606          	ld	a,_r22_CH3REG
 307  004e 81            	ret
 308  004f               L301:
 309                     ; 106 		else if(reg_address == CCSUM) 		return r23_CCSUM;
 311  004f b603          	ld	a,_reg_address
 312  0051 a123          	cp	a,#35
 313  0053 2603          	jrne	L701
 316  0055 b607          	ld	a,_r23_CCSUM
 319  0057 81            	ret
 320  0058               L701:
 321                     ; 107 		else if(reg_address == AUXREG){
 323  0058 b603          	ld	a,_reg_address
 324  005a a124          	cp	a,#36
 325  005c 2605          	jrne	L311
 326                     ; 108 			firstPowerFlag = FALSE;
 328  005e 3f01          	clr	_firstPowerFlag
 329                     ; 109 			return r24_AUXREG;
 331  0060 b608          	ld	a,_r24_AUXREG
 334  0062 81            	ret
 335  0063               L311:
 336                     ; 111 		else if(reg_address == MUXOECTR) 	return r26_MUXOECTR;
 338  0063 b603          	ld	a,_reg_address
 339  0065 a126          	cp	a,#38
 340  0067 2603          	jrne	L711
 343  0069 b609          	ld	a,_r26_MUXOECTR
 346  006b 81            	ret
 347  006c               L711:
 348                     ; 112 		else if(reg_address == VEXTCC1L) 	return r30_VEXTCC1L;
 350  006c b603          	ld	a,_reg_address
 351  006e a130          	cp	a,#48
 352  0070 2603          	jrne	L321
 355  0072 b60a          	ld	a,_r30_VEXTCC1L
 358  0074 81            	ret
 359  0075               L321:
 360                     ; 113 		else if(reg_address == VEXTCC1H) 	return r31_VEXTCC1H;
 362  0075 b603          	ld	a,_reg_address
 363  0077 a131          	cp	a,#49
 364  0079 2603          	jrne	L721
 367  007b b60b          	ld	a,_r31_VEXTCC1H
 370  007d 81            	ret
 371  007e               L721:
 372                     ; 114 		else if(reg_address == VEXTCC2L) 	return r32_VEXTCC2L;
 374  007e b603          	ld	a,_reg_address
 375  0080 a132          	cp	a,#50
 376  0082 2603          	jrne	L331
 379  0084 b60c          	ld	a,_r32_VEXTCC2L
 382  0086 81            	ret
 383  0087               L331:
 384                     ; 115 		else if(reg_address == VEXTCC2H) 	return r33_VEXTCC2H;
 386  0087 b603          	ld	a,_reg_address
 387  0089 a133          	cp	a,#51
 388  008b 2603          	jrne	L731
 391  008d b60d          	ld	a,_r33_VEXTCC2H
 394  008f 81            	ret
 395  0090               L731:
 396                     ; 116 		else if(reg_address == VHOSTCC1L) return r34_VHOSTCC1L;
 398  0090 b603          	ld	a,_reg_address
 399  0092 a134          	cp	a,#52
 400  0094 2603          	jrne	L341
 403  0096 b60e          	ld	a,_r34_VHOSTCC1L
 406  0098 81            	ret
 407  0099               L341:
 408                     ; 117 		else if(reg_address == VHOSTCC1H) return r35_VHOSTCC1H;
 410  0099 b603          	ld	a,_reg_address
 411  009b a135          	cp	a,#53
 412  009d 2603          	jrne	L741
 415  009f b60f          	ld	a,_r35_VHOSTCC1H
 418  00a1 81            	ret
 419  00a2               L741:
 420                     ; 118 		else if(reg_address == VHOSTCC2L) return r36_VHOSTCC2L;
 422  00a2 b603          	ld	a,_reg_address
 423  00a4 a136          	cp	a,#54
 424  00a6 2603          	jrne	L351
 427  00a8 b610          	ld	a,_r36_VHOSTCC2L
 430  00aa 81            	ret
 431  00ab               L351:
 432                     ; 119 		else if(reg_address == VHOSTCC2H) return r37_VHOSTCC2H;		
 434  00ab b603          	ld	a,_reg_address
 435  00ad a137          	cp	a,#55
 436  00af 2603          	jrne	L751
 439  00b1 b611          	ld	a,_r37_VHOSTCC2H
 442  00b3 81            	ret
 443  00b4               L751:
 444                     ; 120 		else return 0x00;
 446  00b4 4f            	clr	a
 449  00b5 81            	ret
 497                     ; 123 	void Writable_Registers(u8 reg_address, u8 u8_RxData)
 497                     ; 124 	{
 498                     	switch	.text
 499  00b6               _Writable_Registers:
 501  00b6 89            	pushw	x
 502       00000000      OFST:	set	0
 505                     ; 125 		if(reg_address==CH1REG){
 507  00b7 9e            	ld	a,xh
 508  00b8 a120          	cp	a,#32
 509  00ba 260a          	jrne	L502
 510                     ; 127 			r20_CH1REG = (r20_CH1REG | 0x7F ) & u8_RxData; 
 512  00bc b604          	ld	a,_r20_CH1REG
 513  00be aa7f          	or	a,#127
 514  00c0 1402          	and	a,(OFST+2,sp)
 515  00c2 b704          	ld	_r20_CH1REG,a
 517  00c4 2030          	jra	L702
 518  00c6               L502:
 519                     ; 129 		else if(reg_address ==CH2REG){
 521  00c6 7b01          	ld	a,(OFST+1,sp)
 522  00c8 a121          	cp	a,#33
 523  00ca 260a          	jrne	L112
 524                     ; 130 			r21_CH2REG = (r21_CH2REG | 0x7F ) & u8_RxData;
 526  00cc b605          	ld	a,_r21_CH2REG
 527  00ce aa7f          	or	a,#127
 528  00d0 1402          	and	a,(OFST+2,sp)
 529  00d2 b705          	ld	_r21_CH2REG,a
 531  00d4 2020          	jra	L702
 532  00d6               L112:
 533                     ; 132 		else if(reg_address ==CH3REG){
 535  00d6 7b01          	ld	a,(OFST+1,sp)
 536  00d8 a122          	cp	a,#34
 537  00da 260a          	jrne	L512
 538                     ; 133 			r22_CH3REG = (r22_CH3REG | 0x7F ) & u8_RxData;
 540  00dc b606          	ld	a,_r22_CH3REG
 541  00de aa7f          	or	a,#127
 542  00e0 1402          	and	a,(OFST+2,sp)
 543  00e2 b706          	ld	_r22_CH3REG,a
 545  00e4 2010          	jra	L702
 546  00e6               L512:
 547                     ; 135 		else if(reg_address == MUXOECTR){
 549  00e6 7b01          	ld	a,(OFST+1,sp)
 550  00e8 a126          	cp	a,#38
 551  00ea 260a          	jrne	L702
 552                     ; 136 			r26_MUXOECTR = 0x01 & u8_RxData;
 554  00ec 7b02          	ld	a,(OFST+2,sp)
 555  00ee a401          	and	a,#1
 556  00f0 b709          	ld	_r26_MUXOECTR,a
 557                     ; 137 			muxoeReceived = TRUE;
 559  00f2 35010000      	mov	_muxoeReceived,#1
 560  00f6               L702:
 561                     ; 139 	}
 564  00f6 85            	popw	x
 565  00f7 81            	ret
 568                     	switch	.ubsct
 569  0000               L322_sr1:
 570  0000 00            	ds.b	1
 571  0001               L522_sr2:
 572  0001 00            	ds.b	1
 573  0002               L722_sr3:
 574  0002 00            	ds.b	1
 630                     ; 147 @far @interrupt void I2C_Slave_check_event(void) {
 632                     	switch	.text
 633  00f8               f_I2C_Slave_check_event:
 635  00f8 8a            	push	cc
 636  00f9 84            	pop	a
 637  00fa a4bf          	and	a,#191
 638  00fc 88            	push	a
 639  00fd 86            	pop	cc
 640  00fe 3b0002        	push	c_x+2
 641  0101 be00          	ldw	x,c_x
 642  0103 89            	pushw	x
 643  0104 3b0002        	push	c_y+2
 644  0107 be00          	ldw	x,c_y
 645  0109 89            	pushw	x
 648                     ; 154 sr1 = I2C->SR1;
 650  010a 5552170000    	mov	L322_sr1,21015
 651                     ; 155 sr2 = I2C->SR2;
 653  010f 5552180001    	mov	L522_sr2,21016
 654                     ; 156 sr3 = I2C->SR3;
 656  0114 5552190002    	mov	L722_sr3,21017
 657                     ; 159   if (sr2 & (I2C_SR2_WUFH | I2C_SR2_OVR |I2C_SR2_ARLO |I2C_SR2_BERR))
 659  0119 b601          	ld	a,L522_sr2
 660  011b a52b          	bcp	a,#43
 661  011d 2708          	jreq	L752
 662                     ; 161     I2C->CR2|= I2C_CR2_STOP;  // stop communication - release the lines
 664  011f 72125211      	bset	21009,#1
 665                     ; 162     I2C->SR2= 0;					    // clear all error flags
 667  0123 725f5218      	clr	21016
 668  0127               L752:
 669                     ; 165   if ((sr1 & (I2C_SR1_RXNE | I2C_SR1_BTF)) == (I2C_SR1_RXNE | I2C_SR1_BTF))
 671  0127 b600          	ld	a,L322_sr1
 672  0129 a444          	and	a,#68
 673  012b a144          	cp	a,#68
 674  012d 2606          	jrne	L162
 675                     ; 167     I2C_byte_received(I2C->DR);
 677  012f c65216        	ld	a,21014
 678  0132 cd0006        	call	_I2C_byte_received
 680  0135               L162:
 681                     ; 170   if (sr1 & I2C_SR1_RXNE)
 683  0135 b600          	ld	a,L322_sr1
 684  0137 a540          	bcp	a,#64
 685  0139 2706          	jreq	L362
 686                     ; 172     I2C_byte_received(I2C->DR);
 688  013b c65216        	ld	a,21014
 689  013e cd0006        	call	_I2C_byte_received
 691  0141               L362:
 692                     ; 175   if (sr2 & I2C_SR2_AF)
 694  0141 b601          	ld	a,L522_sr2
 695  0143 a504          	bcp	a,#4
 696  0145 2707          	jreq	L562
 697                     ; 177     I2C->SR2 &= ~I2C_SR2_AF;	  // clear AF
 699  0147 72155218      	bres	21016,#2
 700                     ; 178 		I2C_transaction_end();
 702  014b cd0005        	call	_I2C_transaction_end
 704  014e               L562:
 705                     ; 181   if (sr1 & I2C_SR1_STOPF) 
 707  014e b600          	ld	a,L322_sr1
 708  0150 a510          	bcp	a,#16
 709  0152 2707          	jreq	L762
 710                     ; 183     I2C->CR2 |= I2C_CR2_ACK;	  // CR2 write to clear STOPF
 712  0154 72145211      	bset	21009,#2
 713                     ; 184 		I2C_transaction_end();
 715  0158 cd0005        	call	_I2C_transaction_end
 717  015b               L762:
 718                     ; 187   if (sr1 & I2C_SR1_ADDR)
 720  015b b600          	ld	a,L322_sr1
 721  015d a502          	bcp	a,#2
 722  015f 2703          	jreq	L172
 723                     ; 189 		I2C_transaction_begin();
 725  0161 cd0000        	call	_I2C_transaction_begin
 727  0164               L172:
 728                     ; 192   if ((sr1 & (I2C_SR1_TXE | I2C_SR1_BTF)) == (I2C_SR1_TXE | I2C_SR1_BTF))
 730  0164 b600          	ld	a,L322_sr1
 731  0166 a484          	and	a,#132
 732  0168 a184          	cp	a,#132
 733  016a 2608          	jrne	L372
 734                     ; 194 		I2C->DR = I2C_byte_write();
 736  016c cd0022        	call	_I2C_byte_write
 738  016f c75216        	ld	21014,a
 739                     ; 195 		reg_address++;
 741  0172 3c03          	inc	_reg_address
 742  0174               L372:
 743                     ; 198   if (sr1 & I2C_SR1_TXE)
 745  0174 b600          	ld	a,L322_sr1
 746  0176 a580          	bcp	a,#128
 747  0178 2708          	jreq	L572
 748                     ; 200 		I2C->DR = I2C_byte_write();
 750  017a cd0022        	call	_I2C_byte_write
 752  017d c75216        	ld	21014,a
 753                     ; 201 		reg_address++;		
 755  0180 3c03          	inc	_reg_address
 756  0182               L572:
 757                     ; 204 	GPIOB->ODR^=1;
 759  0182 90105005      	bcpl	20485,#0
 760                     ; 205 }
 763  0186 85            	popw	x
 764  0187 bf00          	ldw	c_y,x
 765  0189 320002        	pop	c_y+2
 766  018c 85            	popw	x
 767  018d bf00          	ldw	c_x,x
 768  018f 320002        	pop	c_x+2
 769  0192 80            	iret
 791                     ; 210 void Init_I2C (void)
 791                     ; 211 {
 793                     	switch	.text
 794  0193               _Init_I2C:
 798                     ; 214 	CLK->CKDIVR = 0;                
 800  0193 725f50c6      	clr	20678
 801                     ; 216 	GPIOB->CR1 |= 0x30;
 803  0197 c65008        	ld	a,20488
 804  019a aa30          	or	a,#48
 805  019c c75008        	ld	20488,a
 806                     ; 217 	GPIOB->DDR &= ~0x30;
 808  019f c65007        	ld	a,20487
 809  01a2 a4cf          	and	a,#207
 810  01a4 c75007        	ld	20487,a
 811                     ; 218 	GPIOB->CR2 &= ~0x30;	
 813  01a7 c65009        	ld	a,20489
 814  01aa a4cf          	and	a,#207
 815  01ac c75009        	ld	20489,a
 816                     ; 221 	I2C->CR1 |= 0x01;				        	// Enable I2C peripheral
 818  01af 72105210      	bset	21008,#0
 819                     ; 222 	I2C->CR2 = 0x04;					      		// Enable I2C acknowledgement
 821  01b3 35045211      	mov	21009,#4
 822                     ; 223 	I2C->FREQR = 16; 					      	// Set I2C Freq value (16MHz)
 824  01b7 35105212      	mov	21010,#16
 825                     ; 224 	I2C->OARL = (SLAVE_ADDRESS << 1) ;	// set slave address to 0x51 (put 0xA2 for the register dues to7bit address) 
 827  01bb 35a25213      	mov	21011,#162
 828                     ; 225 	I2C->OARH = 0x40;					      	// Set 7bit address mode
 830  01bf 35405214      	mov	21012,#64
 831                     ; 227 	I2C->ITR	= 0x07;					      // all I2C interrupt enable  
 833  01c3 3507521a      	mov	21018,#7
 834                     ; 228 }
 837  01c7 81            	ret
1054                     	xdef	_I2C_byte_write
1055                     	xdef	_I2C_byte_received
1056                     	xdef	_I2C_transaction_end
1057                     	xdef	_I2C_transaction_begin
1058                     	xdef	_r37_VHOSTCC2H
1059                     	xdef	_r36_VHOSTCC2L
1060                     	xdef	_r35_VHOSTCC1H
1061                     	xdef	_r34_VHOSTCC1L
1062                     	xdef	_r33_VEXTCC2H
1063                     	xdef	_r32_VEXTCC2L
1064                     	xdef	_r31_VEXTCC1H
1065                     	xdef	_r30_VEXTCC1L
1066                     	xdef	_r26_MUXOECTR
1067                     	xdef	_r24_AUXREG
1068                     	xdef	_r23_CCSUM
1069                     	xdef	_r22_CH3REG
1070                     	xdef	_r21_CH2REG
1071                     	xdef	_r20_CH1REG
1072                     	xdef	_r12_VERSION
1073                     	xdef	_r10_WHOAMI
1074                     	xdef	_firstPowerFlag
1075                     	xdef	_muxoeReceived
1076                     	switch	.ubsct
1077  0003               _reg_address:
1078  0003 00            	ds.b	1
1079                     	xdef	_reg_address
1080  0004               _MessageBegin:
1081  0004 00            	ds.b	1
1082                     	xdef	_MessageBegin
1083                     	xdef	f_I2C_Slave_check_event
1084                     	xdef	_Init_I2C
1085                     	xdef	_Writable_Registers
1086                     	xref.b	c_x
1087                     	xref.b	c_y
1107                     	end

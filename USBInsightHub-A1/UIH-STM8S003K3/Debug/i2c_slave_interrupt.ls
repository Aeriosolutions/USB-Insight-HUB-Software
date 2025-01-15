   1                     ; C Compiler for STM8 (COSMIC Software)
   2                     ; Parser V4.12.9 - 19 Apr 2023
   3                     ; Generator (Limited) V4.5.6 - 18 Jul 2023
  14                     	bsct
  15  0000               _muxoeReceived:
  16  0000 00            	dc.b	0
  17  0001               _r10_WHOAMI:
  18  0001 10            	dc.b	16
  19  0002               _r12_VERSION:
  20  0002 12            	dc.b	18
  21  0003               _r20_CH1REG:
  22  0003 00            	dc.b	0
  23  0004               _r21_CH2REG:
  24  0004 00            	dc.b	0
  25  0005               _r22_CH3REG:
  26  0005 00            	dc.b	0
  27  0006               _r23_CCSUM:
  28  0006 23            	dc.b	35
  29  0007               _r24_AUXREG:
  30  0007 24            	dc.b	36
  31  0008               _r26_MUXOECTR:
  32  0008 01            	dc.b	1
  33  0009               _r30_VEXTCC1L:
  34  0009 30            	dc.b	48
  35  000a               _r31_VEXTCC1H:
  36  000a 31            	dc.b	49
  37  000b               _r32_VEXTCC2L:
  38  000b 32            	dc.b	50
  39  000c               _r33_VEXTCC2H:
  40  000c 33            	dc.b	51
  41  000d               _r34_VHOSTCC1L:
  42  000d 34            	dc.b	52
  43  000e               _r35_VHOSTCC1H:
  44  000e 35            	dc.b	53
  45  000f               _r36_VHOSTCC2L:
  46  000f 36            	dc.b	54
  47  0010               _r37_VHOSTCC2H:
  48  0010 37            	dc.b	55
  80                     ; 62 	void I2C_transaction_begin(void)
  80                     ; 63 	{
  82                     	switch	.text
  83  0000               _I2C_transaction_begin:
  87                     ; 64 		MessageBegin = TRUE;
  89  0000 35010004      	mov	_MessageBegin,#1
  90                     ; 65 	}
  93  0004 81            	ret
 117                     ; 67 	void I2C_transaction_end(void)
 117                     ; 68 	{
 118                     	switch	.text
 119  0005               _I2C_transaction_end:
 123                     ; 70 	}
 126  0005 81            	ret
 163                     ; 72 	void I2C_byte_received(u8 u8_RxData)
 163                     ; 73 	{
 164                     	switch	.text
 165  0006               _I2C_byte_received:
 167  0006 88            	push	a
 168       00000000      OFST:	set	0
 171                     ; 75 		if (MessageBegin == TRUE) {			
 173  0007 b604          	ld	a,_MessageBegin
 174  0009 a101          	cp	a,#1
 175  000b 2608          	jrne	L74
 176                     ; 77 			reg_address= u8_RxData;
 178  000d 7b01          	ld	a,(OFST+1,sp)
 179  000f b703          	ld	_reg_address,a
 180                     ; 78 			MessageBegin = FALSE;
 182  0011 3f04          	clr	_MessageBegin
 184  0013 200b          	jra	L15
 185  0015               L74:
 186                     ; 82 			Writable_Registers(reg_address, u8_RxData);
 188  0015 7b01          	ld	a,(OFST+1,sp)
 189  0017 97            	ld	xl,a
 190  0018 b603          	ld	a,_reg_address
 191  001a 95            	ld	xh,a
 192  001b cd00b4        	call	_Writable_Registers
 194                     ; 83 			reg_address++;
 196  001e 3c03          	inc	_reg_address
 197  0020               L15:
 198                     ; 86 	}
 201  0020 84            	pop	a
 202  0021 81            	ret
 240                     ; 88 	u8 I2C_byte_write(void)
 240                     ; 89 	{
 241                     	switch	.text
 242  0022               _I2C_byte_write:
 246                     ; 92 		if(reg_address == WHOAMI) 				return WHOAMI_ID;
 248  0022 b603          	ld	a,_reg_address
 249  0024 a110          	cp	a,#16
 250  0026 2603          	jrne	L36
 253  0028 a635          	ld	a,#53
 256  002a 81            	ret
 257  002b               L36:
 258                     ; 93 		else if(reg_address == VERSION) 	return VERNUM;
 260  002b b603          	ld	a,_reg_address
 261  002d a112          	cp	a,#18
 262  002f 2603          	jrne	L76
 265  0031 a603          	ld	a,#3
 268  0033 81            	ret
 269  0034               L76:
 270                     ; 94 		else if(reg_address == CH1REG) 		return r20_CH1REG;
 272  0034 b603          	ld	a,_reg_address
 273  0036 a120          	cp	a,#32
 274  0038 2603          	jrne	L37
 277  003a b603          	ld	a,_r20_CH1REG
 280  003c 81            	ret
 281  003d               L37:
 282                     ; 95 		else if(reg_address == CH2REG) 		return r21_CH2REG;
 284  003d b603          	ld	a,_reg_address
 285  003f a121          	cp	a,#33
 286  0041 2603          	jrne	L77
 289  0043 b604          	ld	a,_r21_CH2REG
 292  0045 81            	ret
 293  0046               L77:
 294                     ; 96 		else if(reg_address == CH3REG) 		return r22_CH3REG;
 296  0046 b603          	ld	a,_reg_address
 297  0048 a122          	cp	a,#34
 298  004a 2603          	jrne	L301
 301  004c b605          	ld	a,_r22_CH3REG
 304  004e 81            	ret
 305  004f               L301:
 306                     ; 97 		else if(reg_address == CCSUM) 		return r23_CCSUM;
 308  004f b603          	ld	a,_reg_address
 309  0051 a123          	cp	a,#35
 310  0053 2603          	jrne	L701
 313  0055 b606          	ld	a,_r23_CCSUM
 316  0057 81            	ret
 317  0058               L701:
 318                     ; 98 		else if(reg_address == AUXREG) 		return r24_AUXREG;
 320  0058 b603          	ld	a,_reg_address
 321  005a a124          	cp	a,#36
 322  005c 2603          	jrne	L311
 325  005e b607          	ld	a,_r24_AUXREG
 328  0060 81            	ret
 329  0061               L311:
 330                     ; 99 		else if(reg_address == MUXOECTR) 	return r26_MUXOECTR;
 332  0061 b603          	ld	a,_reg_address
 333  0063 a126          	cp	a,#38
 334  0065 2603          	jrne	L711
 337  0067 b608          	ld	a,_r26_MUXOECTR
 340  0069 81            	ret
 341  006a               L711:
 342                     ; 100 		else if(reg_address == VEXTCC1L) 	return r30_VEXTCC1L;
 344  006a b603          	ld	a,_reg_address
 345  006c a130          	cp	a,#48
 346  006e 2603          	jrne	L321
 349  0070 b609          	ld	a,_r30_VEXTCC1L
 352  0072 81            	ret
 353  0073               L321:
 354                     ; 101 		else if(reg_address == VEXTCC1H) 	return r31_VEXTCC1H;
 356  0073 b603          	ld	a,_reg_address
 357  0075 a131          	cp	a,#49
 358  0077 2603          	jrne	L721
 361  0079 b60a          	ld	a,_r31_VEXTCC1H
 364  007b 81            	ret
 365  007c               L721:
 366                     ; 102 		else if(reg_address == VEXTCC2L) 	return r32_VEXTCC2L;
 368  007c b603          	ld	a,_reg_address
 369  007e a132          	cp	a,#50
 370  0080 2603          	jrne	L331
 373  0082 b60b          	ld	a,_r32_VEXTCC2L
 376  0084 81            	ret
 377  0085               L331:
 378                     ; 103 		else if(reg_address == VEXTCC2H) 	return r33_VEXTCC2H;
 380  0085 b603          	ld	a,_reg_address
 381  0087 a133          	cp	a,#51
 382  0089 2603          	jrne	L731
 385  008b b60c          	ld	a,_r33_VEXTCC2H
 388  008d 81            	ret
 389  008e               L731:
 390                     ; 104 		else if(reg_address == VHOSTCC1L) return r34_VHOSTCC1L;
 392  008e b603          	ld	a,_reg_address
 393  0090 a134          	cp	a,#52
 394  0092 2603          	jrne	L341
 397  0094 b60d          	ld	a,_r34_VHOSTCC1L
 400  0096 81            	ret
 401  0097               L341:
 402                     ; 105 		else if(reg_address == VHOSTCC1H) return r35_VHOSTCC1H;
 404  0097 b603          	ld	a,_reg_address
 405  0099 a135          	cp	a,#53
 406  009b 2603          	jrne	L741
 409  009d b60e          	ld	a,_r35_VHOSTCC1H
 412  009f 81            	ret
 413  00a0               L741:
 414                     ; 106 		else if(reg_address == VHOSTCC2L) return r36_VHOSTCC2L;
 416  00a0 b603          	ld	a,_reg_address
 417  00a2 a136          	cp	a,#54
 418  00a4 2603          	jrne	L351
 421  00a6 b60f          	ld	a,_r36_VHOSTCC2L
 424  00a8 81            	ret
 425  00a9               L351:
 426                     ; 107 		else if(reg_address == VHOSTCC2H) return r37_VHOSTCC2H;		
 428  00a9 b603          	ld	a,_reg_address
 429  00ab a137          	cp	a,#55
 430  00ad 2603          	jrne	L751
 433  00af b610          	ld	a,_r37_VHOSTCC2H
 436  00b1 81            	ret
 437  00b2               L751:
 438                     ; 108 		else return 0x00;
 440  00b2 4f            	clr	a
 443  00b3 81            	ret
 491                     ; 111 	void Writable_Registers(u8 reg_address, u8 u8_RxData)
 491                     ; 112 	{
 492                     	switch	.text
 493  00b4               _Writable_Registers:
 495  00b4 89            	pushw	x
 496       00000000      OFST:	set	0
 499                     ; 113 		if(reg_address==CH1REG){
 501  00b5 9e            	ld	a,xh
 502  00b6 a120          	cp	a,#32
 503  00b8 260a          	jrne	L502
 504                     ; 115 			r20_CH1REG = (r20_CH1REG | 0x7F ) & u8_RxData; 
 506  00ba b603          	ld	a,_r20_CH1REG
 507  00bc aa7f          	or	a,#127
 508  00be 1402          	and	a,(OFST+2,sp)
 509  00c0 b703          	ld	_r20_CH1REG,a
 511  00c2 2030          	jra	L702
 512  00c4               L502:
 513                     ; 117 		else if(reg_address ==CH2REG){
 515  00c4 7b01          	ld	a,(OFST+1,sp)
 516  00c6 a121          	cp	a,#33
 517  00c8 260a          	jrne	L112
 518                     ; 118 			r21_CH2REG = (r21_CH2REG | 0x7F ) & u8_RxData;
 520  00ca b604          	ld	a,_r21_CH2REG
 521  00cc aa7f          	or	a,#127
 522  00ce 1402          	and	a,(OFST+2,sp)
 523  00d0 b704          	ld	_r21_CH2REG,a
 525  00d2 2020          	jra	L702
 526  00d4               L112:
 527                     ; 120 		else if(reg_address ==CH3REG){
 529  00d4 7b01          	ld	a,(OFST+1,sp)
 530  00d6 a122          	cp	a,#34
 531  00d8 260a          	jrne	L512
 532                     ; 121 			r22_CH3REG = (r22_CH3REG | 0x7F ) & u8_RxData;
 534  00da b605          	ld	a,_r22_CH3REG
 535  00dc aa7f          	or	a,#127
 536  00de 1402          	and	a,(OFST+2,sp)
 537  00e0 b705          	ld	_r22_CH3REG,a
 539  00e2 2010          	jra	L702
 540  00e4               L512:
 541                     ; 123 		else if(reg_address == MUXOECTR){
 543  00e4 7b01          	ld	a,(OFST+1,sp)
 544  00e6 a126          	cp	a,#38
 545  00e8 260a          	jrne	L702
 546                     ; 124 			r26_MUXOECTR = 0x01 & u8_RxData;
 548  00ea 7b02          	ld	a,(OFST+2,sp)
 549  00ec a401          	and	a,#1
 550  00ee b708          	ld	_r26_MUXOECTR,a
 551                     ; 125 			muxoeReceived = TRUE;
 553  00f0 35010000      	mov	_muxoeReceived,#1
 554  00f4               L702:
 555                     ; 127 	}
 558  00f4 85            	popw	x
 559  00f5 81            	ret
 562                     	switch	.ubsct
 563  0000               L322_sr1:
 564  0000 00            	ds.b	1
 565  0001               L522_sr2:
 566  0001 00            	ds.b	1
 567  0002               L722_sr3:
 568  0002 00            	ds.b	1
 624                     ; 135 @far @interrupt void I2C_Slave_check_event(void) {
 626                     	switch	.text
 627  00f6               f_I2C_Slave_check_event:
 629  00f6 8a            	push	cc
 630  00f7 84            	pop	a
 631  00f8 a4bf          	and	a,#191
 632  00fa 88            	push	a
 633  00fb 86            	pop	cc
 634  00fc 3b0002        	push	c_x+2
 635  00ff be00          	ldw	x,c_x
 636  0101 89            	pushw	x
 637  0102 3b0002        	push	c_y+2
 638  0105 be00          	ldw	x,c_y
 639  0107 89            	pushw	x
 642                     ; 142 sr1 = I2C->SR1;
 644  0108 5552170000    	mov	L322_sr1,21015
 645                     ; 143 sr2 = I2C->SR2;
 647  010d 5552180001    	mov	L522_sr2,21016
 648                     ; 144 sr3 = I2C->SR3;
 650  0112 5552190002    	mov	L722_sr3,21017
 651                     ; 147   if (sr2 & (I2C_SR2_WUFH | I2C_SR2_OVR |I2C_SR2_ARLO |I2C_SR2_BERR))
 653  0117 b601          	ld	a,L522_sr2
 654  0119 a52b          	bcp	a,#43
 655  011b 2708          	jreq	L752
 656                     ; 149     I2C->CR2|= I2C_CR2_STOP;  // stop communication - release the lines
 658  011d 72125211      	bset	21009,#1
 659                     ; 150     I2C->SR2= 0;					    // clear all error flags
 661  0121 725f5218      	clr	21016
 662  0125               L752:
 663                     ; 153   if ((sr1 & (I2C_SR1_RXNE | I2C_SR1_BTF)) == (I2C_SR1_RXNE | I2C_SR1_BTF))
 665  0125 b600          	ld	a,L322_sr1
 666  0127 a444          	and	a,#68
 667  0129 a144          	cp	a,#68
 668  012b 2606          	jrne	L162
 669                     ; 155     I2C_byte_received(I2C->DR);
 671  012d c65216        	ld	a,21014
 672  0130 cd0006        	call	_I2C_byte_received
 674  0133               L162:
 675                     ; 158   if (sr1 & I2C_SR1_RXNE)
 677  0133 b600          	ld	a,L322_sr1
 678  0135 a540          	bcp	a,#64
 679  0137 2706          	jreq	L362
 680                     ; 160     I2C_byte_received(I2C->DR);
 682  0139 c65216        	ld	a,21014
 683  013c cd0006        	call	_I2C_byte_received
 685  013f               L362:
 686                     ; 163   if (sr2 & I2C_SR2_AF)
 688  013f b601          	ld	a,L522_sr2
 689  0141 a504          	bcp	a,#4
 690  0143 2707          	jreq	L562
 691                     ; 165     I2C->SR2 &= ~I2C_SR2_AF;	  // clear AF
 693  0145 72155218      	bres	21016,#2
 694                     ; 166 		I2C_transaction_end();
 696  0149 cd0005        	call	_I2C_transaction_end
 698  014c               L562:
 699                     ; 169   if (sr1 & I2C_SR1_STOPF) 
 701  014c b600          	ld	a,L322_sr1
 702  014e a510          	bcp	a,#16
 703  0150 2707          	jreq	L762
 704                     ; 171     I2C->CR2 |= I2C_CR2_ACK;	  // CR2 write to clear STOPF
 706  0152 72145211      	bset	21009,#2
 707                     ; 172 		I2C_transaction_end();
 709  0156 cd0005        	call	_I2C_transaction_end
 711  0159               L762:
 712                     ; 175   if (sr1 & I2C_SR1_ADDR)
 714  0159 b600          	ld	a,L322_sr1
 715  015b a502          	bcp	a,#2
 716  015d 2703          	jreq	L172
 717                     ; 177 		I2C_transaction_begin();
 719  015f cd0000        	call	_I2C_transaction_begin
 721  0162               L172:
 722                     ; 180   if ((sr1 & (I2C_SR1_TXE | I2C_SR1_BTF)) == (I2C_SR1_TXE | I2C_SR1_BTF))
 724  0162 b600          	ld	a,L322_sr1
 725  0164 a484          	and	a,#132
 726  0166 a184          	cp	a,#132
 727  0168 2608          	jrne	L372
 728                     ; 182 		I2C->DR = I2C_byte_write();
 730  016a cd0022        	call	_I2C_byte_write
 732  016d c75216        	ld	21014,a
 733                     ; 183 		reg_address++;
 735  0170 3c03          	inc	_reg_address
 736  0172               L372:
 737                     ; 186   if (sr1 & I2C_SR1_TXE)
 739  0172 b600          	ld	a,L322_sr1
 740  0174 a580          	bcp	a,#128
 741  0176 2708          	jreq	L572
 742                     ; 188 		I2C->DR = I2C_byte_write();
 744  0178 cd0022        	call	_I2C_byte_write
 746  017b c75216        	ld	21014,a
 747                     ; 189 		reg_address++;		
 749  017e 3c03          	inc	_reg_address
 750  0180               L572:
 751                     ; 192 	GPIOB->ODR^=1;
 753  0180 90105005      	bcpl	20485,#0
 754                     ; 193 }
 757  0184 85            	popw	x
 758  0185 bf00          	ldw	c_y,x
 759  0187 320002        	pop	c_y+2
 760  018a 85            	popw	x
 761  018b bf00          	ldw	c_x,x
 762  018d 320002        	pop	c_x+2
 763  0190 80            	iret
 785                     ; 198 void Init_I2C (void)
 785                     ; 199 {
 787                     	switch	.text
 788  0191               _Init_I2C:
 792                     ; 202 	CLK->CKDIVR = 0;                
 794  0191 725f50c6      	clr	20678
 795                     ; 204 	GPIOB->CR1 |= 0x30;
 797  0195 c65008        	ld	a,20488
 798  0198 aa30          	or	a,#48
 799  019a c75008        	ld	20488,a
 800                     ; 205 	GPIOB->DDR &= ~0x30;
 802  019d c65007        	ld	a,20487
 803  01a0 a4cf          	and	a,#207
 804  01a2 c75007        	ld	20487,a
 805                     ; 206 	GPIOB->CR2 &= ~0x30;	
 807  01a5 c65009        	ld	a,20489
 808  01a8 a4cf          	and	a,#207
 809  01aa c75009        	ld	20489,a
 810                     ; 209 	I2C->CR1 |= 0x01;				        	// Enable I2C peripheral
 812  01ad 72105210      	bset	21008,#0
 813                     ; 210 	I2C->CR2 = 0x04;					      		// Enable I2C acknowledgement
 815  01b1 35045211      	mov	21009,#4
 816                     ; 211 	I2C->FREQR = 16; 					      	// Set I2C Freq value (16MHz)
 818  01b5 35105212      	mov	21010,#16
 819                     ; 212 	I2C->OARL = (SLAVE_ADDRESS << 1) ;	// set slave address to 0x51 (put 0xA2 for the register dues to7bit address) 
 821  01b9 35a25213      	mov	21011,#162
 822                     ; 213 	I2C->OARH = 0x40;					      	// Set 7bit address mode
 824  01bd 35405214      	mov	21012,#64
 825                     ; 215 	I2C->ITR	= 0x07;					      // all I2C interrupt enable  
 827  01c1 3507521a      	mov	21018,#7
 828                     ; 216 }
 831  01c5 81            	ret
1038                     	xdef	_I2C_byte_write
1039                     	xdef	_I2C_byte_received
1040                     	xdef	_I2C_transaction_end
1041                     	xdef	_I2C_transaction_begin
1042                     	xdef	_r37_VHOSTCC2H
1043                     	xdef	_r36_VHOSTCC2L
1044                     	xdef	_r35_VHOSTCC1H
1045                     	xdef	_r34_VHOSTCC1L
1046                     	xdef	_r33_VEXTCC2H
1047                     	xdef	_r32_VEXTCC2L
1048                     	xdef	_r31_VEXTCC1H
1049                     	xdef	_r30_VEXTCC1L
1050                     	xdef	_r26_MUXOECTR
1051                     	xdef	_r24_AUXREG
1052                     	xdef	_r23_CCSUM
1053                     	xdef	_r22_CH3REG
1054                     	xdef	_r21_CH2REG
1055                     	xdef	_r20_CH1REG
1056                     	xdef	_r12_VERSION
1057                     	xdef	_r10_WHOAMI
1058                     	xdef	_muxoeReceived
1059                     	switch	.ubsct
1060  0003               _reg_address:
1061  0003 00            	ds.b	1
1062                     	xdef	_reg_address
1063  0004               _MessageBegin:
1064  0004 00            	ds.b	1
1065                     	xdef	_MessageBegin
1066                     	xdef	f_I2C_Slave_check_event
1067                     	xdef	_Init_I2C
1068                     	xdef	_Writable_Registers
1069                     	xref.b	c_x
1070                     	xref.b	c_y
1090                     	end

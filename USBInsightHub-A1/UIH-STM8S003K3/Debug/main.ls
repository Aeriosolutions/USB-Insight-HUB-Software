   1                     ; C Compiler for STM8 (COSMIC Software)
   2                     ; Parser V4.12.9 - 19 Apr 2023
   3                     ; Generator (Limited) V4.5.6 - 18 Jul 2023
  14                     	bsct
  15  0000               _currentTime:
  16  0000 00000000      	dc.l	0
  17  0004               _lastBlink:
  18  0004 00000000      	dc.l	0
  19  0008               _lastCCScan:
  20  0008 00000000      	dc.l	0
  21  000c               _IOScan:
  22  000c 00000000      	dc.l	0
  23  0010               _ccActive:
  24  0010 00            	dc.b	0
  25  0011               _prevccActive:
  26  0011 04            	dc.b	4
  27  0012               _muxDebounce:
  28  0012 00            	dc.b	0
  29  0013               _waitEnd:
  30  0013 00            	dc.b	0
  81                     .const:	section	.text
  82  0000               L6:
  83  0000 000003e8      	dc.l	1000
  84  0004               L01:
  85  0004 00000064      	dc.l	100
  86  0008               L21:
  87  0008 0000000a      	dc.l	10
  88                     ; 44 void main(void)
  88                     ; 45 {
  89                     	scross	off
  90                     	switch	.text
  91  0000               _main:
  95                     ; 48 	BaseR_GPIO_Init();
  97  0000 cd0000        	call	_BaseR_GPIO_Init
  99                     ; 51 	CLK_DeInit(); //
 101  0003 cd0000        	call	_CLK_DeInit
 103                     ; 52   TIM4_init(); //already setup f_Master = HSI/1 = 16MHz
 105  0006 cd0000        	call	_TIM4_init
 107                     ; 54 	CLK->PCKENR2 |= 0x08; //Enable Peripheral Clock for ADC
 109  0009 721650ca      	bset	20682,#3
 110                     ; 56 	Init_I2C();
 112  000d cd0000        	call	_Init_I2C
 114                     ; 59 	enableInterrupts();
 117  0010 9a            rim
 119  0011               L12:
 120                     ; 64 		currentTime = millis();
 122  0011 cd0000        	call	_millis
 124  0014 ae0000        	ldw	x,#_currentTime
 125  0017 cd0000        	call	c_rtol
 127                     ; 66 		if(currentTime >= WAIT_I2C_AT_START || comActive) 
 129  001a ae0000        	ldw	x,#_currentTime
 130  001d cd0000        	call	c_ltor
 132  0020 ae0000        	ldw	x,#L6
 133  0023 cd0000        	call	c_lcmp
 135  0026 2404          	jruge	L72
 137  0028 3d00          	tnz	_comActive
 138  002a 2704          	jreq	L52
 139  002c               L72:
 140                     ; 67 			waitEnd = TRUE;
 142  002c 35010013      	mov	_waitEnd,#1
 143  0030               L52:
 144                     ; 69     if(currentTime - lastBlink >= BLINK_INTERVAL)
 146  0030 ae0000        	ldw	x,#_currentTime
 147  0033 cd0000        	call	c_ltor
 149  0036 ae0004        	ldw	x,#_lastBlink
 150  0039 cd0000        	call	c_lsub
 152  003c ae0004        	ldw	x,#L01
 153  003f cd0000        	call	c_lcmp
 155  0042 2511          	jrult	L13
 156                     ; 71 			lastBlink = currentTime;
 158  0044 be02          	ldw	x,_currentTime+2
 159  0046 bf06          	ldw	_lastBlink+2,x
 160  0048 be00          	ldw	x,_currentTime
 161  004a bf04          	ldw	_lastBlink,x
 162                     ; 72       GPIO_WriteReverse(BLINK);      
 164  004c 4b40          	push	#64
 165  004e ae500f        	ldw	x,#20495
 166  0051 cd0000        	call	_GPIO_WriteReverse
 168  0054 84            	pop	a
 169  0055               L13:
 170                     ; 75 		if(currentTime - lastCCScan >= CCSCAN_INTERVAL)
 172  0055 ae0000        	ldw	x,#_currentTime
 173  0058 cd0000        	call	c_ltor
 175  005b ae0008        	ldw	x,#_lastCCScan
 176  005e cd0000        	call	c_lsub
 178  0061 ae0008        	ldw	x,#L21
 179  0064 cd0000        	call	c_lcmp
 181  0067 2403          	jruge	L41
 182  0069 cc00ff        	jp	L33
 183  006c               L41:
 184                     ; 77 			lastCCScan = currentTime;
 186  006c be02          	ldw	x,_currentTime+2
 187  006e bf0a          	ldw	_lastCCScan+2,x
 188  0070 be00          	ldw	x,_currentTime
 189  0072 bf08          	ldw	_lastCCScan,x
 190                     ; 78 			Update_CC_signals();
 192  0074 cd0000        	call	_Update_CC_signals
 194                     ; 80 			if((r23_CCSUM & 0xC0)== 0x00 || (r23_CCSUM & 0xC0)== 0xC0) ccActive=0;
 196  0077 b600          	ld	a,_r23_CCSUM
 197  0079 a5c0          	bcp	a,#192
 198  007b 2708          	jreq	L73
 200  007d b600          	ld	a,_r23_CCSUM
 201  007f a4c0          	and	a,#192
 202  0081 a1c0          	cp	a,#192
 203  0083 2602          	jrne	L53
 204  0085               L73:
 207  0085 3f10          	clr	_ccActive
 208  0087               L53:
 209                     ; 81 			if((r23_CCSUM & 0xC0)== 0x40) ccActive = 1;
 211  0087 b600          	ld	a,_r23_CCSUM
 212  0089 a4c0          	and	a,#192
 213  008b a140          	cp	a,#64
 214  008d 2604          	jrne	L14
 217  008f 35010010      	mov	_ccActive,#1
 218  0093               L14:
 219                     ; 82 			if((r23_CCSUM & 0xC0)== 0x80) ccActive = 2;
 221  0093 b600          	ld	a,_r23_CCSUM
 222  0095 a4c0          	and	a,#192
 223  0097 a180          	cp	a,#128
 224  0099 2604          	jrne	L34
 227  009b 35020010      	mov	_ccActive,#2
 228  009f               L34:
 229                     ; 84 			if(ccActive != prevccActive){
 231  009f b610          	ld	a,_ccActive
 232  00a1 b111          	cp	a,_prevccActive
 233  00a3 270d          	jreq	L54
 234                     ; 85 				muxDebounce++;
 236  00a5 3c12          	inc	_muxDebounce
 237                     ; 86 				if(muxDebounce > CCDEBOUNCE){
 239  00a7 b612          	ld	a,_muxDebounce
 240  00a9 a104          	cp	a,#4
 241  00ab 2505          	jrult	L54
 242                     ; 87 					muxDebounce = 0;
 244  00ad 3f12          	clr	_muxDebounce
 245                     ; 88 					prevccActive = ccActive;
 247  00af 451011        	mov	_prevccActive,_ccActive
 248  00b2               L54:
 249                     ; 92 			if(prevccActive == 0) 
 251  00b2 3d11          	tnz	_prevccActive
 252  00b4 260b          	jrne	L15
 253                     ; 93 				GPIO_WriteLow(USB3_MUX_OE);
 255  00b6 4b80          	push	#128
 256  00b8 ae5005        	ldw	x,#20485
 257  00bb cd0000        	call	_GPIO_WriteLow
 259  00be 84            	pop	a
 261  00bf 203e          	jra	L33
 262  00c1               L15:
 263                     ; 95 				if(waitEnd && ((r26_MUXOECTR & 0x01)==0x01))
 265  00c1 3d13          	tnz	_waitEnd
 266  00c3 2713          	jreq	L55
 268  00c5 b600          	ld	a,_r26_MUXOECTR
 269  00c7 a401          	and	a,#1
 270  00c9 a101          	cp	a,#1
 271  00cb 260b          	jrne	L55
 272                     ; 96 					GPIO_WriteHigh(USB3_MUX_OE); //Signal inverted by Q2
 274  00cd 4b80          	push	#128
 275  00cf ae5005        	ldw	x,#20485
 276  00d2 cd0000        	call	_GPIO_WriteHigh
 278  00d5 84            	pop	a
 280  00d6 2009          	jra	L75
 281  00d8               L55:
 282                     ; 98 					GPIO_WriteLow(USB3_MUX_OE);
 284  00d8 4b80          	push	#128
 285  00da ae5005        	ldw	x,#20485
 286  00dd cd0000        	call	_GPIO_WriteLow
 288  00e0 84            	pop	a
 289  00e1               L75:
 290                     ; 100 				if(prevccActive == 1){					 
 292  00e1 b611          	ld	a,_prevccActive
 293  00e3 a101          	cp	a,#1
 294  00e5 2609          	jrne	L16
 295                     ; 101 					 GPIO_WriteLow(USB3_MUX_SEL);
 297  00e7 4b40          	push	#64
 298  00e9 ae5005        	ldw	x,#20485
 299  00ec cd0000        	call	_GPIO_WriteLow
 301  00ef 84            	pop	a
 302  00f0               L16:
 303                     ; 103 				if(prevccActive == 2){					 
 305  00f0 b611          	ld	a,_prevccActive
 306  00f2 a102          	cp	a,#2
 307  00f4 2609          	jrne	L33
 308                     ; 104 					 GPIO_WriteHigh(USB3_MUX_SEL);
 310  00f6 4b40          	push	#64
 311  00f8 ae5005        	ldw	x,#20485
 312  00fb cd0000        	call	_GPIO_WriteHigh
 314  00fe 84            	pop	a
 315  00ff               L33:
 316                     ; 109     if(currentTime - IOScan >= IOSCAN_INTERVAL)
 318  00ff ae0000        	ldw	x,#_currentTime
 319  0102 cd0000        	call	c_ltor
 321  0105 ae000c        	ldw	x,#_IOScan
 322  0108 cd0000        	call	c_lsub
 324  010b cd0000        	call	c_lrzmp
 326  010e 2603          	jrne	L61
 327  0110 cc0011        	jp	L12
 328  0113               L61:
 329                     ; 111 			IOScan = currentTime;		
 331  0113 be02          	ldw	x,_currentTime+2
 332  0115 bf0e          	ldw	_IOScan+2,x
 333  0117 be00          	ldw	x,_currentTime
 334  0119 bf0c          	ldw	_IOScan,x
 335                     ; 112       Update_GPIO_from_I2CRegisters();
 337  011b cd0000        	call	_Update_GPIO_from_I2CRegisters
 339  011e ac110011      	jpf	L12
 447                     	xdef	_main
 448                     	xdef	_waitEnd
 449                     	xdef	_muxDebounce
 450                     	xdef	_prevccActive
 451                     	xdef	_ccActive
 452                     	xdef	_IOScan
 453                     	xdef	_lastCCScan
 454                     	xdef	_lastBlink
 455                     	xdef	_currentTime
 456                     	xref.b	_comActive
 457                     	xref.b	_r26_MUXOECTR
 458                     	xref.b	_r23_CCSUM
 459                     	xref	_Update_CC_signals
 460                     	xref	_Update_GPIO_from_I2CRegisters
 461                     	xref	_BaseR_GPIO_Init
 462                     	xref	_millis
 463                     	xref	_TIM4_init
 464                     	xref	_Init_I2C
 465                     	xref	_GPIO_WriteReverse
 466                     	xref	_GPIO_WriteLow
 467                     	xref	_GPIO_WriteHigh
 468                     	xref	_CLK_DeInit
 487                     	xref	c_lrzmp
 488                     	xref	c_lsub
 489                     	xref	c_lcmp
 490                     	xref	c_ltor
 491                     	xref	c_rtol
 492                     	end

   1                     ; C Compiler for STM8 (COSMIC Software)
   2                     ; Parser V4.12.9 - 19 Apr 2023
   3                     ; Generator (Limited) V4.5.6 - 18 Jul 2023
  14                     	bsct
  15  0000               _current_millis:
  16  0000 00000000      	dc.l	0
  50                     ; 27 void TIM4_init(void)
  50                     ; 28 {
  52                     	switch	.text
  53  0000               _TIM4_init:
  57                     ; 31 	CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV);
  59  0000 c650c6        	ld	a,20678
  60  0003 a4e7          	and	a,#231
  61  0005 c750c6        	ld	20678,a
  62                     ; 32 	CLK->CKDIVR |= 0x00;
  64  0008 c650c6        	ld	a,20678
  65                     ; 44 	TIM4_TimeBaseInit(TIM4_PRESCALER_128, TIM4_PERIOD);
  67  000b ae077c        	ldw	x,#1916
  68  000e cd0000        	call	_TIM4_TimeBaseInit
  70                     ; 46 	TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  72  0011 a601          	ld	a,#1
  73  0013 cd0000        	call	_TIM4_ClearFlag
  75                     ; 48 	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  77  0016 ae0101        	ldw	x,#257
  78  0019 cd0000        	call	_TIM4_ITConfig
  80                     ; 51 	enableInterrupts();
  83  001c 9a            rim
  85                     ; 54 	TIM4_Cmd(ENABLE);
  88  001d a601          	ld	a,#1
  89  001f cd0000        	call	_TIM4_Cmd
  91                     ; 56 }
  94  0022 81            	ret
 117                     ; 59 void CLK_DeInit(void)
 117                     ; 60 {
 118                     	switch	.text
 119  0023               _CLK_DeInit:
 123                     ; 61   CLK->ICKR = CLK_ICKR_RESET_VALUE;
 125  0023 350150c0      	mov	20672,#1
 126                     ; 62   CLK->ECKR = CLK_ECKR_RESET_VALUE;
 128  0027 725f50c1      	clr	20673
 129                     ; 63   CLK->SWR  = CLK_SWR_RESET_VALUE;
 131  002b 35e150c4      	mov	20676,#225
 132                     ; 64   CLK->SWCR = CLK_SWCR_RESET_VALUE;
 134  002f 725f50c5      	clr	20677
 135                     ; 65   CLK->CKDIVR = CLK_CKDIVR_RESET_VALUE;
 137  0033 351850c6      	mov	20678,#24
 138                     ; 66   CLK->PCKENR1 = CLK_PCKENR1_RESET_VALUE;
 140  0037 35ff50c7      	mov	20679,#255
 141                     ; 67   CLK->PCKENR2 = CLK_PCKENR2_RESET_VALUE;
 143  003b 35ff50ca      	mov	20682,#255
 144                     ; 68   CLK->CSSR = CLK_CSSR_RESET_VALUE;
 146  003f 725f50c8      	clr	20680
 147                     ; 69   CLK->CCOR = CLK_CCOR_RESET_VALUE;
 149  0043 725f50c9      	clr	20681
 151  0047               L53:
 152                     ; 70   while ((CLK->CCOR & CLK_CCOR_CCOEN)!= 0)
 154  0047 c650c9        	ld	a,20681
 155  004a a501          	bcp	a,#1
 156  004c 26f9          	jrne	L53
 157                     ; 72   CLK->CCOR = CLK_CCOR_RESET_VALUE;
 159  004e 725f50c9      	clr	20681
 160                     ; 73   CLK->HSITRIMR = CLK_HSITRIMR_RESET_VALUE;
 162  0052 725f50cc      	clr	20684
 163                     ; 74   CLK->SWIMCCR = CLK_SWIMCCR_RESET_VALUE;
 165  0056 725f50cd      	clr	20685
 166                     ; 75 }
 169  005a 81            	ret
 193                     ; 78 uint32_t millis(void)
 193                     ; 79 {
 194                     	switch	.text
 195  005b               _millis:
 199                     ; 80 	return current_millis;
 201  005b ae0000        	ldw	x,#_current_millis
 202  005e cd0000        	call	c_ltor
 206  0061 81            	ret
 231                     ; 88 @far @interrupt void TIM4_check_event(void) {
 233                     	switch	.text
 234  0062               f_TIM4_check_event:
 236  0062 8a            	push	cc
 237  0063 84            	pop	a
 238  0064 a4bf          	and	a,#191
 239  0066 88            	push	a
 240  0067 86            	pop	cc
 241  0068 3b0002        	push	c_x+2
 242  006b be00          	ldw	x,c_x
 243  006d 89            	pushw	x
 244  006e 3b0002        	push	c_y+2
 245  0071 be00          	ldw	x,c_y
 246  0073 89            	pushw	x
 249                     ; 92 	current_millis ++;
 251  0074 ae0000        	ldw	x,#_current_millis
 252  0077 a601          	ld	a,#1
 253  0079 cd0000        	call	c_lgadc
 255                     ; 94 	TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
 257  007c a601          	ld	a,#1
 258  007e cd0000        	call	_TIM4_ClearITPendingBit
 260                     ; 95 }
 263  0081 85            	popw	x
 264  0082 bf00          	ldw	c_y,x
 265  0084 320002        	pop	c_y+2
 266  0087 85            	popw	x
 267  0088 bf00          	ldw	c_x,x
 268  008a 320002        	pop	c_x+2
 269  008d 80            	iret
 292                     	xdef	_current_millis
 293                     	xdef	_millis
 294                     	xdef	f_TIM4_check_event
 295                     	xdef	_TIM4_init
 296                     	xref	_TIM4_ClearITPendingBit
 297                     	xref	_TIM4_ClearFlag
 298                     	xref	_TIM4_ITConfig
 299                     	xref	_TIM4_Cmd
 300                     	xref	_TIM4_TimeBaseInit
 301                     	xdef	_CLK_DeInit
 302                     	xref.b	c_x
 303                     	xref.b	c_y
 322                     	xref	c_lgadc
 323                     	xref	c_ltor
 324                     	end

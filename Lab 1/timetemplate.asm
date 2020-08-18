# timetemplate.asm
  # Written 2015 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

.macro	PUSH (%reg)
	addi	$sp,$sp,-4
	sw	%reg,0($sp)
.end_macro

.macro	POP (%reg)
	lw	%reg,0($sp)
	addi	$sp,$sp,4
.end_macro

	.data
	.align 2
mytime:	.word 0x5957

timstr:	.ascii "text more text lots of text\0"
	.text

.data
.align 2

newstring: .ascii "NINE\0"
	   .text

main:
	# print timstr
	la	$a0,timstr 	# timstr = label. Argumentet blir timestr. Argumentet skrivs ut mha syscall $v0,4.
	li	$v0,4		# $v0 = 4, syscall: address of null-terminated string to print. Skriv ut sträng.
				# null-terminated string = character string stored as an array 
				# containing the carachters and terminated with a null character (markerar slutet).
	syscall			
	nop
	
	# wait a little
	li	$a0,2		# när $a0 = 0 så kommer ms = 0 --> delay = 0
	jal	delay		# hoppa till delay, utför subrutinen med arg 2 och forsätt programmet vidare härifrån
				# jal sparar returadressen i $ra, vi kommer ihåg var vi kom ifrån!
	nop
	
	# call tick
	la	$a0,mytime	# argumentet som lagras i $a0 värdet i mytime = 0x5957
	jal	tick		# gå till tick, utför subrutinen och exekvera programmet vidare härifrån
	nop
	
	# call your function time2string
	la	$a0,timstr	# argumentet i $a0 innehåller strängen i timestr som avslutas med null
	la	$t0,mytime	# adressen som finns i mytime lagras i ett tillfälligt register $t0
	lw	$a1,0($t0)	# vi tilldelar adressen i $t0 till argumentet $a1 utan att utföra någon ändring
	jal	time2string	# vi går till time2string och omvandlar då våra argument till strängar som skrivs ut
	nop
	
	# print a newline
	li	$a0,10		
	li	$v0,11
	syscall
	nop
	# go back and do it all again
	j	main
	nop
# tick: update time pointed to by $a0
tick:	lw	$t0,0($a0)	# get time, ladda adressen (utan tillägg) till $t0
	addiu	$t0,$t0,1	# increase, öka med 1
	andi	$t1,$t0,0xf	# check lowest digit, maskar och tar ut 4 sista bits från $t0 och lägger i $t1
	sltiu	$t2,$t1,0xa	# if digit < a, okay | om värdet i $t1 är mindre än 10 (ska ej uttrycka 10-15)
	bnez	$t2,tiend
	nop
	
	addiu	$t0,$t0,0x6	# adjust lowest digit
	andi	$t1,$t0,0xf0	# check next digit
	sltiu	$t2,$t1,0x60	# if digit < 6, okay
	bnez	$t2,tiend	# Om sltiu inte är lika med 0 så branchar till tiend
	nop
	
	addiu	$t0,$t0,0xa0	# adjust digit
	andi	$t1,$t0,0xf00	# check minute digit
	sltiu	$t2,$t1,0xa00	# if digit < a, okay
	bnez	$t2,tiend
	nop
	
	addiu	$t0,$t0,0x600	# adjust digit
	andi	$t1,$t0,0xf000	# check last digit
	sltiu	$t2,$t1,0x6000	# if digit < 6, okay --> minuter och sek kan inte bli mer än 59
	bnez	$t2,tiend
	nop
	
	addiu	$t0,$t0,0xa000	# adjust last digit

tiend:	sw	$t0,0($a0)	# save updated result, vi lägger adressen till $a0 (utan tillägg)
	jr	$ra		# return
	nop

  # you can write your code for subroutine "hexasc" below this line

hexasc:
  	andi	$v0,$a0,0xf		# Vi vill endast spara 4 LSB: f = 1111 AND med ett binärt tal (resten framför bli 0, signextension)
  	addi	$v0,$v0,0x30		# Vi omvandlar värdet i $v0 till ASCII-tabellvärde
 	ble	$v0,0x39,finish		# Villkor: om $v0 (ASCII-omvandlat) är mindre än 0x39, siffran 9 --> branch till finish
  	nop
  
  	addi	$v0,$v0,7		# Om $v0 är större än 0x39 (9) då vill vi addera 7 till 0x39 och kan då nå 0x46 (A-F)
  
finish:
  	jr	$ra			# Returnera det värde som finns i ASCII-tabellen
  	nop
  
delay: 
	PUSH	($ra)
	move	$t0,$a0			# ms = $a0
	addi	$t1,$0,0		# i = 0
	li	$t2,1			# gör det lättare att ändra tidfördröjningen

while:
	ble	$t0,$0,done		# när ms < 0 vill vi går direkt till done
	nop
	addi	$t0,$t0,-1

for:
	ble	$t2,$t1,exitfor		# om i är större än $t2 (4711)
	nop
	addi	$t1,$t1,1
	j	for			# forsätt tills i > $t2
	nop

exitfor:
	li	$t1,0			# reset i till 0 (för nästa klockvarv)
	j 	while
	nop

done:
	POP ($ra)
	jr	$ra
	nop
	
time2string:

PUSH	($ra) 				# sparar argumenten som skickats
	andi 	$a1, $a1, 0xffff	# behåller 16 LSB i mytime
	
	srl 	$t1, $a1,12		# Shiftar 12 steg till höger för att få de 4 MSB i NBCD
	
	PUSH($t1)
	move 	$a0, $t1		# vi lagrar de 4 MSB från mytime till $a0
	jal 	hexasc			# går till hexasc
	nop
	
	POP($t1)
	sb	$t1,0($a0)		# lagra bytes i adressen $a0
	
	srl 	$t2, $a1,8		# Ta ut nästa siffra (shifta 8 steg)
	
	PUSH($t2)
	move 	$a0, $t2		# flytta talen till $a0
	jal 	hexasc			# gå till hexasc
	nop
	
	POP($t2)
	sb	$v0,1($a0)		# lagra byte i adress $a0
	
	addi 	$t4, $t4, 0x3A		# Vi lägger till koden för att få ut : efter det vi har i $a0, dvs 2 första
	sb	$t4,2($a0)		# lagra byte i adressen till $a0, vill ha mer plats
			
	srl 	$t3, $a1,4 		# hämta nästsista talet, dvs på pos 4-8 och shifta 4 steg
	
	PUSH($t3)
	move 	$a0, $t3		# lagra i $a0
	jal 	hexasc			# gå till hexasc
	nop	
	
	POP($t3)
	sb	$v0,3($a0)		# lagra adressen i $a0
	
	
	PUSH($t4)
	move 	$a0, $a1		# move the 4 digits to a2
	jal 	hexasc			# jump to hexasc
	nop
	
	POP($t4)
	sb	$v0,4($a0)		# lagra bytes i adress $a0 (4 byte)
	
	li 	$t4, 0x00		# avsluta strängen med null
	sb	$t4,5($a0)		# lagra den slutliga strängen i adressen till a0
	
	POP($ra)
	jr	$ra
	nop
	
	jal 	hexasc			# hexasc
	nop
	
#nine:
	#la	$a0,newstring
	#li	$v0,4
	#syscall
	#nop
	
	#jr $ra

#break:
	

  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #

.globl hexasc
.globl delay
.globl time2string
.data
.text

.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #

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
	PUSH $ra

	move	$t0,$a0		# ms = a0
	addi 	$t1,$0,0 	# int i = 0
	li 	$t5,1000		# lättare ändra delay värdet.


while:
	ble 	$t0, $0, done 		# while(ms > 0) om ms < 0 då gå till done
	nop
	addi  	$t0,$t0,-1		# ms = ms - 1, (--)

for:	

	ble 	$t5, $t1,backtowhile 	# stoppa for-loopen. Obs! Gammalt värde istället för $t5 var $a0 tidigare
	nop
	addi 	$t1, $t1, 1		# i + 1 --> nästa (i++). Öka i för varje varv i forloopen

	j for				# loopa om
	nop

backtowhile:
	li	$t1, 0			# reset i för nästa klockvarv
	j	while			# jump to while till i är större än värdet i for
	nop

done:
	POP $ra
 	jr 	$ra

 	nop

time2string:
	move	$t6,$ra
	PUSH $ra			# sparar returvärdet

	andi 	$a1, $a1, 0xffff	# maskar: behåller 16 LSB
	srl 	$t1, $a1, 12		# sDe första 4 bitsen (4 MSB) genom att skifta till höger

	PUSH $a0

	move 	$a0,$t1		# flytta 4 MSB till $a0
	jal 	hexasc			# jump till hexasc, omvandla till ASCII
	nop
	POP $a0
	sb	$v0,0($a0)		# lagra denna byte i adressen till $a0

	srl 	$t2,$a1, 8		# nästa tal i mytime genom att skifta 8
	PUSH $a0

	move 	$a0,$t2		# flytta de 4 talen till $a0
	jal 	hexasc			# jump to hexasc
	nop
	POP $a0
	sb	$v0,1($a0)		# store byte in address of a0

	addi 	$t4, $t4, 0x3A		# adds ascii-code for ':'
	sb	$t4,2($a0)		# store byte in address of a0

	srl 	$t3,$a1,4 		# nästa 4 talen

	PUSH $a0

	move 	$a0, $t3

	jal 	hexasc			# jump to hexasc
	nop
	POP $a0

	sb	$v0,3($a0)		# store byte in address of a0

	PUSH $a0

	move 	$a0, $a1		# flytta de fyra sista talen till $a0
	jal 	hexasc			# hoppa hexasc
	nop
	POP $a0


	sb	$v0,4($a0)		# sista siffran lagras i address of a0
	li 	$t4, 0x00		# load null byte till t4, sträng
	sb	$t4,5($a0)		# store byte in address of a0

	POP  $ra

	jr	$t6

	nop

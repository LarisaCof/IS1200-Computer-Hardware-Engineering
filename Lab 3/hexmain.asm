  # hexmain.asm
  # Written 2015-09-04 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

	.text
main:
	li	$a0,17		# change this to test different values

	jal	hexasc		# call hexasc
	nop			# delay slot filler (just in case)	

	move	$a0,$v0		# copy return value to argument register

	li	$v0,11		# syscall with v0 = 11 will print out
	syscall			# one byte from a0 to the Run I/O window
	
stop:	j	stop		# stop after one run
	nop			# delay slot filler (just in case)

  # You can write your own code for hexasc here
  
  hexasc:
  andi	$v0,$a0,0xf		# Vi vill endast spara 4 LSB: f = 1111 AND med ett binärt tal (resten framför bli 0, signextension)
  addi	$v0,$v0,0x30		# Vi omvandlar värdet i $v0 till ASCII-tabellvärde
  				# plussar för att få rätt siffra i ASCII
  ble	$v0,0x39,finish		# Villkor: om $v0 (ASCII-omvandlat) är mindre än 0x39, siffran 9 --> branch till finish
  nop
  addi	$v0,$v0,7		# Om $v0 är större än 0x39 (9) då vill vi addera 7 till 0x39 och kan då nå 0x46 (A-F)
  
  finish:
  jr	$ra			# Returnera det värde som finns i ASCII-tabellen
  	nop
  	
  				
  				
  	
  	
  	
  			


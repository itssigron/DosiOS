; This file includes the Disk Address Packet, we will use this to write the kernel into 

; reference: https://wiki.osdev.org/Disk_access_using_the_BIOS_(INT_13h)
; Offset	Size	Description
; 0			1		size of packet (16 bytes)
; 1			1		always 0
; 2			2		number of sectors to transfer (max 127 on some BIOSes)
; 4			4		transfer buffer (16 bit segment:16 bit offset) (see note #1)
; 8			4		lower 32-bits of 48-bit starting LBA
; 12 		4		upper 16-bits of 48-bit starting LBA


DAP_START:

	; Size of packet
	.DAP_SIZE:

		db 10h

	; Reserved
	.DAP_NULL:

		db 0

	; Write 50 sectors
	.DAP_SECTORS:

		dw 50

	; Initially we write to a 16 bit offset, in this case 0x1000
	.DAP_KERNEL_OFFSET:

		dw 1000h
		dw 0

	; Load from disk, 121h's sector (after FAT and boot)
	.DAP_LBA:

		dd 121h
		dd 0

DAP_END:

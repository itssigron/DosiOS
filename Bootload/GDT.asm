; This file includes the Global Descriptor Table. Limits data to 4GB

gdt_start:

	gdt_null:
		dd 0
		dd 0

   	gdt_code:
   		dw 0FFFFh ; 0-15	limit 		 	0-15
   		dw 0 ; 		16-31	Base address 	0-15
		db 0 ; 		0-7 	Base address 	16-23
		; 			8-12  	Type
		; 			13-14 	Privilege Level
		; 			15    	Present flag
		db 10011010b ; -> Readable code segment, nonconforming, code or data segment, most privileged 

		; 			16-19 	Limit 			16-19 
		;			20-22	Attributes
		;			23		Granularity
		db 11001111b ; -> highest limit, 32bit, Granularity (multiply limit by 4kb)
		db 0 ;		24-31 	Base address	24-31

	gdt_data:
   		dw 0FFFFh ; 0-15	limit 		 	0-15
   		dw 0 ; 		16-31	Base address 	0-15
		db 0 ; 		0-7 	Base address 	16-23
		; 			8-12  	Type
		; 			13-14 	Privilege Level
		; 			15    	Present flag
		db 10010010b ; Writeable code segment, expands down
		; 			16-19 	Limit 			16-19 
		;			20-22	Attributes
		;			23		Granularity
		db 11001111b
		db 0 ; base address 24-31

	gdt_end:


; The GDT descriptor
gdt_desc:
   dw gdt_end - gdt_start - 1
   dd gdt_start

; Offsets to quickly access the code and data segments through the segment registers
code_seg equ gdt_code - gdt_start
data_seg equ gdt_data - gdt_start

.global fib_rec_s


# fibrec - compute the nth fibonacci number

# a0 - int n

fib_rec_s:
	li a1, 2
	blt a0, a1, end
	
    addi sp, sp, -32
    sd ra, (sp)
	sd a0, 8(sp)
	sd a2, 16(sp)

	addi a0, a0, -1
	call fib_rec_s
	add a2, a0, zero

	ld a0, 8(sp)
	addi a0, a0, -2
	call fib_rec_s
	add a0, a0, a2
	
	ld a2, 16(sp)
    ld ra, (sp)
    addi sp, sp, 32
end:
    ret


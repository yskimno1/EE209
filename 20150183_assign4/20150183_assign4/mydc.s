### --------------------------------------------------------------------
### mydc.s
### name : Kim Yunseong
### ID : 20150183
### Assignment 4
### Desk Calculator (dc)
### --------------------------------------------------------------------

	.equ   ARRAYSIZE, 20
	.equ   EOF, -1
	
.section ".rodata"

emptymessage:
	.asciz "dc: stack is empty.\n"

divzeromessage:
	.asciz "dc: divide by zero.\n"
	
scanfFormat:
	.asciz "%s"

printfFormat:
	.asciz "%d\n"
### --------------------------------------------------------------------

        .section ".data"
iPower:
	.long	1
fcount:
	.long	0
### --------------------------------------------------------------------

        .section ".bss"
buffer:
        .skip  ARRAYSIZE
iBase:
	.skip	4
iExp:
	.skip	4
iIndex:
	.skip	4

### --------------------------------------------------------------------

	.section ".text"

	## -------------------------------------------------------------
	## int main(void)
	## Runs desk calculator program.  Returns 0.
	## -------------------------------------------------------------

	.globl  main
	.type   main,@function

main:
	pushl   %ebp
	movl    %esp, %ebp
		
input:
	## dc number stack initialized. %esp = %ebp
	## scanf("%s", buffer)
	pushl	$buffer
	pushl	$scanfFormat
	call    scanf
	addl    $8, %esp
	## check if user input EOF
	cmp	$EOF, %eax
	je	quit

mainloop:

	movl	buffer, %eax
	cmp	$48, %al
	jl	mainfuncloop
	
	movl	buffer, %eax
	cmp	$57, %al
	jg	mainfuncloop

	pushl	$buffer
	call 	atoi
	addl	$4, %esp
	pushl	%eax
	jmp 	input
	
mainfuncloop:
	movl	buffer, %eax
	cmp	$112, %al
	je	ploop

	movl	buffer, %eax
	cmp	$99, %al
	je	cloop

	movl	buffer, %eax
	cmp	$100, %al
	je	dloop

	movl	buffer, %eax
	movl	$0, %ecx
	cmp	$102, %al
	je	floop

	movl	buffer, %eax
	cmp	$114, %al
	je	rloop

	movl	buffer, %eax
	cmp	$113, %al
	je	quit

	movl	buffer, %eax
	cmp	$43, %al
	je	sumfirstloop

	movl	buffer, %eax
	cmp	$45, %al
	je	subfirstloop

	movl	buffer, %eax
	cmp	$42, %al
	je	mulfirstloop

	movl	buffer, %eax
	cmp	$95, %al
	je	signloop

	movl	buffer, %eax
	cmp	$47, %al
	je	divfirstloop

	movl	buffer, %eax
	cmp	$37, %al
	je	remainfirstloop
	
	movl	buffer, %eax
	cmp	$94, %al
	je	powerfirstloop
	
signloop:

	sarl	$8, buffer
	pushl	$buffer
	call 	atoi
	addl	$4, %esp
	movl	$0, %edx
	subl	%eax, %edx
	pushl	%edx
	jmp 	input
	
ploop:	
	cmp	%esp, %ebp
	je	empty

	popl	%edx
	pushl	%edx
	pushl	%edx
	pushl	$printfFormat
	
	call	printf
	addl	$8, %esp
	jmp	input

cloop:
	cmp	%esp, %ebp
	je	cendloop
	popl	%eax
	jmp	cloop
cendloop:
	jmp	input
	
dloop:
	cmp	%esp, %ebp
	je	empty
	popl	%eax
	pushl	%eax
	pushl	%eax
	jmp	input

floop:	
	movl	%esp, %ebx
flooptemp:
	cmp	%ebx, %ebp
	je	fendloop
	pushl	(%ebx)
	pushl	$printfFormat
	call	printf
	addl	$8, %esp
	addl	$4, %ebx
	jmp	flooptemp
fendloop:
	jmp	input
	
rloop:
	cmp	%esp, %ebp
	je	empty
	popl	%eax
	cmp	%esp, %ebp
	jne	rsecondloop
	pushl	%eax
	jmp	empty
	
rsecondloop:
	popl	%edx
	pushl	%eax
	pushl	%edx
	jmp	input
	
sumfirstloop:
	cmp	%esp, %ebp
	je	empty
	popl	%eax
	
	cmp	%esp, %ebp
	jne	sumsecondloop
	pushl	%eax
	jmp	empty

sumsecondloop:
	popl	%edx
	call 	sum
	pushl	%eax
	jmp	input

	
subfirstloop:
	cmp	%esp, %ebp
	je	empty
	popl	%edx
	
	cmp	%esp, %ebp
	jne	subsecondloop
	pushl	%edx
	jmp	empty

subsecondloop:
	popl	%eax
	
	call 	sub
	pushl	%eax
	jmp	input

mulfirstloop:
	cmp	%esp, %ebp
	je	empty
	popl	%eax
	
	cmp	%esp, %ebp
	jne	mulsecondloop
	pushl	%eax
	jmp	empty
mulsecondloop:
	popl	%edx
	
	call 	mul
	pushl	%eax
	jmp	input

divfirstloop:
	cmp	%esp, %ebp
	je	empty
	popl	%ebx

	cmp	$0, %ebx
	je	divbyzero
	
	cmp	%esp, %ebp
	jne	divsecondloop
	pushl	%ebx
	jmp	empty

divsecondloop:
	popl	%eax
	call 	div
	pushl	%eax
	jmp	input
	
remainfirstloop:
	cmp	%esp, %ebp
	je	empty
	popl	%ebx

	cmp	$0, %ebx
	je	divbyzero
	
	cmp	%esp, %ebp
	jne	remainsecondloop
	pushl	%ebx
	jmp	empty
remainsecondloop:	
	popl	%eax
	call 	div
	pushl	%edx
	jmp	input
	
	
powerfirstloop:
	cmp	%esp, %ebp
	je	empty
	popl	iExp
	
	cmp	%esp, %ebp
	jne	powerpoploop
	pushl	iExp
	jmp	empty

powerpoploop:
	popl	iBase
	movl	$1, iPower
	movl	$1, iIndex
	jmp	powersecondloop
	
powersecondloop:
	movl	iIndex, %eax
	cmpl	iExp, %eax
	jg	powerend

	movl	iPower, %eax
	imull	iBase
	movl	%eax, iPower
	incl	iIndex
	jmp	powersecondloop

powerend:	
	pushl	iPower
	jmp	input
sum:
	addl	%edx, %eax
	ret
sub:
	subl	%edx, %eax
	ret
mul:
	imul	%edx
	ret
div:
	cltd
	idiv	%ebx
	ret

	
	## PSEUDO-CODE
	## /*
	##  * In this pseudo-code we are assuming that no local variables are created
	##  * in the _main_ process stack. In case you want to allocate space for local
	##  * variables, please remember to update logic for 'empty dc stack' condition
	##  * (lines 6, 15 and 20) accordingly.
	##  */
	##
	##1 while (1) {
	##2	if (scanf("%s", buffer) == EOF)
	##3		return 0;
	##4 	if (!isdigit(buffer[0])) {
	##5		if (buffer[0] == 'p') {
	##6			if (stack.peek() == NULL) { /* is %esp == %ebp? */
	##7				printf("dc: stack empty\n");
	##8			} else {
	##9				printf("%d\n", (int)stack.top()); /* value is already pushed in the stack */
	##10			}
	##11		} else if (buffer[0] == 'q') {
	##12			goto quit;
	##13		} else if (buffer[0] == '+') {
	##14			int a, b;
	##15			if (stack.peek() == NULL) {
	##16				printf("dc: stack empty\n");
	##17				continue;
	##18			}
	##19			a = (int)stack.pop();
	##20			if (stack.peek() == NULL) {
	##21				printf("dc: stack empty\n");
	##22				stack.push(a); /* pushl some register value */
	##23				continue;
	##24			}
	##25			b = (int)stack.pop(); /* popl to some register */
	##26			res = a + b;
	##27 			stack.push(res);
	##28		} else if (buffer[0] == '-') {
	##29			/* ... */
	##30		} else if (buffer[0] == '^') {
	##31			/* ... powerfunc() ... */
	##32		} else if { /* ... and so on ... */
	##33 	} else { /* the first no. is a digit */
	##34		int no = atoi(buffer);
	##35		stack.push(no);	/* pushl some register value */
	##36	}
	##37 }
divbyzero:
	pushl	%eax
	pushl	$divzeromessage
	call	printf
	add	$4, %esp
	jmp	input
	
empty:
	pushl	$emptymessage
	call	printf
	addl	$4, %esp
	jmp	input
	
quit:	
	## return 0
	movl    $0, %eax
	movl    %ebp, %esp
	popl    %ebp
	ret

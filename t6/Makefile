all:
	gcc -o a ordenacao.c -Wall -pedantic -fsplit-stack
	clear
	
	n=4;for letter in a d c ; do \
		for number in 500000 5000000 50000000 ; do \
			echo $$letter: $$number: ; \
			num2=1 ; while [ $$num2 -le 3 ] ; do \
				./a $$n $$number.$$letter out ; \
				diff out $$number.$$letter.out ; \
				num2=`expr $$num2 + 1`; \
			done ; \
    		done ; \
    	done

clean:
	rm t2

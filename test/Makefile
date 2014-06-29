LIBNAME=rf24-sunxi
LIBDIR=../lib/

all:spidev_test pingtest scanner arduino_rcv arduino_rcv_irq

pingtest:
	g++ pingtest.cpp -I../src/ -L${LIBDIR} -l${LIBNAME} -o pingtest

scanner:
	g++ scanner.cpp -I../src/ -L${LIBDIR} -l${LIBNAME} -o scanner

spidev_test:
	g++ spidev_test.c -o spidev_test

arduino_rcv:
	g++ arduino_rcv.cpp -I../src/ -L${LIBDIR} -l${LIBNAME} -o arduino_rcv

arduino_rcv_irq:
	g++ arduino_rcv_irq.cpp -I../src/ -L${LIBDIR} -l${LIBNAME} -o arduino_rcv_irq -Wno-write-strings

clean:
	rm -f scanner pingtest arduino_rcv arduino_rcv_irq spidev_test

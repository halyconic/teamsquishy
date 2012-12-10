################################################################################
# Makefile for CS640
################################################################################

MAKE := make

all: emulator trace
	$(MAKE) -C emulator/Default all
	$(MAKE) -C trace/Default all
	
emulator:
	$(MAKE) -C emulator/Default all
	
trace:
	$(MAKE) -C trace/Default all
	
clean:
	@echo 'Cleaning all'
	$(MAKE) -C emulator/Default clean
	$(MAKE) -C trace/Default clean
	@echo 'Removing logs'
	-cd test/emulator && rm emulator.txt
	-cd test/trace && rm trace.txt
	
e1:
	-cd test/emulator && rm emulator1.txt
	@echo 'Run emulator 1'
	cd test/emulator && ../../emulator/Default/emulator -p 5001 -f topology.txt -d >> emulator1.txt &

e2:
	-cd test/emulator && rm emulator2.txt
	@echo 'Run emulator 2'
	cd test/emulator && ../../emulator/Default/emulator -p 5002 -f topology.txt -d >> emulator2.txt &

e3:
	-cd test/emulator && rm emulator3.txt
	@echo 'Run emulator 3'
	cd test/emulator && ../../emulator/Default/emulator -p 5003 -f topology.txt -d >> emulator3.txt &

e4:
	-cd test/emulator && rm emulator4.txt
	@echo 'Run emulator 4'
	cd test/emulator && ../../emulator/Default/emulator -p 5004 -f topology.txt -d >> emulator4.txt &

e5:
	-cd test/emulator && rm emulator5.txt
	@echo 'Run emulator 5'
	cd test/emulator && ../../emulator/Default/emulator -p 5005 -f topology.txt -d >> emulator5.txt &

t_1-4:
	-cd test/trace && rm trace.txt
	@echo 'Run routetrace 1-4'
	cd test/trace && ../../trace/Default/trace -a 5000 -b $(host) -c 5001 -d $(host) -e 5004 -f 1 >> trace.txt &

analyze:
	gedit test/emulator/emulator1.txt &
	gedit test/emulator/emulator2.txt &
	gedit test/emulator/emulator3.txt &
	gedit test/emulator/emulator4.txt &
	gedit test/emulator/emulator5.txt &
	gedit test/trace/trace.txt &

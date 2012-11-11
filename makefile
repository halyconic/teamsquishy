################################################################################
# Makefile for CS640
################################################################################
MAKE := make

all: requester sender
	@echo 'Making all'
	$(MAKE) -C requester/Default all
	$(MAKE) -C sender/Default all

requester:
	$(MAKE) -C requester/Default all

sender:
	$(MAKE) -C sender/Default all

clean:
	@echo 'Cleaning all'
	$(MAKE) -C requester/Default clean
	$(MAKE) -C sender/Default clean
	@echo 'Removing logs'
	-cd test/sender && rm sender.txt
	-cd test/sender1 && rm sender1.txt
	-cd test/sender2 && rm sender2.txt
	-cd test/requester && rm requester.txt
	-cd test/requester && rm hello.txt
	-cd test/requester && rm split.txt

p1_1: all
	-cd test/sender && rm sender.txt
	-cd test/requester && rm requester.txt
	@echo 'Run sender'
	cd test/sender && ../../sender/Default/sender -p 5051 -g 5050 -r 1 -q 0 -l 8 -d debug >> sender.txt &
	@echo 'Run requester'
	sleep 1
	cd test/requester && ../../requester/Default/requester -p 5050 -o hello.txt -d debug >> requester.txt &
	@echo 'Done'

p1_2: all
	-cd test/sender1 && rm sender1.txt
	-cd test/sender2 && rm sender2.txt
	-cd test/requester && rm requester.txt
	@echo 'Run sender1'
	cd test/sender1 && ../../sender/Default/sender -p 5052 -g 5050 -r 1 -q 0 -l 4 -d debug >> sender1.txt &
	@echo 'Run sender2'
	cd test/sender2 && ../../sender/Default/sender -p 5053 -g 5050 -r 1 -q 1000 -l 3 -d debug >> sender2.txt &
	sleep 1
	@echo 'Run requester'
	cd test/requester && ../../requester/Default/requester -p 5050 -o split.txt -d debug >> requester.txt &
	@echo 'Done'
	
p2_1: all
	-cd test/sender && rm sender.txt
	-cd test/requester && rm requester.txt
	@echo 'Run sender'
	cd test/sender && ../../sender/Default/sender -p 5051 -g 5050 -r 1 -q 0 -l 8 -d debug >> sender.txt &
	@echo 'Run requester'
	sleep 1
	cd test/requester && ../../requester/Default/requester -p 5050 -o hello.txt -d debug >> requester.txt &
	@echo 'Done'
	
analyze:
	@echo 'Open relevant logs'
	gedit test/sender/sender.txt &
	gedit test/sender1/sender1.txt &
	gedit test/sender2/sender2.txt &
	gedit test/requester/requester.txt &
	gedit test/requester/hello.txt &
	gedit test/requester/split.txt &
	@echo 'Done'

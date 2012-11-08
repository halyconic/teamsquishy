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
	cd test/sender && rm sender.txt
	cd test/requester && rm requester.txt
	cd test/requester && rm hello.txt
	cd test/requester && rm split.txt

run: all
	@echo 'Run sender'
	cd test/sender && ../../sender/Default/sender -p 5051 -g 5050 -r 1 -q 0 -l 8 -d debug >> sender.txt &
	@echo 'Run requester'
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

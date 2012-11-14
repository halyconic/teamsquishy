################################################################################
# Makefile for CS640
################################################################################
MAKE := make

all: requester sender
	@echo 'Making all'
	$(MAKE) -C requester/Default all
	$(MAKE) -C sender/Default all
	$(MAKE) -C emulator/Default all

requester:
	$(MAKE) -C requester/Default all

sender:
	$(MAKE) -C sender/Default all

clean:
	@echo 'Cleaning all'
	$(MAKE) -C requester/Default clean
	$(MAKE) -C sender/Default clean
	$(MAKE) -C emulator/Default clean
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
	cd test/sender && ../../sender/Default/sender -p 5051 -g 5050 -r 1 -q 0 -l 8 -f mumble-30 -h 5050 -i 2 -t 0 -d debug >> sender.txt &
	sleep 1
	@echo 'Run requester'
	cd test/requester && ../../requester/Default/requester -p 5050 -o hello.txt -f mumble-30 -h 5051 -w 0 -d debug >> requester.txt &
	@echo 'Done'

p1_2: all
	-cd test/sender1 && rm sender1.txt
	-cd test/sender2 && rm sender2.txt
	-cd test/requester && rm requester.txt
	@echo 'Run sender1'
	cd test/sender1 && ../../sender/Default/sender -p 5052 -g 5050 -r 1 -q 0 -l 4 -f 0 -h 0 -i 2 -t 0 -d debug >> sender1.txt &
	@echo 'Run sender2'
	cd test/sender2 && ../../sender/Default/sender -p 5053 -g 5050 -r 1 -q 1000 -l 3 -f 0 -h 0 -i 3 -t 0 -d debug >> sender2.txt &
	sleep 1
	@echo 'Run requester'
	cd test/requester && ../../requester/Default/requester -p 5050 -o split.txt -f 0 -h 0 -w 0 -d debug >> requester.txt &
	@echo 'Done'
	
p2_t1: all
	-cd test/emulator && rm emulator.txt
	@echo 'Run emulator'
	cd test/emulator && ../../emulator/Default/emulator -p 5000 -q 3 -f table.txt -l out.txt -d debug >> emulator.txt &
	@echo 'Done'
	
p2_t1_a:
	@echo 'Open relevant logs'
	gedit test/emulator/table.txt &
	gedit test/emulator/emulator.txt &
	
p2_t2: all
	-cd test/emulator && rm emulator.txt
	-cd test/sender && rm sender.txt
	-cd test/requester && rm requester.txt
	@echo 'Run emulator'
	cd test/emulator && ../../emulator/Default/emulator -p 5000 -q 3 -f table.txt -l out.txt -d debug >> emulator.txt &
	@echo 'Run sender'
	cd test/sender && ../../sender/Default/sender -p 5051 -g 5050 -r 1 -q 0 -l 8 -f mumble-30 -h 5000 -i 2 -t 0 -d debug >> sender.txt &
	sleep 1
	@echo 'Run requester'
	cd test/requester && ../../requester/Default/requester -p 5050 -o hello.txt -f mumble-30 -h 5000 -w 0 -d debug >> requester.txt &
	@echo 'Done'
	
p2_t2_a:
	@echo 'Open relevant logs'
	gedit test/requester/tracker.txt &
	gedit test/emulator/table.txt &
	gedit test/emulator/emulator.txt &
	gedit test/sender/sender.txt &
	gedit test/requester/requester.txt &
	gedit test/requester/hello.txt &
	
p2_te: all
	-cd test/emulator && rm emulator.txt
	@echo 'Run emulator'
	cd test/emulator && ../../emulator/Default/emulator -p 5000 -q 3 -f table.txt -l out.txt -d debug >> emulator.txt
	@echo 'Done'

p2_tr: all
	-cd test/requester && rm requester.txt
	@echo 'Run requester'
	cd test/requester && ../../requester/Default/requester -p 5050 -o hello.txt -f mumble-26 -h 5000 -w 0 -d debug >> requester.txt
	@echo 'Done'
	
p2_ts: all
	-cd test/sender && rm sender.txt
	@echo 'Run sender'
	cd test/sender && ../../sender/Default/sender -p 5000 -g 5050 -r 1 -q 0 -l 8 -f mumble-26 -h 5050 -i 2 -t 0 -d debug >> sender.txt
	@echo 'Done'
	
p2_t3: all
	-cd test/sender && rm sender.txt
	-cd test/requester && rm requester.txt
	@echo 'Run sender'
	cd test/sender && ../../sender/Default/sender -p 5000 -g 4000 -r 1 -q 0 -l 8 -f mumble-26 -h 5050 -i 2 -t 0 -d debug >> sender.txt &
	sleep 1
	@echo 'Run requester'
	cd test/requester && ../../requester/Default/requester -p 5050 -o hello.txt -f mumble-26 -h 5000 -w 0 -d debug >> requester.txt &
	@echo 'Done'
	
analyze:
	@echo 'Open relevant logs'
	gedit test/requester/tracker.txt &
	gedit test/sender/sender.txt &
	gedit test/sender1/sender1.txt &
	gedit test/sender2/sender2.txt &
	gedit test/requester/requester.txt &
	gedit test/requester/hello.txt &
	gedit test/requester/split.txt &
	gedit test/emulator/emulator.txt &
	@echo 'Done'

teamsquishy
===========

Now uncorrupted and usable (hopefully)

Sucessful:
	p2_1
	p2_2
	
Failed:
	p2_3: Packets not resent after loss, segfault
	p2_4: Priority queue was full, segfault (expected: Queue full event)
	p2_5: No forwarding entry, segfault (expected: No forwarding entry)
	p2_6: No forwarding entry (expected: log losses, works)

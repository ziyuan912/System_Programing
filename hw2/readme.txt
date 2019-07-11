Execution:
	gcc player.c -o player
	gcc host.c -o host
	gcc bidding_system.c -o bidding_system
	gcc player_bonus.c -o player_bonus
	./bidding_system [host_number] [player_number]

Description:
	The algorithm I used in player_bonus.c is quit simple. First
	I will check whether the money of my player is the most, if 
	it is the most, I money I bid will be the second rich player's
	money + 1, so that I will definately win this round. If the 
	money I have is not the most, the money I bid  will be the
	expected value + 1.

Self-Examination:
	I have finished all of the grading criteria using functions
	such as fifo, fork, and select.
CC = gcc
VIEWER = less

.PHONY: clean run view

run :  
	- curl "http://nhlwc.cdnak.neulion.com/fs1/nhl/league/playerstatsline/20132014/2/$(T1)/iphone/playerstatsline.json" > T1.json
	- curl "http://nhlwc.cdnak.neulion.com/fs1/nhl/league/playerstatsline/20132014/2/$(T2)/iphone/playerstatsline.json" > T2.json
	- $(CC) program.c  cJSON.c -lm
	-./a.out $(T1) $(T2)
	
view : program.c
	- $(VIEWER) program4.c


clean :
	- rm -f *.json
	- rm -f a.out

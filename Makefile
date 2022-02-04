flags= -Wall -lm -lSDL2 -lSDL2_gfx  -lSDL2_ttf -g -o3
cc= gcc

target=chart

main: chart.c
	$(cc) $(flags) chart.c -o $(target)

install: main
	cp $(target) /usr/local/bin
	mkdir /usr/local/lib/$(target)
	cp -r unispace /usr/local/lib/$(target)

uninstall:
	rm /usr/local/bin/$(target)
	rm -r /usr/local/lib/$(target)

run: main
	./$(target)


signal: signal.c
	$(cc) $(flags) signal.c -o signal
	



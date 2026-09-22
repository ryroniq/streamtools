all: clock current-bgm
clean:
	rm -f clock current-bgm
.PHONY: all clean

clock: clock.c
	${CC} -o $@ $<
current-bgm: current-bgm.c
	${CC} -o $@ $<

decode-png: lodepng.o decode-png.o
	gcc -static -o $@ $^ -lcrypto

lodepng.o: lodepng.c lodepng.h
	gcc -c $<

decode-png.o: decode-png.c lodepng.h
	gcc -c $<

.PHONY: clean
clean:
	rm -f *.o decode-png

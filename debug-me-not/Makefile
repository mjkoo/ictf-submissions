crackme: main.o decrypt_enc.o
	musl-gcc -static -o crackme $^
	#strip $@

decrypt_enc.o: decrypt.c
	python3 create_encoded.py

main.o: main.c
	musl-gcc -O3 -c $^

.PHONY: clean
clean:
	rm -rf *.o crackme decrypt.bin decrypt.enc

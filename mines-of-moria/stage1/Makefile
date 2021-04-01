moria.tar: loader.o embed.o file.tar fixup_tar
	ld -Tld.script -o $@ loader.o embed.o
	dd if=file.tar of=$@ skip=120 seek=120 bs=1 conv=notrunc
	sed -e '0,/\x7fELF/ s/\x7fELF\x02\x01/\x7fELF\x2f\x00/' -i $@
	./fixup_tar $@

embed.o: embed.S decode-png_enc file.tar
	gcc -c $<

loader.o: loader.c
	gcc -fno-stack-protector -c $<
	objcopy --remove-section .note.GNU-stack $@

decode-png_enc: decode-png/decode-png
	python3 encode.py $< $@

fixup_tar: fixup_tar.c
	gcc -o $@ $<

file.tar: hint.txt
	# Wrap everything in bash as make hates this
	bash -c "mkdir -p $$'\x7fELF'"
	bash -c "cp hint.txt $$'\x7fELF'"
	bash -c "cp ../stage2/secrets.png $$'\x7fELF'"
	bash -c "tar -cvf file.tar $$'\x7fELF'"
	bash -c "rm -rf $$'\x7fELF'"

.PHONY: clean
clean:
	bash -c "rm -rf $$'\x7fELF'"
	rm -f *.o file.tar fixup_tar hello hello_enc moria.tar

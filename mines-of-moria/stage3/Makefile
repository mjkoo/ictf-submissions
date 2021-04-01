d00rz_0f_dur1n.pdf: moria.pdf flag.zip
	python3 3f.py $^ $@

moria.pdf: moria.jpg
	convert $< $@

flag.zip: flag.jpg thanks.txt comment.txt
	zip -P Mellon -z $@ flag.jpg thanks.txt < comment.txt

.PHONY: clean
clean:
	rm -f flag.zip d00rz_0f_dur1n.pdf moria.pdf

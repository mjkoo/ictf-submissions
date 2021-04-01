secrets.png: secrets.jpg d00rz_0f_dur1n_enc.pdf
	convert $< -set comment "They are wrought of ithildin that mirrors only starlight and m00nl1ght" secrets_temp.png
	python3 embed.py secrets_temp.png d00rz_0f_dur1n_enc.pdf $@
	rm -f secrets_temp.png

d00rz_0f_dur1n_enc.pdf: ../stage3/d00rz_0f_dur1n.pdf
	openssl enc -aes-256-cbc -in $< -out $@ -md sha256 -pbkdf2 -pass "pass:m00nl1ght"

.PHONY: clean
clean:
	rm -f secrets.png d00rz_0f_dur1n_enc.pdf

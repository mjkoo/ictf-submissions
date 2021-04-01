# mines of moria

The Doors of Durin, Lord of Moria. Speak, friend, and enter.

Hint: Did you know Tolkien was a polyglot?

## Writeup

This is a forensics/reversing challenge with two polyglot files representing two stages of the challenge.
We are given `moria.tar`, which is both an ELF executable and a Tar archive.

If we decide to run the executable first, we will see it doesn't get very far.
`strace` will show it checks to see if the tar has been extracted, and dies early if not.
This should be a hint to extract the tar, along with the filename and `file moria.tar` calling the challenge input a POSIX tar archive.

If we decide to extract the tar first, we are given a directory named \177ELF (ELF header magic value) containing `secrets.png` and `hint.txt`.
The hint text is designed to make you look closer at the tar file, if the directory name wasn't enough of a clue.
Examining the PNG closer will reveal a large zTxT chunk containing a file encrypted with openssl and then XOR-encoded.
A user may be able to crack the password without help, but this is not intended (if they can guess that this is an encrypted with a salted password, they can recover the XOR key "g@nd@lf" with the known prefix `Salted__`, then try to brute-force the password).

Once we have the tar extracted and are running the executable, it will prompt for user input.
It is intended that we reverse the ELF to determine the password, which will be used to decrypt the file embedded in the PNG file.
There are a few challenges to reversing here:

- `readelf` doesn't get very far parsing the tar file
- Ghidra refuses to load `moria.tar` as an ELF, so will need to load as a raw binary and locate the entrypoint manually.
- The code in `moria.tar` is a loader which drops an encoded payload into a memfd and execs that

The first goal should be to extract the stage 2 payload.
`strace` output shows the `memfd_create` and `execve` calls, telling us that this program is exec-ing a second stage.
Once the program is at the point where it blocks waiting for input, it is easy to copy out the contents of the ELF from `/proc/<PID>/exe`.
This can then be loaded into your disassembler of choice, bypassing the issues with the loader.
The binary was not stripped, so this should aid reversing.

Loading into Ghidra and looking at `main`, we can see a function `decrypt_stage2` which calls `check_password`.
Examining `check_password`, we see it permutes and then scrambles the bytes of the password and checks against a constant.
We can use normal reversing procedure to determine an input which will satisfy it (the password is `m00nl1ght`).
Once we have the password, entering it into the prompt will drop a new file inside the `\x7fELF` folder called `d00rs_0f_dur1n.pdf`.
This is another polyglot file.

When viewed as a PDF, this file contains an image with a hint that maybe you can treat this as a zip file.
Using `unzip`, we see that this is a password-protected zip file.
It also helpfully shows 125429 extra bytes at the beginning of the zipfile.
`fcrackzip` will find the password, but needs us to trim the extra bytes first.
`dd if=d00rz_0f_dur1n.pdf of=d00rz_0f_dur1n.zip bs=1 skip=125429` and `fcrackzip -u -D -p /usr/share/dict/words ./d00rz_0f_dur1n.zip` finds the password `Mellon`.

Once unzipped, the flag is inside `flag.jpg`.

void decrypt(char* s)
{
    s[8] = 0x8B;
    s[18] = 0x8A;
    s[2] = 0xC8;
    s[0] = 0x98;
    s[17] = 0xBB;
    s[13] = 0xDA;
    s[6] = 0x85;
    s[9] = 0xB2;
    s[15] = 0x7C;
    s[1] = 0x99;
    s[23] = 0xBB;
    s[3] = 0xC9;
    s[12] = 0xC4;
    s[19] = 0x92;
    s[24] = 0x63;
    s[21] = 0x56;
    s[22] = 0x7A;
    s[4] = 0x37;
    s[7] = 0x98;
    s[5] = 0xFE;
    s[20] = 0x81;
    s[11] = 0x75;
    s[10] = 0x30;
    s[14] = 0xDC;
    s[16] = 0x06;

    for (unsigned int rxoCT = 0, aNOWD = 0; rxoCT < 25; rxoCT++) {
        aNOWD = s[rxoCT];
        aNOWD += 0x14;
        aNOWD ^= 0xE8;
        aNOWD = ((aNOWD << 7) | ((aNOWD & 0xFF) >> 1)) & 0xFF;
        aNOWD += rxoCT;
        aNOWD = ((aNOWD << 5) | ((aNOWD & 0xFF) >> 3)) & 0xFF;
        aNOWD += 0x8C;
        aNOWD = ((aNOWD << 1) | ((aNOWD & 0xFF) >> 7)) & 0xFF;
        aNOWD ^= 0x33;
        aNOWD--;
        aNOWD ^= rxoCT;
        aNOWD--;
        aNOWD = ((aNOWD << 7) | ((aNOWD & 0xFF) >> 1)) & 0xFF;
        aNOWD -= rxoCT;
        aNOWD ^= rxoCT;
        aNOWD ^= 0x0A;
        aNOWD -= rxoCT;
        aNOWD++;
        aNOWD = ((aNOWD << 5) | ((aNOWD & 0xFF) >> 3)) & 0xFF;
        aNOWD++;
        s[rxoCT] = aNOWD;
    }
}

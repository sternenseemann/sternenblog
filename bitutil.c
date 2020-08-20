char nibble_hex(short h) {
    switch(h) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            return (h + 48);
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            return (h + 55);
        default:
            return 0;
    }
}


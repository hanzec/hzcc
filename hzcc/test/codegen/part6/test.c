int putchar(int c);

void prints(const char s[]) {
    int i;
    i = 0;
    do {
        putchar((int)s[i++]);
    } while (s[i]);
}

int main() {
    prints("Hello, world!\n");
    return 0;
}

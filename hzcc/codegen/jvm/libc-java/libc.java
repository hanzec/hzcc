import java.io.IOException;

/** @brief Minimal character I/O bridge used by generated JVM programs. */
class libc {
  /**
   * @brief Write the supplied character to standard output.
   * @param c Character code converted to a Java char for output.
   * @return The original input value.
   */
  public static int putchar(int c) {
    System.out.print((char) c);
    return c;
  }
  /**
   * @brief Read one byte from standard input.
   * @return The byte value, or -1 at end of input.
   * @throws IOException If the input stream reports an I/O failure.
   */
  public static int getchar() throws IOException {
    return System.in.read();
  }
};

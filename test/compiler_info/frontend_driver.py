"""Check frontend exit status, input ordering, diagnostics and output files."""

from pathlib import Path
import subprocess
import sys
import tempfile


def check_driver(compiler, work):
    """Invoke the driver in a private directory and verify its public CLI contract."""
    def run(*args, success=True):
        """Return captured output after verifying normal termination and status."""
        result = subprocess.run([compiler, *args], cwd=work, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE)
        assert result.returncode >= 0 and (result.returncode == 0) == success, (
            args, result.returncode, result.stdout, result.stderr)
        return result

    first, second, invalid = "first input.c", "second.c", "invalid input.c"
    first_source = b"int first();\n"
    (work / first).write_bytes(first_source)
    (work / second).write_bytes(b"int second();\n")
    (work / invalid).write_bytes(b'"unterminated\n')
    for args in [(), ("--unknown-flag", first), ("--fsyntax_only", "--flexical_only", first),
                 ("--flexical_only", first, "-o"), ("missing.c",)]:
        assert run(*args, success=False).stderr, args

    summary = "File {} is syntactically correct.".format
    assert run("--fsyntax_only", first, second).stdout.decode().splitlines() == [
        summary(first), summary(second)]
    assert not run(first, second).stdout
    malformed = "malformed.c"
    (work / malformed).write_bytes(b"int broken(;\n")
    for bad in [invalid, malformed]:
        for args in [(first, bad), (bad, first)]:
            result = run("--fsyntax_only", "--fno_color", *args, success=False)
            assert result.stdout.decode().splitlines() == [summary(first)]
            assert bad.encode() in result.stderr
            if bad == malformed:
                lines = result.stderr.splitlines()
                source_row = next(i for i, line in enumerate(lines) if b"| int broken(;" in line)
                assert lines[source_row + 1].index(b"^") == lines[source_row].index(b";")

    one = run("--flexical_only", first).stdout
    two = run("--flexical_only", second).stdout
    assert len(one.splitlines()) == len(two.splitlines()) == 5
    for name, listing in [(first, one), (second, two)]:
        assert all(line.startswith(("File " + name + " Line ").encode())
                   for line in listing.splitlines())
    assert run("--flexical_only", first, second).stdout == one + two
    out = work / "token output.txt"
    assert not run("--flexical_only", "-o", out.name, first, second).stdout
    assert out.read_bytes() == one + two
    assert not (work / "token_output.txt").exists()

    for args in [("--fsyntax_only", "-o", out.name, first), ("-o", out.name, first),
                 ("--flexical_only", "-o", out.name, first, invalid),
                 ("--flexical_only", "-o", out.name, invalid, second)]:
        out.write_bytes(b"original contents\n")
        assert run(*args, success=False).stderr
        assert out.read_bytes() == b"original contents\n"
    assert run("--flexical_only", first, "-o", first, success=False).stderr
    assert (work / first).read_bytes() == first_source
    assert run("--flexical_only", first, "-o", "absent/out", success=False).stderr
    if Path("/dev/full").is_char_device():
        assert run("--flexical_only", first, "-o", "/dev/full", success=False).stderr


if __name__ == "__main__":
    with tempfile.TemporaryDirectory(prefix="hzcc driver ") as temp:
        check_driver(str(Path(sys.argv[1]).resolve()), Path(temp))

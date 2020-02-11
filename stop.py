from pathlib import Path


if __name__ == '__main__':
    Path('stop').write_bytes(b'')

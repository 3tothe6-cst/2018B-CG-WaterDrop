import cv2
import numpy as np


def _main(name: str) -> None:
    img = cv2.imread(f'{name}.jpg').astype(np.float64) / 255.
    with open(f'{name}.dat', 'wb') as data:
        np.array([img.shape[1], img.shape[0]], dtype=np.int64).tofile(data)
        img.tofile(data)

def main() -> None:
    _main('threebody')
    _main('saturn')

if __name__ == '__main__':
    main()

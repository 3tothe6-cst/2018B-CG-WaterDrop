import sys

import cv2
import numpy as np


def main():
    with open('out/scene.dat', 'rb') as data:
        count = int.from_bytes(data.read(8), sys.byteorder)
        xs = int.from_bytes(data.read(8), sys.byteorder)
        ys = int.from_bytes(data.read(8), sys.byteorder)
        img = np.fromfile(data, dtype=np.float64) / count
        img = np.where(img > 1., 1., img)
        img = (img*255).astype(np.uint8).reshape(ys, xs, 3)
        cv2.imwrite('out/scene.png', img)

if __name__ == '__main__':
    main()

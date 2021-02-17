# Block size: 8
# NonML: processes 1 second of audio in 0.0815474 seconds
# ML: processes 1 second of audio in 0.0529038 seconds
# Block size: 16
# NonML: processes 1 second of audio in 0.077933 seconds
# ML: processes 1 second of audio in 0.0463063 seconds
# Block size: 32
# NonML: processes 1 second of audio in 0.0790124 seconds
# ML: processes 1 second of audio in 0.0557 seconds
# Block size: 64
# NonML: processes 1 second of audio in 0.0639211 seconds
# ML: processes 1 second of audio in 0.0510621 seconds
# Block size: 128
# NonML: processes 1 second of audio in 0.092056 seconds
# ML: processes 1 second of audio in 0.0506365 seconds
# Block size: 256
# NonML: processes 1 second of audio in 0.0673617 seconds
# ML: processes 1 second of audio in 0.0513962 seconds
# Block size: 512
# NonML: processes 1 second of audio in 0.0690628 seconds
# ML: processes 1 second of audio in 0.0459101 seconds
# Block size: 1024
# NonML: processes 1 second of audio in 0.0630335 seconds
# ML: processes 1 second of audio in 0.0455831 seconds
# Block size: 2048
# NonML: processes 1 second of audio in 0.0726925 seconds
# ML: processes 1 second of audio in 0.0450243 seconds
# Block size: 4096
# NonML: processes 1 second of audio in 0.0609412 seconds
# ML: processes 1 second of audio in 0.0450535 seconds

import numpy as np
import matplotlib.pyplot as plt

blocks =     [8,         16,        32,        64,        128,       256,       512,       1024,      2048,      4096]
nonmlTimes = [0.0723437, 0.0703079, 0.0652856, 0.0662835, 0.0666593, 0.0696844, 0.0669037, 0.060816,  0.0695175, 0.0623839]
mlTimes =    [0.0528792, 0.0510437, 0.0511147, 0.0502434, 0.0495194, 0.0480298, 0.0477946, 0.0488841, 0.0488309, 0.0472191]

nonmlTimes = np.asarray(nonmlTimes)
mlTimes = np.asarray(mlTimes)

plt.loglog(blocks, nonmlTimes)
plt.loglog(blocks, mlTimes)
plt.ylim(0.045, 0.08)
plt.grid()
plt.show()

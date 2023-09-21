import struct
import sys
from pathlib import Path
import shutil

data_path = Path(__file__).resolve().parent
for level in (data_path / 'levels').iterdir():
    (data_path / 'dist' / 'levels' / level.name).mkdir(exist_ok=True)
    dist_path = data_path / 'dist' / 'levels' / level.name
    shutil.copy(level / 'background.wav', dist_path)
    shutil.copy(level / 'cells.png', dist_path)
    data = list(map(float, (level / 'meta.lvl').read_text().split()))
    if len(data) != 9:
        sys.exit('Wrong number of floats in meta.lvl!')
    with (dist_path / 'meta.chunk').open('wb') as blob:
        blob.write(struct.pack('4s', b'meta'))
        blob.write(struct.pack('I', len(data) * 4))
        blob.write(struct.pack('9f', *data))

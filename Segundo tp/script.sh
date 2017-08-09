#!/bin/bash
cd /home/utnso/Escritorio

git clone https://github.com/sisoputnfrba/so-commons-library
cd so-commons-library
sudo make install

cd /home/utnso/Escritorio/tp-2017-1c-Equipo-Rocket/rocketLibrary/Debug
make clean
make all

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/utnso/Escritorio/tp-2017-1c-Equipo-Rocket/rocketLibrary/Debug

cd /home/utnso/Escritorio/tp-2017-1c-Equipo-Rocket
cd kernel/Debug
make clean
make all

cd /home/utnso/Escritorio/tp-2017-1c-Equipo-Rocket/consola/Debug
make clean
make all

cd /home/utnso/Escritorio/tp-2017-1c-Equipo-Rocket/filesystem/Debug
make clean
make all

cd /home/utnso/Escritorio/tp-2017-1c-Equipo-Rocket/filesystem/Debug
make clean
make all

cd /home/utnso/Escritorio
git clone https://github.com/sisoputnfrba/ansisop-parser
cd ansisop-parser/parser
make all
sudo make install

cd /home/utnso/Escritorio/ansisop-parser
cd programas-ejemplo/evaluacion-final-esther/FS-ejemplo
tar -xzvf FS.tgz
cd /home/utnso/Escritorio


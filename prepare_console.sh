export PVM_ROOT="/home/C/Studia/PR/halter/pvm3"
export PVM_HOME=$PVM_ROOT
export PVM_ARCH=`${PVM_ROOT}/lib/pvmgetarch`
export PATH=${PATH}:${PVM_ROOT}/lib:${PVM_ROOT}/bin/${PVM_ARCH}:./bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PVM_ROOT}/lib
export MANPATH=$MANPATH:$PVM_ROOT/man

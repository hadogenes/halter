export PVM_ROOT="/home/C/Studia/PR/halter/pvm3"
#export PVM_HOME="/home/C/Studia/PR/halter/build"
export PVM_SRC="/home/C/Studia/PR/halter"

export PVM_ARCH="`${PVM_ROOT}/lib/pvmgetarch`"

export PVM_HOME="$HOME/pvm3/bin/$PVM_ARCH"

export PATH="${PATH}:${PVM_ROOT}/lib:${PVM_ROOT}/bin/${PVM_ARCH}:${PVM_ROOT}/bin"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:${PVM_ROOT}/lib"
export MANPATH="$MANPATH:$PVM_ROOT/man"

export PVM_RSH="/usr/bin/ssh"

alias pvm="pvm -nlocalhost"

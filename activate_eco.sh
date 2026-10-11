
export _OLD_ECO_PATH="$PATH"
export _OLD_ECO_PS1="$PS1"

export PS1="(eco-env) $PS1"

export ECO_TOOLCHAIN="$HOME/Dev/toolchain"
export ECO_FRAMEWORK="$HOME/Dev/Eco.Framework"

export ECO_LINKER="$ECO_TOOLCHAIN/eco-x86_64-linux-gnu-gcc-13.2.0/lib/ld-linux-x86-64.so.2"
export ECO_LIBS="$ECO_TOOLCHAIN/eco-x86_64-linux-gnu-gcc-13.2.0/lib:$ECO_TOOLCHAIN/eco-x86_64-linux-gnu-gcc-13.2.0/lib64"

alias make="$ECO_LINKER --library-path $ECO_LIBS $ECO_TOOLCHAIN/eco-gnu-make-4.4.1/bin/make"
alias x86_64-linux-gnu-gcc="$ECO_LINKER --library-path $ECO_LIBS $ECO_TOOLCHAIN/eco-x86_64-linux-gnu-gcc-13.2.0/bin/x86_64-linux-gnu-gcc"
alias gcc="$ECO_LINKER --library-path $ECO_LIBS $ECO_TOOLCHAIN/eco-x86_64-linux-gnu-gcc-13.2.0/bin/x86_64-linux-gnu-gcc"

deactivate() {
    if [ -n "$_OLD_ECO_PATH" ]; then export PATH="$_OLD_ECO_PATH"; unset _OLD_ECO_PATH; fi
    if [ -n "$_OLD_ECO_PS1" ]; then export PS1="$_OLD_ECO_PS1"; unset _OLD_ECO_PS1; fi

    unalias make x86_64-linux-gnu-gcc gcc 2>/dev/null

    unset ECO_TOOLCHAIN ECO_FRAMEWORK ECO_LINKER ECO_LIBS
    unset -f deactivate
    echo "=== [ECO] Environment deactivated ==="
}

echo "=== [ECO] Virtual environment successfully activated ==="
echo "To exit, enter the command: deactivate"

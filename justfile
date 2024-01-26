# -*-Makefile-*-

# Needed to make `"$@"` usable in recipes
set positional-arguments := true

default:
  just run --beam-on 10

setup BUILD SRC:
  #!/usr/bin/env sh
  if [[ ! -d {{BUILD}} ]]; then
    meson setup {{BUILD}} {{SRC}}
  fi

install-tests:
  just install-lib-quiet
  just setup build/fullpet-tests test
  meson compile -C build/fullpet-tests
  meson install -C build/fullpet-tests

test *REST: install-tests
  sh install/fullpet-tests/run-each-test-in-separate-process.sh "$@"

catch2-demo *REST:
  echo "$@"
  meson setup      build/fullpet-tests test
  meson compile -C build/fullpet-tests
  meson install -C build/fullpet-tests
  install/fullpet-tests/bin/catch2-demo-test "$@"

build:
  just setup build/fullpet src
  meson compile -C build/fullpet

install: build
  meson install -C build/fullpet


install-lib-quiet:
  #!/usr/bin/env sh
  if [[ -f install/fullpet/lib/libfullpet.so    ||
        -f install/fullpet/lib/libfullpet.dylib ]]; then
    install_output=$(just install)
    install_ok=$?
    if ! test install_ok; then
      echo $install_output
      exit $install_ok
    fi
  else
    just install
  fi

run *ARGS: install
  #!/usr/bin/env sh
  sh execute-with-nixgl-if-needed.sh ./install/fullpet/bin/fullpet "$@"
  exit $?

clean:
  rm build install -rf

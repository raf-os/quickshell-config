#!/bin/bash

echo "test"

activate() {
  hyprctl --batch "\
    keyword animations:enabled 0;\
    keyword decoration:shadow:enabled 0;\
    keyword decoration:blur:enabled 0;\
    keyword decoration:fullscreen_opacity 1;\
    keyword general:gaps_in 0;\
    keyword general:border_size 1;\
    keyword decoration:rounding 0" > /dev/null 2>&1
}

deactivate() {
  hyprctl reload > /dev/null 2>&1
}

case $1 in
  true|activate)
    activate
    ;;
  *)
    deactivate
    ;;
esac

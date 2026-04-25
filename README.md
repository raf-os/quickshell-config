My own personal quickshell config.

Requires quickshell and all its dependencies.

A bunch of the early code and structure was based off of the [Caelestia](https://github.com/caelestia-dots/shell) shell. It's more feature complete and customizable, so maybe go check it out if it interests you.

# Manual Installation

Make sure you have all the dependencies below, then clone the repository:

```
git clone https://github.com/raf-os/quickshell-config
cd quickshell-config
```

Then build and install with cmake:

```
cmake --preset default
cmake --build --preset default
sudo cmake --install build
```

To run quickshell with this config installed, run:

```
quickshell -c myshell
```

To automaticaly load this config on startup with hyprland, add this somewhere on your hyprland.conf:

```
exec-once = quickshell -c myshell
```

## Dependencies

This list is incomplete as of now

* [Hyprland](https://hypr.land/)
* app2unit
* [Sway Notification Center](https://github.com/ErikReider/SwayNotificationCenter)
* NetworkManager
* libxml2

### Optional build dependencies

* xmllint
* [uv](https://docs.astral.sh/uv/)

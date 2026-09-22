from ..generator.schematypes import *

__rootnode__ = Node(
  name="AppearanceConfig",
  props=[IntProperty(name="scaling", defaultValue=1)],
  children=[
    Node(
      name="FontFamily",
      props=[
        StrProperty(name="sans", defaultValue="Inter"),
        StrProperty(name="mono", defaultValue="RobotoMono Nerd Font Propo"),
      ],
    ),
  ],
)

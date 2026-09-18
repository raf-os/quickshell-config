from ..generator.schematypes import *

__rootnode__ = Node(
  name="Appearance",
  props=[IntProperty(name="scaling", defaultValue=1)],
  children=[
    Node(
      name="FontFamily",
      props=[
        StrProperty(name="sans", defaultValue="DejaVu Sans"),
        StrProperty(name="mono", defaultValue="RobotoMono Nerd Font Propo"),
      ],
    ),
  ],
)

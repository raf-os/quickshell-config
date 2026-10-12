from ..generator.schematypes import *

__rootnode__ = Node(
  name="ModulesConfig",
  children=[
    Node(
      name="EnabledModules",
      props=[
        BoolProperty(name="background", defaultValue=True),
        BoolProperty(name="notifications", defaultValue=True),
        BoolProperty(name="gamemode", defaultValue=True),
        BoolProperty(name="weather", defaultValue=True),
      ],
    ),
    Node(
      name="Weather",
      props=[
        FloatProperty(name="latitude", defaultValue=0.0),
        FloatProperty(name="longitude", defaultValue=0.0),
      ],
    ),
  ],
)

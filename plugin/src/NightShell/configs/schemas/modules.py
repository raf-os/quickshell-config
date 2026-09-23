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
      ],
    )
  ],
)

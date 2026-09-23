from __future__ import annotations

from typing import override

from pydantic import BaseModel


class BaseProperty(BaseModel):
  name: str
  type: str
  readable: bool = True
  writable: bool = True

  def getDefaultValue(self) -> str | None:
    pass


class Property[T](BaseProperty):
  name: str
  type: str
  defaultValue: T | None = None


class IntProperty(Property[int]):
  type: str = "int"

  @override
  def getDefaultValue(self) -> str | None:
    d = self.defaultValue
    if d is None:
      return None
    return f"{str(self.defaultValue)}"


class BoolProperty(Property[bool]):
  type: str = "bool"

  @override
  def getDefaultValue(self) -> str | None:
    d = self.defaultValue
    if d is None:
      return None
    return "true" if self.defaultValue else "false"


class StrProperty(Property[str]):
  type: str = "QString"

  @override
  def getDefaultValue(self) -> str | None:
    return f'"{self.defaultValue}"'


class ListProperty[int](Property[list[int]]):
  type: str = "QList<int>"

  @override
  def getDefaultValue(self) -> str | None:
    dl = self.defaultValue
    if dl is None:
      return None
    return f"[{', '.join([str(i) for i in dl])}]"


class Node(BaseModel):
  name: str
  props: list[BaseProperty] = []
  children: list[Node] = []

#!/usr/bin/env -S uv run --script
# /// script
# dependencies = ["pydantic"]
# ///
# pyright: reportUnusedCallResult=false
# pyright: reportUnusedVariable=false

import os
import subprocess
import sys
from abc import ABC, abstractmethod
from enum import StrEnum
from pathlib import Path
from typing import Any, Literal, final

from pydantic import BaseModel, ValidationError

_NAMESPACES: list[str] = ["ns", "configs"]
_CPP_NAMESPACE = "::".join(_NAMESPACES) + "::"


def toLowerCamelCase(s: str) -> str:
  return s[:1].lower() + s[1:]


def toUpperCamelCase(s: str) -> str:
  return s[:1].upper() + s[1:]


class PropertyType(StrEnum):
  STRING = "QString"
  COLOR = "QColor"
  LIST = "QList"
  FLOAT = "qreal"
  INT = "int"
  BOOL = "bool"


class BaseProperty(BaseModel):
  name: str


class Property[T](BaseProperty):
  type: PropertyType
  defaultValue: T | None = None


class IntProperty(Property[int]):
  type: PropertyType = PropertyType.INT


class StrProperty(Property[str]):
  type: PropertyType = PropertyType.STRING


class ListProperty[T](Property[list[T]]):
  type: PropertyType = PropertyType.LIST


class Node(BaseModel):
  name: str
  props: list[BaseProperty] = []
  children: list[Node] = []


rootConfig = [
  Node(
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
  ),
  Node(name="Bar", props=[IntProperty(name="height", defaultValue=32)]),
]


class HeaderClassMeta(BaseModel):
  qMacros: list[str] = ["Q_PROPERTY", "QML_ELEMENT", 'QML_UNCREATABLE("")']
  qProperties: list[str] = []
  bindableGetters: list[str] = []
  childGetters: list[str] = []

  def addProperty[T](self, prop: Property[T]):
    qprop: list[str] = [
      f"{prop.type.value} {toLowerCamelCase(prop.name)}",
      "READ default",
    ]
    if prop.defaultValue is not None:
      qprop.append("RESET")


class ClassMeta(BaseModel):
  parent: ClassMeta | None = None
  header: HeaderClassMeta = HeaderClassMeta()

  def addProperty[T](self, prop: Property[T]):
    pass


class HeaderDocument(BaseModel):
  pragmas: list[str] = ["#pragma once"]

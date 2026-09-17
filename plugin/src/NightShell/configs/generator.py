#!/usr/bin/env -S uv run --script
# /// script
# dependencies = ["pydantic"]
# ///
# pyright: reportUnusedCallResult=false
# pyright: reportUnusedVariable=false

from __future__ import annotations

import re
from abc import ABC, ABCMeta, abstractmethod
from enum import StrEnum
from typing import Any, cast, override

from pydantic import BaseModel, ConfigDict, PrivateAttr

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
  ERROR = "ERROR"


class BaseProperty(BaseModel):
  name: str
  type: PropertyType

  def getDefaultValue(self) -> str | None:
    pass


class Property[T](BaseProperty):
  name: str
  type: PropertyType
  defaultValue: T | None = None


class IntProperty(Property[int]):
  type: PropertyType = PropertyType.INT

  @override
  def getDefaultValue(self) -> str | None:
    d = self.defaultValue
    if d is None:
      return None
    return f"{str(self.defaultValue)}"


class StrProperty(Property[str]):
  type: PropertyType = PropertyType.STRING

  @override
  def getDefaultValue(self) -> str | None:
    return f'"{self.defaultValue}"'


class ListProperty[int](Property[list[int]]):
  type: PropertyType = PropertyType.LIST

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


rootConfig: list[Node] = [
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


def toIndentedBlock(block: list[str], indentation: int) -> str:
  if len(block) == 0:
    return ""
  indentStr = "\t" * indentation
  return indentStr + (f"\n{indentStr}".join(block))


def wrapNamespace(content: str) -> str:
  ns = "::".join(_NAMESPACES)
  return f"namespace {ns} {{\n{content}\n}} // namespace {ns}"


class DocPair:
  header: str
  body: str

  def __init__(self, header: str, body: str) -> None:
    self.header = header
    self.body = body


class DocumentMeta(BaseModel):
  rootNode: Node
  _root: ClassMeta = PrivateAttr()
  _name: str = PrivateAttr()
  includes: list[str] = [
    "qobject.h",
    "qmlintegration.h",
    "qtmetamacros.h",
    "qproperty.h",
  ]
  headerDefs: list[str] = []
  bodyDefs: list[str] = []

  @override
  def model_post_init(self, context: Any, /) -> None:  # pyright: ignore[reportExplicitAny, reportAny]
    rootNode = self.rootNode
    rootClass = ClassMeta(
      name=rootNode.name, rootNode=None, parent=None, parentDocument=self
    )
    self._root = rootClass
    self._name = re.sub(r"config", "", rootNode.name, flags=re.IGNORECASE).lower()
    self.recursiveCreateClass(rootNode, rootClass)

  def name(self) -> str:
    return self._name

  def className(self) -> str:
    return self._root.name

  def recursiveCreateClass(self, node: Node, currentClass: ClassMeta | None):
    current: ClassMeta
    rnode: ClassMeta | None
    if currentClass is None:
      current = ClassMeta(
        name=node.name, rootNode=None, parent=None, parentDocument=self
      )
      rnode = current
    else:
      current = currentClass
      rnode = current.rootNode

    for prop in node.props:
      bindable = QBindableProp(
        name=prop.name,
        type=prop.type.value,
        parentClass=current,
        defaultValue=prop.getDefaultValue(),
      )
      current.addProperty(bindable)

    for child in node.children:
      childNode = ClassMeta(
        name=child.name, rootNode=current.rootNode, parentDocument=self
      )
      current.addChild(childNode)
      self.recursiveCreateClass(child, childNode)

  def addImport(self, type: str):
    header: str | None
    match type:
      case "QString":
        header = "qstring.h"
      case "QColor":
        header = "qcolor.h"
      case "QList":
        header = "qlist.h"
      case "qreal":
        header = "qreal.h"
      case _:
        header = None

    if header is None:
      return

    if header not in self.includes:
      self.includes.append(header)

  def compile(self) -> DocPair:
    self.headerDefs = []
    self.bodyDefs = []
    self.compileRecursive(self._root)
    header = "\n\n".join(self.headerDefs)
    body = "\n\n".join(self.bodyDefs)
    ns = "::".join(_NAMESPACES)
    return DocPair(
      f'#pragma once\n\n#include "serializableobject.h"\n\n{"\n".join([f"#include <{inc}>" for inc in self.includes])}\n\n{wrapNamespace(header)}',
      f'#include "{self._name}.h"\n#include "serializableobject.h"\n\n{wrapNamespace(body)}',
    )

  def compileRecursive(self, current: ClassMeta) -> None:
    for child in current.children:
      self.compileRecursive(child)

    self.headerDefs.append(cast(ClassHeaderMeta, current.header).compile())
    self.bodyDefs.append(cast(ClassBodyMeta, current.body).compile())


class ClassMeta(BaseModel):
  name: str
  rootNode: ClassMeta | None = None
  parent: ClassMeta | None = None
  parentDocument: DocumentMeta
  data: QChildNodeProp | None = None
  children: list[ClassMeta] = []
  properties: list[QBindableProp] = []

  header: ClassHeaderMeta | None = None
  body: ClassBodyMeta | None = None

  @override
  def model_post_init(self, context: Any, /) -> None:  # pyright: ignore[reportExplicitAny, reportAny]
    super().model_post_init(context)
    self.data = QChildNodeProp(name=self.name, parentClass=self)
    self.header = ClassHeaderMeta(parentClass=self)
    self.body = ClassBodyMeta(parentClass=self)

  def addProperty(self, prop: QBindableProp):
    self.properties.append(prop)

  def addChild(self, child: ClassMeta):
    child.parent = self
    self.children.append(child)


class ClassHeaderMeta(BaseModel):
  macros: list[str] = ["Q_OBJECT", "QML_ELEMENT", 'QML_UNCREATABLE("")']
  parentClass: ClassMeta

  def compile(self) -> str:
    parentClass = self.parentClass
    content: str = ""
    macroBlock = self.macros
    bindableQProps: list[str] = []
    childrenQProps: list[str] = []
    bindableGetters: list[str] = []
    childGetters: list[str] = []
    signals: list[str] = []
    privateMembers: list[str] = []
    privateBindables: list[str] = []

    for prop in parentClass.properties:
      parentClass.parentDocument.addImport(prop.type)
      bindableQProps.append(prop.getQProperty())
      bindableGetters.append(prop.getterDeclaration())
      signals.append(prop.getSignal())
      privateBindables.append(prop.getBindableMacro())
    for child in parentClass.children:
      cprop = cast(QChildNodeProp, child.data)
      childrenQProps.append(cprop.getQProperty())
      childGetters.append(cprop.getterDeclaration())
      privateMembers.append(cprop.getPrivateMember())

    content = f"{toIndentedBlock(macroBlock, 1)}\n\n"
    if len(bindableQProps) != 0 or len(childrenQProps) != 0:
      content += "\n".join(
        [toIndentedBlock(bindableQProps, 1), toIndentedBlock(childrenQProps, 1)]
      )

    content += (
      f"\npublic:\n"
      + f"\texplicit {parentClass.name}(const QString &className, QObject *root, QObject *parent);\n"
    )

    if len(bindableGetters) != 0 or len(childGetters) != 0:
      content += toIndentedBlock(bindableGetters + childGetters, 1)

    if len(signals) != 0:
      content += "\n\nsignals:\n"
      content += toIndentedBlock(signals, 1)

    if len(privateMembers) != 0 or len(privateBindables) != 0:
      content += "\n\nprivate:\n"
      content += toIndentedBlock(privateMembers + privateBindables, 1)

    return (
      f"class {parentClass.name} : public utils::SerializableObject {{\n{content}\n}}"
    )


class ClassBodyMeta(BaseModel):
  parentClass: ClassMeta

  def constructor(self, parentClass: ClassMeta):
    return f"{parentClass.name}::{parentClass.name}(const QString &className, QObject *root, QObject *parent) : utils::SerializableObject(className, root, parent) {{}}"

  def getters(self, parentClass: ClassMeta) -> str:
    getterList: list[str] = []
    for bindable in parentClass.properties:
      getterList.append(bindable.getterImplementation())
    for child in parentClass.children:
      getterList.append(cast(QChildNodeProp, child.data).getterImplementation())
    return "\n".join(getterList)

  def compile(self) -> str:
    return f"{self.constructor(self.parentClass)}\n\n{self.getters(self.parentClass)}"


class QProp(BaseModel, metaclass=ABCMeta):
  name: str
  _reader: str = PrivateAttr()
  _member: str = PrivateAttr()
  parentClass: ClassMeta

  @abstractmethod
  def getQProperty(self) -> str:
    pass

  @abstractmethod
  def getterDeclaration(self) -> str:
    pass

  @abstractmethod
  def getterImplementation(self) -> str:
    pass


class QChildNodeProp(QProp):
  name: str
  _reader: str = PrivateAttr()
  _member: str = PrivateAttr()
  parentClass: ClassMeta

  @override
  def model_post_init(self, context: Any, /) -> None:
    self._reader = f"{toLowerCamelCase(self.name)}"
    self._member = f"m_{toLowerCamelCase(self.name)}"

  @override
  def getQProperty(self) -> str:
    qprop: list[str] = [
      f"{_CPP_NAMESPACE}{self.name} *{self._reader}",
      f"READ {self._reader}",
      "CONSTANT",
    ]

    return f"Q_PROPERTY({' '.join(qprop)}))"

  @override
  def getterDeclaration(self) -> str:
    return f"[[nodiscard]] {self.name} *{self._reader}();"

  @override
  def getterImplementation(self) -> str:
    if self.parentClass.parent is None:
      return ""
    return f"{self.name} *{self.parentClass.parent.name}::{self._reader}() const {{ return {self._member}; }}"

  def getPrivateMember(self) -> str:
    rootNodeStr: str
    if self.parentClass.rootNode is None:
      rootNodeStr = "this"
    else:
      rootNodeStr = "m_rootNode"
    return f'{self.name} {self._member}{{"{self.name}", {rootNodeStr}, this}};'


class QBindableProp(QProp):
  name: str
  type: str
  _member: str = PrivateAttr()
  reader: str = "default"
  _notifier: str = PrivateAttr()
  _bindable: str = PrivateAttr()
  defaultValue: str | None
  parentClass: ClassMeta

  @override
  def model_post_init(self, context: Any, /) -> None:
    self._notifier = f"{toLowerCamelCase(self.name)}Changed"
    self._bindable = f"bindable{toUpperCamelCase(self.name)}"
    self._member = f"b_{toLowerCamelCase(self.name)}"

  @override
  def getQProperty(self) -> str:
    qprop: list[str] = [
      f"{self.type} {self.name}",
      f"READ {self.reader}",
      f"NOTIFY {self._notifier}",
      f"BINDABLE {self._bindable}",
    ]
    if self.defaultValue is not None:
      qprop.append(f"RESET reset{toUpperCamelCase(self.name)}")

    return f"Q_PROPERTY({' '.join(qprop)})"

  @override
  def getterDeclaration(self) -> str:
    return (
      f"[[nodiscard]] QBindable<{self.type}> {toLowerCamelCase(self.name)}() const;"
    )

  @override
  def getterImplementation(self) -> str:
    return f"QBindable<{self.type}> {self.parentClass.name}::{self._bindable} const {{ return &{self._member}; }}"

  def getSignal(self) -> str:
    return f"void {self._notifier}();"

  def getBindableMacro(self) -> str:
    if self.defaultValue is not None:
      return f"Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS({self.parentClass.name}, {self.type}, {self._member}, {self.defaultValue}, &{self.parentClass.name}::{self._notifier})"
    else:
      return f"Q_OBJECT_BINDABLE_PROPERTY({self.parentClass.name}, {self.type}, {self._member}, &{self.parentClass.name}::{self._notifier})"


docs: list[DocumentMeta] = [DocumentMeta(rootNode=node) for node in rootConfig]

genSources: list[str] = []

for doc in docs:
  p = doc.compile()
  genSources.append(doc.name())

cmakeLines: list[str] = [
  "set(GENERATED_SOURCES",
  toIndentedBlock([f"generated/{s}.cpp" for s in genSources], 1),
  "\tPARENT_SCOPE",
  ")",
]

macroLines = "\n".join(
  [f"X({d.className()}, {toLowerCamelCase(d.className())})" for d in docs]
)

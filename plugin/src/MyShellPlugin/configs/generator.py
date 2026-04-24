#!/usr/bin/env -S uv run --script
# /// script
# dependencies = ["pydantic"]
# ///
# pyright: reportUnusedCallResult=false
# pyright: reportUnusedVariable=false

import subprocess
import sys
from typing import Literal, override
from pathlib import Path
from abc import ABC, abstractmethod
from pydantic import BaseModel, ValidationError

_NAMESPACES: list[str] = ["myqmlplugin", "configs"]
_CPP_NAMESPACE = "::".join(_NAMESPACES) + "::"

class ObjectWrapper(BaseModel):
    prefix: str = ""
    suffix: str = ""
    content: str = ""
    lock: bool = False
    isLocked: bool = False
    metaWrapper: ObjectWrapper | None = None

    def wrap(self, content: str | None = None) -> str:
        if self.lock and self.isLocked:
            raise SyntaxError("Attempting to wrap a locked class")
        if self.lock:
            self.isLocked = True
        wstr = f"{self.prefix}{self.content if content == None else content}{self.suffix}"
        if (self.metaWrapper != None):
            return self.metaWrapper.wrap(wstr)
        return wstr

class BaseProperty(BaseModel):
    name: str

class BaseBindableProperty(BaseProperty):
    binding: str | None = None

class SimpleValueString(BaseBindableProperty):
    type: Literal["QString", "QColor"]
    defaultValue: str | None = None

    @override
    def model_post_init(self, context: object, /) -> None:
        if (self.defaultValue != None):
            self.defaultValue = f"\"{self.defaultValue}\""
        return super().model_post_init(context)

class SimpleValueReal(BaseBindableProperty):
    type: Literal["qreal"]
    defaultValue: float | None = None

class SimpleValueInt(BaseBindableProperty):
    type: Literal["int"]
    defaultValue: int | None = None

class SimpleValueBool(BaseBindableProperty):
    type: Literal["bool"]
    defaultValue: bool | None = None

class BaseListProperty(BaseBindableProperty):
    type: Literal["QList"]
    listType: str
    defaultValue: list[str | int | float]
    mutable: bool = False

SimpleValue = SimpleValueString | SimpleValueReal | SimpleValueInt | SimpleValueBool | BaseListProperty

class BaseDocument(BaseModel):
    className: str
    properties: list[SimpleValue | ObjectProperty]

class ObjectProperty(BaseProperty):
    type: Literal["QObject"]
    properties: list[SimpleValue | ObjectProperty]

class ClassMeta(BaseModel):
    header: HeaderClassMeta
    body: BodyClassMeta
    constructor: str | None = None

class HeaderClassMeta(BaseModel):
    macros: str = ""
    public: str = ""
    private: str = ""

class BodyClassMeta(BaseModel):
    content: str = ""

class BaseClassMeta(BaseModel, ABC): #pyright: ignore[reportUnsafeMultipleInheritance]
    parent: CppClassModel
    name: str

    @abstractmethod
    def compile(self) -> ClassMeta:
        pass

class ChildClassMeta(BaseClassMeta):
    @override
    def compile(self) -> ClassMeta:
        nameLower = self.name[:1].lower()+self.name[1:]
        return ClassMeta(
            header=HeaderClassMeta(
                macros=(" ".join([
                        f"\n\tQ_PROPERTY({self.name} *{nameLower}",
                        f"READ {nameLower}",
                        "CONSTANT"
                        ]) + ")"
                ),
                public=(f"\n\t[[nodiscard]] {self.name} *{nameLower}() const;\n"),
                private=(f"\n\t{self.name} *m_{nameLower} = new {self.name}(this);")
            ),
            body=BodyClassMeta(
                content=f"\n{self.name} *{self.parent.className}::{nameLower}() const {{ return m_{nameLower}; }}\n"
            )
        )

class ListProperty(BaseClassMeta):
    parent: CppClassModel
    name: str
    type: str
    mutable: bool = False
    defaultValue: list[int | float | str] = []

    @override
    def compile(self) -> ClassMeta:
        nameCap = self.name[:1].upper() + self.name[1:]
        cList = ", ".join([str(x) for x in self.defaultValue])
        constMod = "" if self.mutable else "const "
        constModSuf = "" if self.mutable else " const"

        contentStr = f"\n{constMod}QList<{self.type}> &{self.parent.className}::{self.name}() {constMod}{{ return m_{self.name}; }}" # GETTER

        publicStr = (
            f"\n\t[[nodiscard]] {constMod}QList<{self.type}> &{self.name}(){constModSuf};"
        )

        if self.mutable:
            contentStr += (
                f"void {self.parent.className}::set{nameCap}(const QList<{self.type}> &newValue) {{"
                f"\n\tif (m_{self.name} == newValue)"
                f"\n\t\treturn;"
                f"\n\tm_{self.name} = newValue;"
                f"\n\temit {self.name}Changed();"
                "\n}"
            )
            publicStr += (
                f"\n\tvoid set{nameCap}(const QList<{self.type}> &newList);"
                f"\n\tQ_SIGNAL void {self.name}Changed();"
            )

        return ClassMeta(
            header=HeaderClassMeta(
                macros=(
                    f"\n\tQ_PROPERTY(QList<{self.type}> {self.name} "
                    f"READ {self.name}"
                    f"{f" WRITE set{nameCap} NOTIFY {self.name}Changed" if self.mutable else " CONSTANT"}"
                    f")"
                ),
                public=publicStr + "\n",
                private=(
                    f"\n\t{constMod}{"mutable " if self.mutable else ""}QList<{self.type}> m_{self.name} = {{{cList}}};"
                )
            ),
            body=BodyClassMeta(
                content=contentStr + "\n"
            )
        )

class BindableProperty(BaseClassMeta):
    parent: CppClassModel
    name: str
    type: str
    defaultValue: int | float | bool | str | None = None
    binding: str | None = None

    def _setterParam(self) -> str:
        if self.type in {"int", "qreal", "bool"}:
            return f"{self.type} value"
        else:
            return f"const {self.type} &value"

    @override
    def compile(self) -> ClassMeta:
        nameCap = self.name[:1].upper() + self.name[1:]
        cStr = ""

        if (self.binding != None):
            cStr += f"\n\tm_{self.name}.setBinding([this]() -> {self.type} {{ return {self.binding}; }});"

        return ClassMeta(
            header=HeaderClassMeta(
                macros=(
                    f"\n\tQ_PROPERTY({self.type} {self.name} "
                    f"READ {self.name} "
                    f"WRITE set{nameCap} "
                    f"NOTIFY {self.name}Changed "
                    f"BINDABLE bindable{nameCap}"
                    f")"
                ),
                public=(
                    f"\n\t[[nodiscard]] {self.type} {self.name}() const;"
                    f"\n\tvoid set{nameCap}({self._setterParam()});"
                    f"\n\tQBindable<{self.type}> bindable{nameCap}();"
                    f"\n\tQ_SIGNAL void {self.name}Changed();"
                    "\n"
                ),
                private=(
                    f"\n\tQProperty<{self.type}> m_{self.name};"
                )
            ),
            body = BodyClassMeta(
                content=(
                    f"\n{self.type} {self.parent.className}::{self.name}() const {{ return m_{self.name}; }}\n" # GETTER
                    f"\nvoid {self.parent.className}::set{nameCap}({self._setterParam()}) {{\n" # SETTER
                    f"\tm_{self.name} = value;\n}}"
                    f"\nQBindable<{self.type}> {self.parent.className}::bindable{nameCap}() {{ return &m_{self.name}; }}" # BINDABLE
                    "\n"
                )
            ),
            constructor = cStr if self.binding != None else None
        )

def BasicPropFactory(
    obj: SimpleValue,
    parent: CppClassModel
) -> ClassMeta:
    if isinstance(obj, BaseListProperty):
        return ListProperty(
            parent=parent,
            name=obj.name,
            type=obj.listType,
            mutable=obj.mutable,
            defaultValue=obj.defaultValue
        ).compile()
    else:
        return BindableProperty(
            parent=parent,
            name=obj.name,
            type=obj.type,
            defaultValue=obj.defaultValue,
            binding=obj.binding
        ).compile()

class CppFileModel(BaseModel):
    name: str
    imports: set[str] = {
        "qobject.h",
        "qqmlintegration.h",
        "qtmetamacros.h",
        "qproperty.h"
    }
    classes: list[CppClassModel] = []
    fileDataHeader: str = "#pragma once \n\n"
    fileDataBody: str = ""
    namespaceWrapper: ObjectWrapper = ObjectWrapper()

    @override
    def model_post_init(self, context: object, /) -> None:
        for ns in _NAMESPACES:
            self.namespaceWrapper.prefix += "\nnamespace " + ns + "{"
            self.namespaceWrapper.suffix = "} // namespace " + ns + "\n" + self.namespaceWrapper.suffix
        return super().model_post_init(context)

    def addImport(self, imp: str):
        self.imports.add(imp)

    def addClass(self, cl: CppClassModel):
        self.classes.append(cl)

    def processHeaders(self):
        self.fileDataBody += f"#include \"{self.name}.h\"\n\n"
        for header in self.imports:
            istr: str = f"#include <{header}>\n"
            self.fileDataHeader += istr
            self.fileDataBody += istr

    def generate(self):
        self.processHeaders()
        headerStr = ""
        contentStr = ""
        for clss in self.classes:
            clss.generate()
            headerStr = clss.header + headerStr
            contentStr = clss.body + contentStr
        self.fileDataHeader += self.namespaceWrapper.wrap(content=headerStr)
        self.fileDataBody += self.namespaceWrapper.wrap(content=contentStr)

class CppClassModel(BaseModel):
    className: str
    fileModel: CppFileModel
    parent: CppClassModel | None = None
    children: list[CppClassModel] = []
    data: list[SimpleValue | ObjectProperty]
    macros: str = (
            "\tQ_OBJECT\n"
            "\tQML_ELEMENT\n"
            "\tQML_UNCREATABLE(\"\")\n"
            )

    public: str = ""
    private: str = ""
    body: str = ""
    header: str = ""
    wrapperHeader: ObjectWrapper = ObjectWrapper()
    wrapperBody: ObjectWrapper = ObjectWrapper()
    wrapperComments: ObjectWrapper = ObjectWrapper()
    constructorFunction: ObjectWrapper = ObjectWrapper(lock=True)
    constructorArguments: list[str] = ["QObject *parent"]
    constructorDependencies: list[str] = ["QObject(parent)"]

    @override
    def model_post_init(self, __context: object) -> None:
        self.fileModel.addClass(self)

        self.constructorFunction.prefix = self.className + "::" + self.className + "("
        self.constructorFunction.suffix = "}"

        self.wrapperHeader.prefix = (
            f"\nclass {self.className} : public QObject {{\n"
        )
        self.wrapperHeader.suffix = (
            "};\n"
        )

        self.wrapperComments.prefix = f"\n// BEGIN CLASS [[ {self.className} ]]"
        self.wrapperComments.suffix = f"// END CLASS [[ {self.className} ]]\n"

        self.wrapperHeader.metaWrapper = self.wrapperComments

        self.wrapperBody.metaWrapper = self.wrapperComments

        self.addPublicMethod(header=f"explicit {self.className}(QObject *parent = nullptr);")
        self.iterateModel()
        return super().model_post_init(__context)

    def addPublicMethod(self, header: str, impl: str | None = None):
        self.public += "\t" + header + "\n";
        if (impl != None):
            self.wrapperBody.content += f"\n{impl}\n"

    def addPrivateMethod(self, header: str, impl: str | None = None):
        self.private += f"\t{header}\n"
        if (impl != None):
            self.wrapperBody.content += f"\n{impl}\n"

    def finalizeConstructor(self):
        self.constructorFunction.prefix += ", ".join(self.constructorArguments) + "): "
        self.constructorFunction.prefix += ", ".join(self.constructorDependencies) + " {"
        if len(self.constructorFunction.content) > 0:
            self.constructorFunction.content += "\n"
        self.wrapperBody.content = "\n" + self.constructorFunction.wrap() + "\n" + self.wrapperBody.content

    def iterateModel(self):
        for child in self.data:
            if (isinstance(child, ObjectProperty)):
                newChild = CppClassModel(
                        fileModel=self.fileModel,
                        parent=self,
                        className=child.name,
                        data=child.properties
                        )
                self.children.append(newChild)
                cc = ChildClassMeta(parent=self, name=child.name).compile()
                self.macros += cc.header.macros
                self.private += cc.header.private
                self.public += cc.header.public
                self.wrapperBody.content += cc.body.content
            else:
                match child.type:
                    case "QString":
                        self.fileModel.addImport("qstring.h")
                    case "QColor":
                        self.fileModel.addImport("qcolor.h")
                    case "qreal":
                        self.fileModel.addImport("qtypes.h")
                    case "QList":
                        self.fileModel.addImport("qlist.h")
                    case _:
                        pass
                bp = BasicPropFactory(child, self)
                self.macros += bp.header.macros
                self.private += bp.header.private
                self.public += bp.header.public
                self.wrapperBody.content += bp.body.content
                if bp.constructor:
                    self.constructorFunction.content += bp.constructor

    def generate(self):
        self.wrapperHeader.content += self.macros + "\n\n"
        if (len(self.public) > 0):
            self.wrapperHeader.content += "public:\n" + self.public + "\n"
        if (len(self.private) > 0):
            self.wrapperHeader.content += "private:" + self.private + "\n"
        self.finalizeConstructor()
        self.header = self.wrapperHeader.wrap()
        self.body = self.wrapperBody.wrap()

def main():
    directory = Path("./schemas/")
    classFiles: list[str] = []

    for item in directory.iterdir():
        if item.full_match("**/*.json"):
            classFiles.append(item.name)

    models: list[BaseDocument] = []

    for file in classFiles:
        try:
            with open(f"./schemas/{file}", "r", encoding="utf-8") as file:
                content = file.read()
                model = BaseDocument.model_validate_json(json_data=content)
                models.append(model)
        except OSError as err:
            raise Exception(f"Error opening file {file}: {err.strerror}")
        except ValidationError as err:
            raise Exception(f"Error parsing model {file}: ${err.errors(include_url=False)}")

    fileList: list[str] = []

    for model in models:
        fileName = model.className.lower()
        fileModel = CppFileModel(name=fileName)
        baseClass = CppClassModel(fileModel=fileModel, data=model.properties, className=model.className)

        fileModel.generate()

        print(f"Generating {fileName}...")
        
        with open("./generated/" + fileName + ".h", "w") as f:
            f.write(fileModel.fileDataHeader)

        with open("./generated/" + fileName + ".cpp", "w") as f:
            f.write(fileModel.fileDataBody)

        fileList.append(f"{fileName}.h {fileName}.cpp")

        print(f"Successfully generated {fileName}.h and {fileName}.cpp. Running clang++...")

        result = subprocess.run(
            ["clang++", "-fsyntax-only", "-std=c++20", "-I/usr/include/qt6", "-I/usr/include/qt6/QtCore", "-I/usr/include/qt6/QtQml", "-I/usr/include/qt6/QtQmlIntegration", f"./generated/{fileName}.cpp"],
            capture_output=True,
            text=True
        )
        
        if (result.returncode == 1):
            raise Exception(result.stderr)

        print(f"Clang++ detected no errors for {fileName}. Onwards.\n")

    print(f"Generation successful!\nGenerating CMakeLists...")

    cmakeStr = (
"""find_package(Qt6 REQUIRED COMPONENTS
    Core
    Qml
    Quick
)

add_library(myshell_config_gen
    STATIC
        """ + "\n\t\t".join(fileList) +
""")

set_target_properties(myshell_config_gen PROPERTIES
    POSITION_INDEPENDENT_CODE ON)

target_include_directories(myshell_config_gen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(myshell_config_gen
    PRIVATE
        Qt::Core
        Qt::Qml
        Qt::Quick
        myshell_include)

qml_module(myshell_config_gen_plugin
    URI MyShellPlugin.Configs.Gen
    SOURCES
        """ + "\n\t\t".join(fileList) + """
    LIBRARIES
        Qt::Core
        Qt::Quick
        myshell_include
    )"""
    )

    with open("./generated/CMakeLists.txt", "w") as f:
        f.write(cmakeStr)

    print("CMakeLists.txt file successfully written to ./generated/CMakeLists.txt")

try:
    main()
except Exception as e:
    print(f"Generation failed!\n{e}", file=sys.stderr)

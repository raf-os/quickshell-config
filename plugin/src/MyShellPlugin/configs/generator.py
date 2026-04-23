#!/usr/bin/env -S uv run --script
# /// script
# dependencies = ["pydantic"]
# ///

import subprocess
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

SimpleValue = SimpleValueString | SimpleValueReal | SimpleValueInt | SimpleValueBool

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
                public=(f"\n\t[[nodiscard]] {self.name} *{nameLower}() const;"),
                private=(f"\n\t{self.name} *m_{nameLower} = new {self.name}(this);")
            ),
            body=BodyClassMeta(
                content=f"\n{self.name} *{self.parent.className}::{nameLower}() const {{ return m_{nameLower}; }}\n"
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
            return f"{self.type} {self.name}"
        else:
            return f"const {self.type} &{self.name}"

    @override
    def compile(self) -> ClassMeta:
        nameCap = self.name[:1].upper() + self.name[1:]
        cStr = ""

        if (self.binding != None):
            cStr += f"\n\tm_{self.name}.setBinding([this]() {{ return {self.binding}; }});\n"

        return ClassMeta(
            header=HeaderClassMeta(
                macros=(
                    f"\n\tQ_PROPERTY({self.name} "
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
                    f"\tQProperty<{self.type}> m_{self.name}{f" = {self.defaultValue}" if self.defaultValue != None else ""};\n"
                )
            ),
            body = BodyClassMeta(
                content=(
                    f"\n{self.type} {self.parent.className}::{self.name} const {{ return m_{self.name}; }}\n" # GETTER
                    f"\nvoid {self.parent.className}::set{nameCap}(const {self.type} &value) {{\n" # SETTER
                    f"\tm_{self.name} = value;\n}}"
                    f"\nQBindable<{self.type}> {self.parent.className}::bindable{nameCap}() {{ return &m_{self.name}; }}" # BINDABLE
                    "\n"
                )
            ),
            constructor = cStr if self.binding != None else None
        )

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
        self.fileDataBody += f"#include <{self.name}.h>\n"
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
                    case _:
                        pass
                bp = BindableProperty(
                    parent=self,
                    name=child.name,
                    type=child.type,
                    defaultValue=child.defaultValue,
                    binding=child.binding
                ).compile()
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
            self.wrapperHeader.content += "private:\n" + self.private + "\n"
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
            print(f"Error opening file {file}: {err.strerror}")
            return;
        except ValidationError as err:
            print(f"Error parsing model {file}: ${err.errors(include_url=False)}")
            return;

    cppFileModel: list[CppFileModel] = []

    for model in models:
        fileName = model.className.lower()
        fileModel = CppFileModel(name=fileName)
        baseClass = CppClassModel(fileModel=fileModel, data=model.properties, className=model.className)

        fileModel.generate()
        
        with open("./generated/" + fileName + ".h", "w") as f:
            f.write(fileModel.fileDataHeader)


        with open("./generated/" + fileName + ".cpp", "w") as f:
            f.write(fileModel.fileDataBody)

        result = subprocess.run(
            ["clang++", "-fsyntax-only", "-std=c++20", "-I/usr/include/qt6", "-I/usr/include/qt6/QtCore", "-I/usr/include/qt6/QtQml", "-I/usr/include/qt6/QtQmlIntegration", f"./generated/{fileName}.cpp"],
            capture_output=True,
            text=True
        )
        print(result)

main()

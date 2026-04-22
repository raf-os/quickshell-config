#!/usr/bin/env -S uv run --script
# /// script
# dependencies = ["pydantic"]
# ///

from typing import Literal, override
from pathlib import Path
from pydantic import BaseModel, ValidationError

_CPP_NAMESPACE = "myqmlplugin::configs::"

class BaseProperty(BaseModel):
    name: str

class SimpleValueString(BaseProperty):
    type: Literal["QString", "QColor"]
    defaultValue: str

class SimpleValueReal(BaseProperty):
    type: Literal["qreal"]
    defaultValue: float

class SimpleValueInt(BaseProperty):
    type: Literal["int"]
    defaultValue: int

class SimpleValueBool(BaseProperty):
    type: Literal["bool"]
    defaultValue: bool

SimpleValue = SimpleValueString | SimpleValueReal | SimpleValueInt | SimpleValueBool

class BaseDocument(BaseModel):
    className: str
    properties: list[SimpleValue | ObjectProperty]

class ObjectProperty(BaseModel):
    type: Literal["QObject"]
    properties: list[SimpleValue | ObjectProperty]

class CppFileModel(BaseModel):
    imports: set[str] = {
            "qobject.h",
            "qqmlintegration.h",
            "qtmetamacros.h",
            "propertymacros.h"
            }
    classes: list[CppClassModel] = []

    def addImport(self, imp: str):
        self.imports.add(imp)

    def addClass(self, cl: CppClassModel):
        self.classes.append(cl)

class CppClassModel(BaseModel):
    className: str | None = None
    fileModel: CppFileModel
    parent: CppClassModel | None = None
    children: list[SimpleValue | ObjectProperty]
    macros: list[str] = [
            "Q_OBJECT",
            "QML_ELEMENT",
            "QML_UNCREATABLE(\"\")",
            ""
            ]
    public: list[str] = []
    private: list[str] = []

    @override
    def model_post_init(self, __context: object) -> None:
        self.fileModel.addClass(self)
        return super().model_post_init(__context)

    def iterateModel(self):
        for child in self.children:
            if (isinstance(child, ObjectProperty)):
                pass
            else:
                pass

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
        fileModel = CppFileModel();
        baseClass = CppClassModel(fileModel=fileModel, children=model.properties);
        baseClass.iterateModel();

main()

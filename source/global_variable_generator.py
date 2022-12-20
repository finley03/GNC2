import json
from jinja2 import Environment, FileSystemLoader
import binascii

sourceFile = open("source/global_variables.json", "r")
parsedJSON = json.loads(sourceFile.read())

strippedParsedJSON = {key: value for (key, value) in parsedJSON.items() if len(value) != 0}
JSONCRC = hex(binascii.crc32(str(strippedParsedJSON).encode()))

fileLoader = FileSystemLoader("source")
jinjaEnv = Environment(loader = fileLoader)

rendered = jinjaEnv.get_template("global_variables_template.j2").render(variableTypes=parsedJSON, JSONCRC = JSONCRC)

outputFile = open("source/global_variables.h", "w")
outputFile.write(rendered)

sourceFile.close()
outputFile.close()
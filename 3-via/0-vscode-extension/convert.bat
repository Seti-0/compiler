@echo off

echo Converting yaml language description to json for VSCode...

(npx js-yaml syntaxes/via.tmLanguage.yaml > syntaxes/via.tmLanguage.json) && echo Complete.
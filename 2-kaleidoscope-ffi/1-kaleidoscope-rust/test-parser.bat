@echo off

rem Most of this file is me being silly with formating.
rem Only 2 commands are actually executed, plus 'cd' commands.
rem
rem     tree-sitter generate
rem     tree-sitter test
rem
rem The 'cd' commands are because tree-sitter is run from the 
rem src/tree-sitter-via folder.

set BOLD=[1m
set RED=[91m
set GREEN=[32m
set GRAY=[90m
set CYAN=[36m
set RESET=[0m

echo %GRAY%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%RESET%
echo %CYAN%Running 'tree-sitter generate'%RESET%
echo %GRAY%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%RESET%

cd src/compiler/tree-sitter-via
tree-sitter generate && (
    echo %GRAY%~~~~~~~~~~~~~~~~~~~~~~~~~~%RESET%
    echo %CYAN%Running 'tree-sitter test'%RESET%
    echo %GRAY%~~~~~~~~~~~~~~~~~~~~~~~~~~%RESET%
    
    tree-sitter test -f "Comments"
    cd ../../..
) || (
    echo %GRAY%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%RESET%
    echo %RED%'tree-sitter generate' failed.%RESET%
    echo %GRAY%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%RESET%
    cd ../../..
)


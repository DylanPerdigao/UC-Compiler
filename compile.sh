
 # @author Dylan Perdigão   -   2018233092
 # @author Bruno Faria      -   2018295474

# Terminal Escape Colors
RED="\033[31m"
GREEN="\x1B[32m"
RESET="\x1B[0m"
BLUE="\x1B[34m"
#I/O
inputDIR="test_input"
outputDIR="test_output"
originalDIR="test_output_terceiros"
# Default Configurations
SHOW_DIFF="true"
CHEATS="false"

lex uccompiler.l
yacc -d uccompiler.y

if test -f "uccompiler"; then
    rm uccompiler
fi
gcc -o uccompiler symbolTable.c ASTree.c LLVMgenerator.c y.tab.c lex.yy.c
    rm y.tab.h
    rm y.tab.c
    rm lex.yy.c
for file_path in "$inputDIR"/*.c; do
    #CRIAR OS FICHEIROS OUTPUT
    ucfile=$(basename "$file_path")
    outfile=${ucfile%.*}.ll
    ./uccompiler < $file_path > "$outputDIR"/$outfile
	#output arvore
	#outfile2=${ucfile%.*}.out
	#./uccompiler -s < $file_path > "$outputDIR"/$outfile2
    #CRIAR FICHEIRO DE CHEATS
    if [ $CHEATS == "true" ]; then
        if [[ "$OSTYPE" == "linux-gnu"* ]]; then
            echo LINUX
            clang-3.9 -S -emit-llvm $file_path
        elif [[ "$OSTYPE" == "darwin"* ]]; then
            echo MACOS
            clang -S -emit-llvm $file_path
        fi
        mv $outfile "$originalDIR"/$outfile
    fi
    #./uccompiler -s < test_input/first.uc > test_output/first.out
    #FAZER DIFF ENTRE OS FICHEIROS
    (diff -y "$outputDIR"/$outfile "$originalDIR"/$outfile) &>DIFFOUT
    if [ $? -eq 0 ]; then
        echo -e "✅ ${GREEN} TEST PASSED!! ${RESET}" $outfile \\t\\t\\t\\t\\t\\t "${BLUE} OUTPUT DESEJADO ${RESET}"
    else
        if [ $SHOW_DIFF == "true" ]; then
            echo -e  "❌ ${RED}TEST FAILED!!${RESET}" $outfile \\t\\t\\t\\t\\t\\t "${BLUE}OUTPUT DESEJADO${RESET}"
            [ $SHOW_DIFF == "true" ] && echo && cat -n DIFFOUT && echo
        else
            echo -e "❌ ${RED}TEST FAILED!!${RESET}" $outfile
        fi

    fi
done
rm DIFFOUT
if test -f "uccompiler.zip"; then
    rm uccompiler.zip
fi
zip uccompiler.zip uccompiler.l uccompiler.y ASTree.h ASTree.c symbolTable.h symbolTable.c LLVMgenerator.h LLVMgenerator.c

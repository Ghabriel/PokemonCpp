import * as fs from 'fs';
import ts from 'typescript';

const originPath = '../resources/scripts/original/';
const fileNames = ['new-battle-formulas.ts'];

for (const fileName of fileNames) {
    console.log(`[PRINTING] ${fileName}`);

    const inputPath = originPath + fileName;

    const sourceFile = ts.createSourceFile(
        inputPath,
        fs.readFileSync(inputPath).toString(),
        ts.ScriptTarget.ES2015,
        true
    );

    printAST(sourceFile);
}

function printAST(sourceFile: ts.SourceFile): void {
    printNode(sourceFile, 0);

    function printNode(node: ts.Node, level: number): void {
        const indentation = Array(level + 1).join('    ');
        console.log(`${indentation}${ts.SyntaxKind[node.kind]}`);

        ts.forEachChild(node, child => printNode(child, level + 1));
    }
}

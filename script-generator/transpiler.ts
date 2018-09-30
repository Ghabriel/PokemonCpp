import * as fs from 'fs';
import ts from 'typescript';
import { LuaTranspiler } from './LuaTranspiler';

const originPath = '../resources/scripts/original/';
const targetPath = '../resources/scripts/';
const ignoredFiles = ['types.ts'];
const transpiler = new LuaTranspiler();
const fileNames = fs.readdirSync(originPath);

for (const fileName of fileNames) {
    if (ignoredFiles.indexOf(fileName) >= 0) {
        continue;
    }

    console.log(`[TRANSPILING] ${fileName}`);

    const inputPath = originPath + fileName;
    const outputPath = targetPath + fileName.replace('.ts', '.lua');

    const sourceFile = ts.createSourceFile(
        inputPath,
        fs.readFileSync(inputPath).toString(),
        ts.ScriptTarget.ES2015,
        true
    );

    const luaCode = transpiler.transpile(sourceFile);
    fs.writeFileSync(outputPath, luaCode);

    console.log('[ COMPLETED ]');
}

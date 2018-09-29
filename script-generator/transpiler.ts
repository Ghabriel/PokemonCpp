import * as fs from 'fs';
import ts from 'typescript';
import { LuaBaseTranspiler } from './LuaBaseTranspiler';

const originPath = '../resources/scripts/original/';
const targetPath = '../resources/scripts/';
const fileNames = fs.readdirSync(originPath);
const transpiler = new LuaBaseTranspiler();

for (const fileName of fileNames) {
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

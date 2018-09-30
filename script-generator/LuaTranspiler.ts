import ts from 'typescript';
import { LuaBaseTranspiler } from './LuaBaseTranspiler';

/**
 * Adds Standard Library conversions to the base Lua transpiler.
 */
export class LuaTranspiler extends LuaBaseTranspiler {
    protected transpileIdentifier(node: ts.Identifier): void {
        this.emit(this.getAdaptedIdentifierText(node));
    }

    private getAdaptedIdentifierText(node: ts.Identifier): string {
        switch (node.text) {
            case 'Math':
                return 'math';
            default:
                return node.text;
        }
    }
}

import ts from 'typescript';
import { LuaBaseTranspiler } from './LuaBaseTranspiler';

/**
 * Adds project-specific conversions to the base Lua transpiler.
 */
export class LuaTranspiler extends LuaBaseTranspiler {
    protected transpileStatement(node: ts.Statement): void {
        if (this.isFunctionCallTo(node, 'luaImplicitSelf')) {
            const args = this.getFunctionCallArguments(node);

            if (args.length === 1 && args[0].kind === ts.SyntaxKind.Identifier) {
                const aggregate = (args[0] as ts.Identifier).text;
                this.implicitSelf.push(aggregate);
                return;
            }
        }

        if (this.isFunctionCallTo(node, 'luaImport')) {
            const args = this.getFunctionCallArguments(node);

            if (args.length === 1 && args[0].kind === ts.SyntaxKind.StringLiteral) {
                const path = (args[0] as ts.StringLiteral).text;
                this.emitIndented(`require "${path}"\n`);
                return;
            }
        }

        super.transpileStatement(node);
    }

    private isFunctionCallTo(node: ts.Statement, name: string): boolean {
        if (node.kind !== ts.SyntaxKind.ExpressionStatement) {
            return false;
        }

        const exprStatementNode = node as ts.ExpressionStatement;

        if (exprStatementNode.expression.kind !== ts.SyntaxKind.CallExpression) {
            return false;
        }

        const callExpression = exprStatementNode.expression as ts.CallExpression;

        if (callExpression.expression.kind !== ts.SyntaxKind.Identifier) {
            return false;
        }

        const calledFunction = callExpression.expression as ts.Identifier;
        return calledFunction.text === name;
    }

    private getFunctionCallArguments(node: ts.Statement): ts.NodeArray<ts.Expression> {
        const exprStatementNode = node as ts.ExpressionStatement;
        const callExpression = exprStatementNode.expression as ts.CallExpression;
        return callExpression.arguments;
    }

    protected transpileFunctionMetadata(node: ts.FunctionDeclaration): void {
        for (const parameter of node.parameters) {
            if (parameter.type === undefined) {
                continue;
            }

            const metaTable = this.getMetaTableForType(parameter.type);

            if (metaTable !== null) {
                const parameterName = parameter.name.getText();
                this.emitIndented(`setmetatable(${parameterName}, ${metaTable})\n`);
            }
        }
    }

    private getMetaTableForType(type: ts.TypeNode): string | null {
        switch (type.getText()) {
            case 'Pokemon':
                return 'pokemonMetatable';
            case 'Move':
                return 'moveMetatable';
            default:
                return null;
        }
    }

    protected transpilePropertyAccessExpression(node: ts.PropertyAccessExpression): void {
        if (node.expression.kind === ts.SyntaxKind.Identifier) {
            const aggregate = (node.expression as ts.Identifier).text;

            if (this.implicitSelf.indexOf(aggregate) >= 0) {
                this.transpileExpression(node.expression);
                this.emit(':');
                this.transpileIdentifier(node.name);
                return;
            }
        }

        super.transpilePropertyAccessExpression(node);
    }

    protected transpileIdentifier(node: ts.Identifier): void {
        const adaptedText = this.getAdaptedIdentifierText(node);

        if (adaptedText !== node.text) {
            this.emit(adaptedText);
            return;
        }

        super.transpileIdentifier(node);
    }

    private getAdaptedIdentifierText(node: ts.Identifier): string {
        switch (node.text) {
            case 'Math':
                return 'math';
            default:
                return node.text;
        }
    }

    private implicitSelf: string[] = [];
}

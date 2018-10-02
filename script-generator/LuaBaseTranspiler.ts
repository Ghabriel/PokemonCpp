import ts from 'typescript';

export class LuaBaseTranspiler {
    transpile(sourceFile: ts.SourceFile): string {
        this.output = '';
        this.indentationLevel = 0;
        ts.forEachChild(sourceFile, node => this.transpileNode(node));
        return this.output;
    }

    protected transpileNode(node: ts.Node): void {
        if (node.kind === ts.SyntaxKind.EndOfFileToken) {
            return;
        }

        this.transpileStatement(node as ts.Statement);
    }

    protected transpileStatement(node: ts.Statement): void {
        switch (node.kind) {
            case ts.SyntaxKind.ImportDeclaration:
            case ts.SyntaxKind.InterfaceDeclaration:
                // type-only, nothing to do
                break;
            case ts.SyntaxKind.ModuleDeclaration:
                this.transpileNamespaceDeclaration(node as ts.ModuleDeclaration);
                break;
            case ts.SyntaxKind.FunctionDeclaration:
                this.transpileFunctionDeclaration(node as ts.FunctionDeclaration);
                break;
            case ts.SyntaxKind.EnumDeclaration:
                this.transpileEnumDeclaration(node as ts.EnumDeclaration);
                break;
            case ts.SyntaxKind.Block:
                this.transpileBlock(node as ts.Block);
                break;
            case ts.SyntaxKind.VariableStatement:
                this.transpileVariableStatement(node as ts.VariableStatement);
                this.emit('\n');
                break;
            case ts.SyntaxKind.ExpressionStatement:
                this.emitIndented('');
                this.transpileExpression((node as ts.ExpressionStatement).expression);
                this.emit('\n');
                break;
            case ts.SyntaxKind.IfStatement:
                this.transpileIfStatement(node as ts.IfStatement);
                this.emit('\n');
                break;
            case ts.SyntaxKind.SwitchStatement:
                this.transpileSwitchStatement(node as ts.SwitchStatement);
                this.emit('\n');
                break;
            case ts.SyntaxKind.BreakStatement:
                // handled in switch statements, not handled in other cases
                break;
            case ts.SyntaxKind.ForStatement:
                this.transpileForStatement(node as ts.ForStatement);
                this.emit('\n');
                break;
            case ts.SyntaxKind.ReturnStatement:
                this.emitIndented('');
                this.transpileReturnStatement(node as ts.ReturnStatement);
                this.emit('\n');
                break;
            default:
                return this.handleUnsupportedNode(node, 'transpileStatement');
        }
    }

    protected transpileNamespaceDeclaration(node: ts.ModuleDeclaration): void {
        if (!node.body || node.body.kind !== ts.SyntaxKind.ModuleBlock) {
            return this.handleUnsupportedNode(node, 'transpileModuleDeclaration');
        }

        const namespaceName = node.name.text;
        this.emitIndented(`${namespaceName} = {}\n`);

        this.namespace = namespaceName;
        const body = node.body as ts.ModuleBlock;
        body.statements.forEach(statement => this.transpileStatement(statement));
        this.namespace = null;
    }

    protected transpileFunctionDeclaration(node: ts.FunctionDeclaration): void {
        this.transpileFunctionSignature(node);
        this.indentationLevel++;
        this.transpileBlock(node.body!);
        this.indentationLevel--;
        this.emitIndented('end\n');
    }

    protected transpileFunctionSignature(node: ts.FunctionDeclaration): void {
        const functionName = node.name!.text;
        const parameterNames = node.parameters.map(parameter => parameter.name.getText());
        const parameterList = parameterNames.join(', ');

        this.emitIndented('function ');

        if (this.namespace !== null) {
            this.emit(`${this.namespace}:`);
        }

        this.emit(`${functionName}(${parameterList})\n`);
    }

    protected transpileEnumDeclaration(node: ts.EnumDeclaration): void {
        this.emitIndented(`${node.name.text} = {\n`);
        this.indentationLevel++;

        let nextEnumValue: number = 0;
        for (const member of node.members) {
            if (member.initializer) {
                console.log('Warning: enums with initializers are not supported');
                continue;
            }

            this.emitIndented(`${member.name.getText()} = ${nextEnumValue},\n`);
            nextEnumValue++;
        }

        this.indentationLevel--;
        this.emitIndented('}\n');
    }

    protected transpileBlock(node: ts.Block): void {
        node.statements.forEach(statement => this.transpileStatement(statement));
    }

    protected transpileVariableStatement(node: ts.VariableStatement): void {
        node.declarationList.declarations.forEach(varDeclaration => {
            this.transpileVariableDeclaration(varDeclaration);
        });
    }

    protected transpileVariableDeclaration(node: ts.VariableDeclaration): void {
        this.emitIndented('');

        if (!this.isInGlobalScope()) {
            this.emit('local ');
        }

        this.emit(node.name.getText());

        if (node.initializer) {
            this.emit(' = ');
            this.transpileExpression(node.initializer);
        }
    }

    protected transpileIfStatement(node: ts.IfStatement): void {
        this.emitIndented('if ');
        this.transpileExpression(node.expression);
        this.emit(' then\n');
        this.indentationLevel++;
        this.transpileStatement(node.thenStatement);
        this.indentationLevel--;

        if (node.elseStatement) {
            this.emitIndented('else\n');
            this.indentationLevel++;
            this.transpileStatement(node.elseStatement);
            this.indentationLevel--;
        }

        this.emitIndented('end');
    }

    protected transpileSwitchStatement(node: ts.SwitchStatement): void {
        const expression = node.expression.getText();

        let first = true;
        node.caseBlock.clauses.forEach(caseOrDefaultClause => {
            this.emitIndented('');

            if (!first) {
                this.emit('else');
            }
            first = false;

            if (ts.isCaseClause(caseOrDefaultClause)) {
                const caseExpression = caseOrDefaultClause.expression.getText();
                this.emit(`if ${expression} == ${caseExpression} then`);
            }

            this.emit('\n');

            this.indentationLevel++;
            caseOrDefaultClause.statements.forEach(
                statement => this.transpileStatement(statement)
            );
            this.indentationLevel--;

            const statementList = caseOrDefaultClause.statements;
            if (statementList[statementList.length - 1].kind !== ts.SyntaxKind.BreakStatement) {
                this.handleUnsupportedNode(node, 'transpileSwitchStatement');
            }
        });

        this.emitIndented('end');
    }

    protected transpileForStatement(node: ts.ForStatement): void {
        const initializer = this.decomposeForInitializer(node);
        const upperBound = this.getForUpperBound(node);
        const incrementor = node.incrementor;

        if (
            initializer === null ||
            upperBound === null ||
            incrementor === undefined ||
            incrementor.kind !== ts.SyntaxKind.PostfixUnaryExpression ||
            (incrementor as ts.PostfixUnaryExpression).operator !== ts.SyntaxKind.PlusPlusToken
        ) {
            return this.handleUnsupportedNode(node, 'transpileForStatement');
        }

        const { loopVariable, initialValue } = initializer;
        this.emitIndented(`for ${loopVariable} = ${initialValue}, ${upperBound} do\n`);
        this.indentationLevel++;
        this.transpileStatement(node.statement);
        this.indentationLevel--;
        this.emitIndented('end');
    }

    private decomposeForInitializer(
        node: ts.ForStatement
    ): { loopVariable: string, initialValue: string } | null {
        const initDeclaration = this.getForInitializerDeclaration(node);

        if (initDeclaration === null) {
            return null;
        }

        const loopVariable = initDeclaration.name.getText();
        const initializer = initDeclaration.initializer;

        if (initializer === undefined || initializer.kind !== ts.SyntaxKind.FirstLiteralToken) {
            return null;
        }

        const initialValue = (initializer as ts.LiteralExpression).text;
        return { loopVariable, initialValue };
    }

    private getForInitializerDeclaration(node: ts.ForStatement): ts.VariableDeclaration | null {
        if (!node.initializer || node.initializer.kind !== ts.SyntaxKind.VariableDeclarationList) {
            return null;
        }

        const initializer = node.initializer as ts.VariableDeclarationList;

        if (initializer.declarations.length !== 1) {
            return null;
        }

        return initializer.declarations[0];
    }

    private getForUpperBound(node: ts.ForStatement): string | null {
        if (
            node.condition === undefined ||
            node.condition.kind !== ts.SyntaxKind.BinaryExpression
        ) {
            return null;
        }

        const condition = node.condition as ts.BinaryExpression;
        const rhsText = condition.right.getText();

        switch (condition.operatorToken.getText()) {
            case '<':
                return rhsText + ' - 1';
            case '<=':
                return rhsText;
            default:
                return null;
        }
    }

    protected transpileReturnStatement(node: ts.ReturnStatement): void {
        this.emit('return');

        if (node.expression) {
            this.emit(' ');
            this.transpileExpression(node.expression);
        }
    }

    protected transpileExpression(node: ts.Expression): void {
        switch (node.kind) {
            case ts.SyntaxKind.AsExpression:
                this.transpileAsExpression(node as ts.AsExpression);
                break;
            case ts.SyntaxKind.CallExpression:
                this.transpileCallExpression(node as ts.CallExpression);
                break;
            case ts.SyntaxKind.PropertyAccessExpression:
                this.transpilePropertyAccessExpression(node as ts.PropertyAccessExpression);
                break;
            case ts.SyntaxKind.ElementAccessExpression:
                this.transpileElementAccessExpression(node as ts.ElementAccessExpression);
                break;
            case ts.SyntaxKind.Identifier:
                this.transpileIdentifier(node as ts.Identifier);
                break;
            case ts.SyntaxKind.PrefixUnaryExpression:
                this.transpilePrefixUnaryExpression(node as ts.PrefixUnaryExpression);
                break;
            case ts.SyntaxKind.PostfixUnaryExpression:
                this.transpilePostfixUnaryExpression(node as ts.PostfixUnaryExpression);
                break;
            case ts.SyntaxKind.BinaryExpression:
                this.transpileBinaryExpression(node as ts.BinaryExpression);
                break;
            case ts.SyntaxKind.FirstLiteralToken:
                this.emit((node as ts.LiteralExpression).text);
                break;
            case ts.SyntaxKind.ParenthesizedExpression:
                this.transpileParenthesizedExpression(node as ts.ParenthesizedExpression);
                break;
            case ts.SyntaxKind.StringLiteral:
                this.transpileStringLiteral(node as ts.StringLiteral);
                break;
            case ts.SyntaxKind.TrueKeyword:
                this.emit('true');
                break;
            case ts.SyntaxKind.FalseKeyword:
                this.emit('false');
                break;
            case ts.SyntaxKind.ArrayLiteralExpression:
                this.transpileArrayLiteralExpression(node as ts.ArrayLiteralExpression);
                break;
            case ts.SyntaxKind.ObjectLiteralExpression:
                this.transpileObjectLiteralExpression(node as ts.ObjectLiteralExpression);
                break;
            default:
                return this.handleUnsupportedNode(node, 'transpileExpression');
        }
    }

    protected transpileAsExpression(node: ts.AsExpression): void {
        this.transpileExpression(node.expression);
    }

    protected transpileCallExpression(node: ts.CallExpression): void {
        this.transpileExpression(node.expression);
        this.emit('(');

        let first = true;
        for (const argument of node.arguments) {
            if (!first) {
                this.emit(', ');
            }

            this.transpileExpression(argument);
            first = false;
        }

        this.emit(')');
    }

    protected transpilePropertyAccessExpression(node: ts.PropertyAccessExpression): void {
        this.transpileExpression(node.expression);
        this.emit('.');
        this.transpileIdentifier(node.name);
    }

    protected transpileElementAccessExpression(node: ts.ElementAccessExpression): void {
        this.transpileExpression(node.expression);
        this.emit('[');
        this.transpileExpression(node.argumentExpression);
        this.emit(']');
    }

    protected transpileIdentifier(node: ts.Identifier) {
        this.emit(node.text === 'undefined' ? 'nil' : node.text);
    }

    protected transpilePrefixUnaryExpression(node: ts.PrefixUnaryExpression): void {
        switch (node.operator) {
            case ts.SyntaxKind.PlusToken:
                this.emit('+');
                this.transpileExpression(node.operand);
                break;
            case ts.SyntaxKind.MinusToken:
                this.emit('-');
                this.transpileExpression(node.operand);
                break;
            case ts.SyntaxKind.PlusPlusToken:
            case ts.SyntaxKind.MinusMinusToken:
                this.transpileUnaryIncrementOrDecrement(node);
                break;
            default:
                return this.handleUnsupportedNode(node, 'transpilePrefixUnaryExpression');
        }
    }

    protected transpilePostfixUnaryExpression(node: ts.PostfixUnaryExpression): void {
        this.transpileUnaryIncrementOrDecrement(node);
    }

    protected transpileUnaryIncrementOrDecrement(
        node: ts.PrefixUnaryExpression | ts.PostfixUnaryExpression
    ): void {
        this.transpileExpression(node.operand);
        this.emit(' = ');
        this.transpileExpression(node.operand);

        if (node.operator === ts.SyntaxKind.PlusPlusToken) {
            this.emit(' + ');
        } else if (node.operator == ts.SyntaxKind.MinusMinusToken) {
            this.emit(' - ');
        } else {
            return this.handleUnsupportedNode(node, 'transpileUnaryIncrementOrDecrement');
        }

        this.emit('1');
    }

    protected transpileBinaryExpression(node: ts.BinaryExpression): void {
        if (!this.isCompoundAssignmentOperator(node.operatorToken)) {
            this.transpileExpression(node.left);
            this.emit(' ');
            this.emit(this.convertBinaryOperator(node.operatorToken));
            this.emit(' ');
            this.transpileExpression(node.right);
            return;
        }

        this.transpileExpression(node.left);
        this.emit(' = ');
        this.transpileExpression(node.left);
        this.emit(' ');
        this.emit(node.operatorToken.getText().slice(0, -1));
        this.emit(' ');
        this.transpileExpression(node.right);
    }

    protected isCompoundAssignmentOperator(token: ts.Token<ts.SyntaxKind>): boolean {
        const options: string[] = ['+=', '-=', '*=', '/=', '%=', '**=', '&=', '|=', '^='];
        const tokenText = token.getText();
        return options.indexOf(tokenText) >= 0;
    }

    protected convertBinaryOperator(token: ts.Token<ts.BinaryOperator>): string {
        const tokenText = token.getText();

        switch (tokenText) {
            case '&&':
                return 'and';
            case '||':
                return 'or';
            case '===':
                return '==';
            default:
                return tokenText;
        }
    }

    protected transpileParenthesizedExpression(node: ts.ParenthesizedExpression): void {
        this.emit('(');
        this.transpileExpression(node.expression);
        this.emit(')');
    }

    protected transpileStringLiteral(node: ts.StringLiteral): void {
        this.emit(`"${node.text}"`);
    }

    protected transpileArrayLiteralExpression(node: ts.ArrayLiteralExpression): void {
        this.emit('{');

        let first = true;
        for (const element of node.elements) {
            if (!first) {
                this.emit(', ');
            }

            this.transpileExpression(element);
            first = false;
        }

        this.emit('}');
    }

    protected transpileObjectLiteralExpression(node: ts.ObjectLiteralExpression): void {
        this.emit('{');
        this.indentationLevel++;

        let first = true;
        for (const property of node.properties) {
            if (!first) {
                this.emit(',');
            }

            this.emit('\n');
            this.emitIndented('');
            this.transpileObjectLiteralProperty(property);
            first = false;
        }

        this.indentationLevel--;

        if (!first) {
            this.emit('\n');
            this.emitIndented('');
        }

        this.emit('}');
    }

    protected transpileObjectLiteralProperty(node: ts.ObjectLiteralElementLike): void {
        if (node.kind !== ts.SyntaxKind.PropertyAssignment) {
            return this.handleUnsupportedNode(node, 'transpileObjectLiteralProperty');
        }

        const propertyName = (node as ts.PropertyAssignment).name;

        if (ts.isLiteralExpression(propertyName)) {
            this.emit(`[${propertyName.getText()}]`);
        } else if (ts.isComputedPropertyName(propertyName)) {
            this.emit(propertyName.getText());
        } else {
            return this.handleUnsupportedNode(propertyName, 'transpileObjectLiteralProperty');
        }

        this.emit(' = ');
        this.transpileExpression(node.initializer);
    }

    protected emitIndented(content: string): void {
        const indentation = Array(this.indentationLevel + 1).join(this.indentation);
        this.emit(indentation + content);
    }

    protected emit(content: string): void {
        this.output += content;
    }

    protected handleUnsupportedNode(node: ts.Node, method: string) {
        const nodeKind = ts.SyntaxKind[node.kind];
        const message = `(${method}) Warning: Node kind not supported: ${nodeKind}`;
        const nodeText = node.getText();
        console.log(message + '\n' + this.indentText(nodeText));
    }

    protected indentText(text: string): string {
        const lines = text.split('\n');
        const indentedLines = lines.map(line => this.indentation + line);
        return indentedLines.join('\n');
    }

    protected isInGlobalScope(): boolean {
        return this.indentationLevel === 0;
    }

    private output: string = '';
    private indentationLevel: number = 0;
    private namespace: string | null = null;
    private readonly indentation = '    ';
}

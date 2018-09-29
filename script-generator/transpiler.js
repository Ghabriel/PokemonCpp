"use strict";
exports.__esModule = true;
var fs = require("fs");
var typescript_1 = require("typescript");
var LuaBaseTranspiler = /** @class */ (function () {
    function LuaBaseTranspiler() {
        this.output = '';
        this.indentationLevel = 0;
    }
    LuaBaseTranspiler.prototype.transpile = function (sourceFile) {
        var _this = this;
        this.output = '';
        this.indentationLevel = 0;
        typescript_1["default"].forEachChild(sourceFile, function (node) { return _this.transpileNode(node); });
        return this.output;
    };
    LuaBaseTranspiler.prototype.transpileNode = function (node) {
        switch (node.kind) {
            case typescript_1["default"].SyntaxKind.FunctionDeclaration:
                this.transpileFunctionDeclaration(node);
                break;
            case typescript_1["default"].SyntaxKind.EndOfFileToken:
                break;
            default:
                return this.handleUnsupportedNode(node, 'transpileNode');
        }
    };
    LuaBaseTranspiler.prototype.transpileFunctionDeclaration = function (node) {
        this.transpileFunctionSignature(node);
        this.indentationLevel++;
        this.transpileBlock(node.body);
        this.indentationLevel--;
        this.emitIndented('end\n');
    };
    LuaBaseTranspiler.prototype.transpileFunctionSignature = function (node) {
        var parameterNames = node.parameters.map(function (parameter) { return parameter.name.getText(); });
        var parameterList = parameterNames.join(', ');
        var result = "function " + node.name.text + "(" + parameterList + ")\n";
        this.emitIndented(result);
    };
    LuaBaseTranspiler.prototype.transpileBlock = function (node) {
        var _this = this;
        node.statements.forEach(function (statement) { return _this.transpileStatement(statement); });
    };
    LuaBaseTranspiler.prototype.transpileStatement = function (node) {
        switch (node.kind) {
            case typescript_1["default"].SyntaxKind.Block:
                this.transpileBlock(node);
                break;
            case typescript_1["default"].SyntaxKind.VariableStatement:
                this.transpileVariableStatement(node);
                this.emit('\n');
                break;
            case typescript_1["default"].SyntaxKind.ExpressionStatement:
                this.emitIndented('');
                this.transpileExpression(node.expression);
                this.emit('\n');
                break;
            case typescript_1["default"].SyntaxKind.IfStatement:
                this.transpileIfStatement(node);
                this.emit('\n');
                break;
            case typescript_1["default"].SyntaxKind.ReturnStatement:
                this.emitIndented('');
                this.transpileReturnStatement(node);
                this.emit('\n');
                break;
            default:
                return this.handleUnsupportedNode(node, 'transpileStatement');
        }
    };
    LuaBaseTranspiler.prototype.transpileVariableStatement = function (node) {
        var _this = this;
        node.declarationList.declarations.forEach(function (varDeclaration) {
            _this.transpileVariableDeclaration(varDeclaration);
        });
    };
    LuaBaseTranspiler.prototype.transpileVariableDeclaration = function (node) {
        var varName = node.name.getText();
        this.emitIndented("local " + varName);
        if (node.initializer) {
            this.emit(' = ');
            this.transpileExpression(node.initializer);
        }
    };
    LuaBaseTranspiler.prototype.transpileIfStatement = function (node) {
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
    };
    LuaBaseTranspiler.prototype.transpileReturnStatement = function (node) {
        this.emit('return');
        if (node.expression) {
            this.emit(' ');
            this.transpileExpression(node.expression);
        }
    };
    LuaBaseTranspiler.prototype.transpileExpression = function (node) {
        switch (node.kind) {
            case typescript_1["default"].SyntaxKind.CallExpression:
                this.transpileCallExpression(node);
                break;
            case typescript_1["default"].SyntaxKind.PropertyAccessExpression:
                this.transpilePropertyAccessExpression(node);
                break;
            case typescript_1["default"].SyntaxKind.Identifier:
                this.transpileIdentifier(node);
                break;
            case typescript_1["default"].SyntaxKind.PostfixUnaryExpression:
                this.transpilePostfixUnaryExpression(node);
                break;
            case typescript_1["default"].SyntaxKind.BinaryExpression:
                this.transpileBinaryExpression(node);
                break;
            case typescript_1["default"].SyntaxKind.FirstLiteralToken:
                this.emit(node.text);
                break;
            case typescript_1["default"].SyntaxKind.ParenthesizedExpression:
                this.transpileParenthesizedExpression(node);
                break;
            default:
                return this.handleUnsupportedNode(node, 'transpileExpression');
        }
    };
    LuaBaseTranspiler.prototype.transpileCallExpression = function (node) {
        this.transpileExpression(node.expression);
        this.emit('(');
        var first = true;
        for (var _i = 0, _a = node.arguments; _i < _a.length; _i++) {
            var argument = _a[_i];
            if (!first) {
                this.emit(', ');
            }
            this.transpileExpression(argument);
            first = false;
        }
        this.emit(')');
    };
    LuaBaseTranspiler.prototype.transpilePropertyAccessExpression = function (node) {
        this.transpileExpression(node.expression);
        this.emit('.');
        this.transpileIdentifier(node.name);
    };
    LuaBaseTranspiler.prototype.transpileIdentifier = function (node) {
        this.emit(node.text);
    };
    LuaBaseTranspiler.prototype.transpilePostfixUnaryExpression = function (node) {
        this.transpileExpression(node.operand);
        this.emit(' = ');
        this.transpileExpression(node.operand);
        if (node.operator === typescript_1["default"].SyntaxKind.PlusPlusToken) {
            this.emit(' + ');
        }
        else if (node.operator == typescript_1["default"].SyntaxKind.MinusMinusToken) {
            this.emit(' - ');
        }
        else {
            return this.handleUnsupportedNode(node, 'transpilePostfixUnaryExpression');
        }
        this.emit('1');
    };
    LuaBaseTranspiler.prototype.transpileBinaryExpression = function (node) {
        this.transpileExpression(node.left);
        this.emit(' ');
        this.emit(node.operatorToken.getText());
        this.emit(' ');
        this.transpileExpression(node.right);
    };
    LuaBaseTranspiler.prototype.transpileParenthesizedExpression = function (node) {
        this.emit('(');
        this.transpileExpression(node.expression);
        this.emit(')');
    };
    LuaBaseTranspiler.prototype.emitIndented = function (content) {
        var indentation = Array(this.indentationLevel + 1).join('    ');
        this.emit(indentation + content);
    };
    LuaBaseTranspiler.prototype.emit = function (content) {
        this.output += content;
    };
    LuaBaseTranspiler.prototype.handleUnsupportedNode = function (node, method) {
        var nodeKind = typescript_1["default"].SyntaxKind[node.kind];
        // throw Error(`Node kind not supported: ${ts.SyntaxKind[node.kind]}`);
        console.log("(" + method + ") Warning: Node kind not supported: " + nodeKind);
    };
    return LuaBaseTranspiler;
}());
exports.LuaBaseTranspiler = LuaBaseTranspiler;
var fileNames = ['test.ts'];
var luaTranspiler = new LuaBaseTranspiler();
for (var _i = 0, fileNames_1 = fileNames; _i < fileNames_1.length; _i++) {
    var fileName = fileNames_1[_i];
    var sourceFile = typescript_1["default"].createSourceFile(fileName, fs.readFileSync(fileName).toString(), typescript_1["default"].ScriptTarget.ES2015, true);
    // printAST(sourceFile);
    var luaCode = luaTranspiler.transpile(sourceFile);
    console.log(luaCode);
}
function printAST(sourceFile) {
    printNode(sourceFile, 0);
    function printNode(node, level) {
        var indentation = Array(level).join('    ');
        console.log("" + indentation + typescript_1["default"].SyntaxKind[node.kind]);
        typescript_1["default"].forEachChild(node, function (child) { return printNode(child, level + 1); });
    }
}
